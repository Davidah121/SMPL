#include "WavAudio.h"
#include "StringTools.h"
#include "MathExt.h"

const float WavAudio::C0 = 16.35f;
const float WavAudio::C0_SHARP = 17.32f;
const float WavAudio::D0 = 18.35f;
const float WavAudio::D0_SHARP = 19.45f;
const float WavAudio::E0 = 20.60f;
const float WavAudio::F0 = 21.83f;
const float WavAudio::F0_SHARP = 23.12f;
const float WavAudio::G0 = 24.50f;
const float WavAudio::G0_SHARP = 25.96f;
const float WavAudio::A0 = 27.50f;
const float WavAudio::A0_SHARP = 29.14f;
const float WavAudio::B0 = 30.87f;

const float WavAudio::C1 = 32.7f;
const float WavAudio::C1_SHARP = 34.64f;
const float WavAudio::D1 = 36.71f;
const float WavAudio::D1_SHARP = 38.89f;
const float WavAudio::E1 = 41.20f;
const float WavAudio::F1 = 43.65f;
const float WavAudio::F1_SHARP = 46.25f;
const float WavAudio::G1 = 49.00f;
const float WavAudio::G1_SHARP = 51.91f;
const float WavAudio::A1 = 55.00f;
const float WavAudio::A1_SHARP = 58.27f;
const float WavAudio::B1 = 61.74f;

const float WavAudio::C2 = 65.41f;
const float WavAudio::C2_SHARP = 69.3f;
const float WavAudio::D2 = 73.42f;
const float WavAudio::D2_SHARP = 77.78f;
const float WavAudio::E2 = 82.41f;
const float WavAudio::F2 = 87.31f;
const float WavAudio::F2_SHARP = 92.50f;
const float WavAudio::G2 = 98.00f;
const float WavAudio::G2_SHARP = 103.83f;
const float WavAudio::A2 = 110.00f;
const float WavAudio::A2_SHARP = 116.54f;
const float WavAudio::B2 = 123.47f;

const float WavAudio::C3 = 130.81f;
const float WavAudio::C3_SHARP = 138.59f;
const float WavAudio::D3 = 146.83f;
const float WavAudio::D3_SHARP = 155.56f;
const float WavAudio::E3 = 164.81f;
const float WavAudio::F3 = 174.61f;
const float WavAudio::F3_SHARP = 185.00f;
const float WavAudio::G3 = 196.00f;
const float WavAudio::G3_SHARP = 207.65f;
const float WavAudio::A3 = 220.00f;
const float WavAudio::A3_SHARP = 233.08f;
const float WavAudio::B3 = 246.94f;

const float WavAudio::C4 = 261.63f;
const float WavAudio::C4_SHARP = 277.18f;
const float WavAudio::D4 = 293.66f;
const float WavAudio::D4_SHARP = 311.13f;
const float WavAudio::E4 = 329.63f;
const float WavAudio::F4 = 349.23f;
const float WavAudio::F4_SHARP = 369.99f;
const float WavAudio::G4 = 392.00f;
const float WavAudio::G4_SHARP = 415.30f;
const float WavAudio::A4 = 440.00f;
const float WavAudio::A4_SHARP = 466.16f;
const float WavAudio::B4 = 493.88f;

const float WavAudio::C5 = 523.25f;
const float WavAudio::C5_SHARP = 554.37f;
const float WavAudio::D5 = 587.33f;
const float WavAudio::D5_SHARP = 622.25f;
const float WavAudio::E5 = 659.25f;
const float WavAudio::F5 = 698.46f;
const float WavAudio::F5_SHARP = 739.99f;
const float WavAudio::G5 = 783.99f;
const float WavAudio::G5_SHARP = 830.61f;
const float WavAudio::A5 = 880.00f;
const float WavAudio::A5_SHARP = 932.33f;
const float WavAudio::B5 = 987.77f;

const float WavAudio::C6 = 1046.50f;
const float WavAudio::C6_SHARP = 1108.73f;
const float WavAudio::D6 = 1174.66f;
const float WavAudio::D6_SHARP = 1244.51f;
const float WavAudio::E6 = 1318.51f;
const float WavAudio::F6 = 1396.91f;
const float WavAudio::F6_SHARP = 1479.98f;
const float WavAudio::G6 = 1567.98f;
const float WavAudio::G6_SHARP = 1661.22f;
const float WavAudio::A6 = 1760.00f;
const float WavAudio::A6_SHARP = 1864.66f;
const float WavAudio::B6 = 1975.53f;

