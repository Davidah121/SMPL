#pragma once
#include <Windows.h>
#include <vector>
#include "Sound.h"
#include <thread>
#include <mutex>
#include <atomic>

#pragma comment(lib,"winmm.lib")

struct AudioSegment
{
    void* audioStuff;
    bool used;
};

class AudioOut
{
public:

	struct AudioOutError : public std::exception
	{
		const char* what() noexcept { return "Exception occured while submitting buffers."; }
	};

	struct AudioOutInitError : public std::exception
	{
		const char* what() noexcept { return "Exception occured while initializing AudioOut."; }
	};

	/**
	 * @brief 
	 * 		Opens a device specified by the deviceID value to output audio too.
	 * 		Spawns a thread to process that audio and output audio.
	 * 		Can not be recalled until dispose() is called and successfully returned.
	 * @param deviceID 
	 * 		The deviceID should be obtained from the operating system.
	 * 		The default value is -1 which will map to the default output device.
	 * @throws AudioOutInitError
	 * 		Error is thrown if the output device could not be initialized
	 */
	static void init(unsigned int deviceID = -1);

	/**
	 * @brief 
	 * 		Ends output of audio.
	 * 		Closes the audio thread that processes and outputs audio.
	 */
	static void dispose();

	/**
	 * @brief 
	 * 		Sets the Global Volume which will be applied to every sound that is
	 * 		output.
	 * @param v 
	 * 		The volume value that must be between 0.0 and 1.0.
	 * 		It will be clamped between 0.0 and 1.0
	 */
	static void setVolume(double v);

	/**
	 * @brief
	 * 		Returns the global volume value.
	 * @return double 
	 */
	static double getVolume();

	/**
	 * @brief 
	 * 		Pauses all sounds.
	 */
	static void pauseAll();

	/**
	 * @brief 
	 * 		Stops all sounds.
	 */
	static void stopAll();

	/**
	 * @brief
	 * 		Set the amount of simultaneous sounds allowed to be played.
	 * 		The default value is set to 16.
	 * 		Note that sounds are sorted by their priority with insertion sort.
	 * @param s 
	 * 		A positive value above 0.
	 */
	static void setAmountOfSimultaneousSounds(int s);

	/**
	 * @brief 
	 * 		Returns the amount of simultaneous sounds allowed
	 * @return int 
	 */
	static int getAmountOfSimultaneousSounds();

	/**
	 * @brief 
	 * 		Adds a sound to be played.
	 * 		The sound is referenced by a pointer.
	 * 		Note that sounds are sorted by their priority with insertion sort.
	 * @param s 
	 * 		A valid Sound pointer must be supplied.
	 * 		A crash will occur if an invalid pointer is supplied.
	 */
	static void addSound(Sound* s);

	/**
	 * @brief 
	 * 		Removes a sound from the playable sounds.
	 * 		A sound should be removed before it is deleted to avoid
	 * 		memory access violations.
	 * 		Note that all sounds with the same pointer will be removed.
	 * @param s 
	 * 		A valid Sound pointer that should be removed.
	 */
	static void removeSound(Sound* s);

	/**
	 * @brief
	 * 		Set the amount of buffers for playing audio.
	 * 		The default is 2 buffers.
	 * 		Must be set before calling init.
	 * @param b 
	 * 		The number of buffers allows for decreases in the size of the buffers
	 * 		and keeps the OS fed with data to reduce studdering.
	 * 		The current value is set to reduce studdering and keep latency good.
	 */
	static void setBuffers(int b);

	/**
	 * @brief
	 * 		Return the amount of buffers 
	 * @return int 
	 */
	static int getBuffers();

	/**
	 * @brief
	 * 		Sets the size of the individual buffers.
	 * 		The default is 2205 samples.
	 * 		Must be set before calling init.
	 * @param b 
	 * 		The size of the buffers in samples.
	 * 		The larger the size, the more milliseconds of audio that must be
	 * 		prepared at a time.
	 * 		The value 2205 was selected based on 2 buffers, stereo audio,
	 * 		16 bit samples, and 44100 samples per second.
	 * 		This results in 50 milliseconds of audio latency with no studdering.
	 */
	static void setSizeOfBuffer(int b);

