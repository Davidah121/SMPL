#include "Sound.h"
#include "SimpleFile.h"
#include "MathExt.h"

namespace smpl
{

    //Structures that could potentially help
    const int RIFF_ID = 0x46464952; //RIFF characters in hex
    const int WAVE_VALUE = 0x45564157; //WAVE characters in hex
    const int FMT_ID = 0x20746D66; //"fmt " characters in hex. The space is there on purpose
    const int DATA_ID = 0x61746164; //RIFF characters in hex

    struct WAV_CHUNK
    {
        int subchunkID;
        int subchunkSize;
    };

    struct RIFF_HEADER
    {
        WAV_CHUNK chunkID;
        int format;
    };

    struct FMT_HEADER
    {
        WAV_CHUNK chunkID;
        short audioFormat;
        short numChannels;
        int sampleRate;
        int byteRate;
        short blockAlign;
        short bitsPerSample;
    };

    struct DATA_HEADER
    {
        WAV_CHUNK chunkID;
        std::vector<unsigned char> data;
    };

    
    bool readChunkHeader(StreamableFile& f, WAV_CHUNK& output)
    {
        return f.read((unsigned char*)&output, sizeof(WAV_CHUNK)) == sizeof(WAV_CHUNK);
    }

    Vec2f* resampleAudio(Vec2f* inputData, size_t inputSize, int samplesPerSecond, size_t* outputLength)
    {
        //take the cheap way out and just lerp my way through
        double totalSeconds = (double)inputSize / samplesPerSecond;
        size_t newSize = (size_t)(44100*totalSeconds);
        Vec2f* newOutput = new Vec2f[newSize];

        for(size_t i=0; i<newSize; i++)
        {
            double timeInSeconds = (double)i / 44100;
            double originalSampleSlot = timeInSeconds * samplesPerSecond;
            double fractionalPart = MathExt::frac(originalSampleSlot);
            if(fractionalPart == 0)
            {
                //exact sample. Good
                newOutput[i] = inputData[(size_t)originalSampleSlot];
            }
            else
            {
                //lerp between the 2 samples
                newOutput[i] = (1-fractionalPart)*inputData[(size_t)originalSampleSlot] + (fractionalPart)*inputData[(size_t)originalSampleSlot+1];
            }
        }

        *outputLength = newSize;
        return newOutput;
    }

    Vec2f* readDataChunk(StreamableFile& f, FMT_HEADER& formatStuff, DATA_HEADER& audioData, size_t* outputLength)
    {
        int divVal = 1 << (formatStuff.bitsPerSample-1);
        int incVal = (formatStuff.bitsPerSample/8) * (formatStuff.numChannels);

        Vec2f* data = new Vec2f[audioData.chunkID.subchunkSize / incVal];
        size_t d = 0;

        //assume that the sample rate is 44100 hz
        for(unsigned int i=0; i<audioData.chunkID.subchunkSize; i += incVal)
        {
            double leftChannel = 0;
            double rightChannel = 0;
            if(formatStuff.numChannels == 1)
            {
                if(formatStuff.bitsPerSample == 8)
                {
                    unsigned char byteData;
                    f.read(&byteData, 1);
                    leftChannel = ((double)byteData) / divVal;
                    rightChannel = leftChannel;
                }
                else if(formatStuff.bitsPerSample == 16)
                {
                    short k;
                    f.read((unsigned char*)&k, 2);
                    leftChannel = ((double)k) / divVal;
                    rightChannel = leftChannel;
                }
            }
            else
            {
                if(formatStuff.bitsPerSample == 8)
                {
                    unsigned char byteData;
                    f.read(&byteData, 1);
                    leftChannel = ((double)byteData) / divVal;
                    f.read(&byteData, 1);
                    rightChannel = ((double)byteData) / divVal;
                }
                else if(formatStuff.bitsPerSample == 16)
                {
                    short k;
                    f.read((unsigned char*)&k, 2);
                    short k2;
                    f.read((unsigned char*)&k2, 2);

                    leftChannel = ((double)k) / divVal;
                    rightChannel = ((double)k2) / divVal;
                }
            }

            data[d] = Vec2f(leftChannel, rightChannel);
            d++;
        }

        *outputLength = d;

        if(formatStuff.sampleRate != 44100)
        {
            Vec2f* newData = resampleAudio(data, *outputLength, formatStuff.sampleRate, outputLength);
            delete[] data;
            data = newData;
        }
        return data;
    }

