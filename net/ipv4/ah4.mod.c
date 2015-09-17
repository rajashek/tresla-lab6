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
	{ 0xdaf694b1, "inet_del_protocol" },
	{ 0x77141062, "xfrm_unregister_type" },
	{ 0xc3376284, "inet_add_protocol" },
	{ 0xc5b8278f, "xfrm_register_type" },
	{ 0x2110cee6, "__xfrm_state_destroy" },
	{ 0x6a786875, "xfrm_state_lookup" },
	{ 0x27e1a049, "printk" },
	{ 0xd05dc2a3, "xfrm_aalg_get_byname" },
	{ 0xf8f5b728, "crypto_ahash_setkey" },
	{ 0xe1b36a29, "crypto_alloc_ahash" },
	{ 0xbeaca70e, "kmem_cache_alloc_trace" },
	{ 0x871cf2d6, "kmalloc_caches" },
	{ 0x8917442b, "crypto_destroy_tfm" },
	{ 0xc2588ab2, "xfrm_input_resume" },
	{ 0xf0b3f8e4, "__pskb_pull_tail" },
	{ 0x2fa5a500, "memcmp" },
	{ 0x61994ee7, "pskb_expand_head" },
	{ 0xee247ae3, "xfrm_output_resume" },
	{ 0x37a0cba, "kfree" },
	{ 0x44cf7472, "crypto_ahash_digest" },
	{ 0x2702f21f, "skb_to_sgvec" },
	{ 0xc897c382, "sg_init_table" },
	{ 0x236c8c64, "memcpy" },
	{ 0x8e9da88a, "skb_push" },
	{ 0xdd9b0536, "skb_cow_data" },
	{ 0x5a34a45c, "__kmalloc" },
	{ 0xde0bdcff, "memset" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "2438D5E0FA2327C01B76D38");
