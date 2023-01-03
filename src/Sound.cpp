#include "Sound.h"
#include "SimpleFile.h"
#include "MathExt.h"

namespace glib
{

    const float Sound::C0 = 16.35f;
    const float Sound::C0_SHARP = 17.32f;
    const float Sound::D0 = 18.35f;
    const float Sound::D0_SHARP = 19.45f;
    const float Sound::E0 = 20.60f;
    const float Sound::F0 = 21.83f;
    const float Sound::F0_SHARP = 23.12f;
    const float Sound::G0 = 24.50f;
    const float Sound::G0_SHARP = 25.96f;
    const float Sound::A0 = 27.50f;
    const float Sound::A0_SHARP = 29.14f;
    const float Sound::B0 = 30.87f;

    const float Sound::C1 = 32.7f;
    const float Sound::C1_SHARP = 34.64f;
    const float Sound::D1 = 36.71f;
    const float Sound::D1_SHARP = 38.89f;
    const float Sound::E1 = 41.20f;
    const float Sound::F1 = 43.65f;
    const float Sound::F1_SHARP = 46.25f;
    const float Sound::G1 = 49.00f;
    const float Sound::G1_SHARP = 51.91f;
    const float Sound::A1 = 55.00f;
    const float Sound::A1_SHARP = 58.27f;
    const float Sound::B1 = 61.74f;

    const float Sound::C2 = 65.41f;
    const float Sound::C2_SHARP = 69.3f;
    const float Sound::D2 = 73.42f;
    const float Sound::D2_SHARP = 77.78f;
    const float Sound::E2 = 82.41f;
    const float Sound::F2 = 87.31f;
    const float Sound::F2_SHARP = 92.50f;
    const float Sound::G2 = 98.00f;
    const float Sound::G2_SHARP = 103.83f;
    const float Sound::A2 = 110.00f;
    const float Sound::A2_SHARP = 116.54f;
    const float Sound::B2 = 123.47f;

    const float Sound::C3 = 130.81f;
    const float Sound::C3_SHARP = 138.59f;
    const float Sound::D3 = 146.83f;
    const float Sound::D3_SHARP = 155.56f;
    const float Sound::E3 = 164.81f;
    const float Sound::F3 = 174.61f;
    const float Sound::F3_SHARP = 185.00f;
    const float Sound::G3 = 196.00f;
    const float Sound::G3_SHARP = 207.65f;
    const float Sound::A3 = 220.00f;
    const float Sound::A3_SHARP = 233.08f;
    const float Sound::B3 = 246.94f;

    const float Sound::C4 = 261.63f;
    const float Sound::C4_SHARP = 277.18f;
    const float Sound::D4 = 293.66f;
    const float Sound::D4_SHARP = 311.13f;
    const float Sound::E4 = 329.63f;
    const float Sound::F4 = 349.23f;
    const float Sound::F4_SHARP = 369.99f;
    const float Sound::G4 = 392.00f;
    const float Sound::G4_SHARP = 415.30f;
    const float Sound::A4 = 440.00f;
    const float Sound::A4_SHARP = 466.16f;
    const float Sound::B4 = 493.88f;

    const float Sound::C5 = 523.25f;
    const float Sound::C5_SHARP = 554.37f;
    const float Sound::D5 = 587.33f;
    const float Sound::D5_SHARP = 622.25f;
    const float Sound::E5 = 659.25f;
    const float Sound::F5 = 698.46f;
    const float Sound::F5_SHARP = 739.99f;
    const float Sound::G5 = 783.99f;
    const float Sound::G5_SHARP = 830.61f;
    const float Sound::A5 = 880.00f;
    const float Sound::A5_SHARP = 932.33f;
    const float Sound::B5 = 987.77f;

    const float Sound::C6 = 1046.50f;
    const float Sound::C6_SHARP = 1108.73f;
    const float Sound::D6 = 1174.66f;
    const float Sound::D6_SHARP = 1244.51f;
    const float Sound::E6 = 1318.51f;
    const float Sound::F6 = 1396.91f;
    const float Sound::F6_SHARP = 1479.98f;
    const float Sound::G6 = 1567.98f;
    const float Sound::G6_SHARP = 1661.22f;
    const float Sound::A6 = 1760.00f;
    const float Sound::A6_SHARP = 1864.66f;
    const float Sound::B6 = 1975.53f;

