default: build
all: default

build:
	cd src && make

clean:
	cd src && make clean
