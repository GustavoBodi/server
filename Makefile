all:
	if [ ! -d build ]; \
		then mkdir build; \
	fi && \
	if [ ! -d "venv" ]; \
		then python3 -m venv venv && \
		. ./venv/bin/activate && \
		pip install conan && \
		conan profile detect; \
	fi && \
	. ./venv/bin/activate && \
	cd build && \
	conan install .. --build=missing -of=. -g CMakeDeps -g CMakeToolchain && \
	cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=build/Release/generators/conan_toolchain.cmake ../ && \
	cmake --build . && \
	make && \
	./WebServer

run:
	cd build && \
	make && \
	./WebServer

test:
	cd build && \
	cmake ../ && \
	make && \
	./tests-main

clean: 
	rm -rf build/
