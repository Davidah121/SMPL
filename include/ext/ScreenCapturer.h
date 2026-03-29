#pragma once

#ifdef __WIN32
	#include <windows.h>
	#include <Image.h>
	#include "ext/DXSingleton.h"

	namespace smpl
	{
		class BitBltCapturer
		{
		public:
			BitBltCapturer();
			~BitBltCapturer();

			smpl::Image* capture();
		private:
			bool findCaptureSize();
			void recreatePixelBuffer();
			HWND desktopWindow;
			HDC desktopDC;
			HDC myCompatibleDC;
			HBITMAP temporaryBitmap;
			int lastWindowWidth = 0;
			int lastWindowHeight = 0;
			int paddingNeeded = 0;
			unsigned char* pixelStorage = nullptr;
		};

		#ifdef USE_DIRECTX

		class DXCapturer
		{
		public:
			DXCapturer();
			~DXCapturer();

			smpl::Image* capture();
		private:
			void initD3D11();
			void initDXGI();
			ID3D11Device* d3dDevice = nullptr;
			ID3D11DeviceContext* d3dContext = nullptr;
			IDXGIOutputDuplication* desktopDuplicator = nullptr;
			ID3D11Texture2D* acquiredImage = nullptr;
			
		};
		
		#endif
	}
#endif