@echo OFF
ninja -f ./build/Release/build.ninja -v
clang++ -O3 %WLIBPATH64% %WLIBVALUES% -L C:/Libraries/glew-2.1.0/lib/Release/x64 -l glew32.lib -l opengl32.lib ./bin/Release/obj/*.o -o ./bin/Release/GLib.exe