#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
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
	{ 0x896b5afa, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x2c6cc3d, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0xd2b09ce5, __VMLINUX_SYMBOL_STR(__kmalloc) },
	{ 0xd5aef0e9, __VMLINUX_SYMBOL_STR(nbt_disassociate) },
	{ 0xa43588f5, __VMLINUX_SYMBOL_STR(skb_clone) },
	{ 0x771ade11, __VMLINUX_SYMBOL_STR(get_dev) },
	{ 0x3cb13ffc, __VMLINUX_SYMBOL_STR(kthread_create_on_node) },
	{ 0x5c86511c, __VMLINUX_SYMBOL_STR(nbt_get_mac) },
	{ 0x6244ac5d, __VMLINUX_SYMBOL_STR(nbt_hash_func) },
	{ 0xfb578fc5, __VMLINUX_SYMBOL_STR(memset) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x219750a, __VMLINUX_SYMBOL_STR(kthread_stop) },
	{ 0x449ad0a7, __VMLINUX_SYMBOL_STR(memcmp) },
	{ 0x18337716, __VMLINUX_SYMBOL_STR(dev_remove_pack) },
	{ 0x4d85d2db, __VMLINUX_SYMBOL_STR(__alloc_skb) },
	{ 0xf0fdf6cb, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{ 0x77bde766, __VMLINUX_SYMBOL_STR(kfree_skb) },
	{ 0x415f0eb, __VMLINUX_SYMBOL_STR(maccmp) },
	{ 0x98b2d759, __VMLINUX_SYMBOL_STR(wake_up_process) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
	{ 0xcc5005fe, __VMLINUX_SYMBOL_STR(msleep_interruptible) },
	{ 0x6120f71c, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0xb3f7646e, __VMLINUX_SYMBOL_STR(kthread_should_stop) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x69acdf38, __VMLINUX_SYMBOL_STR(memcpy) },
	{ 0x62f8722, __VMLINUX_SYMBOL_STR(nbt_destroy) },
	{ 0x80cb39de, __VMLINUX_SYMBOL_STR(dev_add_pack) },
	{ 0xb1e54c24, __VMLINUX_SYMBOL_STR(consume_skb) },
	{ 0xe4033da4, __VMLINUX_SYMBOL_STR(dev_queue_xmit) },
	{ 0xbdff24ba, __VMLINUX_SYMBOL_STR(skb_put) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=nbt";


MODULE_INFO(srcversion, "679E91EC0DE1ADAA950BD8D");