const float WavAudio::C7 = 2093.00f;
const float WavAudio::C7_SHARP = 2217.46f;
const float WavAudio::D7 = 2349.32f;
const float WavAudio::D7_SHARP = 2489.02f;
const float WavAudio::E7 = 2637.02f;
const float WavAudio::F7 = 2793.83f;
const float WavAudio::F7_SHARP = 2959.96f;
const float WavAudio::G7 = 3135.96f;
const float WavAudio::G7_SHARP = 3322.44f;
const float WavAudio::A7 = 3520.00f;
const float WavAudio::A7_SHARP = 3729.31f;
const float WavAudio::B7 = 3951.07f;

const float WavAudio::C8 = 4186.01f;
const float WavAudio::C8_SHARP = 4434.92f;
const float WavAudio::D8 = 4698.63f;
const float WavAudio::D8_SHARP = 4978.03f;
const float WavAudio::E8 = 5274.04f;
const float WavAudio::F8 = 5284.65f;
const float WavAudio::F8_SHARP = 5919.91f;
const float WavAudio::G8 = 6271.93f;
const float WavAudio::G8_SHARP = 6644.88f;
const float WavAudio::A8 = 7040.00f;
const float WavAudio::A8_SHARP = 7458.62f;
const float WavAudio::B8 = 7902.13f;

const float WavAudio::NoteArray[108] = { C0, C0_SHARP, D0, D0_SHARP, E0, F0, F0_SHARP, G0, G0_SHARP, A0, A0_SHARP, B0,
										C1, C1_SHARP, D1, D1_SHARP, E1, F1, F1_SHARP, G1, G1_SHARP, A1, A1_SHARP, B1, 
										C2, C2_SHARP, D2, D2_SHARP, E2, F2, F2_SHARP, G2, G2_SHARP, A2, A2_SHARP, B2, 
										C3, C3_SHARP, D3, D3_SHARP, E3, F3, F3_SHARP, G3, G3_SHARP, A3, A3_SHARP, B3, 
										C4, C4_SHARP, D4, D4_SHARP, E4, F4, F4_SHARP, G4, G4_SHARP, A4, A4_SHARP, B4, 
										C5, C5_SHARP, D5, D5_SHARP, E5, F5, F5_SHARP, G5, G5_SHARP, A5, A5_SHARP, B5, 
										C6, C6_SHARP, D6, D6_SHARP, E6, F6, F6_SHARP, G6, G6_SHARP, A6, A6_SHARP, B6, 
										C7, C7_SHARP, D7, D7_SHARP, E7, F7, F7_SHARP, G7, G7_SHARP, A7, A7_SHARP, B7,
										C8, C8_SHARP, D8, D8_SHARP, E8, F8, F8_SHARP, G8, G8_SHARP, A8, A8_SHARP, B8};
HWAVEOUT WavAudio::waveOutHandle;
WAVEFORMATEX WavAudio::format;

WavAudio::WavAudio()
{
}


WavAudio::~WavAudio()
{
}

void WavAudio::init()
{
	format.cbSize = 0; //Extra data size
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.wBitsPerSample = 16; //Bits per sample
	format.nSamplesPerSec = 44100; //standard
	format.nChannels = 2; //Stereo = 2, Mono = 1
	format.nBlockAlign = (format.wBitsPerSample * format.nChannels) / 8;//
	format.nAvgBytesPerSec = format.nBlockAlign * format.nSamplesPerSec;//
	
	StringTools::out << format.nBlockAlign << StringTools::lineBreak;
	StringTools::out << format.nAvgBytesPerSec << StringTools::lineBreak;

	waveOutOpen(&waveOutHandle, WAVE_MAPPER, &format, NULL, NULL, CALLBACK_NULL);
}

void WavAudio::dispose()
{
	//waveOutRestart(waveOutHandle);
	waveOutClose(waveOutHandle);
}

