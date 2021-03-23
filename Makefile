# Makefile - makefile of our first driver

# if KERNELRELEASE is not defined, we've been called directly from the command line.
# Invoke the kernel build system.
ifeq (${KERNELRELEASE},)
	#if KERNEL_SRC is not defined, we assume it to be /lib/modules/etc... otherwise we take it
	ifeq (${KERNEL_SRC},)
		KERNEL_SRC := /lib/modules/$(shell uname -r)/build
	endif
	PWD := $(shell pwd)
default:
	${MAKE} -C ${KERNEL_SRC} M=${PWD} modules

modules_install:
	${MAKE} -C ${KERNEL_SRC} M=${PWD} modules_install

clean:
	${MAKE} -C  ${KERNEL_SRC} M=${PWD} clean

# Otherwise KERNELRELEASE is defined; we've been invoked from the
# kernel build system and can use its language.
else
	obj-m := ofd.o
endif
