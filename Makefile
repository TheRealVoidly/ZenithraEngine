main.exe: Dev/zenithra_main.o Dev/zenithra_core.o Dev/zenithra_debug.o Dev/zenithra_movement.o Dev/zenithra_events.o Dev/zenithra_graphics.o Dev/zenithra_core_windows.o Dev/zenithra_main_windows.o Dev/zenithra_debug_windows.o Dev/zenithra_movement_windows.o Dev/zenithra_events_windows.o Dev/zenithra_graphics_windows.o
	gcc Dev/zenithra_main.o Dev/zenithra_core.o Dev/zenithra_debug.o Dev/zenithra_movement.o Dev/zenithra_events.o Dev/zenithra_graphics.o -g -o main -rdynamic `sdl2-config --cflags --libs` -lm -lGLU -lGLEW -lOpenGL
	x86_64-w64-mingw32-gcc -D__USE_MINGW_ANSI_STDIO=1 -D_WIN32 Dev/zenithra_main_windows.o Dev/zenithra_core_windows.o Dev/zenithra_debug_windows.o Dev/zenithra_movement_windows.o Dev/zenithra_events_windows.o Dev/zenithra_graphics_windows.o -g -o main.exe -Wall -lmingw32 `/usr/x86_64-w64-mingw32/bin/sdl2-config --cflags --libs` -lmsvcrt -lglew32 -lopengl32 -lglu32

zenithra_core.o: Dev/zenithra_core.c Dev/zenithra_debug.h
	gcc -c Dev/zenithra_core.c -o Dev/zenithra_core.o

Dev/zenithra_core_windows.o: Dev/zenithra_core.c Dev/zenithra_debug.h
	x86_64-w64-mingw32-gcc -D__USE_MINGW_ANSI_STDIO=1 -D_WIN32 -c Dev/zenithra_core.c -o Dev/zenithra_core_windows.o

zenithra_main.o: Dev/zenithra_main.c Dev/zenithra_debug.h Dev/zenithra_core.h
	gcc -c Dev/zenithra_main.c -o Dev/zenithra_main.o

Dev/zenithra_main_windows.o: Dev/zenithra_main.c Dev/zenithra_debug.h Dev/zenithra_core.h
	x86_64-w64-mingw32-gcc -D__USE_MINGW_ANSI_STDIO=1 -D_WIN32 -c Dev/zenithra_main.c -o Dev/zenithra_main_windows.o

zenithra_debug.o: Dev/zenithra_debug.c Dev/zenithra_debug.h
	gcc -c Dev/zenithra_debug.c -o Dev/zenithra_debug.o

Dev/zenithra_debug_windows.o: Dev/zenithra_debug.c Dev/zenithra_debug.h
	x86_64-w64-mingw32-gcc -D__USE_MINGW_ANSI_STDIO=1 -D_WIN32 -c Dev/zenithra_debug.c -o Dev/zenithra_debug_windows.o

zenithra_movement.o: Dev/zenithra_movement.c Dev/zenithra_core.h
	gcc -c Dev/zenithra_movement.c -o Dev/zenithra_movement.o

Dev/zenithra_movement_windows.o: Dev/zenithra_movement.c Dev/zenithra_core.h
	x86_64-w64-mingw32-gcc -D__USE_MINGW_ANSI_STDIO=1 -D_WIN32 -c Dev/zenithra_movement.c -o Dev/zenithra_movement_windows.o

zenithra_events.o: Dev/zenithra_events.c Dev/zenithra_core.h
	gcc -c Dev/zenithra_events.c -o Dev/zenithra_events.o

Dev/zenithra_events_windows.o: Dev/zenithra_events.c Dev/zenithra_core.h
	x86_64-w64-mingw32-gcc -D__USE_MINGW_ANSI_STDIO=1 -D_WIN32 -c Dev/zenithra_events.c -o Dev/zenithra_events_windows.o

zenithra_graphics.o: Dev/zenithra_graphics.c Dev/zenithra_core.h Dev/vertex_shader.h Dev/fragment_shader.h
	gcc -c Dev/zenithra_graphics.c -o Dev/zenithra_graphics.o

Dev/zenithra_graphics_windows.o: Dev/zenithra_graphics.c Dev/zenithra_core.h Dev/vertex_shader.h Dev/fragment_shader.h
	x86_64-w64-mingw32-gcc -D__USE_MINGW_ANSI_STDIO=1 -D_WIN32 -c Dev/zenithra_graphics.c -o Dev/zenithra_graphics_windows.o

clear:
	rm Dev/*.o main main.exe