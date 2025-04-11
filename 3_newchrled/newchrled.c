#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/cdev.h>

#define LED_MAJOR	200
#define NEWCHRLED_NAME	"NEWCHRLED"
#define NEWCHRCOUNT	1
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


void led_switch(u8 sta)
{
	u32 val = 0;
	if(sta == LEDON) {
		val = readl(GPIO1_DR);
		val &= ~(1 << 3);	
		writel(val, GPIO1_DR);
	}else if(sta == LEDOFF) {
		val = readl(GPIO1_DR);
		val|= (1 << 3);	
		writel(val, GPIO1_DR);
	}	
}

/* 
void cdev_init(struct cdev *cdev, const struct file_operations *fops)
*/
/* LED设备结构体 */
struct newchrled_dev{
	struct cdev cdev;	/* 字符设备 */
	dev_t	devid;
	int		major;
	int		minor;
};

static int newchrled_open(struct inode *inode, struct file *filp){

	return 0;
}

static int newchrled_release(struct inode *inode, struct file *filp){

	return 0;
}

static ssize_t newchrled_write(struct file *filp, const char __user *buf, size_t cnt, loff_t *offt){
	int retvalue;
	unsigned char databuf[1];
	unsigned char ledstat;

	retvalue = copy_from_user(databuf, buf, cnt);
	if(retvalue < 0) {
		printk("kernel write failed!\r\n");
		return -EFAULT;
	}

	ledstat = databuf[0];		/* 获取状态值 */

	if(ledstat == LEDON) {	
		led_switch(LEDON);		/* 打开LED灯 */
	} else if(ledstat == LEDOFF) {
		led_switch(LEDOFF);	/* 关闭LED灯 */
	}
	return 0;
}




static const struct file_operations newchrled_fops = {
	.owner 		= THIS_MODULE,
	.open 		= newchrled_open,
	// .read 		= newchrled_read,
	.write 		= newchrled_write,
	.release 	= newchrled_release,
};

struct newchrled_dev newchrled;


/* 入口 */
static int __init newchrled_init(void)
{
	int ret = 0;
	// int retvalue = 0;
	u32 val = 0;
	/* 初始化led */
	printk("newchrled_init!\r\n");
	IMX6U_CCM_CCGR1 = ioremap(CCM_CCGR1_BASE, 4);
	SW_MUX_GPIO1_IO03 = ioremap(SW_MUX_GPIO1_IO03_BASE, 4);
  	SW_PAD_GPIO1_IO03 = ioremap(SW_PAD_GPIO1_IO03_BASE, 4);
	GPIO1_DR = ioremap(GPIO1_DR_BASE, 4);
	GPIO1_GDIR = ioremap(GPIO1_GDIR_BASE, 4);

	/* 2、使能GPIO1时钟 */
	val = readl(IMX6U_CCM_CCGR1);
	val &= ~(3 << 26);	/* 清楚以前的设置 */
	val |= (3 << 26);	/* 设置新值 */
	writel(val, IMX6U_CCM_CCGR1);

	/* 3、设置GPIO1_IO03的复用功能，将其复用为
	 *    GPIO1_IO03，最后设置IO属性。
	 */
	writel(5, SW_MUX_GPIO1_IO03);

	writel(0x10B0, SW_PAD_GPIO1_IO03);

	/* 4、设置GPIO1_IO03为输出功能 */
	val = readl(GPIO1_GDIR);
	val &= ~(1 << 3);	/* 清除以前的设置 */
	val |= (1 << 3);	/* 设置为输出 */
	writel(val, GPIO1_GDIR);

	/* 5、默认关闭LED */
	val = readl(GPIO1_DR);
	val |= (1 << 3);	
	writel(val, GPIO1_DR);
	/* 注册设备号 */
	if (newchrled.major) {
		newchrled.devid = MKDEV(newchrled.major, 0);
		ret = register_chrdev_region(newchrled.devid, 1, "NEWCHRLED_NAME");
	} else {
		ret = alloc_chrdev_region(&newchrled.devid, 0, 1, "NEWCHRLED_NAME");
		newchrled.major = MAJOR(newchrled.devid);
		newchrled.minor = MINOR(newchrled.devid);
	}
	if(ret < 0)
	{
		printk("chrdev_region err!\r\n");
		return -1;
	}else{
		printk("newchrled major=%d, minor=%d\r\n", newchrled.major, newchrled.minor);

	}
	/* 注册字符设备 */
	newchrled.cdev.owner = THIS_MODULE;
	cdev_init(&newchrled.cdev, &newchrled_fops);
	ret = cdev_add(&newchrled.cdev, newchrled.devid, NEWCHRCOUNT);




	return 0;
}

/* 出口 */
static void __exit newchrled_exit(void)
{
	printk("exit!\r\n");
	/* 删除字符设备 */
	cdev_del(&newchrled.cdev);
	iounmap(IMX6U_CCM_CCGR1);
	iounmap(SW_MUX_GPIO1_IO03);
	iounmap(SW_PAD_GPIO1_IO03);
	iounmap(GPIO1_DR);
	iounmap(GPIO1_GDIR);

	/* 注销设备号 */
	unregister_chrdev_region(newchrled.devid, NEWCHRCOUNT);
}




/* 注册和卸载 */
module_init(newchrled_init);
module_exit(newchrled_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("fzp");


