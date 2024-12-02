
src_files := src/main.c \
             src/error.c \
			 src/game.c

compiler := cc
linker   := cc

opengl_cflags := $(shell pkgconf --cflags opengl)
opengl_libs   := $(shell pkgconf --libs   opengl)

glew_cflags := $(shell pkgconf --cflags glew)
glew_libs   := $(shell pkgconf --libs   glew)

sdl2_cflags := $(shell pkgconf --cflags sdl2)
sdl2_libs   := $(shell pkgconf --libs   sdl2)

cflags := $(opengl_cflags) $(glew_cflags) $(sdl2_cflags) -I./src
libs   := $(opengl_libs)   $(glew_libs)   $(sdl2_libs)

cflags_debug   := $(cflags) -g3 -O0 -D_DEBUG
cflags_release := $(cflags) -s -O2


all: clean build

build: build_debug

build_debug:
	$(compiler) -o opengl_test $(src_files) $(cflags_debug) $(libs)

build_release:
	$(compiler) -o opengl_test $(src_files) $(cflags_release) $(libs)


clean:
	rm -f opengl_test


run:
	$(shell ./opengl_test)


# vim noet
# end of file
