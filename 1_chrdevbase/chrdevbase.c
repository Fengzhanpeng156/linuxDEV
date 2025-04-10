#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>


#define CHRDEVBASE_MAJOR 200
#define CHEDEVBASE_NAME  "CHRDEVBASE"

static char readbuf[10]; //read buf
static char writebuf[10]; //write buf
static char kerneldata[] = {"kenel data!"};


static int chrdevbase_open(struct inode *inode, struct file *filp){
	// printk("chrdevbase_open\r\n");
	return 0;
}

static int chedevbase_release(struct inode *inode, struct file *filp){
	// printk("chedevbase_release\r\n");
	return 0;
}

static ssize_t chrdevbase_read(struct file *filp, __user char *buf, size_t count,
	loff_t *ppos){
	int ret = 0 ;
	// printk("chedevbase_read\r\n");
	memcpy(readbuf, kerneldata, sizeof(kerneldata));
	ret = copy_to_user(buf,readbuf, count);
	if (ret == 0)
	{
		/* code */
	}
	
	return 0;

}

static ssize_t chrdevbase_write(struct file *filp, const char __user *buf,
	size_t count, loff_t *ppos){
		int ret = 0;
		// printk("chedevbase_write\r\n");
		ret = copy_from_user(writebuf, buf, count);
		if(ret == 0){
		printk("kernel data: %s\r\n",writebuf);

		}
		return 0;
}


static struct file_operations chrdevbase_fops={
	.owner = THIS_MODULE,
	.open = chrdevbase_open,
	.release = chedevbase_release,
	.read = chrdevbase_read,
	.write = chrdevbase_write,


};

static int __init chrdevbase_init(void)
{
	int ret = 0;
	printk("chrdevbase_init\r\n");
	ret = register_chrdev(CHRDEVBASE_MAJOR, CHEDEVBASE_NAME,
						&chrdevbase_fops);
	if (ret < 0)
	{
		printk("chrdevbase init failed\r\n");
		/* code */
	}

	return 0;
}

static void __exit chrdevbase_exit(void)
{

	printk("chrdevbase_exit\r\n");
	unregister_chrdev(CHRDEVBASE_MAJOR, CHEDEVBASE_NAME);
}

//mod init and exit






module_init(chrdevbase_init);
module_exit(chrdevbase_exit);
MODULE_LICENSE("GPL");






