@echo OFF
ar -rcs libs/Debug/x64/GLIB.lib bin/Debug/x64/obj/*.o
ar -rcs libs/Debug/x86/GLIB.lib bin/Debug/x86/obj/*.o

ar -rcs libs/Release/x64/GLIB.lib bin/Release/x64/obj/*.o
ar -rcs libs/Release/x86/GLIB.lib bin/Release/x86/obj/*.o