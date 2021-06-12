@echo OFF
del ".\bin\Debug\x64\obj\Main.o"
del ".\libs\Debug\x64\GLIB.lib"
del ".\libs\Debug\x64\GLIB.pdb"
llvm-ar -rcs libs/Debug/x64/GLIB.lib bin/Debug/x64/obj/*.o
copy /b ".\bin\Debug\x64\GLib.pdb" ".\libs\Debug\x64\GLIB.pdb"