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
	{ 0x89a46fa3, "tcp_vegas_get_info" },
	{ 0xe0a448c3, "tcp_vegas_cwnd_event" },
	{ 0x95fde95e, "tcp_vegas_state" },
	{ 0xfa08cdda, "tcp_reno_min_cwnd" },
	{ 0x7aa2dd9a, "tcp_unregister_congestion_control" },
	{ 0x607a0337, "tcp_register_congestion_control" },
	{ 0x67d2dc8c, "tcp_cong_avoid_ai" },
	{ 0xbee5868f, "tcp_slow_start" },
	{ 0xc7aa5f58, "tcp_is_cwnd_limited" },
	{ 0x814f6a55, "tcp_vegas_init" },
	{ 0x65a31e2, "tcp_vegas_pkts_acked" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=tcp_vegas";


MODULE_INFO(srcversion, "5614C869E58C5A944B682A4");
