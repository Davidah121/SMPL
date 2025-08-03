@echo OFF
ninja -f ./build/Release/build.ninja -v
del ".\bin\Release\obj\Main.o"
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
set semlLibs=-L C:/Libraries/_MyLibraries/SEML -l SEML
set serializationLibs=-L C:/Libraries/_MyLibraries/SimpleSerialization -l SimpleSerialization
set linkOptions=%windowsSDKLibs% %sslLibs% %oglLibs%  %networkLibs% %windowLibs% %audioLibs% %xinputLibs% %dinputLibs% %performanceLibs% %semlLibs% %serializationLibs%
set extraOptions=-O3 -fopenmp -pipe -D OPTI=2 -std=c++17 -mavx2
set catch2Options=-I C:/Libraries/catch2_UnitTest -L C:/Libraries/catch2_UnitTest/catch2 -l catch
set includeOptions=-I ./include -I C:/Libraries/glew-2.1.0/include -I C:/Libraries/openssl-master/openssl/include -I C:/Libraries/_MyLibraries/SEML -I C:/Libraries/_MyLibraries/SimpleSerialization
set projectCommand=./bin/Release/obj/*.o ./test/*.cpp -o ./test/TestExe.exe
clang++ %debugOptions% %linkOptions% %includeOptions% %catch2Options% %extraOptions% %projectCommand%
