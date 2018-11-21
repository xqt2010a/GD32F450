#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/of.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/wait.h>
#include <asm/uaccess.h>

#define ADC_NUM_MINORS  3

struct nextvpu_adc
{
    unsigned int SCRATCH;
    unsigned int CTRL1;
    unsigned int CTRL2;
    unsigned int INTR_CTRL;
    unsigned int INTR_CTRL_STS;
};

struct nextvpu_adc_device{
    int irq;
    unsigned int base;
    struct cdev cdev;
};

dev_t devid;
static struct class *cls;
struct cdev adc_cdev;
struct nextvpu_adc_device adcs[ADC_NUM_MINORS];
volatile struct nextvpu_adc *adc_base = NULL;

DECLARE_WAIT_QUEUE_HEAD(wait);

static int adc_open(struct inode *inode, struct file *file)
{
    unsigned int temp;
    int minor = iminor(inode);
    printk("enter %s.\r\n", __func__);
    if ((minor < 0)&&(minor >= ADC_NUM_MINORS)) {
        printk("%s: minor error.\r\n", __func__);
    }
    adc_base = (struct nextvpu_adc *)adcs[minor].base;
    adc_base->SCRATCH = 0x0a;
    adc_base->CTRL1 = (unsigned int)(1<<31);
    adc_base->CTRL2 = 0;
    adc_base->CTRL1 = (unsigned int)((1<<0)|(1<<1)|(1<<31));
    temp = adc_base->CTRL1;
    temp |= (unsigned int)(1<<10);
    adc_base->CTRL1 = temp;
    return 0;
}

static int adc_release(struct inode *inode, struct file *file)
{
    printk("enter %s.\r\n", __func__);
    adc_base = NULL;
    return 0;
}

static int adc_read(struct file *file, char *buf, size_t count, loff_t *off)
{
    int data, ret;
    
    printk("enter %s.\r\n", __func__);
    adc_base->INTR_CTRL = 1;
    wait_event_interruptible(wait, ((adc_base->INTR_CTRL_STS >> 31) & 0x01));
    data = adc_base->INTR_CTRL_STS & 0xFFF;
    ret = copy_to_user(buf, &data, count);
    printk("data:%x\r\n", data);
    if (ret < 0){
        printk("copy_to_user error\r\n");
        return -EFAULT;
    }
    return count;
}

static long adc_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    return 0;
}

static struct file_operations adc_fops = {
    .owner = THIS_MODULE,
    .open = adc_open,
    .read = adc_read,
    .unlocked_ioctl = adc_ioctl,
    .release = adc_release,
};

static irqreturn_t adc_isr(int irq, void *dev_id)
{
    printk("enter irq\r\n");
    wake_up(&wait);
    adc_base->INTR_CTRL_STS = (unsigned int)(1<<31);  //clear interrupt
    return IRQ_HANDLED;
}

static int adc_probe(struct platform_device *pdev)
{
    struct resource *res = NULL;
    int major, minor, irq;
    unsigned int adc_addr;

    printk("enter %s.\r\n", __func__);

    /* priv = devm_kzalloc(&pdev->dev, sizeof(struct nextvpu_adc_device), GFP_KERNEL);
    if(!priv){
        return -ENOMEM;
    }
    platform_set_drvdata(pdev, priv); */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if(res == NULL){
        printk(KERN_ERR "%s: Driver Initialisation failed\n", __func__);
        return -EINVAL;
    }
    adc_addr = (unsigned int)devm_ioremap_resource(&pdev->dev, res);

    irq = platform_get_irq(pdev, 0);
    if(irq < 0){
        dev_err(&pdev->dev, "no irq resource\r\n");
        return -EINVAL;
    }

    if(request_irq(irq, adc_isr, 0, "adc", NULL) < 0){
        dev_err(&pdev->dev, "request_irq error.\r\n");
        return -EINVAL;
    }
    minor = (unsigned char)*(res->name+4)-0x30;

    printk("num:%d, adc_base:%x, irq=%d, name:%s.\r\n", minor, (unsigned int)res->start, irq, res->name);
    
    if((minor < 0)&&(minor >= ADC_NUM_MINORS)){
        printk("adc minor error.\r\n");
        return -EINVAL;
    }
    major = MAJOR(devid);
    devid = MKDEV(major, minor);
    //cdev_init(&adcs[minor].cdev, &adc_fops);
    //cdev_add(&adcs[minor].cdev, devid, ADC_NUM_MINORS);
    cdev_init(&adc_cdev, &adc_fops);
    cdev_add(&adc_cdev, devid, ADC_NUM_MINORS);

    adcs[minor].irq = irq;
    adcs[minor].base = adc_addr;//(unsigned int)res->start;
    printk("adc maid:%d, miid:%d.\r\n",MAJOR(devid),MINOR(devid));
    device_create(cls, NULL, devid, NULL, "adc-%d", minor);
    return 0;
}

static int adc_remove(struct platform_device *pdev)
{
    struct resource *res = NULL;
    int major, minor, irq;

    printk("enter %s.\r\n", __func__);
    
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    irq = platform_get_irq(pdev, 0);
    major = MAJOR(devid);
    minor = (unsigned char)*(res->name+4)-0x30;
    device_destroy(cls, MKDEV(major, minor));
    //cdev_del(&adc_cdev);
    //unregister_chrdev_region(MKDEV(major, minor), ADC_NUM_MINORS);
    free_irq(irq, NULL);
    return 0;
}

static const struct of_device_id adc_dt_ids[] = {
    {.compatible = "nextvpu,dwc-adc", },
    {},
};

static struct platform_driver adc_driver = {
    .driver = {
        .name = "nextvpu_adc",
        .of_match_table = of_match_ptr(adc_dt_ids),
    },
    .probe = adc_probe,
    .remove = adc_remove,
}; 

static int adc_init(void)
{
    printk("enter %s.\r\n", __func__);
    cls = class_create(THIS_MODULE, "nextvpu-dwc-adc");
    alloc_chrdev_region(&devid, 0, ADC_NUM_MINORS, "adc");
    
    return platform_driver_register(&adc_driver);
}

static void adc_exit(void)
{
    printk("enter %s.\r\n", __func__);
    class_destroy(cls);
    platform_driver_unregister(&adc_driver);
}

module_init(adc_init);
module_exit(adc_exit);
MODULE_LICENSE("GPL");