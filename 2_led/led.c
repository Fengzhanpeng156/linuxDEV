#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define LED_MAJOR	200
#define LED_NAME	"LED"

static int led_open(struct inode *inode, struct file *filp){



	return 0;
}

static int led_close(struct inode *inode, struct file *filp){



	return 0;
}

static ssize_t led_write(struct file *filp, const char __user *buf,
	size_t count, loff_t *ppos)
{



	return 0;
}


/* 字符设备操作集 */
static const struct file_operations led_fops = {
	.owner	= THIS_MODULE,
	// .read	= cmm_read,
	.write	= led_write,
	// .unlocked_ioctl	= cmm_ioctl,
	.open	= led_open,
	.release= led_close,
	// .llseek = no_llseek,
};




//entry
static int __init led_init(void)
{
	int ret = 0;
	//注册字符设备
/* 	static inline int register_chrdev(unsigned int major, const char *name,
		const struct file_operations *fops) */
	ret = register_chrdev(LED_MAJOR, LED_NAME, &led_fops);
	if (ret < 0)
	{
		printk("register failed!\r\n");
		return -EIO;
		/* code */
	}
	

	printk("led_init\r\n");

	return 0;
}


//exit
static void  __exit led_exit(void)
{
	unregister_chrdev(LED_MAJOR, LED_NAME);
	printk("led_exit\r\n");

}



// 注册驱动加载和卸载
module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("FZP");


