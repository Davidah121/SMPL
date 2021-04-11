#include "Audio.h"
#include "StringTools.h"
#include "MathExt.h"
#include "System.h"
#include "Sort.h"

HWAVEOUT Audio::waveOutHandle;
WAVEFORMATEX Audio::format;

int Audio::amtOfSoundsAllowed = 16;
double Audio::volume = 1.0;
std::thread Audio::audioThread;
std::mutex Audio::audioMutex;

int Audio::currentBuf = 0;
bool Audio::running = false;
bool Audio::hasInit = false;

int Audio::amtBuffers = 2;
int Audio::bufferSize = 2205;

std::vector<Sound*> Audio::sounds;
std::vector<Sound*> Audio::copySounds;
std::vector<AudioSegment> Audio::buffers;

Audio::Audio()
{
}


Audio::~Audio()
{
}

void Audio::init()
{
	hasInit = true;

	format.cbSize = 0; //Extra data size
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.wBitsPerSample = 16; //Bits per sample
	format.nSamplesPerSec = 44100; //standard
	format.nChannels = 2; //Stereo = 2, Mono = 1
	format.nBlockAlign = (format.wBitsPerSample * format.nChannels) / 8;//
	format.nAvgBytesPerSec = format.nBlockAlign * format.nSamplesPerSec;//

	for(int i=0; i<amtBuffers; i++)
	{
		buffers.push_back({nullptr, true});
	}

	MMRESULT result = waveOutOpen(&waveOutHandle, WAVE_MAPPER, &format, (DWORD_PTR)Audio::audioCallBack, NULL, CALLBACK_FUNCTION);

	if(result == MMSYSERR_NOERROR)
	{
		running = true;
		Audio::audioThread = std::thread(Audio::prepareData);
	}
}

void Audio::dispose()
{
	//waveOutRestart(waveOutHandle);
	setRunning(false);
	for(int k=0; k<buffers.size(); k++)
	{
		unprepareData(k);
	}
	
	if(audioThread.joinable())
	{
		audioThread.join();
	}

	waveOutClose(waveOutHandle);
	hasInit = false;
}

void Audio::submitData(short* buffer, int b)
{
	if(getRunning())
	{
		WAVEHDR* whdr = new WAVEHDR();
		ZeroMemory(whdr, sizeof(WAVEHDR));
		whdr->dwBufferLength = bufferSize*2*2;
		whdr->lpData = (LPSTR)buffer;

		buffers[b].audioStuff = whdr;

		waveOutPrepareHeader(waveOutHandle, whdr, sizeof(WAVEHDR));
		MMRESULT wResult = waveOutWrite(waveOutHandle, whdr, sizeof(WAVEHDR));
		if(wResult != 0)
		{
			//error has occured
			dispose();
			StringTools::println("AUDIO ERROR: ", (int) wResult);
		}
	}
}

void Audio::prepareData()
{
	//gathers up x seconds of audio from all of the audio sources playing
	//sudo code
	int b=0;
	int totalLength = bufferSize;
	int totalBuffers = amtBuffers;

	while(true)
	{
		Vec2f* audioData = new Vec2f[totalLength];
		prepareSounds();

		for(Sound* s : copySounds)
		{
			int index = s->getPosition();

			int end = s->getLength();
			if(s->getLoop())
			{
				end = s->getLoopEnd();
			}

			if(index >= end)
			{
				s->stop();
				continue;
			}

			for(int i=0; i<totalLength; i++)
			{
				Vec2f audio = s->get(index); //left = x, right = y
				audioData[i] += audio;

				index++;
				if(index >= end)
				{
					if(s->getLoop())
					{
						index = s->getLoopStart();
					}
					else
					{
						break;
					}
				}
			}

			s->setPosition(index);
		}

		//clamp down to 1.0 and convert to short*
		short* finalData = new short[totalLength*2];
		ZeroMemory(finalData, sizeof(short)*totalLength*2);
		int k = 0;
		for(int i=0; i<totalLength; i++)
		{
			audioData[i].x = MathExt::clamp(audioData[i].x*volume, -1.0, 1.0);
			audioData[i].y = MathExt::clamp(audioData[i].y*volume, -1.0, 1.0);

			finalData[k] = (short)(audioData[i].x * 0x7FFF);
			finalData[k+1] = (short)(audioData[i].y * 0x7FFF);
			
			k+=2;
		}

		while(buffers[b].used==false)
		{
			System::sleep(0,1);
		}

		if(!getRunning())
		{
			delete[] finalData;
			delete[] audioData;
			break;
		}

		buffers[b].used = false;

		submitData(finalData, b);
		b = (b+1) % totalBuffers;

		delete[] audioData;
	}
}

void Audio::unprepareData(int b)
{
	WAVEHDR* a = (WAVEHDR*)buffers[b].audioStuff;
	
	if(a!=nullptr)
	{
		waveOutUnprepareHeader(waveOutHandle, a, sizeof(WAVEHDR));
		delete a->lpData;
		delete a;
	}

	buffers[b].audioStuff = nullptr;
	buffers[b].used = true;
}

void CALLBACK Audio::audioCallBack(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	if(uMsg == WOM_DONE)
	{
		unprepareData(currentBuf);
		currentBuf = (currentBuf+1) % amtBuffers;
	}
}

void Audio::setVolume(double v)
{
	volume = v;
}

double Audio::getVolume()
{
	return volume;
}

void Audio::pauseAll()
{
	for(Sound* s : sounds)
	{
		s->pause();
	}
}

void Audio::stopAll()
{
	for(Sound* s : sounds)
	{
		s->stop();
	}
}

void Audio::setAmountOfSimultaneousSounds(int s)
{
	amtOfSoundsAllowed = s;
}

int Audio::getAmountOfSimultaneousSounds()
{
	return amtOfSoundsAllowed;
}

void Audio::addSound(Sound* s)
{
	bool canAdd = true;
	for(Sound* m : sounds)
	{
		if(s == m)
		{
			canAdd = false;
			break;
		}
	}

	if(canAdd)
	{
		sounds.push_back(s);

		Sort::insertionSort<Sound*>(copySounds.data(), copySounds.size(), [](Sound* a, Sound* b) -> bool{
			return a->getPriority() > b->getPriority();
		});
	}
}

void Audio::removeSound(Sound* s)
{
	int index = -1;
	for(int i=0; i<sounds.size(); i++)
	{
		if(s == sounds[i])
		{
			index = i;
			break;
		}
	}

	if(index >= 0)
	{
		for(int i=index; i<sounds.size()-1; i++)
		{
			sounds[i] = sounds[i+1];
		}
		sounds.pop_back();
	}
}

void Audio::prepareSounds()
{
	copySounds.clear();
	int maxSize = MathExt::min(amtOfSoundsAllowed, (int)sounds.size());
	int i=0;
	for(Sound* s : sounds)
	{
		if(s->getPlaying())
		{
			copySounds.push_back(s);
			i++;
			if(i>=maxSize)
			{
				break;
			}
		}
	}
	
}

void Audio::setBuffers(int b)
{
	if(!hasInit)
		amtBuffers = b;
}

int Audio::getBuffers()
{
	return amtBuffers;
}

void Audio::setSizeOfBuffer(int b)
{
	if(!hasInit)
		bufferSize = b;
}

int Audio::getSizeOfBuffer()
{
	return bufferSize;
}

void Audio::setRunning(bool b)
{
	audioMutex.lock();
	running = b;
	audioMutex.unlock();
}

bool Audio::getRunning()
{
	bool k;
	audioMutex.lock();
	k = running;
	audioMutex.unlock();
	return k;
}