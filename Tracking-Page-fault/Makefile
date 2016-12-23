obj-m := mypipe.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
compile:
	gcc -o c c.c
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f c *~
