@echo OFF
ninja -f ./build/buildx64.ninja -v
clang -g %WLIBPATH64% %WLIBVALUES% ./bin/x64/obj/*.o -o ./bin/x64/FixGLIB.exe