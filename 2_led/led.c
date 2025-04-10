#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>

#define LED_MAJOR	200
#define LED_NAME	"LED"

/* 寄存器物理地址 */
#define CCM_CCGR1_BASE			(0X020C406C)
#define SW_MUX_GPIO1_IO03_BASE	(0X020E0068)
#define SW_PAD_GPIO1_IO03_BASE	(0X020E02F4)
#define GPIO1_DR_BASE			(0X0209C000)
#define GPIO1_GDIR_BASE			(0X0209C004)

/* 地址映射后的虚拟地质指针 */
static void __iomem *IMX6U_CCM_CCGR1;
static void __iomem *SW_MUX_GPIO1_IO03;
static void __iomem *SW_PAD_GPIO1_IO03;
static void __iomem *GPIO1_DR;
static void __iomem *GPIO1_GDIR;

#define LEDOFF	0
#define LEDON	1



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
	int val = 0;
	/* 初始化led,地址映射 */
	IMX6U_CCM_CCGR1		= ioremap(CCM_CCGR1_BASE, 4);
	SW_MUX_GPIO1_IO03	= ioremap(SW_MUX_GPIO1_IO03_BASE, 4);
	SW_PAD_GPIO1_IO03	= ioremap(SW_PAD_GPIO1_IO03_BASE, 4);
	GPIO1_DR			= ioremap(GPIO1_DR_BASE, 4);
	GPIO1_GDIR			= ioremap(GPIO1_GDIR_BASE, 4);

	/* 时钟初始化 */
	val = readl(IMX6U_CCM_CCGR1);
	val &= ~(3 << 26); /* 先清除以前配置bit26与27 */
	val |= 3 << 26;
	writel(val, IMX6U_CCM_CCGR1);

	writel(0x5, SW_MUX_GPIO1_IO03);
	writel(0x10b0, SW_PAD_GPIO1_IO03);/* 电气属性 */

	val = readl(GPIO1_GDIR);
	val &= ~(1 << 3);	/* bit3置1，设置为输出 */
	val |= (1 << 3);
	writel(val, GPIO1_GDIR);

	val = readl(GPIO1_DR);
	val &= (1 << 3);	
	writel(val, GPIO1_DR);


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
	/* 取消地址映射 */
	iounmap(IMX6U_CCM_CCGR1	);
	iounmap(SW_MUX_GPIO1_IO03);
	iounmap(SW_PAD_GPIO1_IO03);
	iounmap(GPIO1_DR);
	iounmap(GPIO1_GDIR);

	/* 注销字符设备 */
	unregister_chrdev(LED_MAJOR, LED_NAME);
	printk("led_exit\r\n");

}



// 注册驱动加载和卸载
module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("FZP");


