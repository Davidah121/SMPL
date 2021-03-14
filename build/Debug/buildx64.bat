@echo OFF
ninja -f ./build/Debug/buildx64.ninja -v
clang -g -fsanitize=address %WLIBPATH64% %WLIBVALUES% ./bin/Debug/x64/obj/*.o -o ./bin/Debug/x64/GLib.exe