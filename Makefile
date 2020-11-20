subdirs := $(wildcard */)
VPATH = $(subdirs)

sources := $(wildcard src/*.c) $(wildcard src/**/*.c)
objects := $(patsubst src/%.c,obj/%.o,$(sources))

CCFLAGS = -g
GCC = x86_64-w64-mingw32-gcc
DLL_PATH = C:\cygwin64\usr\x86_64-w64-mingw32\sys-root\mingw\bin

all: bin/program.exe

bin/program.exe: $(objects)
	mkdir -p "bin"
	cp $(DLL_PATH)\SDL2.dll bin\SDL2.dll
	cp $(DLL_PATH)\iconv.dll bin\iconv.dll
	$(GCC) $(CCFLAGS) $(objects) -o $@ -lmingw32 -lsdl2main -lsdl2

-include $(objects:.o=.d)

$(objects) : obj/%.o : %.c
	mkdir -p "$(dir $@)"
	$(GCC) $(CCFLAGS) -c -o $@ ./$<

clean:
	rm obj bin -r