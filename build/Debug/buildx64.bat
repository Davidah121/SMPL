@echo OFF
ninja -f ./build/Debug/buildx64.ninja -v
clang -g %WLIBPATH64% %WLIBVALUES% -fsanitize=address ./bin/Debug/x64/obj/*.obj -o ./bin/Debug/x64/GLib.exe