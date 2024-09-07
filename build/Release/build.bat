@echo OFF
ninja -f ./build/Release/build.ninja -v
set debugOptions=-g
set linkOptions=%WLIBPATH64% %WLIBVALUES% -L C:/Libraries/glew-2.1.0/lib/Release/x64 -l glew32.lib -l opengl32.lib -L C:/Libraries/openssl/Static/Release/lib -l libssl -l libcrypto
set extraOptions=-O3 -fopenmp
set projectCommand=./bin/Release/obj/*.o -o ./bin/Release/Smpl.exe
clang++ %debugOptions% %linkOptions% %extraOptions% %projectCommand%