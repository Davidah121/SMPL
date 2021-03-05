@echo OFF
ninja -f ./build/Release/buildx64.ninja -v
clang %WLIBPATH64% %WLIBVALUES% -march=native -O2 ./bin/Release/x64/obj/*.o -o ./bin/Release/x64/GLib.exe