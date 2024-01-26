#
foo: ./build
	./build imsmake
clean: ./build 
	./build clean
help: ./build 
	./build help
install: ./build 
	./build install
all: ./build 
	./build all
config: ./build 
	./build imspro imsmake

tar:
	tar -czpvf ims.`date --utc +%Y%m%d`.tgz 00README 00NOTES Makefile Makefile.linux build main.cpp client.cpp client.h microLynxErrors.h timeMethods.h ims.pro ims.qrc images/ osmosPlc/ rotatorPlc/ xmisPlc/

