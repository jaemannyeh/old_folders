
#include <linux/init.h>
#include <linux/module.h>
MODULE_LICENSE("Dual BSD/GPL");

static int hello_integer;

static void hello_function()
{
}

static int hello_init(void)
{
    printk(KERN_ALERT "hello, world 0x%08x 0x%08x\n",&hello_integer,hello_function);
    return 0;
}

static void hello_exit(void)
{
    printk(KERN_ALERT "Goodbye, cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);

