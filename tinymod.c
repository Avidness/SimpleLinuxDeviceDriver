#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/init.h>
#include <linux/device.h>
#include <linux/fs.h>

#define DEVICE_NAME "tinymod"

static int majorNumber;
static char message[256] = {0};
static short size_of_message;
static int numberOpens = 0;

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

	return 0;
}

static void __exit tinymod_exit(void){
	printk(KERN_INFO "Removing module.\n");				
}

static int dev_open(struct inode *inodep, struct file *filep){
	numberOpens++;
	printk(KERN_INFO "Tinymod device has been opened %d time(s)\n", numberOpens);
	return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
	//int error_count = 0;

	printk(KERN_INFO "Read instructions unfinished");
	return (size_of_message=0);

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
