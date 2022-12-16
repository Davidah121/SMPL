#pragma once
#include "Vec2f.h"
#include "File.h"

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

		/**
		 * @brief Construct a new Sound object
		 * 		Contains the data for stereo sound.
		 * 		Can be a shallow copy or deep copy.
		 * 
		 * 		Assumes all data is stereo and uses 44100 samples per sec.
		 * 
		 */
		Sound();

		/**
		 * @brief Construct a new Sound object from another Sound object
		 * 		Contains the data for stereo sound.
		 * 		Performs a shallow copy.
		 * 		Assumes all data is stereo and uses 44100 samples per sec.
		 * 
		 * @param other
		 */
		Sound(const Sound& other);

		/**
		 * @brief Copies a Sound object.
		 * 		Performs a shallow copy.
		 * 		Assumes all data is stereo and uses 44100 samples per sec.
		 * 
		 * @param other
		 */
		void operator=(const Sound& other);

		/**
		 * @brief Destroy the Sound object
		 * 		Deletes the original data if it is not a shallow copy.
		 * 
		 */
		~Sound();

		/**
		 * @brief Copies another sound object.
		 * 
		 * @param shallowCopy 
		 * 		Whether this should be a shallow copy or deep copy.
		 * @return Sound 
		 */
		Sound copy(bool shallowCopy);

		/**
		 * @brief Sets whether the sound should be playing.
		 * 		Only plays if it has been added to AudioOut sound list
		 * 
		 */
		void play();

		/**
		 * @brief Returns whether the is currently playing.
		 * 		Only plays if it has been added to AudioOut sound list
		 * 
		 * @return true 
		 * @return false 
		 */
		bool getPlaying();

		/**
		 * @brief Stops playback of the sound.
		 * 		Sets the position back to 0.
		 */
		void stop();

		/**
		 * @brief Pauses the playback of the sound.
		 * 		Maintains the current sound position.
		 */
		void pause();

		/**
		 * @brief Sets the left and right channel volumes.
		 * 
		 * @param left 
		 * @param right 
		 */
		void setVolume(double left, double right);

		/**
		 * @brief Sets the left and right channel volumes.
		 * 
		 * @param v
		 */
		void setVolume(Vec2f v);

		/**
		 * @brief Gets the Volume of the sound for both channels
		 * 
		 * @return Vec2f 
		 */
		Vec2f getVolume();

		/**
		 * @brief Sets if the sound should loop.
		 * 		Loops based on the loop start point and loop end point.
		 * @param l 
		 */
		void setLoop(bool l);

		/**
		 * @brief Gets if the sound should loop.
		 * 
		 * @return true 
		 * @return false 
		 */
		bool getLoop();

		/**
		 * @brief Gets the raw sound data.
		 * 
		 * @return Vec2f* 
		 */
		Vec2f* getData();

		/**
		 * @brief Gets the sound data at the specified index.
		 * 
		 * @param index 
		 * @return Vec2f& 
		 */
		Vec2f& operator[](size_t index);

		/**
		 * @brief Gets the sound data at the specified index.
		 * 
		 * @param index 
		 * @return Vec2f
		 */
		Vec2f get(size_t index);

		/**
		 * @brief Gets the time position in the audio.
		 * 
		 * @return size_t 
		 */
		size_t getPosition();

		/**
		 * @brief Sets the time position in the audio.
		 * 
		 * @param index 
		 */
		void setPosition(size_t index);

		/**
		 * @brief Sets the Priority of the sound. Higher Priority sounds are less likely to be cut when 
		 * 		the amount of sounds allowed has been exceeded.
		 * 
		 * @param v 
		 */
		void setPriority(int v);

		/**
		 * @brief Gets the Priority of the sound.
		 * 
		 * @return int 
		 */
		int getPriority();

		/**
		 * @brief Get the Length of the sound.
		 * 		Returns the amount of samples.
		 * 
		 * @return size_t 
		 */
		size_t getLength();

		/**
		 * @brief Gets the time for Loop Start
		 * 
		 * @return size_t 
		 */
		size_t getLoopStart();

		/**
		 * @brief Gets the time for Loop End
		 * 
		 * @return size_t 
		 */
		size_t getLoopEnd();

		/**
		 * @brief Sets the time for Loop Start
		 * 
		 * @param index 
		 */
		void setLoopStart(size_t index);

		/**
		 * @brief Sets the time for Loop Start
		 * 		Converts back to samples per second.
		 * @param time 
		 */
		void setLoopStart(double time);

		/**
		 * @brief Sets the time for Loop End
		 * 
		 * @param index 
		 */
		void setLoopEnd(size_t index);

		/**
		 * @brief Sets the time for Loop End
		 * 		Converts back to samples per second.
		 * @param time 
		 */
		void setLoopEnd(double time);

		/**
		 * @brief Set the raw sound data.
		 * 		Assumes all data is stereo and uses 44100 samples per sec.
		 * 
		 * @param data 
		 * @param size 
		 * @param isShallowCopy 
		 * 		Whether it will be a shallow copy of the data or take ownership of the data.
		 * 		Default is false
		 */
		void setData(Vec2f* data, size_t size, bool isShallowCopy=false);

		/**
		 * @brief Makes a copy of the raw sound data.
		 * 		Assumes all data is stereo and uses 44100 samples per sec.
		 * 
		 * @param data 
		 * @param size 
		 */
		void copyData(Vec2f* data, size_t size);

		/**
		 * @brief Loads a sound from a file.
		 * 		Valid file types are .wav
		 * 		Assumes all data is stereo and uses 44100 samples per sec.
		 * 
		 * @param file 
		 */
		void loadSound(File file);

		/**
		 * @brief Saves a sound as a wav file.
		 * 		Assumes all data is stereo and uses 44100 samples per sec.
		 * 
		 * @param file 
		 */
		void saveWAV(File file);

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
		
		size_t priority = 0;
		size_t position = 0;
		size_t length = 0;

		size_t loopStart = 0;
		size_t loopEnd = 0;
	};

} //NAMESPACE glib END