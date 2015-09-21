#include <linux/mm.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/inet_diag.h>

#include <net/tcp.h>

#define __VERBOSE_

struct mili {
	char *current_state;
	bool first;
};

static void tcp_mili_init(struct sock *sk)
{
	struct mili *mili = inet_csk_ca(sk);
	struct tcp_sock *tp = tcp_sk(sk);
	struct inet_connection_sock *icsk = inet_csk(sk);

	if (!mili->first) {
		mili->first = 1;
		mili->current_state = (char *) kmalloc(50, GFP_KERNEL);
		strcpy(mili->current_state, "");
	}

	tp->srtt = 405U;
	tp->rttvar = 0U;
	icsk->icsk_rto = 0U;
	tp->rcv_ssthresh = ~0U;
}

static void tcp_mili_pkts_acked(struct sock *sk, u32 cnt, s32 rtt_us)
{

}

static void tcp_mili_state(struct sock *sk, u8 ca_state)
{

	struct tcp_sock *tp = tcp_sk(sk);
	struct mili *w = inet_csk_ca(sk);


        if ( ca_state == TCP_CA_Open ) {
                strcpy(w->current_state, "TCP_CA_Open");
        }
        else if ( ca_state == TCP_CA_CWR ) {
		tp->snd_cwnd = max_t(u32, 25500U, tp->snd_cwnd + 100U);
		tcp_set_ca_state(sk, TCP_CA_Recovery);
                strcpy(w->current_state, "TCP_CA_CWR");
        }
        else if ( ca_state == TCP_CA_Loss ) {
		tp->snd_cwnd = max_t(u32, 25500U, tp->snd_cwnd + 100U);
		tcp_set_ca_state(sk, TCP_CA_Recovery); 
                strcpy(w->current_state, "TCP_CA_Loss"); 
        }
        else if ( ca_state == TCP_CA_Recovery ) { 
		tp->snd_cwnd = max_t(u32, 25500U, tp->snd_cwnd + 100U);
                strcpy(w->current_state, "TCP_CA_Recovery");
        }
        else if ( ca_state == TCP_CA_Disorder ) {
		tp->snd_cwnd = max_t(u32, 25500U, tp->snd_cwnd + 100U);
                tcp_set_ca_state(sk, TCP_CA_Recovery);
		strcpy(w->current_state, "TCP_CA_Disorder");
        }

	#ifdef __VERBOSE
        printk(KERN_INFO "++++++++++++++++++++++++++++++ %s ++++++++++++++++++++++++++++++++\n", w->current_state);
	#endif

}

