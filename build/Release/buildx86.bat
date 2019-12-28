@echo OFF
ninja -f ./build/Release/buildx86.ninja -v
"C:\Program Files (x86)\LLVM\bin\clang" -O3 %WLIBPATH32% %WLIBVALUES% ./bin/Release/x86/obj/*.o -o ./bin/Release/x86/FixGLIB.exe