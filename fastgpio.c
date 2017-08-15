#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <asm/uaccess.h>

static dev_t gpio_dev_nr;
static struct cdev *driver_object;
static struct class *gpio_class;
static struct device *gpio_dev;

static int driver_open( struct inode *geraete_datei,struct file *instanz)
{
	int err;
	err = gpio_request(4,"rpi-gpio-4");
	if (err) {
		printk("gpio_request %d\n",err);
		return -1;
	}
	err = gpio_direction_output(4,0);
	if (err) {
		printk("gpio_dir_output %d\n",err);
		gpio_free(4);
		return -1;
	}
	printk("gpio 4 configured\n");
	return 0;
}

static int driver_close( struct inode *geraete_datei, struct file *instanz)
{
	printk( "driver_close called\n" );
	gpio_free( 4 );
	return 0;
}

static ssize_t driver_write(struct file *instanz, const char __user *user, size_t count, loff_t *offset)
{
	unsigned long not_copied, to_copy;
	u32 value=0;
	
	to_copy = min(count, sizeof(value));
	not_copied = copy_from_user(&value,user,to_copy);
	if (value==1)
		gpio_set_value(4,1);
	else
		gpio_set_value(4,0);
	return to_copy-not_copied;
}

static struct file_operations fops = {
	.owner= THIS_MODULE,
	.write= driver_write,
	.open= driver_open,
	.release= driver_close,
};

static int __init mod_init( void )
{
	if(alloc_chrdev_region(&gpio_dev_nr,0,1,"fastgpio4")<0)
		return -EIO;
	driver_object = cdev_alloc();
	if (driver_object==NULL)
		goto free_device_number;
	driver_object->owner = THIS_MODULE;
	driver_object->ops = &fops;
	if(cdev_add(driver_object,gpio_dev_nr,1))
		goto free_cdev;
	gpio_class = class_create(THIS_MODULE,"fastgpio4");
	if(IS_ERR(gpio_class))
	{
		pr_err("fastgpio4:no udev support\n");
		goto free_cdev;
	}
	gpio_dev=device_create(gpio_class,NULL,gpio_dev_nr,NULL,"%s","fastgpio4");
	dev_info(gpio_dev, "mod_init");
	return 0;
	free_cdev:
		kobject_put( &driver_object->kobj );
	free_device_number:
		unregister_chrdev_region(gpio_dev_nr,1);
		return -EIO;
}

static void __exit mod_exit(void)
{
	dev_info(gpio_dev, "mod_exit");
	device_destroy(gpio_class,gpio_dev_nr);
	class_destroy( gpio_class );
	cdev_del( driver_object );
	unregister_chrdev_region(gpio_dev_nr,1);
	return;
}

module_init( mod_init );
module_exit( mod_exit );
MODULE_LICENSE("GPL");
