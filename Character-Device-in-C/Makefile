obj-m := mypipe.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	sudo insmod mypipe.ko FIFO_queue_size=5
	sudo chmod 0606 /dev/mypipe	
compile:
	gcc -o c c.c
	gcc -o p p.c
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f c *~
	rm -f p *~
