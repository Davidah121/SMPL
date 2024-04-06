@echo OFF
del ".\bin\Release\obj\Main.o"
del ".\libs\Release\SMPL.lib"
llvm-ar -rcs libs/Release/SMPL.lib bin/Release/obj/*.o