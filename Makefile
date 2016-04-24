obj-m += tinymod.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	$(CC) testTinymod.c -o test

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