    const float Sound::C7 = 2093.00f;
    const float Sound::C7_SHARP = 2217.46f;
    const float Sound::D7 = 2349.32f;
    const float Sound::D7_SHARP = 2489.02f;
    const float Sound::E7 = 2637.02f;
    const float Sound::F7 = 2793.83f;
    const float Sound::F7_SHARP = 2959.96f;
    const float Sound::G7 = 3135.96f;
    const float Sound::G7_SHARP = 3322.44f;
    const float Sound::A7 = 3520.00f;
    const float Sound::A7_SHARP = 3729.31f;
    const float Sound::B7 = 3951.07f;

    const float Sound::C8 = 4186.01f;
    const float Sound::C8_SHARP = 4434.92f;
    const float Sound::D8 = 4698.63f;
    const float Sound::D8_SHARP = 4978.03f;
    const float Sound::E8 = 5274.04f;
    const float Sound::F8 = 5284.65f;
    const float Sound::F8_SHARP = 5919.91f;
    const float Sound::G8 = 6271.93f;
    const float Sound::G8_SHARP = 6644.88f;
    const float Sound::A8 = 7040.00f;
    const float Sound::A8_SHARP = 7458.62f;
    const float Sound::B8 = 7902.13f;

    Sound::Sound()
    {

    }

    Sound::Sound(const Sound& other)
    {
        shallowCopy(other);
    }

    void Sound::operator=(const Sound& other)
    {
        shallowCopy(other);
    }

    Sound Sound::copy(bool shallowCopy)
    {
        Sound s = Sound();
        if(shallowCopy)
        {
            s.shallowCopy(*this);
        }
        else
        {
            s.deepCopy(*this);
        }

        return s;
    }

    void Sound::shallowCopy(const Sound& other)
    {
        playing = other.playing;
        looping = other.looping;
        volume = other.volume;

        isShallowCopy = true;
        data = other.data;
        
        priority = other.priority;
        position = other.position;
        length = other.length;

        loopStart = other.loopStart;
        loopEnd = other.loopEnd;
    }

    void Sound::deepCopy(const Sound& other)
    {
        playing = other.playing;
        looping = other.looping;
        volume = other.volume;

        isShallowCopy = false;

        data = new Vec2f[other.length];
        for(size_t i=0; i<other.length; i++)
        {
            data[i] = other.data[i];
        }
        
        priority = other.priority;
        position = other.position;
        length = other.length;

        loopStart = other.loopStart;
        loopEnd = other.loopEnd;
    }

    Sound::~Sound()
    {
        dispose();
    }

    void Sound::dispose()
    {
        //Delete the array
        if(data!=nullptr && !isShallowCopy)
            delete[] data;

        data = nullptr;
        playing = false;
        looping = false;

        volume = Vec2f(1.0, 1.0);
        isShallowCopy = false;

        priority = 0;
        position = 0;
        length = 0;

        loopStart = 0;
        loopEnd = 0;
    }

    void Sound::play()
    {
        playing = true;
    }

    bool Sound::getPlaying()
    {
        return playing;
    }

    void Sound::stop()
    {
        position = 0;
        playing = false;
    }

    void Sound::pause()
    {
        playing = false;
    }

    void Sound::setVolume(double left, double right)
    {
        volume.x = left;
        volume.y = right;
    }

    void Sound::setVolume(Vec2f v)
    {
        volume = v;
    }

    Vec2f Sound::getVolume()
    {
        return volume;
    }

    void Sound::setLoop(bool l)
    {
        looping = l;
    }

    bool Sound::getLoop()
    {
        return looping;
    }

    Vec2f* Sound::getData()
    {
        return data;
    }

    Vec2f& Sound::operator[](size_t index)
    {
        return data[index];
    }

    Vec2f Sound::get(size_t index)
    {
        return Vec2f(data[index].x * volume.x, data[index].y * volume.y);
    }

    size_t Sound::getPosition()
    {
        return position;
    }

    void Sound::setPosition(size_t index)
    {
        position = index;
    }

    void Sound::setPriority(int v)
    {
        priority = v;
    }

    int Sound::getPriority()
    {
        return priority;
    }

    size_t Sound::getLength()
    {
        return length;
    }

    size_t Sound::getLoopStart()
    {
        return loopStart;
    }

    size_t Sound::getLoopEnd()
    {
        return loopEnd;
    }

    void Sound::setLoopStart(size_t index)
    {
        loopStart = MathExt::clamp<size_t>(index, 0ull, length);
    }

    void Sound::setLoopStart(double time)
    {
        //time in seconds
        size_t actualIndex = (size_t)(time * 44100);
        loopStart = MathExt::clamp<size_t>(actualIndex, 0ull, length);
    }

