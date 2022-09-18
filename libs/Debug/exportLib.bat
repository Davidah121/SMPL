@echo OFF
del ".\bin\Debug\obj\Main.o"
del ".\libs\Debug\GLIB.lib"
del ".\libs\Debug\GLIB.pdb"
llvm-ar -rcs libs/Debug/GLIB.lib bin/Debug/obj/*.obj
copy /b ".\bin\Debug\GLib.pdb" ".\libs\Debug\GLIB.pdb"