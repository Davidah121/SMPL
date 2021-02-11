#pragma once
#include <Windows.h>
#include <vector>
#include "Sound.h"
#include <thread>
#include <mutex>

#pragma comment(lib,"winmm.lib")

struct AudioSegment
{
    void* audioStuff;
    bool used;
};

class Audio
{
public:
	Audio();
	~Audio();

	static void init();
	static void dispose();

	static void setVolume(double v);
	static double getVolume();

	static void pauseAll();
	static void stopAll();

	static void setAmountOfSimultaneousSounds(int s);
	static int getAmountOfSimultaneousSounds();

	static void addSound(Sound* s);
	static void removeSound(Sound* s);

	static void setBuffers(int b);
	static int getBuffers();
	static void setSizeOfBuffer(int b);
	static int getSizeOfBuffer();

private:
	static int amtOfSoundsAllowed;
	static double volume;
	static std::thread audioThread;
	static std::mutex audioMutex;

	static bool running;
	static bool getRunning();
	static void setRunning(bool b);

	static int amtBuffers;
	static int bufferSize;

	static std::vector<Sound*> sounds;
	static std::vector<Sound*> copySounds;
	static std::vector<AudioSegment> buffers;

	static int currentBuf;
	static bool hasInit;

	static void submitData(short* buffer, int b);
    static void prepareData();
	static void unprepareData(int b);

	static void prepareSounds();

	static HWAVEOUT waveOutHandle;
	static WAVEFORMATEX format;
	static void CALLBACK audioCallBack(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
};

