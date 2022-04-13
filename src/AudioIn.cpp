#include "Audio.h"
#include "StringTools.h"
#include "System.h"

namespace glib
{

    #ifdef LINUX

	#else
        HWAVEIN AudioIn::waveInHandle;
        WAVEFORMATEX AudioIn::format;
    #endif

    unsigned int AudioIn::deviceID = -1;
    std::vector<Vec2f> AudioIn::soundData;
    int AudioIn::bufferSize = 2205*2;

    int AudioIn::amtBuffers = 1;
    bool AudioIn::hasInit = false;
    bool AudioIn::running = false;
    std::thread AudioIn::audioThread;
    std::mutex AudioIn::audioMutex;

    std::atomic_bool AudioIn::bufferDone = false;
    std::atomic_bool AudioIn::recording = false;

    void AudioIn::init(unsigned int device)
    {
        if(!hasInit)
        {
            #ifdef LINUX

            #else
                hasInit = true;

                deviceID = device;

                ZeroMemory(&format, sizeof(WAVEFORMATEX));
                ZeroMemory(&waveInHandle, sizeof(HWAVEIN));
                
                format.cbSize = 0; //Extra data size
                format.wFormatTag = WAVE_FORMAT_PCM;
                format.wBitsPerSample = 16; //Bits per sample
                format.nSamplesPerSec = 44100; //standard
                format.nChannels = 2; //Stereo = 2, Mono = 1
                format.nBlockAlign = (format.wBitsPerSample * format.nChannels) / 8;//
                format.nAvgBytesPerSec = format.nBlockAlign * format.nSamplesPerSec;//

                MMRESULT result = waveInOpen(&waveInHandle, deviceID, &format, (DWORD_PTR)AudioIn::audioInCallBack, NULL, CALLBACK_FUNCTION);

                if(result == MMSYSERR_NOERROR)
                {
                    //good
                    setRunning(true);
                    audioThread = std::thread(threadFunc);
                }
                else
                {
                    dispose();
                }
            #endif
        }
    }

    void AudioIn::dispose()
    {
        setRunning(false);
        stop();
        
        StringTools::println("AFTER STOP");
        if(audioThread.joinable())
            audioThread.join();

        StringTools::println("AFTER JOIN");

        #ifdef LINUX

		#else
            waveInClose(waveInHandle);
        #endif

        StringTools::println("AFTER CLOSE");

        soundData.clear();
        hasInit = false;
    }

    void AudioIn::record()
    {
        #ifdef LINUX

		#else
            waveInStart(waveInHandle);
        #endif
        recording = true;
    }

    bool AudioIn::getRecording()
    {
        return recording;
    }

    void AudioIn::stop()
    {
        #ifdef LINUX

		#else
            waveInStop(waveInHandle);
        #endif
        recording = false;
    }

    #ifdef LINUX

	#else
        void CALLBACK AudioIn::audioInCallBack(HWAVEIN hWaveIn, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
        {
            if(uMsg == WIM_DATA)
            {
                bufferDone = true;
            }
        }
    #endif

    void AudioIn::addAudioData(short* data, int size)
    {
        audioMutex.lock();
        for(int i=0; i<size; i+=2)
        {
            Vec2f soundPoint = Vec2f();
            soundPoint.x = ((double)data[i]) / 32768;
            soundPoint.y = ((double)data[i+1]) / 32768;
            
            soundData.push_back(soundPoint);
        }
        audioMutex.unlock();
    }

    void AudioIn::clearAudioData()
    {
        audioMutex.lock();
        soundData.clear();
        audioMutex.unlock();
    }

    void AudioIn::threadFunc()
    {
        #ifdef LINUX

		#else
            short* buffer = new short[bufferSize];

            WAVEHDR hdr;
            ZeroMemory(&hdr, sizeof(WAVEHDR));
            
            hdr.dwBufferLength = bufferSize*2;
            hdr.lpData = (LPSTR)buffer;
            hdr.dwFlags = 0;

            while(getRunning())
            {
                if(recording)
                {
                    MMRESULT result = waveInPrepareHeader(waveInHandle, &hdr, sizeof(WAVEHDR));
                    
                    if(result != MMSYSERR_NOERROR)
                    {
                        //error has occured
                        StringTools::println("ERROR WAVE_IN_PREPARE %d", result);
                        break;
                    }

                    bufferDone = false;

                    result = waveInAddBuffer(waveInHandle, &hdr, sizeof(WAVEHDR));
                    if(result != MMSYSERR_NOERROR)
                    {
                        //error has occured
                        waveInUnprepareHeader(waveInHandle, &hdr, sizeof(WAVEHDR));
                        StringTools::println("ERROR WAVE_IN_ADD %d", result);
                        break;
                    }
                    
                    while(bufferDone != true)
                    {
                        System::sleep(0,1);
                    }
                    
                    result = waveInUnprepareHeader(waveInHandle, &hdr, sizeof(WAVEHDR));
                    if(result != MMSYSERR_NOERROR)
                    {
                        //error has occured
                        StringTools::println("ERROR WAVE_IN_UN_PREPARE %d", result);
                        break;
                    }

                    addAudioData(buffer, hdr.dwBytesRecorded/2);

                    ZeroMemory(&hdr, sizeof(WAVEHDR));
                    memset(buffer, 0, bufferSize*2);
                    
                    hdr.dwBufferLength = bufferSize*2;
                    hdr.lpData = (LPSTR)buffer;
                }
                else
                {
                    System::sleep(0,1);
                }
            }

            delete[] buffer;
        #endif
    }

    std::vector<Vec2f> AudioIn::getAudioData()
    {
        std::vector<Vec2f> data;

        audioMutex.lock();
        data = soundData;
        audioMutex.unlock();

        return data;
    }

    void AudioIn::setBuffers(int b)
    {
        if(!hasInit)
            amtBuffers = b;
    }

    int AudioIn::getBuffers()
    {
        return amtBuffers;
    }

    void AudioIn::setSizeOfBuffer(int b)
    {
        if(!hasInit)
            bufferSize = b;
    }

    int AudioIn::getSizeOfBuffer()
    {
        return bufferSize;
    }

    void AudioIn::setRunning(bool b)
    {
        audioMutex.lock();
        running = b;
        audioMutex.unlock();
    }

    bool AudioIn::getRunning()
    {
        bool k;
        audioMutex.lock();
        k = running;
        audioMutex.unlock();
        return k;
    }

} //NAMESPACE glib END