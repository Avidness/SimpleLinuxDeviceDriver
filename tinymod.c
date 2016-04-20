#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#define DEVICE_NAME "tinymod"
#define CLASS_NAME "tinymd"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alan Ness");
MODULE_DESCRIPTION("A simple linux char driver");
MODULE_VERSION("0.1");

// to test, use echo and cat on the device 'file' in /dev

static int majorNumber;
static char message[256] = {0};
static short size_of_message;
static int numberOpens = 0;
static struct class* tinymodClass = NULL;
static struct device* tinymodDevice = NULL;

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops =
{
	.open = dev_open,
	.read = dev_read,
	.write = dev_write,
	.release = dev_release,
};

static int __init tinymod_init(void){
	printk(KERN_INFO "Installing module.\n");

	majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
	if(majorNumber<0){
		printk(KERN_ALERT "Failed to register major number");
		return majorNumber;
	}
	printk(KERN_INFO "Registered correctly with major number %d\n", majorNumber);

	tinymodClass = class_create(THIS_MODULE, CLASS_NAME);
	if(IS_ERR(tinymodClass)){
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "Failed to register device class\n");
		return PTR_ERR(tinymodClass);
	}
	printk(KERN_INFO "tinymod: device class registered correctly\n");

	tinymodDevice = device_create(tinymodClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
	if(IS_ERR(tinymodDevice)){
		class_destroy(tinymodClass);
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "Failed to create the device\n");
		return PTR_ERR(tinymodDevice);
	}

	printk(KERN_INFO "tinymod: device class created correctly\n");
	return 0;
}

static void __exit tinymod_exit(void){
	device_destroy(tinymodClass, MKDEV(majorNumber, 0));
	class_unregister(tinymodClass);
	class_destroy(tinymodClass);
	unregister_chrdev(majorNumber, DEVICE_NAME);
	printk(KERN_INFO "Removing the tinymod module.\n");
}

static int dev_open(struct inode *inodep, struct file *filep){
	numberOpens++;
	printk(KERN_INFO "Tinymod device has been opened %d time(s)\n", numberOpens);
	return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
	int error_count = 0;
	error_count = copy_to_user(buffer, message, size_of_message);

	if(error_count==0){
		printk(KERN_INFO "tinymod: sent %d characters to the user\n", size_of_message);
		return (size_of_message=0);
	}
	else{
		printk(KERN_INFO "tinymod: Failed to send %d characters to the user\n", error_count);
		return -EFAULT;
	}
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
	sprintf(message, "%s(%d letters)", buffer, len);
	size_of_message = strlen(message);
	printk(KERN_INFO "Received %d characters from the user\n", len);
	return len;
}

static int dev_release(struct inode *inodep, struct file *filep){
	printk(KERN_INFO "Device successfully closed\n");
	return 0;
}

module_init(tinymod_init);
module_exit(tinymod_exit);
