#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h>
#include <linux/errno.h> /* error codes */
#include <linux/types.h> /* size_t */
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/semaphore.h>
#include <linux/list.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Prashant and Rishi");
MODULE_DESCRIPTION("Pipe");

//module parameter : FIFO queue (i.e. a pipe) of maximum N strings (configured as a module parameter).
int FIFO_queue_size = 0;
module_param(FIFO_queue_size,int,S_IRUSR|S_IWUSR);

int counter_write = 0;   // write counter
int counter_read = 0;    // read counter
char **my_queue;

//char *my_queue[10];
struct semaphore mutex;
struct semaphore empty;
struct semaphore full;

//method signatures for my_fpos

int my_open(struct inode *inode, struct file *filp);
int my_close(struct inode *inode, struct file *filp);
ssize_t my_read(struct file *filp,char *buf, size_t count, loff_t *f_pos);
ssize_t my_write(struct file *filp,const char *buf, size_t count, loff_t *f_pos);
static void mypipe_exit(void);
static int mypipe_init(void);

//Step 2: Declare the file operations struct 
static struct file_operations my_fops = {
	.owner = THIS_MODULE,
	.open = my_open,
    .release = my_close,
    .read = my_read,
    .write = my_write
};

// device struct
//Step 1: Declare a device struct
static struct miscdevice my_device_pipe  = {
.minor = MISC_DYNAMIC_MINOR,
.name = "mypipe",
.fops = &my_fops
};

int __init mypipe_init(void) 
{ 
		int return_registr;
		printk(KERN_ALERT "(kernel printk) : mypipe init called\n"); 	

		return_registr = misc_register(&my_device_pipe);
		if(return_registr < 0){
			printk(KERN_ERR "(kernel printk) : mypipe misc_register failed\n"); 
		}else{
			printk(KERN_ALERT "(kernel printk) : mypipe misc_registered successfully !!! \n"); 
			printk(KERN_ALERT "(kernel printk) : minor - %i \n", my_device_pipe.minor);
		}
        my_queue = kmalloc(FIFO_queue_size,GFP_KERNEL);
        if(!my_queue){
        	printk(KERN_ERR "(kernel printk) : KMalloc error\n");
		}
		else {
			printk(KERN_ALERT "(kernel printk) : Malloc done - %d size allocated \n",FIFO_queue_size);
		}

		sema_init(&empty,FIFO_queue_size);
		sema_init(&full,0);
		sema_init(&mutex,1);

        return 0; 
}

// called when module is removed 
void __exit mypipe_exit(void) 
{ 
	printk(KERN_ALERT "(kernel printk) : my_device_pipe exit \n"); 

	misc_deregister(&my_device_pipe);
	//free kmalloc memory
	if (my_queue) {
			kfree(my_queue);
	}
	printk(KERN_ALERT "(kernel printk) : my_device_pipe removed\n");
}


int my_open(struct inode *inode, struct file *filp){
	int t = 0;
	printk(KERN_ALERT "(kernel printk) : my_device_pipe open for operation\n");
	return t;
}

int my_close(struct inode *inode, struct file *filp){
	int t = 0;
	printk(KERN_ALERT "(kernel printk) : my_device_pipe released\n");
	return t;
}


ssize_t my_read(struct file *filp,char *buf, size_t count, loff_t *f_pos){
	int temp;
	//int x = 0;
	//char * buffer = kmalloc(count,GFP_KERNEL);
	//int i = 0;
	////////printk("(kernel printk) :  %s %zu \n", __func__, count);
	//*************************/*Semaphores operations*/*************************

	if(down_interruptible(&full)!=0) {
		printk(KERN_ALERT "(kernel printk) : down_interruptible :  Error down full in my_read\n");
                        return -ERESTARTSYS;
			}
	    if(down_interruptible(&mutex)!=0) {
			printk(KERN_ALERT "(kernel printk) : down_interruptible : Error down mutex in my_read\n");
		                return -ERESTARTSYS;
		}
	//*************************/*Semaphores operations*/*************************
	

		/*for(i=0;i<count;i++){
			buffer[i] = *(my_queue[counter_read]+i);
			buffer[i] = my_queue[counter_read][x];
			x++;
		}
		*/
		//consume
	    if ( copy_to_user( buf, my_queue[counter_read], count ) > 0 )
		{
			printk(KERN_ALERT "(kernel printk) :  copy to user failed \n");
			return -EFAULT;
		}
		//kfree(buffer);	
		//printk("@@@@@@@@@@@@@@@@@@@@@@(kernel printk) :    copied to user buffer - ");

		printk(KERN_ALERT "\t\t my_read : copied to user buffer - ");
		for(temp=0;temp<count;temp++)
			printk("%c",buf[temp]);
			
		printk(KERN_ALERT "\n");
		

		kfree(my_queue[counter_read]);
		my_queue[counter_read] = NULL;
		
		counter_read = ( counter_read + 1) % FIFO_queue_size;

		//*************************/*Semaphores operations*/*************************
		up(&mutex);
		up(&empty);
		//*************************/*Semaphores operations*/*************************

    
	return count;

}

ssize_t my_write(struct file *filp,const char *buf, size_t count, loff_t *f_pos){
		int temp = 0;

		//*************************/*Semaphores operations*/*************************

		if(down_interruptible(&empty) < 0) {
			printk(KERN_ALERT "(kernel printk) : down_interruptible : Error down Empty in my_write\n");
						 return -ERESTARTSYS;
				}

			if(down_interruptible(&mutex) < 0) {
				printk(KERN_ALERT "(kernel printk) : down_interruptible : Error down mutex in my_write\n");
							return -ERESTARTSYS;
			}

		//*************************/*Semaphores operations*/*************************
		
		
		my_queue[counter_write] = kmalloc(count,GFP_KERNEL);
		
		 if(!my_queue[counter_write]){
				printk(KERN_ALERT "(kernel printk) : KMalloc error\n");
				return -1;
			}
			
			
		if ( copy_from_user( my_queue[counter_write], buf, count) > 0 )
			{
				printk(KERN_ALERT "(kernel printk) :  copy from user failed\n");
				return -EFAULT;
			}
		
		printk(KERN_ALERT "my_write : ");
		
		for(temp=0;temp<count;temp++)
			printk("%c",my_queue[counter_write][temp]);
		
		printk(KERN_ALERT "\n");
		
		counter_write = ( counter_write + 1) % FIFO_queue_size;

		//*************************/*Semaphores operations*/*************************
		up(&mutex);
		up(&full);
		//*************************/*Semaphores operations*/*************************
		return count;
}
module_init(mypipe_init); 
module_exit(mypipe_exit);
