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
	{ 0x2b22e0c3, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x2b056eed, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x1742baec, __VMLINUX_SYMBOL_STR(single_open) },
	{ 0x630034ef, __VMLINUX_SYMBOL_STR(single_release) },
	{ 0xf8eb7e38, __VMLINUX_SYMBOL_STR(skb_clone) },
	{ 0xa05f372e, __VMLINUX_SYMBOL_STR(seq_printf) },
	{ 0xce814a29, __VMLINUX_SYMBOL_STR(remove_proc_entry) },
	{ 0x91715312, __VMLINUX_SYMBOL_STR(sprintf) },
	{ 0x79bc9e36, __VMLINUX_SYMBOL_STR(seq_read) },
	{ 0x5f395d28, __VMLINUX_SYMBOL_STR(kthread_create_on_node) },
	{ 0xd4e840fa, __VMLINUX_SYMBOL_STR(kthread_stop) },
	{ 0x449ad0a7, __VMLINUX_SYMBOL_STR(memcmp) },
	{ 0xec854e59, __VMLINUX_SYMBOL_STR(skb_push) },
	{ 0xcac9246d, __VMLINUX_SYMBOL_STR(dev_remove_pack) },
	{ 0xca40afbf, __VMLINUX_SYMBOL_STR(init_net) },
	{ 0xd2ca0a37, __VMLINUX_SYMBOL_STR(__alloc_skb) },
	{ 0xf0fdf6cb, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{ 0x75654026, __VMLINUX_SYMBOL_STR(kfree_skb) },
	{ 0xa3e7df22, __VMLINUX_SYMBOL_STR(wake_up_process) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
	{ 0xcc5005fe, __VMLINUX_SYMBOL_STR(msleep_interruptible) },
	{ 0xb153f6c5, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0xb3f7646e, __VMLINUX_SYMBOL_STR(kthread_should_stop) },
	{ 0x736fc972, __VMLINUX_SYMBOL_STR(proc_create_data) },
	{ 0x9d05da80, __VMLINUX_SYMBOL_STR(seq_lseek) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0xb378b3df, __VMLINUX_SYMBOL_STR(dev_add_pack) },
	{ 0xc646b449, __VMLINUX_SYMBOL_STR(consume_skb) },
	{ 0x27e0efc9, __VMLINUX_SYMBOL_STR(dev_queue_xmit) },
	{ 0x1ff8d2e8, __VMLINUX_SYMBOL_STR(skb_put) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "30B4404EF8A57A5235C4322");
