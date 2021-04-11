@echo OFF
del bin/Debug/x64/obj/Main.o
llvm-ar -rcs libs/Debug/x64/GLIB.lib bin/Debug/x64/obj/*.o