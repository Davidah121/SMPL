@echo OFF
ar -rcs EXPORT/x64/GLIB.lib bin/x64/obj/*.o
ar -rcs EXPORT/x86/GLIB.lib bin/x86/obj/*.o