#SimpleLinuxDeviceDriver

#############
# 1. Set up #
#############

a) Extract the contents of the zip file into an empty dedicated directory.

b) Go into the directory and run 'make'.

c) Run 'sudo insmod tinymod.ko'

d) 'dmesg' to confirm the device was correctly initiated.

###########
# 2. Test #
###########

a) To write to the buffer: 
	sudo bash -c 'echo "test1" > /dev/tinymod' // running through bash will override any permission issues
	
b) To read the full buffer: 
	sudo cat /dev/tinymod

c) To read a single character from the buffer (FIFO)
	sudo head -c 1 /dev/tinymod

d) To remove the module:
	sudo rmmod tinymod
