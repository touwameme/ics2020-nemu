NAME = dummy
SRCS = dummy.c
include $(NAVY_HOME)/Makefile
MV_PATH = /home/ltr/ics2020/nanos-lite/build

move: 
	export NAVY_HOME=/home/ltr/ics2020/navy-apps
	make ISA=x86
	mv $(NAVY_HOME)/test/$(NAME)/build/$(NAME)-$(ISA) ramdisk.img
	rm $(MV_PATH)/ramdisk.img
	mv randisk.img  $(MV_PATH)/ramdisk.img

