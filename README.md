# lkm_example
This is a very simple character driver acting like a parrot. It has been build as an extension of the examples shown in [this](https://sysplay.github.io/books/LinuxDrivers/book/index.html) very interesting book.

This module includes the creation of device files to interact with the user space.

The principle is simple, the sequence of characters received in the **write** function is stored and retrieved in the **read** function.

## Compile and install
This is shipped with a Makefile so to compile it just issue `make`

To clean your build `make clean`

If you want to install the module, issue as root (or sudo) `make modules_install`

## Usage
This is a parrot. So it will repeate just what you told him!

To told him something: `echo "Hello you gentle parrot!" > /dev/lkm_example`

And to listen to the answer: `cat /dev/lkm_example`
