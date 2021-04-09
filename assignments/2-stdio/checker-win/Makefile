CFLAGS = /nologo /MD /D_CRT_SECURE_NO_WARNINGS /Z7

build: so_stdio

so_stdio: so_stdio.obj utils.obj
	link /nologo /dll /out:so_stdio.dll /implib:so_stdio.lib so_stdio.obj utils.obj
so_stdio.obj: so_stdio.c
	cl $(CFLAGS) /Foso_stdio.obj /c so_stdio.c
	
utils.obj: utils.c
	cl $(CFLAGS) /Foutils.obj /c utils.c

clean : exe_clean obj_clean
 
obj_clean :
	del *.obj
 
exe_clean :
	del so_stdio.dll