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


/* 模块入口和出口 */
static int __init dtsof_init(void)
{

	int ret = 0;
	struct device_node *nd;/* 节点 */
	struct property		*comppro;
	const char *str;
	u32 def_value = 0;
	u32 elemsize = 0;
	u32 *brival;
	u8 i = 0 ;


	/* 找到backlight节点 */
	nd = of_find_node_by_path("/backlight");
	if(nd == NULL)/* 失败 */
	{
		ret = -EINVAL;
		goto fail_findnd;

	}
	/* 获取属性 */
	comppro = of_find_property(nd, "compatible", NULL);
	if(comppro == NULL)/* 失败 */
	{
		ret = -EINVAL;
		goto fail_findpro;

	}else{
		printk("compatible = %s\r\n",(char*)comppro -> value);

	}
	/* 
	extern int of_property_read_string(struct device_node *np,
				   const char *propname,
				   const char **out_string);
	*/
	ret = of_property_read_string(nd, "status", &str);
	if(ret < 0){
		goto fail_rs;
	}else{
		printk("status = %s\r\n",str);
	}
	/* 读取 数字属性值*/
	ret = of_property_read_u32(nd, "default-brightness-level",&def_value);
	if(ret < 0){
		goto fail_read32;
	}else{
		printk("default-brightness-level = %d\r\n",def_value);
	}

	/* 获取数组类型属性 */
	/* 
	static inline int of_property_count_elems_of_size(const struct device_node *np,
			const char *propname, int elem_size)
	{
		return -ENOSYS;
	}	
	*/
	elemsize = of_property_count_elems_of_size(nd, "brightness-levels", sizeof(u32));
	if(elemsize < 0){
		ret = -EINVAL;
		goto fail_readelems;
	}else{
		printk("brightness-levels elems size = %d\r\n",elemsize);
	}
	/* 申请内存 */
	brival = kmalloc(elemsize * sizeof(u32), GFP_KERNEL);
	if(!brival){
		ret = -EINVAL;
		goto fail_mem;
	}
	/* 获取数组 */
	ret = of_property_read_u32_array(nd,"brightness-levels", brival, elemsize);
	if(ret < 0){
		goto fail_readu32array;
	}else{
		for(i = 0; i < elemsize; i++){
			printk("brightness-levels[%d] = %d\r\n", i, brival[i]);
		}
		
	}

	kfree(brival);


	return 0;




	fail_readu32array:
		kfree(brival);
	fail_mem:
	fail_readelems:
	fail_read32:
	fail_rs:
	fail_findnd:
	fail_findpro:
	return ret;
}

static void __exit dtsof_exit(void){


}




/* 注册和卸载 */
module_init(dtsof_init);
module_exit(dtsof_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("fzp");


