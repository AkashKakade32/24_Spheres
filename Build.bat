cls

del *.exe
del *.obj
del *.res

cl.exe /c /EHsc OGL.c
rc.exe OGL.rc
link.exe OGL.obj OGL.res User32.lib GDI32.lib /SUBSYSTEM:WINDOWS
OGL.exe 