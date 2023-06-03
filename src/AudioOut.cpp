#ifndef NO_AUDIO

	#include "Audio.h"
	#include "StringTools.h"
	#include "MathExt.h"
	#include "System.h"
	#include "Sort.h"
	#include "Input.h"

	namespace glib
	{

		#ifdef __unix__

		#else
			HWAVEOUT AudioOut::waveOutHandle;
			WAVEFORMATEX AudioOut::format;
		#endif

		int AudioOut::amtOfSoundsAllowed = 16;
		double AudioOut::volume = 1.0;
		std::thread AudioOut::audioThread;
		std::mutex AudioOut::audioMutex;

		int AudioOut::currentBuf = 0;
		bool AudioOut::running = false;
		bool AudioOut::hasInit = false;

		int AudioOut::amtBuffers = 2;
		int AudioOut::bufferSize = 2205;

		unsigned int AudioOut::deviceID = -1;

		std::vector<Sound*> AudioOut::sounds;
		std::vector<Sound*> AudioOut::copySounds;
		std::vector<AudioSegment> AudioOut::buffers;

		void AudioOut::init(unsigned int deviceID)
		{
			if(!hasInit)
			{
				#ifdef __unix__

				#else
					hasInit = true;

					memset(&format, 0, sizeof(WAVEFORMATEX));

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

					MMRESULT result = waveOutOpen(&waveOutHandle, deviceID, &format, (DWORD_PTR)AudioOut::audioOutCallBack, NULL, CALLBACK_FUNCTION);

					AudioOut::deviceID = deviceID;
					if(result == MMSYSERR_NOERROR)
					{
						running = true;
						AudioOut::audioThread = std::thread(AudioOut::prepareData);
					}
					else
					{
						dispose();
						
						#ifdef USE_EXCEPTIONS
						throw AudioOutInitError();
						#endif
					}
				#endif
			}
		}

		void AudioOut::dispose()
		{
			//waveOutRestart(waveOutHandle);
			stopAll();
			setRunning(false);

			if(audioThread.joinable())
			{
				audioThread.join();
			}

			for(size_t k=0; k<buffers.size(); k++)
			{
				unprepareData((int)k);
			}
			
			#ifdef __unix__

			#else
				waveOutClose(waveOutHandle);
			#endif

			hasInit = false;
		}

		void AudioOut::submitData(short* buffer, int b)
		{
			if(getRunning())
			{
				#ifdef __unix__

				#else
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
						// dispose();
						// StringTools::println("AUDIO ERROR: ", (int) wResult);
						#ifdef USE_EXCEPTIONS
						throw AudioOutError();
						#else
						throw -1;
						#endif
					}
				#endif
			}
		}

		void AudioOut::prepareData()
		{
			//gathers up x seconds of audio from all of the audio sources playing
			//sudo code
			int b=0;
			int totalLength = bufferSize;
			int totalBuffers = amtBuffers;

			while(true)
			{
				prepareSounds();

				if(copySounds.size()>0)
				{
					Vec2f* audioData = new Vec2f[totalLength];
					
					for(Sound* s : copySounds)
					{
						size_t index = s->getPosition();

						size_t end = s->getLength();
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
					memset(finalData, 0, sizeof(short)*totalLength*2);
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

					try
					{
						submitData(finalData, b);
						b = (b+1) % totalBuffers;
					}
					catch(AudioOutError& e)
					{
						StringTools::println(e.what());
						delete[] audioData;
						break;
					}
					catch(...)
					{
						//do nothing.
						StringTools::println("EXCEPTION THROWN");
						delete[] audioData;
						break;
					}

					delete[] audioData;
				}
				else
				{
					System::sleep(0,1);
				}
			}
		}

		void AudioOut::unprepareData(int b)
		{
			#ifdef __unix__

			#else
				WAVEHDR* a = (WAVEHDR*)buffers[b].audioStuff;
				
				if(a!=nullptr)
				{
					waveOutUnprepareHeader(waveOutHandle, a, sizeof(WAVEHDR));
					delete a->lpData;
					delete a;
				}
			#endif

			buffers[b].audioStuff = nullptr;
			buffers[b].used = true;
		}

		#ifdef __unix__

		#else
			void CALLBACK AudioOut::audioOutCallBack(HWAVEOUT hWaveOut, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
			{
				if(uMsg == WOM_DONE)
				{
					unprepareData(currentBuf);
					currentBuf = (currentBuf+1) % amtBuffers;
				}
			}
		#endif

		void AudioOut::setVolume(double v)
		{
			volume = v;
		}

		double AudioOut::getVolume()
		{
			return volume;
		}

		void AudioOut::pauseAll()
		{
			audioMutex.lock();
			for(Sound* s : sounds)
			{
				s->pause();
			}
			audioMutex.unlock();
		}

		void AudioOut::stopAll()
		{
			audioMutex.lock();
			for(Sound* s : sounds)
			{
				s->stop();
			}
			audioMutex.unlock();
		}

		void AudioOut::setAmountOfSimultaneousSounds(int s)
		{
			amtOfSoundsAllowed = s;
		}

		int AudioOut::getAmountOfSimultaneousSounds()
		{
			return amtOfSoundsAllowed;
		}

		void AudioOut::addSound(Sound* s)
		{
			audioMutex.lock();
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
			audioMutex.unlock();
		}

		void AudioOut::removeSound(Sound* s)
		{
			audioMutex.lock();
			int index = -1;
			for(size_t i=0; i<sounds.size(); i++)
			{
				if(s == sounds[i])
				{
					index = (int)i;
					break;
				}
			}

			if(index >= 0)
			{
				for(size_t i=(size_t)index; i<sounds.size()-1; i++)
				{
					sounds[i] = sounds[i+1];
				}
				sounds.pop_back();
			}
			audioMutex.unlock();
		}

		void AudioOut::prepareSounds()
		{
			copySounds.clear();

			audioMutex.lock();
			size_t maxSize = MathExt::min((size_t)amtOfSoundsAllowed, sounds.size());
			for(size_t i=0; i<sounds.size(); i++)
			{
				if(sounds[i]->getPlaying())
				{
					copySounds.push_back(sounds[i]);
				}
				if(i >= maxSize)
				{
					break;
				}
			}
			audioMutex.unlock();
		}

		void AudioOut::setBuffers(int b)
		{
			if(!hasInit)
				amtBuffers = b;
		}

		int AudioOut::getBuffers()
		{
			return amtBuffers;
		}

		void AudioOut::setSizeOfBuffer(int b)
		{
			if(!hasInit)
				bufferSize = b;
		}

		int AudioOut::getSizeOfBuffer()
		{
			return bufferSize;
		}

		void AudioOut::setRunning(bool b)
		{
			audioMutex.lock();
			running = b;
			audioMutex.unlock();
		}

		bool AudioOut::getRunning()
		{
			bool k;
			audioMutex.lock();
			k = running;
			audioMutex.unlock();
			return k;
		}

	} //NAMESPACE glib END

#endif