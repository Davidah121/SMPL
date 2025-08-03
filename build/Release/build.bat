@echo OFF
ninja -f ./build/Release/build.ninja -v
set debugOptions=-g
set windowsSDKLibs=-l kernel32 -l user32 -l gdi32 -l winspool -l comdlg32 -l advapi32 -l shell32 -l ole32 -l oleaut32 -l uuid -l odbc32 -l odbccp32
set dxLibs=-l dxgi -l d3d11 -l d3dcompiler
set sslLibs=-L "C:\Program Files\OpenSSL-Win64\lib\VC\x64\MT" -l libssl -l libcrypto -l crypt32
set oglLibs=-L C:/Libraries/glew-2.1.0/lib/Release/x64 -l glew32 -l opengl32
set audioLibs=-l winmm
set xinputLibs=-l xinput
set dinputLibs=-l dinput8 -l dxguid
set networkLibs=-l ws2_32 -l mswsock
set windowLibs=-l msimg32 -l dwmapi
set performanceLibs=-l pdh -l winmm
set semlLibs=-L C:/Libraries/_MyLibraries/SEML -l SEML
set serializationLibs=-L C:/Libraries/_MyLibraries/SimpleSerialization -l SimpleSerialization
set linkOptions=%windowsSDKLibs% %sslLibs% %oglLibs%  %networkLibs% %windowLibs% %audioLibs% %xinputLibs% %dinputLibs% %performanceLibs% %semlLibs% %serializationLibs%
set extraOptions=-O3 -fopenmp
set projectCommand=./bin/Release/obj/*.o -o ./bin/Release/Smpl.exe
clang++ %debugOptions% %linkOptions% %extraOptions% %projectCommand%