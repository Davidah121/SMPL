@echo OFF
ninja -f ./build/Debug/build.ninja -v
clang++ -g %WLIBPATH64% %WLIBVALUES% -L C:/Libraries/glew-2.1.0/lib/Release/x64 -l glew32.lib -l opengl32.lib ./bin/Debug/obj/*.o -o ./bin/Debug/GLib.exe