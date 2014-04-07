#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x4c46c04, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xbb323702, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0xd2b09ce5, __VMLINUX_SYMBOL_STR(__kmalloc) },
	{ 0x8ffa2322, __VMLINUX_SYMBOL_STR(single_open) },
	{ 0xd5aef0e9, __VMLINUX_SYMBOL_STR(nbt_disassociate) },
	{ 0x7d3420dc, __VMLINUX_SYMBOL_STR(single_release) },
	{ 0x6034be1f, __VMLINUX_SYMBOL_STR(skb_clone) },
	{ 0x5f2ea640, __VMLINUX_SYMBOL_STR(seq_printf) },
	{ 0x48f9a0c5, __VMLINUX_SYMBOL_STR(remove_proc_entry) },
	{ 0x91715312, __VMLINUX_SYMBOL_STR(sprintf) },
	{ 0x4a9cd35b, __VMLINUX_SYMBOL_STR(seq_read) },
	{ 0x68b8d296, __VMLINUX_SYMBOL_STR(get_dev) },
	{ 0xe4e8a2d5, __VMLINUX_SYMBOL_STR(kthread_create_on_node) },
	{ 0x5c86511c, __VMLINUX_SYMBOL_STR(nbt_get_mac) },
	{ 0x6244ac5d, __VMLINUX_SYMBOL_STR(nbt_hash_func) },
	{ 0xfb578fc5, __VMLINUX_SYMBOL_STR(memset) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x1f3147c7, __VMLINUX_SYMBOL_STR(kthread_stop) },
	{ 0x449ad0a7, __VMLINUX_SYMBOL_STR(memcmp) },
	{ 0x6ae2f57e, __VMLINUX_SYMBOL_STR(dev_remove_pack) },
	{ 0xc4964d3d, __VMLINUX_SYMBOL_STR(__alloc_skb) },
	{ 0xf0fdf6cb, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{ 0xb0c0e999, __VMLINUX_SYMBOL_STR(kfree_skb) },
	{ 0x415f0eb, __VMLINUX_SYMBOL_STR(maccmp) },
	{ 0x79bcd2f2, __VMLINUX_SYMBOL_STR(wake_up_process) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
	{ 0xcc5005fe, __VMLINUX_SYMBOL_STR(msleep_interruptible) },
	{ 0x7149fc2a, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0xb3f7646e, __VMLINUX_SYMBOL_STR(kthread_should_stop) },
	{ 0xed798b1, __VMLINUX_SYMBOL_STR(proc_create_data) },
	{ 0x5449ff9, __VMLINUX_SYMBOL_STR(seq_lseek) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x69acdf38, __VMLINUX_SYMBOL_STR(memcpy) },
	{ 0x62f8722, __VMLINUX_SYMBOL_STR(nbt_destroy) },
	{ 0x185b9df4, __VMLINUX_SYMBOL_STR(dev_add_pack) },
	{ 0xda72c5ca, __VMLINUX_SYMBOL_STR(consume_skb) },
	{ 0x16d886e6, __VMLINUX_SYMBOL_STR(dev_queue_xmit) },
	{ 0x66263888, __VMLINUX_SYMBOL_STR(skb_put) },
	{ 0xe914e41e, __VMLINUX_SYMBOL_STR(strcpy) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=nbt";


MODULE_INFO(srcversion, "35140233B33EE0BC8E107A0");
