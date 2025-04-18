@echo OFF
ninja -f ./build/Release/build.ninja -v
set debugOptions=-g
set windowsSDKLibs=-l kernel32 -l user32 -l gdi32 -l winspool -l comdlg32 -l advapi32 -l shell32 -l ole32 -l oleaut32 -l uuid -l odbc32 -l odbccp32
set dxLibs=-l dxgi -l d3d11 -l d3dcompiler
set sslLibs=-L C:/Libraries/openssl-master/openssl -l ssl -l crypto -l crypt32
set oglLibs=-L C:/Libraries/glew-2.1.0/lib/Release/x64 -l glew32 -l opengl32
set audioLibs=-l winmm
set xinputLibs=-l XInput
set dinputLibs=-l dinput8 -l dxguid
set networkLibs=-l Ws2_32 -l Mswsock
set windowLibs=-l Msimg32 -l Dwmapi
set performanceLibs=-l Pdh -l Winmm
set linkOptions=%windowsSDKLibs% %sslLibs% %oglLibs%  %networkLibs% %windowLibs% %audioLibs% %xinputLibs% %dinputLibs% %performanceLibs%
set extraOptions=-O3 -fopenmp
set projectCommand=./bin/Release/obj/*.o -o ./bin/Release/Smpl.exe
clang++ %debugOptions% %linkOptions% %extraOptions% %projectCommand%