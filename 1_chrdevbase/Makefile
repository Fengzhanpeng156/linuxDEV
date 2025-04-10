# KERNELDIR :=/home/feng/linux/IMX6ULL/linux/linux-imx-rel_imx_4.1.15_2.1.0_ga

# CURRENT_PATH := $(shell pwd)
# obj-m := chrdevbase.o
# bulid: kernel_modules

# kernel_modules:
# 	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules 

# clean:
# 	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean

KERNELDIR := /home/feng/linux/IMX6ULL/linux/linux-imx-rel_imx_4.1.15_2.1.0_ga
CURRENT_PATH := $(shell pwd)
CROSS_COMPILE := arm-linux-gnueabihf-
ARCH := arm

obj-m := chrdevbase.o

build: kernel_modules

kernel_modules:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules 

clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) clean
