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
	{ 0x64e3ec9a, "xfrm4_prepare_output" },
	{ 0x620c95e2, "xfrm_prepare_input" },
	{ 0xb1cc5c14, "xfrm_unregister_mode" },
	{ 0xe59a023c, "xfrm_register_mode" },
	{ 0xf0b3f8e4, "__pskb_pull_tail" },
	{ 0xa3a5be95, "memmove" },
	{ 0x8e9da88a, "skb_push" },
	{ 0xde0bdcff, "memset" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "DB72384D4775B9BA0B77A83");
