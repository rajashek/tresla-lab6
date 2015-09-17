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
	{ 0x166f9089, "xfrm4_tunnel_deregister" },
	{ 0xbb27488e, "unregister_pernet_device" },
	{ 0xf9067070, "xfrm4_tunnel_register" },
	{ 0x4f724bf7, "register_pernet_device" },
	{ 0x27e1a049, "printk" },
	{ 0x609f1c7e, "synchronize_net" },
	{ 0x4f8b5ddb, "_copy_to_user" },
	{ 0x83716699, "netdev_state_change" },
	{ 0x4f6b400b, "_copy_from_user" },
	{ 0xa1c76e0a, "_cond_resched" },
	{ 0xc6cbbc89, "capable" },
	{ 0x6797f9c5, "__secpath_destroy" },
	{ 0x1fdb761e, "__xfrm_policy_check" },
	{ 0xa9c977ac, "netif_rx" },
	{ 0x2a18c74, "nf_conntrack_destroy" },
	{ 0x9272dff0, "__ip_select_ident" },
	{ 0xd595a54, "icmp_send" },
	{ 0x741a79e7, "ip_local_out" },
	{ 0x7628f3c7, "this_cpu_off" },
	{ 0x37a0cba, "kfree" },
	{ 0x4433391a, "kfree_skb" },
	{ 0x8e9da88a, "skb_push" },
	{ 0xafd90aa7, "sock_wfree" },
	{ 0x9b305ed6, "skb_realloc_headroom" },
	{ 0xfa23b7fe, "consume_skb" },
	{ 0x78edd252, "register_netdev" },
	{ 0x7d11c268, "jiffies" },
	{ 0x6e720ff2, "rtnl_unlock" },
	{ 0x9fdecc31, "unregister_netdevice_many" },
	{ 0x55750067, "unregister_netdevice_queue" },
	{ 0xc7a4fbed, "rtnl_lock" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x672144bd, "strlcpy" },
	{ 0xe914e41e, "strcpy" },
	{ 0x87a78606, "register_netdevice" },
	{ 0x55f2580b, "__alloc_percpu" },
	{ 0xcda9271f, "alloc_netdev_mqs" },
	{ 0x24cc09bc, "__dev_get_by_index" },
	{ 0x61caeb8f, "dst_release" },
	{ 0x4f3d3e08, "ip_route_output_flow" },
	{ 0xfe7c4287, "nr_cpu_ids" },
	{ 0xc0a3d105, "find_next_bit" },
	{ 0xb9249d16, "cpu_possible_mask" },
	{ 0x3928efe9, "__per_cpu_offset" },
	{ 0x3d3f95d2, "free_netdev" },
	{ 0xc9ec4e21, "free_percpu" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=tunnel4";


MODULE_INFO(srcversion, "2A6CEB897DACB7D7BB06CEF");
