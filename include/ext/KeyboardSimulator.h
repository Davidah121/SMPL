#pragma once

#ifdef _WIN32
    #include "TargetVer.h"
	#include <Windows.h>

	namespace smpl
	{
		class KeyboardSimulator
		{
		public:
			/**
			 * @brief Emulates a key press.
			 * 		Key is considered down until it is released.
			 * 		May not work in other OS.
			 * 
			 * @param key 
			 */
			static void emulateKeyPress(int key);

			/**
			 * @brief Emulates a key release.
			 * 		May not work in other OS.
			 * 
			 * @param key 
			 */
			static void emulateKeyRelease(int key);

		private:
		};
	}
#endif