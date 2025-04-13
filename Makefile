OPTIMIZATION ?= Debug

.PHONY: all configure build clean

all: configure build

configure: CMakeLists.txt
	mkdir -p build
	sudo -E cmake --preset=default -B./build -DCMAKE_BUILD_TYPE=$(OPTIMIZATION)

build: 
	sudo -E cmake --build build

build_server:
	sudo -E cmake --build build --target NEOCOOP_Server 

build_client:
	sudo -E cmake --build build --target NEOCOOP_Client

server:
	./build/Server/$(OPTIMIZATION)/NEOCOOP_Server $(ARGS)

clean:
	sudo rm -rf build
