@echo OFF
del ".\bin\Release\x64\obj\Main.o"
del ".\libs\Release\x64\GLIB.lib"
llvm-ar -rcs libs/Release/x64/GLIB.lib bin/Release/x64/obj/*.obj