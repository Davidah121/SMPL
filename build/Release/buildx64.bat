@echo OFF
ninja -f ./build/Release/buildx64.ninja -v
clang -O3 %WLIBPATH64% %WLIBVALUES% ./bin/Release/x64/obj/*.obj -o ./bin/Release/x64/GLib.exe