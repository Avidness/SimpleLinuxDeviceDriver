#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#define DEVICE_NAME "tinymod"
#define CLASS_NAME "tinymd"
#define BUFFER_MAX 1000

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alan Ness, Connor Roggero, Evan Glazer");
MODULE_DESCRIPTION("A simple linux char driver");
MODULE_VERSION("0.1");

static int majorNumber;
static char message[BUFFER_MAX];
static long size_of_message = 0;
static int numberOpens = 0;
static struct class* tinymodClass = NULL;
static struct device* tinymodDevice = NULL;

// Functions required by character driver
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

	//Create Major Number and Check for Errors
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
	printk(KERN_INFO "tinymod: device has been opened %d time(s)\n", numberOpens);
	return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
	int error_id = 0, i = 0;
	long requestedlength = len;
	long length_to_send = requestedlength;

	// Only return what is actually in the message if the user requests more
	if(size_of_message <= requestedlength){
		length_to_send = size_of_message;
	}

	printk(KERN_INFO "tinymod: Current message: '%s', messagelength: %lu", message, size_of_message);

	error_id = copy_to_user(buffer, message, length_to_send);
	if (error_id == 0) {
		printk(KERN_INFO "tinymod: Sent %lu characters to the user\n", requestedlength);

		// Wipe length of the readout
		printk(KERN_INFO "Message: ");
		for(i = 0; i < length_to_send; i++) {
			printk("%c", message[i]);
			message[i] = '\0';	
		} 

		// If we haven't wiped the whole message
		if(size_of_message > requestedlength){

			// Shift the remaining characters towards the head of the queue
			for(i = 0; i < BUFFER_MAX; i++) {
		  		message[i] = message[i + len];
			}
		}

		size_of_message = strlen(message);
		printk(KERN_INFO "New message: '%s', Size: %lu\n", message, size_of_message);
		return 0; // Success
	}
	else {
		printk(KERN_INFO "tinymod: Failed to send %d to the user\n", error_id);
		return -EFAULT; // Failed -- return a bad address message (i.e. -14)
	}
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
	int i = 0, j = 0;
	long buflength = len - 1;

	printk(KERN_INFO "Existing message: '%s', Size: %lu, Buflen: %lu \nPending addition:'%s'\n", message, size_of_message, buflength, buffer);

	// Loop through all possible 1000 bytes
	for(i=0; i<BUFFER_MAX; i++) {
		
		// Copy a new character from 'buffer' into 'message' if:
		// 	1. We have finished looping over the existing characters in 'message' AND
		//	2. We haven't iterated past the end of the buffer AND
		//	3. We haven't reached the end of the 1000 bytes
		if(i >= size_of_message && i < (size_of_message+buflength) && i < BUFFER_MAX){
			message[i] = buffer[j];
			printk(KERN_INFO "Adding %c, at index %d\n", buffer[j], i);
			j++;
		}
	}
	size_of_message = strlen(message);
	printk(KERN_INFO "New message: '%s' \nMessage size: %lu\n", message, size_of_message);
	return size_of_message;
}

static int dev_release(struct inode *inodep, struct file *filep){
	printk(KERN_INFO "Device successfully closed\n");
	return 0;
}

module_init(tinymod_init);
module_exit(tinymod_exit);
