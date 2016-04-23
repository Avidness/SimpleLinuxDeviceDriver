obj-m += tinymod.o

all:
	make -C /lib/modules/4.2.0-27-generic/build M=/home/anothername/SimpleLinuxDeviceDriver modules
