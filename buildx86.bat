@echo OFF
ninja -f ./build/buildx86.ninja -v
"C:\Program Files (x86)\LLVM\bin\clang" -g %WLIBPATH32% %WLIBVALUES% ./bin/x86/obj/*.o -o ./bin/x86/FixGLIB.exe