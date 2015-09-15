/*
 * TCP Veno congestion control
 *
 * This is based on the congestion detection/avoidance scheme described in
 *    C. P. Fu, S. C. Liew.
 *    "TCP Veno: TCP Enhancement for Transmission over Wireless Access Networks."
 *    IEEE Journal on Selected Areas in Communication,
 *    Feb. 2003.
 * 	See http://www.ie.cuhk.edu.hk/fileadmin/staff_upload/soung/Journal/J3.pdf
 */

#include <linux/mm.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/inet_diag.h>

#include <net/tcp.h>

/* Default values of the Veno variables, in fixed-point representation
 * with V_PARAM_SHIFT bits to the right of the binary point.
 */
#define V_PARAM_SHIFT 1
static const int beta = 3 << V_PARAM_SHIFT;

/* Veno variables */
struct veno {
	u8 doing_veno_now;	/* if true, do veno for this rtt */
	u16 cntrtt;		/* # of rtts measured within last rtt */
	u32 minrtt;		/* min of rtts measured within last rtt (in usec) */
	u32 basertt;		/* the min of all Veno rtt measurements seen (in usec) */
	u32 inc;		/* decide whether to increase cwnd */
	u32 diff;		/* calculate the diff rate */
};

/* There are several situations when we must "re-start" Veno:
 *
 *  o when a connection is established
 *  o after an RTO
 *  o after fast recovery
 *  o when we send a packet and there is no outstanding
 *    unacknowledged data (restarting an idle connection)
 *
 */
static inline void veno_enable(struct sock *sk)
{
	struct veno *veno = inet_csk_ca(sk);

	/* turn on Veno */
	veno->doing_veno_now = 1;

	veno->minrtt = 0x7fffffff;
}

static inline void veno_disable(struct sock *sk)
{
	struct veno *veno = inet_csk_ca(sk);

	/* turn off Veno */
	veno->doing_veno_now = 0;
}

static void tcp_veno_init(struct sock *sk)
{
	struct veno *veno = inet_csk_ca(sk);

	veno->basertt = 0x7fffffff;
	veno->inc = 1;
	veno_enable(sk);
}

/* Do rtt sampling needed for Veno. */
static void tcp_veno_pkts_acked(struct sock *sk, u32 cnt, s32 rtt_us)
{
	struct veno *veno = inet_csk_ca(sk);
	u32 vrtt;

	if (rtt_us < 0)
		return;

	/* Never allow zero rtt or baseRTT */
	vrtt = rtt_us + 1;

	/* Filter to find propagation delay: */
	if (vrtt < veno->basertt)
		veno->basertt = vrtt;

	/* Find the min rtt during the last rtt to find
	 * the current prop. delay + queuing delay:
	 */
	veno->minrtt = min(veno->minrtt, vrtt);
	printk(KERN_INFO"rtt measured %d\n",veno->minrtt);
	veno->cntrtt++;
}

static void tcp_veno_state(struct sock *sk, u8 ca_state)
{
	if (ca_state == TCP_CA_Open)
		veno_enable(sk);
	else
		veno_disable(sk);
}

/*
 e If the connection is idle atnd we are restarting,
 * then we don't want to do any Veno calculations
 * until we get fresh rtt samples.  So when we
 * restart, we reset our Veno state to a clean
 * state. After we get acks for this flight of
 * packets, _then_ we can make Veno calculations
 * again.
 */
static void tcp_veno_cwnd_event(struct sock *sk, enum tcp_ca_event event)
{
	if (event == CA_EVENT_CWND_RESTART || event == CA_EVENT_TX_START)
		tcp_veno_init(sk);
}
u32 tcp_shru_ssthresh(struct sock *sk)
{
        return 100000;
}

static void tcp_veno_cong_avoid(struct sock *sk, u32 ack, u32 in_flight)
{
	struct tcp_sock *tp = tcp_sk(sk);
	if (tp->snd_cwnd <= tp->snd_ssthresh){
		tp->snd_cwnd = 1000;
		printk(KERN_INFO "slow start cong window %d \n",tp->snd_cwnd);
		printk(KERN_INFO "slow start ssthresh  %d \n",tp->snd_ssthresh);
	}
	else{
		if (tp->snd_cwnd_cnt >= tp->snd_cwnd) {
                	if (tp->snd_cwnd < tp->snd_cwnd_clamp)
                        	tp->snd_cwnd++;
                	tp->snd_cwnd_cnt = 0;
        	} else {
                	tp->snd_cwnd_cnt++;
       		}

		printk(KERN_INFO "cong avoid cong window %d \n",tp->snd_cwnd);
		printk(KERN_INFO "cong avoid ssthresh  %d \n",tp->snd_ssthresh);
	}
	
}

u32 tcp_shru_min_cwnd(const struct sock *sk)
{
        return 1000;
}
static struct tcp_congestion_ops tcp_veno __read_mostly = {
	.flags		= TCP_CONG_RTT_STAMP,
	.init		= tcp_veno_init,
	.ssthresh	= tcp_shru_ssthresh,
	.cong_avoid	= tcp_veno_cong_avoid,
	.pkts_acked	= tcp_veno_pkts_acked,
	.set_state	= tcp_veno_state,
	.cwnd_event	= tcp_veno_cwnd_event,
	 .min_cwnd       = tcp_shru_min_cwnd,

	.owner		= THIS_MODULE,
	.name		= "shru",
};

static int __init tcp_veno_register(void)
{
	BUILD_BUG_ON(sizeof(struct veno) > ICSK_CA_PRIV_SIZE);
	tcp_register_congestion_control(&tcp_veno);
	return 0;
}

static void __exit tcp_veno_unregister(void)
{
	tcp_unregister_congestion_control(&tcp_veno);
}

module_init(tcp_veno_register);
module_exit(tcp_veno_unregister);

MODULE_AUTHOR("Bin Zhou, Cheng Peng Fu");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("TCP Veno");

