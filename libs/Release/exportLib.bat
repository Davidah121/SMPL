@echo OFF
del ".\bin\Release\obj\Main.o"
del ".\libs\Release\GLIB.lib"
llvm-ar -rcs libs/Release/GLIB.lib bin/Release/obj/*.obj