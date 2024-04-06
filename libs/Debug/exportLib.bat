@echo OFF
del ".\bin\Debug\obj\Main.o"
del ".\libs\Debug\SMPL.lib"
del ".\libs\Debug\SMPL.pdb"
llvm-ar -rcs libs/Debug/SMPL.lib bin/Debug/obj/*.o
copy /b ".\bin\Debug\SMPL.pdb" ".\libs\Debug\SMPL.pdb"