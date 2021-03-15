# lkm_example
Very simple kernel module example from [https://blog.sourcerer.io/writing-a-simple-linux-kernel-module-d9dc3762c234](https://blog.sourcerer.io/writing-a-simple-linux-kernel-module-d9dc3762c234)

To compile just issue 'make'

To test issue 'make test'

Create a device file

`sudo mknod /dev/lkm_example c MAJOR 0`
Where 'MAJOR' is obtained from the test result
