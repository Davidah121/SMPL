@echo OFF
ninja -f ./build/Release/build.ninja -v
del ".\bin\Release\obj\Main.o"
clang++ -pipe -Wno-unused-command-line-argument -D OPTI=2 -std=c++17 -O3 -march=skylake %WLIBPATH64% %WLIBVALUES% -I ./include -I C:/Libraries/glew-2.1.0/include -I C:/Libraries/catch2_UnitTest -L C:/Libraries/catch2_UnitTest/catch2 -L C:/Libraries/glew-2.1.0/lib/Release/x64 -L C:/Libraries/openssl/Static/Release/lib -l libssl -l libcrypto -l glew32.lib -l opengl32.lib -l catch.lib ./bin/Release/obj/*.o ./test/*.cpp -o ./test/TestExe.exe
