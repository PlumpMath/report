# Project
PROJECT	:= `grep -m 1 'project(' CMakeLists.txt | sed 's/project(\([^ ]*\).*/\1/'`

# Compiler
CC	:= `which gcc6 gcc|head -1`
CXX	:= `which g++6 g++|head -1`

# Targets
all: build/CMakeCache.txt
	@cmake --build build

build/CMakeCache.txt: CMakeLists.txt build
	@cd build && CC="$(CC)" CXX="$(CXX)" cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX:PATH=.. ..

install: build/CMakeCache.txt
	@cmake --build build --target install

build:
	@mkdir build

run: all
	@build/$(PROJECT)

push: install
	@scp -r bin deus.xiphos.de:deus

clean:
	rm -rf build
