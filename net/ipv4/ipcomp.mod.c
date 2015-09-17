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
	{ 0xb98b45e2, "xfrm4_rcv" },
	{ 0x99f31531, "ipcomp_output" },
	{ 0x4ac00626, "ipcomp_input" },
	{ 0xa6222e07, "ipcomp_destroy" },
	{ 0xdaf694b1, "inet_del_protocol" },
	{ 0x77141062, "xfrm_unregister_type" },
	{ 0xc3376284, "inet_add_protocol" },
	{ 0xc5b8278f, "xfrm_register_type" },
	{ 0x27e1a049, "printk" },
	{ 0xa20ce1b8, "net_msg_warn" },
	{ 0x2110cee6, "__xfrm_state_destroy" },
	{ 0xf5e57006, "xfrm_state_insert" },
	{ 0x4e4a642f, "xfrm_init_state" },
	{ 0x59a50cf4, "xfrm_state_alloc" },
	{ 0x6a786875, "xfrm_state_lookup" },
	{ 0xa69845a2, "ipcomp_init_state" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=xfrm_ipcomp";


MODULE_INFO(srcversion, "8AE29BA64E674F6C44F607A");
