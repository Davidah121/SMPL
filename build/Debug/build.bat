@echo OFF
ninja -f ./build/Debug/build.ninja -v
set debugOptions=-g
@REM set windowsSDKLibs=%WLIBPATH64% %WLIBVALUES%
set windowsSDKLibs=%WLIBVALUES%
set dxLibs=-l dxgi -l d3d11 -l d3dcompiler
@REM set sslLibs=-L C:/Libraries/openssl/Static/Release/lib -l libssl -l libcrypto -l crypt32
set sslLibs=-L C:/Libraries/openssl-master/openssl -l ssl -l crypto -l crypt32
set oglLibs=-L C:/Libraries/glew-2.1.0/lib/Release/x64 -l glew32 -l opengl32
set audioLibs=-l winmm
set xinputLibs=-l XInput
set dinputLibs=-l dinput8 -l dxguid
set networkLibs=-l Ws2_32 -l Mswsock
set windowLibs=-l Msimg32 -l Dwmapi
set performanceLibs=-l Pdh -l Winmm
set linkOptions=%windowsSDKLibs% %sslLibs% %oglLibs% %networkLibs% %windowLibs% %audioLibs% %xinputLibs% %dinputLibs% %performanceLibs%
set extraOptions=-fopenmp
set projectCommand=./bin/Debug/obj/*.o -o ./bin/Debug/Smpl.exe
clang++ %debugOptions% %linkOptions% %extraOptions% %projectCommand%