    void Sound::setLoopEnd(size_t index)
    {
        loopEnd = MathExt::clamp<size_t>(index, 0ull, length);
    }

    void Sound::setLoopEnd(double time)
    {
        //time in seconds
        size_t actualIndex = (size_t)(time * 44100);
        loopEnd = MathExt::clamp<size_t>(actualIndex, 0ull, length);
    }

    void Sound::setData(Vec2f* data, size_t size, bool isShallowCopy)
    {
        dispose();
        this->data = data;
        this->length = size;
        this->isShallowCopy = isShallowCopy;
    }

    void Sound::copyData(Vec2f* data, size_t size)
    {
        dispose();
        this->data = new Vec2f[size];
        for(size_t i=0; i<size; i++)
        {
            this->data[i] = data[i];
        }
        this->length = size;
        isShallowCopy = false;
    }

    void Sound::loadSound(File file)
    {
        //load uncompressed .WAV
        dispose();

        SimpleFile f = SimpleFile(file, SimpleFile::READ);
        std::vector<unsigned char> bytes = f.readFullFileAsBytes();
        f.close();

        std::string ext = file.getExtension();

        if(ext == ".wav")
        {
            loadWAV(bytes);
        }
        else
        {
            //can't load
        }
    }

    void Sound::loadWAV(std::vector<unsigned char> bytes)
    {
        int index = 0;

        //Structures that could potentially help
        struct RIFF_HEADER
        {
            std::string chunkID;
            int chunkSize;
            std::string format;
        };

        struct FMT_HEADER
        {
            std::string subchunkID;
            int subchunkSize;
            short audioFormat;
            short numChannels;
            int sampleRate;
            int byteRate;
            short blockAlign;
            short bitsPerSample;
        };

        struct DATA_HEADER
        {
            std::string subchunkID;
            int subchunkSize;
            std::vector<unsigned char> data;
        };

        //RIFF HEADER
        RIFF_HEADER rHeader;
        rHeader.chunkID = bytes[index];
            rHeader.chunkID += bytes[index+1];
                rHeader.chunkID += bytes[index+2];
                    rHeader.chunkID += bytes[index+3];
        index += 4;

        rHeader.chunkSize = ((int)bytes[index]) + ((int)bytes[index+1]<<8) + ((int)bytes[index+2]<<16) + ((int)bytes[index+3]<<24);
        index += 4;

        rHeader.format = bytes[index];
            rHeader.format += bytes[index+1];
                rHeader.format += bytes[index+2];
                    rHeader.format += bytes[index+3];
        index += 4;

        if(rHeader.chunkID != "RIFF")
        {
            return;
        }
        if(rHeader.format != "WAVE")
        {
            return;
        }

        //FMT HEADER
        FMT_HEADER formatStuff;

        formatStuff.subchunkID = bytes[index];
            formatStuff.subchunkID += bytes[index+1];
                formatStuff.subchunkID += bytes[index+2];
                    formatStuff.subchunkID += bytes[index+3];
        index += 4;

        formatStuff.subchunkSize = ((int)bytes[index]) + ((int)bytes[index+1]<<8) + ((int)bytes[index+2]<<16) + ((int)bytes[index+3]<<24);
        index += 4;

        if(formatStuff.subchunkID != "fmt ")
        {
            return;
        }

        formatStuff.audioFormat = ((int)bytes[index]) + ((int)bytes[index+1]<<8);
        index+=2;
        formatStuff.numChannels = ((int)bytes[index]) + ((int)bytes[index+1]<<8);
        index+=2;
        formatStuff.sampleRate = ((int)bytes[index]) + ((int)bytes[index+1]<<8) + ((int)bytes[index+2]<<16) + ((int)bytes[index+3]<<24);
        index+=4;
        formatStuff.byteRate = ((int)bytes[index]) + ((int)bytes[index+1]<<8) + ((int)bytes[index+2]<<16) + ((int)bytes[index+3]<<24);
        index+=4;
        formatStuff.blockAlign = ((int)bytes[index]) + ((int)bytes[index+1]<<8);
        index+=2;
        formatStuff.bitsPerSample = ((int)bytes[index]) + ((int)bytes[index+1]<<8);
        index+=2;

        DATA_HEADER audioData;

        audioData.subchunkID = bytes[index];
            audioData.subchunkID += bytes[index+1];
                audioData.subchunkID += bytes[index+2];
                    audioData.subchunkID += bytes[index+3];
        index += 4;

        audioData.subchunkSize = ((int)bytes[index]) + ((int)bytes[index+1]<<8) + ((int)bytes[index+2]<<16) + ((int)bytes[index+3]<<24);
        index += 4;

        if(audioData.subchunkID != "data")
        {
            return;
        }

        int divVal = 1 << (formatStuff.bitsPerSample-1);
        int incVal = (formatStuff.bitsPerSample/8) * (formatStuff.numChannels);

        data = new Vec2f[audioData.subchunkSize / incVal];
        int d = 0;

        //assume that the sample rate is 44100 hz
        for(int i=0; i<audioData.subchunkSize; i += incVal)
        {
            double leftChannel = 0;
            double rightChannel = 0;
            if(formatStuff.numChannels == 1)
            {
                if(formatStuff.bitsPerSample == 8)
                {
                    leftChannel = ((double)bytes[index+i]) / divVal;
                    rightChannel = leftChannel;
                }
                else if(formatStuff.bitsPerSample == 16)
                {
                    short k = ((short)bytes[index+i] + ((short)bytes[index+i+1]<<8));

                    leftChannel = ((double)k) / divVal;
                    rightChannel = leftChannel;
                }
            }
            else
            {
                if(formatStuff.bitsPerSample == 8)
                {
                    leftChannel = ((double)bytes[index+i]) / divVal;
                    rightChannel = ((double)bytes[index+i+1]) / divVal;
                }
                else if(formatStuff.bitsPerSample == 16)
                {
                    short k = ((short)bytes[index+i] + ((short)bytes[index+i+1]<<8));
                    short k2 = ((short)bytes[index+i+2] + ((short)bytes[index+i+3]<<8));

                    leftChannel = ((double)k) / divVal;
                    rightChannel = ((double)k2) / divVal;
                }
            }

            data[d] = Vec2f(leftChannel, rightChannel);
            d++;
        }

        length = d;
    }

