#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

static dev_t first; // Global variable for the first device number
static struct cdev c_dev; // Global variable for the character device structure
static struct class *cl; // Global variable for the device class
static char *data = NULL;
static size_t data_len;
static DEFINE_MUTEX(data_mutex);

// Here we define our file operations functions
static int my_open(struct inode *i, struct file *f)
{
	printk(KERN_INFO "Driver: open()\n");
	return 0;
}

static int my_close(struct inode *i, struct file *f)
{
	printk(KERN_INFO "Driver: close()\n");
	return 0;
}

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
	
	ssize_t retval = 0;
	char *start;
	printk(KERN_INFO "Driver: read()\n");

	mutex_lock(&data_mutex); // Here we lock the data to avoid concurency
	
	if (!data)
	{
		retval = -EINVAL;
		goto out;
	}

	if (*off >= data_len)
		goto out; // EOF

	start = data + *off;
	retval = data_len - *off;

	if (retval > len)
		retval = len;

	if (copy_to_user(buf, start, retval) !=0)
	{
		retval = -EFAULT;
		goto out;
	}

	*off += retval;

out:
	mutex_unlock(&data_mutex);
	return retval;
}

static ssize_t my_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
	
	ssize_t retval = -ENOMEM;
	printk(KERN_INFO "Driver: write()\n");
	mutex_lock(&data_mutex);

	if (data)
		kfree(data);

	data = kmalloc(len, GFP_KERNEL);

	if (!data)
		goto out;

	if (copy_from_user(data, buf, len))
	{
		kfree(data);
		retval = -EFAULT;
		goto out;
	}
	
	*off = len;
	retval = len;
	data_len = len;

out:

	mutex_unlock(&data_mutex);
	return retval;
}

static struct file_operations my_fops =
{
	.owner = THIS_MODULE,
	.open = my_open,
	.release = my_close,
	.read = my_read,
	.write = my_write
};

static int __init my_init(void)
{
	int ret;
	struct device *dev_ret;
	printk(KERN_INFO "Hey! lkm_example module registered!");
	if ((ret = alloc_chrdev_region(&first, 0, 1, "lkm_example")))
	{
		return ret;
	}

	if (IS_ERR(cl = class_create(THIS_MODULE, "chardrv")))
	{
		unregister_chrdev_region(first, 1);
		return PTR_ERR(cl);
	}

	if (IS_ERR(dev_ret = device_create(cl, NULL, first, NULL, "lkm_example")))
	{
		class_destroy(cl);
		unregister_chrdev_region(first, 1);
		return PTR_ERR(dev_ret);
	}

	cdev_init(&c_dev, &my_fops); // Link the file operations to the device
	if ((ret = cdev_add(&c_dev, first, 1)) < 0)
	{
		device_destroy(cl, first);
		class_destroy(cl);
		unregister_chrdev_region(first, 1);
		return ret;
	}

	return 0;
}

static void __exit my_exit(void)
{
	cdev_del(&c_dev);
	device_destroy(cl, first);
	class_destroy(cl);
	unregister_chrdev_region(first, 1);
	printk(KERN_INFO "Goodby! lkm_example module unregistered");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Thomas Dalla Piazza <tdallapiazza@digger.ch>");
MODULE_DESCRIPTION("A parrot kernel module driver!");