	/**
	 * @brief
	 * 		Returns the number of samples for each buffer.
	 * @return int 
	 */
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

	static unsigned int deviceID;

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
	static void CALLBACK audioOutCallBack(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
};

class AudioIn
{
public:
	struct AudioInError : public std::exception
	{
		const char* what() noexcept { return "Exception occured while retrieving buffers."; }
	};

	struct AudioInInitError : public std::exception
	{
		const char* what() noexcept { return "Exception occured while initializing AudioIn."; }
	};

	/**
	 * @brief 
	 * 		Opens a device specified by the deviceID value to get audio input from.
	 * 		Spawns a thread to retrieve the audio.
	 * 		Can not be recalled until dispose() is called and successfully returned.
	 * @param deviceID 
	 * 		The deviceID should be obtained from the operating system.
	 * 		The default value is -1 which will map to the default input device.
	 * @throws AudioInInitError
	 * 		Error is thrown if the input device could not be initialized
	 */
	static void init(unsigned int device = -1);

	/**
	 * @brief 
	 * 		Ends input of audio.
	 * 		Closes the audio thread that retrieves input audio.
	 */
	static void dispose();

	/**
	 * @brief 
	 * 		Starts recording of audio from the audio device.
	 */
	static void record();

	/**
	 * @brief
	 * 		Returns whether or not the class is recording audio from a device.
	 * @return true 
	 * @return false 
	 */
	static bool getRecording();

	/**
	 * @brief 
	 * 		Stops recording of audio from the audio device.
	 */
	static void stop();

	/**
	 * @brief
	 * 		Gets the current recorded data.
	 * 		Does not clear the recorded data. That must be done with
	 * 		clearAudioData()
	 * @return std::vector<Vec2f> 
	 * 		A vector that stores the recorded audio in stereo.
	 */
	static std::vector<Vec2f> getAudioData();

	/**
	 * @brief 
	 * 		Clears the current recorded audio.
	 * 		This should be called to remove audio that you no longer need.
	 */
	static void clearAudioData();

	/**
	 * @brief
	 * 		Set the amount of buffers for playing audio.
	 * 		The default is 2 buffers.
	 * 		Must be set before calling init.
	 * @param b 
	 * 		The number of buffers allows for decreases in the size of the buffers
	 * 		and keeps the OS fed with data to reduce studdering.
	 * 		The current value is set to reduce studdering and keep latency good.
	 */
	static void setBuffers(int b);

	/**
	 * @brief
	 * 		Return the amount of buffers 
	 * @return int 
	 */
	static int getBuffers();

	/**
	 * @brief
	 * 		Sets the size of the individual buffers.
	 * 		The default is 2205 samples.
	 * 		Must be set before calling init.
	 * @param b 
	 * 		The size of the buffers in samples.
	 * 		The larger the size, the more milliseconds of audio captured at a time.
	 */
	static void setSizeOfBuffer(int b);

	/**
	 * @brief
	 * 		Returns the number of samples for each buffer.
	 * @return int 
	 */
	static int getSizeOfBuffer();

private:
	static unsigned int deviceID;
	static std::vector<Vec2f> soundData;
	static int amtBuffers;
	static int bufferSize;
	static bool hasInit;

	static bool running;
	static void setRunning(bool b);
	static bool getRunning();

	static std::atomic_bool recording;
	static std::atomic_bool bufferDone;

	static void addAudioData(short* data, int size);

	static void threadFunc();

	static std::thread audioThread;
	static std::mutex audioMutex;

	static HWAVEIN waveInHandle;
	static WAVEFORMATEX format;
	static void CALLBACK audioInCallBack(HWAVEIN hWaveIn, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
};