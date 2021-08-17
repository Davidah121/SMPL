#pragma once
#include "Vec2f.h"

namespace glib
{

	class Sound
	{
	public:

		static const float C0;
		static const float C0_SHARP;
		static const float D0;
		static const float D0_SHARP;
		static const float E0;
		static const float F0;
		static const float F0_SHARP;
		static const float G0;
		static const float G0_SHARP;
		static const float A0;
		static const float A0_SHARP;
		static const float B0;

		static const float C1;
		static const float C1_SHARP;
		static const float D1;
		static const float D1_SHARP;
		static const float E1;
		static const float F1;
		static const float F1_SHARP;
		static const float G1;
		static const float G1_SHARP;
		static const float A1;
		static const float A1_SHARP;
		static const float B1;

		static const float C2;
		static const float C2_SHARP;
		static const float D2;
		static const float D2_SHARP;
		static const float E2;
		static const float F2;
		static const float F2_SHARP;
		static const float G2;
		static const float G2_SHARP;
		static const float A2;
		static const float A2_SHARP;
		static const float B2;

		static const float C3;
		static const float C3_SHARP;
		static const float D3;
		static const float D3_SHARP;
		static const float E3;
		static const float F3;
		static const float F3_SHARP;
		static const float G3;
		static const float G3_SHARP;
		static const float A3;
		static const float A3_SHARP;
		static const float B3;

		static const float C4;
		static const float C4_SHARP;
		static const float D4;
		static const float D4_SHARP;
		static const float E4;
		static const float F4;
		static const float F4_SHARP;
		static const float G4;
		static const float G4_SHARP;
		static const float A4;
		static const float A4_SHARP;
		static const float B4;

		static const float C5;
		static const float C5_SHARP;
		static const float D5;
		static const float D5_SHARP;
		static const float E5;
		static const float F5;
		static const float F5_SHARP;
		static const float G5;
		static const float G5_SHARP;
		static const float A5;
		static const float A5_SHARP;
		static const float B5;

		static const float C6;
		static const float C6_SHARP;
		static const float D6;
		static const float D6_SHARP;
		static const float E6;
		static const float F6;
		static const float F6_SHARP;
		static const float G6;
		static const float G6_SHARP;
		static const float A6;
		static const float A6_SHARP;
		static const float B6;

		static const float C7;
		static const float C7_SHARP;
		static const float D7;
		static const float D7_SHARP;
		static const float E7;
		static const float F7;
		static const float F7_SHARP;
		static const float G7;
		static const float G7_SHARP;
		static const float A7;
		static const float A7_SHARP;
		static const float B7;

		static const float C8;
		static const float C8_SHARP;
		static const float D8;
		static const float D8_SHARP;
		static const float E8;
		static const float F8;
		static const float F8_SHARP;
		static const float G8;
		static const float G8_SHARP;
		static const float A8;
		static const float A8_SHARP;
		static const float B8;

		Sound();
		Sound(const Sound& other);
		void operator=(const Sound& other);
		~Sound();

		Sound copy(bool shallowCopy);

		void play();
		bool getPlaying();
		void stop();
		void pause();

		void setVolume(double left, double right);
		void setVolume(Vec2f v);
		Vec2f getVolume();

		void setLoop(bool l);
		bool getLoop();

		Vec2f* getData();
		Vec2f& operator[](int index);
		Vec2f get(int index);

		int getPosition();
		void setPosition(int index);

		void setPriority(int v);
		int getPriority();

		int getLength();

		int getLoopStart();
		int getLoopEnd();

		void setLoopStart(int index);
		void setLoopStart(double time);

		void setLoopEnd(int index);
		void setLoopEnd(double time);

		void setData(Vec2f* data, int size, bool isShallowCopy=false);
		void copyData(Vec2f* data, int size);

		void loadSound(std::string filename);
		void saveWAV(std::string filename);

	private:
		void loadWAV(std::vector<unsigned char> data);
		
		void dispose();

		void deepCopy(const Sound& other);
		void shallowCopy(const Sound& other);

		bool playing = false;
		bool looping = false;
		Vec2f volume = Vec2f(1.0, 1.0);

		bool isShallowCopy = false;
		Vec2f* data = nullptr;
		
		int priority = 0;
		int position = 0;
		int length = 0;

		int loopStart = 0;
		int loopEnd = 0;
	};

} //NAMESPACE glib END