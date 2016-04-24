#SimpleLinuxDeviceDriver

#############
# 1. Set up #
#############

a) Extract the contents of the zip file into an empty dedicated directory.

b) Run 'uname -a' and make a note the kernel version you are running. (E.g., 4.2.0-27-generic)

c) Open the Makefile. 

d) Edit the kernel version if necessary. 

e) Edit the file path to point to the directory you chose.

################
# 2. Compiling #
################

a) On the console, run 'make'.

b) Run 'sudo insmod tinymod.ko'

c) 'dmesg' to confirm the device was correctly initiated.

###########
# 3. Test #
###########

a) To write to the buffer: 
	sudo bash -c 'echo "test1" > /dev/tinymod' // running through bash will override any permission issues
	
b) to read from the buffer: 
	sudo cat /dev/tinymod

c) To remove the module:
	sudo rmmod tinymod
	sudo rm /dev/tinymod // ensures the module was fully removed
