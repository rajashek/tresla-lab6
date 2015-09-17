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
	{ 0xfa08cdda, "tcp_reno_min_cwnd" },
	{ 0xbba20feb, "tcp_reno_ssthresh" },
	{ 0x3ec8886f, "param_ops_int" },
	{ 0x7aa2dd9a, "tcp_unregister_congestion_control" },
	{ 0x607a0337, "tcp_register_congestion_control" },
	{ 0x3b2d361c, "tcp_reno_cong_avoid" },
	{ 0xc7aa5f58, "tcp_is_cwnd_limited" },
	{ 0x3bd1b1f6, "msecs_to_jiffies" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "6C66D5268B9C1187B4C83F5");