    void Sound::loadWAV(StreamableFile& f)
    {
        //RIFF HEADER
        RIFF_HEADER rHeader;
        
        //FMT HEADER
        FMT_HEADER formatStuff;
        
        //DATA HEADER
        DATA_HEADER audioData;

        while(true)
        {
            //read chunk.
            WAV_CHUNK chunk;
            if(!readChunkHeader(f, chunk))
            {
                break;
            }
            if(chunk.subchunkID == RIFF_ID)
            {
                rHeader.chunkID = chunk;
                if(f.read((unsigned char*)&rHeader.format, sizeof(int)) != sizeof(int))
                    return;
                if(rHeader.format != WAVE_VALUE)
                    return;
            }
            else if(chunk.subchunkID == FMT_ID)
            {
                formatStuff.chunkID = chunk;
                if(chunk.subchunkSize != 16)
                    return;
                if(f.read((unsigned char*)&formatStuff.audioFormat, 16) != 16)
                    return;
                StringTools::println("%d", formatStuff.sampleRate);
            }
            else if(chunk.subchunkID == DATA_ID)
            {
                audioData.chunkID = chunk;
                data = readDataChunk(f, formatStuff, audioData, &length);
            }
            else
            {
                //dump subchunk data size
                f.read(nullptr, chunk.subchunkSize); //will dump bytes and is safe
            }
        }
    }

    void Sound::saveWAV(File file)
    {
        // SimpleFile f = SimpleFile(file, SimpleFile::WRITE | SimpleFile::ASCII);

        // if(f.isOpen())
        // {
        //     unsigned int totalSize = 4 + 4+16 + 4+length*2*2;

        //     //RIFF HEADER
        //     RIFF_HEADER rHeader;
        //     rHeader.chunkID = "RIFF";
        //     rHeader.chunkSize = totalSize;
        //     rHeader.format = "WAVE";

        //     f.writeString(rHeader.chunkID);
        //     f.writeBytes((unsigned char*)&rHeader.chunkSize, 4);
        //     f.writeString(rHeader.format);

        //     //FMT HEADER
        //     FMT_HEADER formatStuff;

        //     formatStuff.subchunkID = "fmt ";
        //     formatStuff.subchunkSize = 16;

        //     formatStuff.audioFormat = 1; //WAVE_FORMAT_PCM = 1
        //     formatStuff.numChannels = 2;
        //     formatStuff.bitsPerSample = 16;
        //     formatStuff.sampleRate = 44100;
        //     formatStuff.blockAlign = (formatStuff.bitsPerSample * formatStuff.numChannels) / 8;
        //     formatStuff.byteRate = formatStuff.blockAlign * formatStuff.sampleRate;
            
        //     f.writeString(formatStuff.subchunkID);
        //     f.writeBytes((unsigned char*)&formatStuff + sizeof(std::string), 4+formatStuff.subchunkSize);

        //     DATA_HEADER audioData;

        //     audioData.subchunkID = "data";
        //     audioData.subchunkSize = length*2*2;

        //     f.writeString(audioData.subchunkID);
        //     f.writeBytes((unsigned char*)&audioData.subchunkSize, 4);

        //     short* dataBlock = new short[length*2];
        //     int multVal = 1 << (formatStuff.bitsPerSample-1);
        //     int k = 0;

        //     for(size_t i=0; i<length; i++)
        //     {
        //         dataBlock[k] = (short)(data[i].x * multVal);
        //         dataBlock[k+1] = (short)(data[i].y * multVal);
        //         k+=2;
        //     }

        //     f.writeBytes((unsigned char*)dataBlock, audioData.subchunkSize);

        //     delete[] dataBlock;
        // }

        // f.close();
    }
}