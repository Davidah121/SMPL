@echo OFF
ninja -f ./build/Release/buildx64.ninja -v
clang -O3 -march=skylake %WLIBPATH64% %WLIBVALUES% ./bin/Release/x64/obj/*.o -o ./bin/Release/x64/GLib.exe