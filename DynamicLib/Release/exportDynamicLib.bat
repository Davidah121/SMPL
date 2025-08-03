@echo OFF
ninja -f ./build/Release/build.ninja -v
del ".\bin\Release\obj\Main.o"
del ".\DynamicLib\Release\SMPL.dll"
set debugOptions=-g
set windowsSDKLibs=-l kernel32 -l user32 -l gdi32 -l winspool -l comdlg32 -l advapi32 -l shell32 -l ole32 -l oleaut32 -l uuid -l odbc32 -l odbccp32
set dxLibs=-l dxgi -l d3d11 -l d3dcompiler
set sslLibs=-L "C:\Program Files\OpenSSL-Win64\lib\VC\x64\MT" -l libssl -l libcrypto -l crypt32
set oglLibs=-L C:/Libraries/glew-2.1.0/lib/Release/x64 -l glew32 -l opengl32
set audioLibs=-l winmm
set xinputLibs=-l XInput
set dinputLibs=-l dinput8 -l dxguid
set networkLibs=-l Ws2_32 -l Mswsock
set windowLibs=-l Msimg32 -l Dwmapi
set performanceLibs=-l Pdh -l Winmm
set semlLibs=-L C:/Libraries/_MyLibraries/SEML -l SEML
set serializationLibs=-L C:/Libraries/_MyLibraries/SimpleSerialization -l SimpleSerialization
set linkOptions=%windowsSDKLibs% %sslLibs% %oglLibs%  %networkLibs% %windowLibs% %audioLibs% %xinputLibs% %dinputLibs% %performanceLibs% %semlLibs% %serializationLibs%
set extraOptions=-O3 -fopenmp
set projectCommand=-shared -o ./DynamicLib/Release/SMPL.dll ./bin/Release/obj/*.o
clang++ %debugOptions% %linkOptions% %extraOptions% %projectCommand%