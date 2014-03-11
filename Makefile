CC=gcc
EPSDEF=-DEPS=(1e-7)
CFLAGS=-O3 -march=native -static -Wall -Wextra $(EPSDEF) -L../libpng -I../libpng -L../lua -I../lua

3d_gazou_seisei.exe: 3d_gazou_seisei.o get_color.o script_loader.o lua_library.o 3d_hansya.o heimen_koten.o kyu_koten.o enchu_koten.o
	$(CC) $(CFLAGS) -s -o 3d_gazou_seisei.exe 3d_gazou_seisei.o \
		get_color.o script_loader.o lua_library.o \
		3d_hansya.o heimen_koten.o kyu_koten.o enchu_koten.o \
		-lpng -lz -llua

3d_gazou_seisei_omp.exe: 3d_gazou_seisei_omp.o get_color.o script_loader.o lua_library.o 3d_hansya.o heimen_koten.o kyu_koten.o enchu_koten.o
	$(CC) $(CFLAGS) -fopenmp -s -o 3d_gazou_seisei_omp.exe 3d_gazou_seisei_omp.o \
		get_color.o script_loader.o lua_library.o \
		3d_hansya.o heimen_koten.o kyu_koten.o enchu_koten.o \
		-lpng -lz -llua

3d_gazou_seisei.o: 3d_gazou_seisei.c
	$(CC) $(CFLAGS) -c -o 3d_gazou_seisei.o 3d_gazou_seisei.c

3d_gazou_seisei_omp.o: 3d_gazou_seisei.c
	$(CC) $(CFLAGS) -fopenmp -c -o 3d_gazou_seisei_omp.o 3d_gazou_seisei.c

get_color.o: get_color.c
	$(CC) $(CFLAGS) -c -o get_color.o get_color.c

script_loader.o: script_loader.c
	$(CC) $(CFLAGS) -c -o script_loader.o script_loader.c

lua_library.o: lua_library.c
	$(CC) $(CFLAGS) -c -o lua_library.o lua_library.c

3d_hansya.o: 3d_hansya.c
	$(CC) $(CFLAGS) -DBUILD_AS_COMPONENT -c -o 3d_hansya.o 3d_hansya.c

heimen_koten.o: heimen_koten.c
	$(CC) $(CFLAGS)  -DBUILD_AS_COMPONENT -c -o heimen_koten.o heimen_koten.c

kyu_koten.o: kyu_koten.c
	$(CC) $(CFLAGS) -DBUILD_AS_COMPONENT -c -o kyu_koten.o kyu_koten.c

enchu_koten.o: enchu_koten.c
	$(CC) $(CFLAGS) -DBUILD_AS_COMPONENT -c -o enchu_koten.o enchu_koten.c

3d_hansya.exe: 3d_hansya.c
	$(CC) $(CFLAGS) -s -o 3d_hansya.exe 3d_hansya.c

heimen_koten.exe: heimen_koten.c
	$(CC) $(CFLAGS) -s -o heimen_koten.exe heimen_koten.c

kyu_koten.exe: kyu_koten.c
	$(CC) $(CFLAGS) -s -o kyu_koten.exe kyu_koten.c

enchu_koten.exe: enchu_koten.c
	$(CC) $(CFLAGS) -s -o enchu_koten.exe enchu_koten.c

.PHONY: all
all: 3d_gazou_seisei.exe 3d_gazou_seisei_omp.exe ;

.PHONY: clean
clean:
	rm *.o *.exe
