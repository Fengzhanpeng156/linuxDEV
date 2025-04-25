#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/slab.h>

#define DTSLED_CNT 1
#define DTSLED_NAME "dtsled"

/* 地址映射后的虚拟地质指针 */
static void __iomem *IMX6U_CCM_CCGR1;
static void __iomem *SW_MUX_GPIO1_IO03;
static void __iomem *SW_PAD_GPIO1_IO03;
static void __iomem *GPIO1_DR;
static void __iomem *GPIO1_GDIR;

/* dtsled设备结构体 */
struct dtsled_dev{
    dev_t devid;
	struct cdev cdev;
	struct class *class;
	struct device *device;
	int major;
	int minor;
	struct device_node *nd; // 设备树节点
};

struct dtsled_dev dtsled;



// 静态函数，用于打开设备文件
static int dtsled_open(struct inode *inode, struct file *filp){
    // 将设备结构体指针赋值给文件指针的私有数据
    filp->private_data = &dtsled;
	return 0;
};

// 静态函数，用于释放dtsled设备
static int dtsled_release(struct inode *inode, struct file *filp){
    // 将filp->private_data转换为dtsled_dev结构体指针
    struct dtsled_dev *dev = (struct dtsled_dev*)filp->private_data;
	// 返回0，表示成功
	return 0;
};

static ssize_t dtsled_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos){
    // 将filp->private_data转换为dtsled_dev结构体指针
    struct dtsled_dev *dev = (struct dtsled_dev*)filp->private_data;
    // 将用户空间的数据拷贝到内核空间
    
	return 0;
};



/* dtsled设备操作函数 */
static const struct file_operations dtsled_fops = {
	.owner = THIS_MODULE,
	.write = dtsled_write,
	.open = dtsled_open,
	.release = dtsled_release,
};













/* ENTRY */
// 静态初始化函数，用于初始化dtsled模块
static int __init dtsled_init(void){
	
	int ret = 0;
	const char *str;
	u32 regdata[10];
	u8 i = 0;
	u32 val = 0;

	/* 注册字符设备 */
	/* 申请设备号 */
	dtsled.major = 0;
	if(dtsled.major){
		dtsled.devid = MKDEV(dtsled.major, 0);
		ret = register_chrdev_region(dtsled.devid, DTSLED_CNT, DTSLED_NAME);
	}else{
		ret = alloc_chrdev_region(&dtsled.devid, 0, DTSLED_CNT, DTSLED_NAME);
		dtsled.major = MAJOR(dtsled.devid);
		dtsled.minor = MINOR(dtsled.devid);
	}
	if(ret < 0){
		pr_err("can't register char device region.\n");
		goto fail;
	}

	/* 添加字符设备 */
	dtsled.cdev.owner = THIS_MODULE;
	cdev_init(&dtsled.cdev, &dtsled_fops);
	ret = cdev_add(&dtsled.cdev, dtsled.devid, DTSLED_CNT);
	if(ret < 0){
		pr_err("can't add char device.\n");
		goto fail_cdev;
	}
	/* 自动创建设备节点 */
	dtsled.class = class_create(THIS_MODULE, DTSLED_NAME);
	if(IS_ERR(dtsled.class)){
		pr_err("can't create class for device.\n");
		goto fail_class;

	};


	dtsled.device = device_create(dtsled.class, NULL, dtsled.devid, NULL, DTSLED_NAME);
	if(IS_ERR(dtsled.device)){
		pr_err("can't create device.\n");
		goto fail_device;
	};

	/* 获取设备树属性内容 */

	dtsled.nd = of_find_node_by_path("/alphaled");
	if(dtsled.nd == NULL){
		ret = -EINVAL;
		goto fail_findnd;
	};

	/*  */
	ret = of_property_read_string(dtsled.nd, "status", &str);
	if(ret < 0){
		goto fail_findrs;
	}else{
		printk("status = %s\r\n",str);
	}

	ret = of_property_read_string(dtsled.nd, "compatible", &str);
	if(ret < 0){
		goto fail_findrs;
	}else{
		printk("compatible = %s\r\n",str);
	}
	ret = of_property_read_u32_array(dtsled.nd, "reg", regdata, 10);
	if(ret < 0){
		goto fail_findrs;
	}else{
		for( i = 0; i < 10; i++){
			printk("reg = %x  ",regdata[i]);
		}
		printk("\r\n");
	};

	IMX6U_CCM_CCGR1 = ioremap(regdata[0], regdata[1]);
	SW_MUX_GPIO1_IO03 = ioremap(regdata[2], regdata[3]);
  	SW_PAD_GPIO1_IO03 = ioremap(regdata[4], regdata[5]);
	GPIO1_DR = ioremap(regdata[6], regdata[7]);
	GPIO1_GDIR = ioremap(regdata[8], regdata[9]);

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



	return 0;

fail_findrs:
fail_findnd:
	device_destroy(dtsled.class, dtsled.devid);
fail_device:
	class_destroy(dtsled.class);
fail_class:
	cdev_del(&dtsled.cdev);
fail_cdev:
	unregister_chrdev_region(dtsled.devid, DTSLED_CNT);
fail:
	return ret ;

} ;

/* exit */
// 静态退出函数，用于退出dtsled模块
static void __exit dtsled_exit(void){

	/* 删除字符设备 */
	cdev_del(&dtsled.cdev);
	iounmap(IMX6U_CCM_CCGR1);
	iounmap(SW_MUX_GPIO1_IO03);
	iounmap(SW_PAD_GPIO1_IO03);
	iounmap(GPIO1_DR);
	iounmap(GPIO1_GDIR);

	cdev_del(&dtsled.cdev);
    unregister_chrdev_region(dtsled.devid, DTSLED_CNT);
	device_destroy(dtsled.class, dtsled.devid);
	class_destroy(dtsled.class);
    // 退出函数体为空
};









/* 注册驱动与卸载驱动 */

module_init(dtsled_init);
module_exit(dtsled_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("FZP");

