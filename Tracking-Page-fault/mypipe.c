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
#include <linux/kprobes.h>
#include <linux/unistd.h>
#include <linux/time.h>
#include "info.h"
/* Our own ioctl numbers */
#define RET_SUCCESS 0
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Prashant");
MODULE_DESCRIPTION("Pipe");

int pid_to_track = 0;
module_param(pid_to_track,int,S_IRUSR|S_IWUSR);

int counter_write = 0;   // write counter

struct information{
	unsigned long address;
	struct timespec timenow;
};

struct information record[1000];


static long j_handle_mm_fault(struct mm_struct *mm, struct vm_area_struct *vma,unsigned long address, unsigned int flags)
  {
		if(current->pid == pid_to_track){             
            record[counter_write].address = address;
            getnstimeofday(&record[counter_write].timenow);
            pr_info("pid is %d jprobe: address = 0x%lx ", pid_to_track, address);
            printk("time %lld.%.9ld \n", (long long)record[counter_write].timenow.tv_sec, record[counter_write].timenow.tv_nsec);
            counter_write = ( counter_write + 1 ) % 1000;
            }
         /* Always end with a call to jprobe_return(). */
          jprobe_return();	 
          return 0;
 }
  
  static struct jprobe my_jprobe = {
         .entry                  = j_handle_mm_fault,
         .kp = {
                 .symbol_name    = "handle_mm_fault",
          },
  };
  
  static int __init jprobe_init(void)
  {
          int ret;
  
          ret = register_jprobe(&my_jprobe);
         if (ret < 0) {
                printk(KERN_INFO "register_jprobe failed, returned %d\n", ret);
                  return -1;
          }
          printk(KERN_INFO "Planted jprobe at %p, handler addr %p\n",
                 my_jprobe.kp.addr, my_jprobe.entry);
          return 0;
 }
  
  static void __exit jprobe_exit(void)
  {
          unregister_jprobe(&my_jprobe);
          printk(KERN_INFO "jprobe at %p unregistered\n", my_jprobe.kp.addr);
  }
//method signatures for my_fpos

int my_open(struct inode *inode, struct file *filp);
int my_close(struct inode *inode, struct file *filp);
static void mypipe_exit(void);
static int mypipe_init(void);

size_t track_fault_ioctl(struct inode *inode, struct file *file, unsigned int ioctl_num, unsigned long ioctl_param);


//Step 2: Declare the file operations struct 
static struct file_operations my_fops = {
	.owner = THIS_MODULE,
	.open = my_open,
    .release = my_close,
    .unlocked_ioctl = track_fault_ioctl   /* ioctl */
};

// device struct
//Step 1: Declare a device struct
static struct miscdevice my_device_pipe  = {
.minor = MISC_DYNAMIC_MINOR,
.name = "mypipe",
.fops = &my_fops
};


size_t track_fault_ioctl(struct inode *inode, struct file *file, unsigned int ioctl_num, unsigned long ioctl_param)
{
  int ret_val;
  int c =0;
  //struct params *obj= kmalloc(sizeof(struct params), GFP_DMA);
  printk("ioctl_num %d",ioctl_num);
  switch (ioctl_num)
    {
    case 9999:
    break;
    default:
    //  printk(KERN_INFO "\ndefault case\n");

      
      for(c =0;c<1000;c++){
      	printk("-------------------");
      	pr_info("address = 0x%lx ", record[c].address);
        printk("time %lld.%.9ld \n", (long long)record[c].timenow.tv_sec, record[c].timenow.tv_nsec);
        printk("-------------------");
      }

      ret_val = copy_to_user((void *)ioctl_param, &record,1000*(unsigned long)sizeof(struct information));
      if(ret_val < 0){
        printk(KERN_INFO " track_fault_ioctl error %d\n", ret_val);
                  return -1;
      }
      printk(KERN_INFO "\ndevice_ioctl() = %d.\n", ret_val);
      break;
    }
  

  return RET_SUCCESS;
}


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
        
		jprobe_init();
        return 0; 
}





// called when module is removed 
void __exit mypipe_exit(void) 
{ 
	printk(KERN_ALERT "(kernel printk) : my_device_pipe exit \n"); 

	misc_deregister(&my_device_pipe);
	
	printk(KERN_ALERT "(kernel printk) : my_device_pipe removed\n");
	jprobe_exit();
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

module_init(mypipe_init); 
module_exit(mypipe_exit);
