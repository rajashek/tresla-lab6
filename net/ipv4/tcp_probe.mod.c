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

MODULE_INFO(intree, "Y");

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xb58b8065, "module_layout" },
	{ 0xbfc3d13d, "noop_llseek" },
	{ 0xc3fe87c8, "param_ops_uint" },
	{ 0x3ec8886f, "param_ops_int" },
	{ 0x3a3f86d, "unregister_jprobe" },
	{ 0x37a0cba, "kfree" },
	{ 0x3aba531b, "proc_net_remove" },
	{ 0x27e1a049, "printk" },
	{ 0xb94db510, "register_jprobe" },
	{ 0xb4a30bd8, "proc_net_fops_create" },
	{ 0x1bef8a0, "init_net" },
	{ 0x5a34a45c, "__kmalloc" },
	{ 0x6395be94, "__init_waitqueue_head" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0xfa66f77c, "finish_wait" },
	{ 0x5c8b5ce8, "prepare_to_wait" },
	{ 0x1000e51, "schedule" },
	{ 0xc8b57c27, "autoremove_wake_function" },
	{ 0x4f8b5ddb, "_copy_to_user" },
	{ 0xa1c76e0a, "_cond_resched" },
	{ 0x28343bad, "scnprintf" },
	{ 0x77fa5d1f, "ns_to_timespec" },
	{ 0x1e8e3c30, "current_task" },
	{ 0xba63339c, "_raw_spin_unlock_bh" },
	{ 0x1637ff0f, "_raw_spin_lock_bh" },
	{ 0x1b9aca3f, "jprobe_return" },
	{ 0xcf21d241, "__wake_up" },
	{ 0xadaabe1b, "pv_lock_ops" },
	{ 0xc87c1f84, "ktime_get" },
	{ 0xd52bf1ce, "_raw_spin_lock" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "75F60D45272ED3C524C9D76");