    void Sound::saveWAV(File file)
    {
        //Structures that could potentially help
        struct RIFF_HEADER
        {
            std::string chunkID;
            int chunkSize;
            std::string format;
        };

        struct FMT_HEADER
        {
            std::string subchunkID;
            int subchunkSize;
            short audioFormat;
            short numChannels;
            int sampleRate;
            int byteRate;
            short blockAlign;
            short bitsPerSample;
        };

        struct DATA_HEADER
        {
            std::string subchunkID;
            int subchunkSize;
            std::vector<unsigned char> data;
        };

        SimpleFile f = SimpleFile(file, SimpleFile::WRITE | SimpleFile::ASCII);

        if(f.isOpen())
        {
            unsigned int totalSize = 4 + 4+16 + 4+length*2*2;

            //RIFF HEADER
            RIFF_HEADER rHeader;
            rHeader.chunkID = "RIFF";
            rHeader.chunkSize = totalSize;
            rHeader.format = "WAVE";

            f.writeString(rHeader.chunkID);
            f.writeBytes((unsigned char*)&rHeader.chunkSize, 4);
            f.writeString(rHeader.format);

            //FMT HEADER
            FMT_HEADER formatStuff;

            formatStuff.subchunkID = "fmt ";
            formatStuff.subchunkSize = 16;

            formatStuff.audioFormat = 1; //WAVE_FORMAT_PCM = 1
            formatStuff.numChannels = 2;
            formatStuff.bitsPerSample = 16;
            formatStuff.sampleRate = 44100;
            formatStuff.blockAlign = (formatStuff.bitsPerSample * formatStuff.numChannels) / 8;
            formatStuff.byteRate = formatStuff.blockAlign * formatStuff.sampleRate;
            
            f.writeString(formatStuff.subchunkID);
            f.writeBytes((unsigned char*)&formatStuff + sizeof(std::string), 4+formatStuff.subchunkSize);

            DATA_HEADER audioData;

            audioData.subchunkID = "data";
            audioData.subchunkSize = length*2*2;

            f.writeString(audioData.subchunkID);
            f.writeBytes((unsigned char*)&audioData.subchunkSize, 4);

            short* dataBlock = new short[length*2];
            int multVal = 1 << (formatStuff.bitsPerSample-1);
            int k = 0;

            for(size_t i=0; i<length; i++)
            {
                dataBlock[k] = (short)(data[i].x * multVal);
                dataBlock[k+1] = (short)(data[i].y * multVal);
                k+=2;
            }

            f.writeBytes((unsigned char*)dataBlock, audioData.subchunkSize);

            delete[] dataBlock;
        }

        f.close();
    }

} //NAMESPACE glib END