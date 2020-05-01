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

#ifdef RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x12c2450, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x9580deb, __VMLINUX_SYMBOL_STR(init_timer_key) },
	{ 0xaf104e50, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x9aed71aa, __VMLINUX_SYMBOL_STR(__register_chrdev) },
	{ 0x17a23a24, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0x9d0b8a91, __VMLINUX_SYMBOL_STR(register_netdev) },
	{ 0x9ada94c9, __VMLINUX_SYMBOL_STR(alloc_netdev_mqs) },
	{ 0x29dd89eb, __VMLINUX_SYMBOL_STR(free_netdev) },
	{ 0x1f14a432, __VMLINUX_SYMBOL_STR(unregister_netdev) },
	{ 0x6c09c2a4, __VMLINUX_SYMBOL_STR(del_timer) },
	{ 0x6bc3fbc0, __VMLINUX_SYMBOL_STR(__unregister_chrdev) },
	{ 0xaada717, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x99230a9, __VMLINUX_SYMBOL_STR(class_unregister) },
	{ 0xb1fefe0, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0x9630f8eb, __VMLINUX_SYMBOL_STR(__alloc_skb) },
	{ 0x16e5c2a, __VMLINUX_SYMBOL_STR(mod_timer) },
	{ 0x15ba50a6, __VMLINUX_SYMBOL_STR(jiffies) },
	{ 0xab80f9a1, __VMLINUX_SYMBOL_STR(skb_copy) },
	{ 0x7aa1756e, __VMLINUX_SYMBOL_STR(kvfree) },
	{ 0x33f110aa, __VMLINUX_SYMBOL_STR(skb_put) },
	{ 0x7f8f12e5, __VMLINUX_SYMBOL_STR(consume_skb) },
	{ 0xda3e43d1, __VMLINUX_SYMBOL_STR(_raw_spin_unlock) },
	{ 0xd52bf1ce, __VMLINUX_SYMBOL_STR(_raw_spin_lock) },
	{ 0xd2b09ce5, __VMLINUX_SYMBOL_STR(__kmalloc) },
	{ 0xc3aaf0a9, __VMLINUX_SYMBOL_STR(__put_user_1) },
	{ 0x91715312, __VMLINUX_SYMBOL_STR(sprintf) },
	{ 0xdb7305a1, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{ 0x779a18af, __VMLINUX_SYMBOL_STR(kstrtoll) },
	{ 0x69acdf38, __VMLINUX_SYMBOL_STR(memcpy) },
	{ 0x128b7500, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0xd98397d8, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "534F7AF32C4EA0A447ABC4F");
