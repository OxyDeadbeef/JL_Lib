# Makefile to build the C version of jl_lib.
DEPS_VER_SDL = SDL2-2.0.4
DEPS_VER_SDL_IMAGE = SDL2_image-2.0.1
DEPS_VER_SDL_MIXER = SDL2_mixer-2.0.1
DEPS_VER_SDL_NET = SDL2_net-2.0.1
DEPS_VER_ZIP = libzip-1.1.2

SRC_SDL = src/lib/sdl
SRC_SDL_IMAGE = src/lib/sdl-image
SRC_SDL_MIXER = src/lib/sdl-mixer
SRC_SDL_NET = src/lib/sdl-net
SRC_LIBZIP = src/lib/libzip

OBJ_CLUMP = build/deps/clump.o
OBJ_SDL = build/deps/sdl.o
OBJ_SDL_IMAGE = build/deps/sdl-image.o
OBJ_SDL_MIXER = build/deps/sdl-mixer.o
OBJ_SDL_NET = build/deps/sdl-net.o
OBJ_LIBZIP = build/deps/libzip.o

# Default target
default: build/ $(OBJ_CLUMP) $(OBJ_SDL) $(OBJ_SDL_IMAGE) $(OBJ_SDL_MIXER) $(OBJ_SDL_NET) $(OBJ_LIBZIP) ~/.libaldaron
	rm -f build/deps.o
	# Linking library dependencies....
	ar csr build/deps.o build/deps/*.o
	# Built library dependencies!

~/.libaldaron:
	echo `pwd` >> ~/.libaldaron
	echo /bin >> ~/.libaldaron

documentation:
	doxygen compile-scripts/doxygen

ifneq ("$(shell uname | grep Linux)", "")
 ifneq ("$(shell uname -m | grep arm)", "")
  include compile-scripts/rpi.mk
 else
  include compile-scripts/linux.mk
 endif
else
 include compile-scripts/windows.mk
# $(error "Platform is not supported")
endif
#TODO: Darwin is mac OS for uname

# The Clean Options.
clean-all:
	rm -r build/
clean-build:
	# Empty directory: build/obj/
	printf "[COMP] Cleaning up....\n"
	rm -f -r build/obj/
	mkdir -p build/obj/
	rm -f build/*.o
	printf "[COMP] Done!\n"

build/:
	mkdir -p build/deps/
	mkdir -p build/obj/

src/lib/include/:
	mkdir -p src/lib/include/

$(SRC_SDL):
	cd src/lib/ && \
	$(WGET) https://www.libsdl.org/release/$(DEPS_VER_SDL).zip && \
	unzip $(DEPS_VER_SDL).zip && \
	rm $(DEPS_VER_SDL).zip && \
	mv $(DEPS_VER_SDL) sdl

download-ems:
	cd deps/&& \
	$(WGET) https://s3.amazonaws.com/mozilla-games/emscripten/releases/emsdk-portable.tar.gz && \
	tar -xzf emsdk-portable.tar.gz && \
	rm emsdk-portable.tar.gz

# Low Level / Build
build-emscripten:
	printf "[COMP] comiling emscripten...\n" && \
	cd deps/emsdk_portable/ && \
	./emsdk update && \
	./emsdk install latest && \
	./emsdk activate latest
$(OBJ_LIBZIP):
	# Downloading libzip....
	cd src/lib && \
	$(WGET) http://www.nih.at/libzip/$(DEPS_VER_ZIP).tar.gz && \
	tar -xzf $(DEPS_VER_ZIP).tar.gz && \
	rm $(DEPS_VER_ZIP).tar.gz && \
	mv $(DEPS_VER_ZIP)/ libzip
	# Compiling libzip....
	cd $(SRC_LIBZIP)/ && sh configure && make
	$(LD) $(SRC_LIBZIP)/lib/*.o -o $(OBJ_LIBZIP)
	# Done!
$(OBJ_SDL_IMAGE):
	# Downloading SDL_Image....
	cd src/lib/ && \
	$(WGET) https://www.libsdl.org/projects/SDL_image/release/$(DEPS_VER_SDL_IMAGE).zip && \
	unzip $(DEPS_VER_SDL_IMAGE).zip && \
	rm $(DEPS_VER_SDL_IMAGE).zip && \
	mv $(DEPS_VER_SDL_IMAGE) sdl-image
	# Compiling SDL_image....
	export PATH=$$PATH:`pwd`/$(SRC_SDL)/usr_local/bin/ && \
	cd $(SRC_SDL_IMAGE)/ && sh configure && make
	$(LD) $(SRC_SDL_IMAGE)/.libs/*.o -o $(OBJ_SDL_IMAGE)
	# Done!
$(OBJ_SDL_NET):
	# Downloading SDL_net....
#	cd src/lib/ && \
#	$(WGET) https://www.libsdl.org/projects/SDL_net/release/$(DEPS_VER_SDL_NET).zip && \
#	unzip $(DEPS_VER_SDL_NET).zip && \
#	rm $(DEPS_VER_SDL_NET).zip && \
#	mv $(DEPS_VER_SDL_NET) sdl-net
#	# Compiling SDL_net...,
#	cd $(SRC_SDL_NET)/ && export SDL2_CONFIG=`pwd`/../sdl/usr_local/bin/sdl2-config && ./configure --disable-gui --with-sdl-exec-prefix=`pwd`/../sdl/usr_local/bin/ && make
#	# Linking....
#	$(LD) $(SRC_SDL_NET)/.libs/*.o -o $(OBJ_SDL_NET)
#	printf "[COMP] done!\n"

$(OBJ_SDL_MIXER):
	# Downloading SDL_mixer....
	cd src/lib/ && \
	$(WGET) https://www.libsdl.org/projects/SDL_mixer/release/$(DEPS_VER_SDL_MIXER).zip && \
	unzip $(DEPS_VER_SDL_MIXER).zip && \
	rm $(DEPS_VER_SDL_MIXER).zip && \
	mv $(DEPS_VER_SDL_MIXER) sdl-mixer
	# Compiling SDL_mixer....
	export PATH=$$PATH:`pwd`/$(SRC_SDL)/usr_local/bin/ && \
	cd $(SRC_SDL_MIXER)/ && sh configure && make
	# Linking....
	rm -f $(SRC_SDL_MIXER)/build/playmus.o $(SRC_SDL_MIXER)/build/playwave.o
	$(LD) $(SRC_SDL_MIXER)/build/*.o -o $(OBJ_SDL_MIXER)
	# Done!

$(OBJ_CLUMP): src/lib/clump/*
	# Compiling clump....
	gcc src/lib/clump/array.c -c -std=c99 -o build/obj/clump_array.o
	gcc src/lib/clump/bitarray.c -std=c99 -c -o build/obj/clump_bitarray.o
	gcc src/lib/clump/clump.c -c -std=c99 -o build/obj/clump_clump.o
	gcc src/lib/clump/hash.c -c -std=c99 -o build/obj/clump_hash.o
	gcc src/lib/clump/hcodec.c -std=c99 -c -o build/obj/clump_hcodec.o
	gcc src/lib/clump/list.c -c -std=c99 -o build/obj/clump_list.o
	gcc src/lib/clump/pool.c -c -std=c99 -o build/obj/clump_pool.o
	gcc src/lib/clump/rhash.c -c -std=c99 -o build/obj/clump_rhash.o
	gcc src/lib/clump/tree.c -c -std=c99 -o build/obj/clump_tree.o
	$(LD) build/obj/clump_*.o -o $(OBJ_CLUMP)
	# Done!

################################################################################
