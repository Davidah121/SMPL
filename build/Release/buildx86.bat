@echo OFF
ninja -f ./build/Release/buildx86.ninja -v
"C:\Program Files (x86)\LLVM\bin\clang" -march=native -O2 %WLIBPATH32% %WLIBVALUES% ./bin/Release/x86/obj/*.o -o ./bin/Release/x86/GLib.exe