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
	{ 0x402b8281, "__request_module" },
	{ 0x871cf2d6, "kmalloc_caches" },
	{ 0x25ec1b28, "strlen" },
	{ 0x70859096, "inet_twsk_put" },
	{ 0x1637ff0f, "_raw_spin_lock_bh" },
	{ 0x66338a23, "mutex_unlock" },
	{ 0x6b81ebf3, "sock_i_ino" },
	{ 0xe0c1fc91, "__rta_fill" },
	{ 0x7d11c268, "jiffies" },
	{ 0xcc9e6af1, "skb_trim" },
	{ 0xcf145ec8, "netlink_kernel_create" },
	{ 0xde0bdcff, "memset" },
	{ 0x37befc70, "jiffies_to_msecs" },
	{ 0x2fa5a500, "memcmp" },
	{ 0xd79627c2, "netlink_kernel_release" },
	{ 0xcd279169, "nla_find" },
	{ 0xc18f51bb, "netlink_rcv_skb" },
	{ 0xb4390f9a, "mcount" },
	{ 0x16305289, "warn_slowpath_null" },
	{ 0xa636425f, "mutex_lock" },
	{ 0x947acd14, "sk_free" },
	{ 0xcd4e7994, "netlink_unicast" },
	{ 0x1bef8a0, "init_net" },
	{ 0x3ff62317, "local_bh_disable" },
	{ 0xc16084b2, "__alloc_skb" },
	{ 0xba63339c, "_raw_spin_unlock_bh" },
	{ 0x4433391a, "kfree_skb" },
	{ 0x799aca4, "local_bh_enable" },
	{ 0x31f0123a, "inet6_lookup" },
	{ 0xbeaca70e, "kmem_cache_alloc_trace" },
	{ 0xfdee7d42, "_raw_read_lock_bh" },
	{ 0xf37260ab, "_raw_read_unlock_bh" },
	{ 0xf8d779bc, "netlink_dump_start" },
	{ 0x37a0cba, "kfree" },
	{ 0x56f107f8, "__inet_lookup_listener" },
	{ 0xc928bedc, "skb_put" },
	{ 0xd7b88400, "sock_i_uid" },
	{ 0xe914e41e, "strcpy" },
	{ 0xae4c6c0e, "__inet_lookup_established" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "D3FF1CBBDBC66C877733293");
