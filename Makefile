all:
	cd build && \
	conan install .. --build=missing -of=. -g CMakeDeps -g CMakeToolchain && \
	cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake ../ && \
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

