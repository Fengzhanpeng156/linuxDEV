#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>

#define LED_MAJOR	200
#define NEWCHRLED_NAME	"NEWCHRLED"
#define NEWCHRCOUNT	1
/* 寄存器物理地址 */
// #define CCM_CCGR1_BASE			(0X020C406C)
// #define SW_MUX_GPIO1_IO03_BASE	(0X020E0068)
// #define SW_PAD_GPIO1_IO03_BASE	(0X020E02F4)
// #define GPIO1_DR_BASE			(0X0209C000)
// #define GPIO1_GDIR_BASE			(0X0209C004)

// /* 地址映射后的虚拟地质指针 */
// static void __iomem *IMX6U_CCM_CCGR1;
// static void __iomem *SW_MUX_GPIO1_IO03;
// static void __iomem *SW_PAD_GPIO1_IO03;
// static void __iomem *GPIO1_DR;
// static void __iomem *GPIO1_GDIR;

// #define LEDOFF	0
// #define LEDON	1



/* LED设备结构体 */
struct newchrled_dev{
	dev_t	devid;
	int		major;
	int		minor;
};

struct newchrled_dev newchrled;


/* 入口 */
static int __init newchrled_init(void)
{
	int ret = 0;
	/* 初始化led */

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




	return 0;
}

/* 出口 */
static void __exit newchrled_exit(void)
{

	/* 注销设备号 */
	unregister_chrdev_region(newchrled.devid, NEWCHRCOUNT);
}




/* 注册和卸载 */
module_init(newchrled_init);
module_exit(newchrled_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("fzp");