void WavAudio::playMelody()
{
	struct temp
	{
		float note;
		int time;
	};

	int size = 60;
	temp* melody = new temp[size];

	melody[0] = { G4, 800 };
	melody[1] = { D4, 800 };

	melody[2] = { NULL, 600 };


	melody[3] = { G4, 200 };
	melody[4] = { G4, 200 };
	melody[5] = { A4, 200 };
	melody[6] = { A4_SHARP, 200 };
	melody[7] = { C5, 200 };
	melody[8] = { D5, 1500 };

	melody[9] = { NULL, 500 };

	melody[10] = { D5, 400 };
	melody[11] = { D5, 300 };
	melody[12] = { D5_SHARP, 300 };
	melody[13] = { F5, 300 };
	melody[14] = { G5, 1250 };

	melody[15] = { NULL, 750 };

	melody[16] = { G5, 300 };
	melody[17] = { G5, 300 };
	melody[18] = { F5, 300 };
	melody[19] = { D5_SHARP, 300 };
	melody[20] = { F5, 600 };
	melody[21] = { D5_SHARP, 300 };
	melody[22] = { D5, 1100 };

	melody[23] = { NULL, 700 };

	melody[24] = { D5, 800 };
	melody[25] = { C5, 400 };
	melody[26] = { C5, 200 };
	melody[27] = { D5, 200 };
	melody[28] = { D5_SHARP, 1000 };

	melody[29] = { NULL, 800 };

	melody[30] = { D5, 400 };
	melody[31] = { C5, 400 };
	melody[32] = { A4_SHARP, 400 };
	melody[33] = { A4_SHARP, 200 };
	melody[34] = { C5, 200 };
	melody[35] = { D5, 1000 };

	melody[36] = { NULL, 800 };

	melody[37] = { C5, 400 };
	melody[38] = { A4_SHARP, 400 };
	melody[39] = { A4, 400 };
	melody[40] = { A4, 200 };
	melody[41] = { A4_SHARP, 200 };
	melody[42] = { C5, 1200 };

	melody[43] = { NULL, 600 };

	melody[44] = { F5, 800 };
	melody[45] = { D5_SHARP, 400 };

	melody[46] = { NULL, 100 };

	melody[47] = { D4, 200 };
	melody[48] = { D4, 200 };
	melody[49] = { D4, 200 };

	melody[50] = { NULL, 200 };

	melody[51] = { D4, 200 };
	melody[52] = { D4, 200 };
	melody[53] = { D4, 200 };

	melody[54] = { NULL, 200 };

	melody[55] = { D4, 200 };
	melody[56] = { D4, 200 };
	melody[57] = { D4, 200 };

	melody[58] = { NULL, 200 };

	melody[59] = { D4, 200 };
	//melody[8] = { E5, 600 };
	for (int i = 0; i < size; i++)
	{
		if (melody[i].note != NULL)
			playNote(melody[i].note, melody[i].time);
		else
			Sleep(melody[i].time);
	}
}

void WavAudio::addNote(float value, int location, int duration)
{
}

void WavAudio::setNote(float value, int location, int duration)
{
}

void WavAudio::setLength(int timeInMillis)
{
	//notes.resize((int)(44.1*timeInMillis));
}

void WavAudio::playNote(float value, int duration)
{
	WAVEHDR header;
	ZeroMemory(&header, sizeof(WAVEHDR));

	int size = (int) (44.1 * duration*(format.nChannels* (format.wBitsPerSample/8) ));
	int length = (int) 44.1*duration;

	char* data = new char[size];
	float temp;
	
	char data1, data2;

	int leftChannel = 0;
	int rightChannel = 2;

	short waveData = 0;

	for (int t = 0; t < length; t += 1)
	{
		temp = 0.3 * sin((2 * (double)value * PI * t) / 44100);

		waveData = (short)MathExt::clamp(temp*32768.0f, -32768.0f, 32767.0f);

		data1 = (waveData >> 8) & 0xFF; //First Byte
		data2 = (waveData >> 0) & 0xFF; //Second byte

		//Every 2 bytes is equal to a channel
		data[leftChannel] = data2; //Left Channel
		data[leftChannel+1] = data1; //Left Channel

		data[leftChannel + 2] = data2; //Right Channel
		data[leftChannel + 3] = data1; //Right Channel

		leftChannel += 4;
	}

	/*
	for (int t = 0; t < length; t+=2)
	{
		temp = 0.5 * sin((value * PI * t) / 44100);
		
		//Every 2 bytes is equal to a channel
		data[leftChannel] = MathExt::clamp(temp*128.0f, -128.0f, 127.0f); //Left Channel
		data[rightChannel] = MathExt::clamp(temp*128.0f, -128.0f, 127.0f); //Right Channel
		
		temp = 0.5 * sin((value * PI * (t+1)) / 44100);

		//Every 2 bytes is equal to a channel
		data[leftChannel+1] = MathExt::clamp(temp*128.0f, -128.0f, 127.0f); //Left Channel
		data[rightChannel+1] = MathExt::clamp(temp*128.0f, -128.0f, 127.0f); //Right Channel

		leftChannel += 4;
		rightChannel += 4;
	}
	*/

	header.dwBufferLength = size;
	header.lpData = data;
	header.dwFlags = 0;
	header.dwLoops = 0;

	waveOutPrepareHeader(waveOutHandle, &header, sizeof(WAVEHDR));

	waveOutWrite(waveOutHandle, &header, sizeof(WAVEHDR));
	
	bool stillPlaying = true;
	while(stillPlaying)
	{ 
		stillPlaying = (waveOutUnprepareHeader(waveOutHandle, &header, sizeof(WAVEHDR)) == WAVERR_STILLPLAYING);
	}
	StringTools::out << header.dwFlags << StringTools::lineBreak;

	delete[] data;
}
