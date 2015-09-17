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
	{ 0xf0b3f8e4, "__pskb_pull_tail" },
	{ 0xe52947e7, "__phys_addr" },
	{ 0xde0bdcff, "memset" },
	{ 0xb6244511, "sg_init_one" },
	{ 0x2702f21f, "skb_to_sgvec" },
	{ 0xc897c382, "sg_init_table" },
	{ 0xba63339c, "_raw_spin_unlock_bh" },
	{ 0x1637ff0f, "_raw_spin_lock_bh" },
	{ 0x8e9da88a, "skb_push" },
	{ 0x9ff7cad8, "pskb_put" },
	{ 0xdd9b0536, "skb_cow_data" },
	{ 0x2110cee6, "__xfrm_state_destroy" },
	{ 0x6a786875, "xfrm_state_lookup" },
	{ 0x27e1a049, "printk" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x683728d8, "crypto_aead_setauthsize" },
	{ 0xa20ce1b8, "net_msg_warn" },
	{ 0xd05dc2a3, "xfrm_aalg_get_byname" },
	{ 0x236c8c64, "memcpy" },
	{ 0x50720c5f, "snprintf" },
	{ 0x14ebe85b, "crypto_alloc_aead" },
	{ 0xbeaca70e, "kmem_cache_alloc_trace" },
	{ 0x871cf2d6, "kmalloc_caches" },
	{ 0x8917442b, "crypto_destroy_tfm" },
	{ 0xc2588ab2, "xfrm_input_resume" },
	{ 0xec7d5901, "___pskb_trim" },
	{ 0x2ab8a716, "km_new_mapping" },
	{ 0x548e7765, "skb_copy_bits" },
	{ 0xee247ae3, "xfrm_output_resume" },
	{ 0x37a0cba, "kfree" },
	{ 0x5a34a45c, "__kmalloc" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "9918FBF3958808C263CA104");
