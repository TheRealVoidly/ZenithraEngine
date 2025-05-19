#Linux:

make: dev/src/zenithra_main.o dev/src/zenithra_core.o dev/src/zenithra_debug.o dev/src/zenithra_movement.o dev/src/zenithra_events.o dev/src/zenithra_graphics.o dev/src/zenithra_editor.o
	gcc dev/src/zenithra_main.o dev/src/zenithra_core.o dev/src/zenithra_debug.o dev/src/zenithra_movement.o dev/src/zenithra_events.o dev/src/zenithra_graphics.o dev/src/zenithra_editor.o -g -o main -rdynamic `sdl2-config --cflags --libs` -lm -lGLU -lGLEW -lOpenGL

zenithra_core.o: dev/src/zenithra_core.c dev/src/zenithra_debug.h
	gcc -c dev/src/zenithra_core.c -o dev/src/zenithra_core.o

zenithra_main.o: dev/src/zenithra_main.c dev/src/zenithra_debug.h dev/src/zenithra_core.h
	gcc -c dev/src/zenithra_main.c -o dev/src/zenithra_main.o

zenithra_editor.o: dev/src/zenithra_editor.c dev/src/zenithra_core.h
	gcc -c dev/src/zenithra_editor.c -o dev/src/zenithra_editor.o

zenithra_debug.o: dev/src/zenithra_debug.c dev/src/zenithra_debug.h
	gcc -c dev/src/zenithra_debug.c -o dev/src/zenithra_debug.o

zenithra_movement.o: dev/src/zenithra_movement.c dev/src/zenithra_core.h
	gcc -c dev/src/zenithra_movement.c -o dev/src/zenithra_movement.o

zenithra_events.o: dev/src/zenithra_events.c dev/src/zenithra_core.h
	gcc -c dev/src/zenithra_events.c -o dev/src/zenithra_events.o

zenithra_graphics.o: dev/src/zenithra_graphics.c dev/src/zenithra_core.h dev/src/vertex_shader.h dev/src/fragment_shader.h
	gcc -c dev/src/zenithra_graphics.c -o dev/src/zenithra_graphics.o

#Windows:

windows: dev/src/zenithra_core_windows.o dev/src/zenithra_main_windows.o dev/src/zenithra_debug_windows.o dev/src/zenithra_movement_windows.o dev/src/zenithra_events_windows.o dev/src/zenithra_graphics_windows.o dev/src/zenithra_editor_windows.o
	x86_64-w64-mingw32-gcc -D__USE_MINGW_ANSI_STDIO=1 -D_WIN32 dev/src/zenithra_main_windows.o dev/src/zenithra_core_windows.o dev/src/zenithra_debug_windows.o dev/src/zenithra_movement_windows.o dev/src/zenithra_events_windows.o dev/src/zenithra_graphics_windows.o dev/src/zenithra_editor_windows.o -g -o main.exe -Wall -lmingw32 `/usr/x86_64-w64-mingw32/bin/sdl2-config --cflags --libs` -lmsvcrt -lglew32 -lopengl32 -lglu32

dev/src/zenithra_editor_windows.o: dev/src/zenithra_editor.c dev/src/zenithra_core.h
	x86_64-w64-mingw32-gcc -D__USE_MINGW_ANSI_STDIO=1 -D_WIN32 -c dev/src/zenithra_editor.c -o dev/src/zenithra_editor_windows.o

dev/src/zenithra_core_windows.o: dev/src/zenithra_core.c dev/src/zenithra_debug.h
	x86_64-w64-mingw32-gcc -D__USE_MINGW_ANSI_STDIO=1 -D_WIN32 -c dev/src/zenithra_core.c -o dev/src/zenithra_core_windows.o

dev/src/zenithra_main_windows.o: dev/src/zenithra_main.c dev/src/zenithra_debug.h dev/src/zenithra_core.h
	x86_64-w64-mingw32-gcc -D__USE_MINGW_ANSI_STDIO=1 -D_WIN32 -c dev/src/zenithra_main.c -o dev/src/zenithra_main_windows.o

dev/src/zenithra_debug_windows.o: dev/src/zenithra_debug.c dev/src/zenithra_debug.h
	x86_64-w64-mingw32-gcc -D__USE_MINGW_ANSI_STDIO=1 -D_WIN32 -c dev/src/zenithra_debug.c -o dev/src/zenithra_debug_windows.o

dev/src/zenithra_movement_windows.o: dev/src/zenithra_movement.c dev/src/zenithra_core.h
	x86_64-w64-mingw32-gcc -D__USE_MINGW_ANSI_STDIO=1 -D_WIN32 -c dev/src/zenithra_movement.c -o dev/src/zenithra_movement_windows.o

dev/src/zenithra_events_windows.o: dev/src/zenithra_events.c dev/src/zenithra_core.h
	x86_64-w64-mingw32-gcc -D__USE_MINGW_ANSI_STDIO=1 -D_WIN32 -c dev/src/zenithra_events.c -o dev/src/zenithra_events_windows.o

dev/src/zenithra_graphics_windows.o: dev/src/zenithra_graphics.c dev/src/zenithra_core.h dev/src/vertex_shader.h dev/src/fragment_shader.h
	x86_64-w64-mingw32-gcc -D__USE_MINGW_ANSI_STDIO=1 -D_WIN32 -c dev/src/zenithra_graphics.c -o dev/src/zenithra_graphics_windows.o

#Clear:

clear:
	rm dev/src/*.o main main.exe