static void tcp_mili_cwnd_event(struct sock *sk, enum tcp_ca_event event)
{

	struct tcp_sock *tp = tcp_sk(sk);
	struct inet_connection_sock *icsk = inet_csk(sk);
	#ifdef __VERBOSE
	struct mili *w = inet_csk_ca(sk);
	#endif

	if (event == CA_EVENT_FAST_ACK) {
		#ifdef __VERBOSE
		printk(KERN_INFO "************* CA_EVENT_FAST_ACK     cwnd=%5u.%u ssthresh=%u\n", tp->snd_cwnd, tp->snd_cwnd_cnt, tp->snd_ssthresh);
		#endif
	}
	else if (event == CA_EVENT_SLOW_ACK) {
		#ifdef __VERBOSE
		printk(KERN_INFO "************* CA_EVENT_SLOW_ACK     cwnd=%5u.%u ssthresh=%u\n", tp->snd_cwnd, tp->snd_cwnd_cnt, tp->snd_ssthresh);
		#endif
		tcp_set_ca_state(sk, TCP_CA_Recovery);
	}
        else if (event == CA_EVENT_TX_START) {
		#ifdef __VERBOSE
		printk(KERN_INFO "************* CA_EVENT_TX_START     cwnd=%5u.%u ssthresh=%u\n", tp->snd_cwnd, tp->snd_cwnd_cnt, tp->snd_ssthresh);
		#endif
	}
        else if (event == CA_EVENT_CWND_RESTART) {
		#ifdef __VERBOSE
		printk(KERN_INFO "************* CA_EVENT_CWND_RESTART cwnd=%5u.%u ssthresh=%u\n", tp->snd_cwnd, tp->snd_cwnd_cnt, tp->snd_ssthresh);
		#endif
                tcp_set_ca_state(sk, TCP_CA_Recovery);
	}
	else if (event == CA_EVENT_COMPLETE_CWR){
		#ifdef __VERBOSE
		printk(KERN_INFO "************* CA_EVENT_COMPLETE_CWR cwnd=%5u.%u ssthresh=%u\n", tp->snd_cwnd, tp->snd_cwnd_cnt, tp->snd_ssthresh);
		#endif
		tcp_set_ca_state(sk, TCP_CA_Recovery);
	}
        else if (event == CA_EVENT_LOSS) {
		#ifdef __VERBOSE
		printk(KERN_INFO "************* CA_EVENT_LOSS         cwnd=%5u.%u ssthresh=%u\n", tp->snd_cwnd, tp->snd_cwnd_cnt, tp->snd_ssthresh);
		#endif
		tcp_set_ca_state(sk, TCP_CA_Recovery);
	}
        else if (event == CA_EVENT_FRTO) {
		#ifdef __VERBOSE
		printk(KERN_INFO "************* CA_EVENT_FRTO         cwnd=%5u.%u ssthresh=%u\n", tp->snd_cwnd, tp->snd_cwnd_cnt, tp->snd_ssthresh);
		#endif
		tcp_set_ca_state(sk, TCP_CA_Recovery);
	}
        else {
		#ifdef __VERBOSE
		printk(KERN_INFO "************* else\n");
		#endif
		tcp_set_ca_state(sk, TCP_CA_Recovery);
	}

	#ifdef __VERBOSE
	printk(KERN_INFO "                tcp:  state=%s\n", w->current_state);
	printk(KERN_INFO "                tp:   cwnd=%5u.%u ssthresh=%u cwnd_clamp=%u srtt=%u rttvar=%u packets_out=%u\n", tp->snd_cwnd, tp->snd_cwnd_cnt, tp->snd_ssthresh, tp->snd_cwnd_clamp, tp->srtt, tp->rttvar, tp->packets_out);
	printk(KERN_INFO "                icsk: rto=%u backoff=%u retransmits=%u", icsk->icsk_rto, icsk->icsk_backoff, icsk->icsk_retransmits);
	printk(KERN_INFO "                kernel: RTO_MAX=%u RTO_MIN=%u INIT=%u\n", TCP_RTO_MAX, TCP_RTO_MIN, TCP_TIMEOUT_INIT);
	#endif

	tp->srtt = 405U;
	tp->rttvar = 0U;
	tp->snd_cwnd = max_t(u32, 25500U, tp->snd_cwnd + 100U);
	tp->snd_ssthresh = 65535U;
	tp->prior_cwnd = 25500U;
	icsk->icsk_rto = 200U;
	tp->rcv_ssthresh = ~0U;
	//tp->rx_opt.rcv_wscale = 3U;
	tp->tso_deferred = 0U;
	icsk->icsk_ack.ato = 1U;
	icsk->icsk_timeout = 1U;
	icsk->icsk_retransmits = 0U;

}

static u32 tcp_mili_ssthresh(struct sock *sk)
{
        struct tcp_sock *tp = tcp_sk(sk);

	tp->snd_ssthresh = 65535U;

	return tp->snd_ssthresh;

}

static void tcp_mili_cong_avoid(struct sock *sk, u32 ack, u32 in_flight)
{	
	struct tcp_sock *tp = tcp_sk(sk);
	
	#ifdef __VERBOSE
	printk(KERN_INFO " ++++ ACK ack=%u in_flight=%u\n", ack, in_flight);
	#endif

	tp->snd_cwnd = max_t(u32, 25500U, tp->snd_cwnd + 100U);
	tp->snd_ssthresh = 65535U;
	tp->prior_cwnd = 25500U;

}


static u32 tcp_mili_min_cwnd(const struct sock *sk)
{
	struct tcp_sock *tp = tcp_sk(sk);

	tp->snd_cwnd = max_t(u32, 25500U, tp->snd_cwnd + 100U);
 	return tp->snd_cwnd;

}

static u32 tcp_mili_undo_cwnd(struct sock *sk) {
	return 25500U;
}

static struct tcp_congestion_ops tcp_mili __read_mostly = {
	.flags		= TCP_CONG_RTT_STAMP,
	.init		= tcp_mili_init,
	.ssthresh	= tcp_mili_ssthresh,
	.cong_avoid	= tcp_mili_cong_avoid,
	.pkts_acked	= tcp_mili_pkts_acked,
	.set_state	= tcp_mili_state,
	.cwnd_event	= tcp_mili_cwnd_event,
	.min_cwnd       = tcp_mili_min_cwnd,
	.undo_cwnd	= tcp_mili_undo_cwnd,

	.owner		= THIS_MODULE,
	.name		= "mili",
};

static int __init tcp_mili_register(void)
{
	BUILD_BUG_ON(sizeof(struct mili) > ICSK_CA_PRIV_SIZE);
	tcp_register_congestion_control(&tcp_mili);
	return 0;
}

static void __exit tcp_mili_unregister(void)
{
	tcp_unregister_congestion_control(&tcp_mili);
}

module_init(tcp_mili_register);
module_exit(tcp_mili_unregister);

MODULE_AUTHOR("Mili, Shuruthi, Peera");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("TCP Mili");

