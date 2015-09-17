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


static int alpha = 16;
static int beta  = 16;
static int gamma = 4; 
    
module_param(alpha, int, 0644);
MODULE_PARM_DESC(alpha, "lower bound of packets in network");
module_param(beta, int, 0644);
MODULE_PARM_DESC(beta, "upper bound of packets in network");
module_param(gamma, int, 0644);
MODULE_PARM_DESC(gamma, "limit on increase (scale by 2)");


/* Veno variables */
struct veno {
	u8 doing_veno_now;	/* if true, do veno for this rtt */
	u16 cntrtt;		/* # of rtts measured within last rtt */
	u32 minrtt;		/* min of rtts measured within last rtt (in usec) */
	u32 basertt;		/* the min of all Veno rtt measurements seen (in usec) */
	u32 inc;		/* decide whether to increase cwnd */
	u32 diff;		/* calculate the diff rate */
	
	char *current_state;
	bool first;

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
	//printk(KERN_INFO "Mininmum RTT set in enable is %d",veno->minrtt);
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
	veno->inc = 0;


	if (!veno->first) {
		veno->first = 1;
		veno->current_state = (char *) kmalloc(50, GFP_KERNEL);
	}

	veno_enable(sk);

}

/* Do rtt sampling needed for Veno. */
static void tcp_veno_pkts_acked(struct sock *sk, u32 cnt, s32 rtt_us)
{
	struct veno *veno = inet_csk_ca(sk);
	u32 vrtt;
	cnt = 0;
	//printk(KERN_INFO "Got acknowleadement NOW so made count = 0!!!!!!!!\n");
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
	veno->cntrtt++;
}

static void tcp_veno_state(struct sock *sk, u8 ca_state)
{

	struct tcp_sock *tp = tcp_sk(sk);
	struct veno *w = inet_csk_ca(sk);

	if (ca_state == TCP_CA_Open)
		veno_enable(sk);
	else
		veno_disable(sk);



        if ( ca_state == TCP_CA_Open ) {
                strcpy(w->current_state, "TCP_CA_Open");
        }
        else if ( ca_state == TCP_CA_CWR ) {
                strcpy(w->current_state, "TCP_CA_CWR");
        }
        else if ( ca_state == TCP_CA_Loss ) { 
                strcpy(w->current_state, "TCP_CA_Loss"); 
		tp->snd_cwnd = 2200;
        }
        else if ( ca_state == TCP_CA_Recovery ) { 
                strcpy(w->current_state, "TCP_CA_Recovery");
        }
        else if ( ca_state == TCP_CA_Disorder ) {
                strcpy(w->current_state, "TCP_CA_Disorder");
		tp->snd_cwnd += 20;
        }

        printk(KERN_INFO "++++++++++++++++++++++++++++++ %s ++++++++++++++++++++++++++++++++\n", w->current_state);


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

	struct tcp_sock *tp = tcp_sk(sk);
	struct veno *w = inet_csk_ca(sk);
	struct inet_connection_sock *icsk = inet_csk(sk);


	if (event == CA_EVENT_FAST_ACK)          printk(KERN_INFO "************* CA_EVENT_FAST_ACK     cwnd=%5u.%u ssthresh=%u\n", tp->snd_cwnd, tp->snd_cwnd_cnt, tp->snd_ssthresh);
        else if (event == CA_EVENT_SLOW_ACK)     printk(KERN_INFO "************* CA_EVENT_SLOW_ACK     cwnd=%5u.%u ssthresh=%u\n", tp->snd_cwnd, tp->snd_cwnd_cnt, tp->snd_ssthresh);
        else if (event == CA_EVENT_TX_START)     printk(KERN_INFO "************* CA_EVENT_TX_START     cwnd=%5u.%u ssthresh=%u\n", tp->snd_cwnd, tp->snd_cwnd_cnt, tp->snd_ssthresh);
        else if (event == CA_EVENT_CWND_RESTART) printk(KERN_INFO "************* CA_EVENT_CWND_RESTART cwnd=%5u.%u ssthresh=%u\n", tp->snd_cwnd, tp->snd_cwnd_cnt, tp->snd_ssthresh);
        else if (event == CA_EVENT_COMPLETE_CWR) printk(KERN_INFO "************* CA_EVENT_COMPLETE_CWR cwnd=%5u.%u ssthresh=%u\n", tp->snd_cwnd, tp->snd_cwnd_cnt, tp->snd_ssthresh);
        else if (event == CA_EVENT_LOSS)         printk(KERN_INFO "************* CA_EVENT_LOSS         cwnd=%5u.%u ssthresh=%u\n", tp->snd_cwnd, tp->snd_cwnd_cnt, tp->snd_ssthresh);
        else if (event == CA_EVENT_FRTO)         printk(KERN_INFO "************* CA_EVENT_FRTO         cwnd=%5u.%u ssthresh=%u\n", tp->snd_cwnd, tp->snd_cwnd_cnt, tp->snd_ssthresh);
        else printk(KERN_INFO "************* else\n");


	if (event == CA_EVENT_CWND_RESTART || event == CA_EVENT_TX_START)
		tcp_veno_init(sk);


	printk(KERN_INFO "                tcp:  state=%s\n", w->current_state);
	printk(KERN_INFO "                tp:   cwnd=%5u.%u ssthresh=%u cwnd_clamp=%u srtt=%u rttvar=%u packets_out=%u\n", tp->snd_cwnd, tp->snd_cwnd_cnt, tp->snd_ssthresh, tp->snd_cwnd_clamp, tp->srtt, tp->rttvar, tp->packets_out);
	printk(KERN_INFO "                icsk: rto=%u backoff=%u retransmits=%u", icsk->icsk_rto, icsk->icsk_backoff, icsk->icsk_retransmits);
	printk(KERN_INFO "                kernel: RTO_MAX=%u RTO_MIN=%u INIT=%u\n", TCP_RTO_MAX, TCP_RTO_MIN, TCP_TIMEOUT_INIT);
	//printk(KERN_INFO "                veno: cntrtt=%u minrtt=%u basertt=%u diff=%u\n", w->cntrtt, w->minrtt, w->basertt, w->diff);


	tp->srtt = 200;
	tp->rttvar = 5;

	if (strcmp(w->current_state, "TCP_CA_Loss")==0) {
                tp->snd_cwnd = 2200;
        }

}

u32 tcp_shru_ssthresh(struct sock *sk)
{
        struct tcp_sock *tp = tcp_sk(sk);
	//printk(KERN_INFO "the sender congestion window is .... %d\n",tp->snd_cwnd);
	tp->snd_ssthresh = 65535;
	return tp->snd_ssthresh;
}

static void tcp_mili_slow_start(struct tcp_sock *tp){
        tp->snd_cwnd = 2200;
}

static void tcp_veno_cong_avoid(struct sock *sk, u32 ack, u32 in_flight)
{	
       struct tcp_sock *tp = tcp_sk(sk);
       struct veno *veno = inet_csk_ca(sk);
        u32 rtt, diff;
        u64 target_cwnd;
        rtt = veno->minrtt;
        target_cwnd = (u64)tp->snd_cwnd * veno->basertt;
        do_div(target_cwnd, rtt);
        diff = tp->snd_cwnd * (rtt-veno->basertt) / veno->basertt;

	 if (diff > gamma && tp->snd_cwnd <= tp->snd_ssthresh) {
		 //printk(KERN_INFO "ENTERING DIFF LOOP\n");
                 tp->snd_cwnd = 2200;//change
                 tp->snd_ssthresh = tcp_shru_ssthresh(sk);

        } else if (tp->snd_cwnd <= tp->snd_ssthresh) {
                                /* Slow start.  */
                 tcp_mili_slow_start(tp);
         } else {
                 if (diff > beta) {
                           tp->snd_cwnd=2200;//change
                           tp->snd_ssthresh = tcp_shru_ssthresh(sk);
                  } else if (diff < alpha) {
                           tp->snd_cwnd=2200;//change
                  } else {
                  }
 	}
         if (tp->snd_cwnd < 4)
                tp->snd_cwnd = 2200;
         else if (tp->snd_cwnd > tp->snd_cwnd_clamp){
                //printk(KERN_INFO "I AM IN CLAM\n");
		tp->snd_cwnd = tp->snd_cwnd_clamp;
                tp->snd_ssthresh = tcp_current_ssthresh(sk);
         }

                /* Wipe the slate clean for the next RTT. */
                veno->cntrtt = 0;
                veno->minrtt = 0x7fffffff;
		printk(KERN_INFO " ++++ ACK ack=%u in_flight=%u\n", ack, in_flight);
        /* Use normal slow start */

}


u32 tcp_shru_min_cwnd(const struct sock *sk)
{
	
	struct tcp_sock *tp = tcp_sk(sk);
	tp->snd_cwnd =2200;
 	return tp->snd_cwnd;
}

u32  tcp_undo_cwnd(struct sock *sk) {
	return 2200U;
}

/*static void tcp_veno_set_state(struct sock *sk, u8 new_state) {

        //struct tcp_sock *tp = tcp_sk(sk);
	struct veno *w = inet_csk_ca(sk);

	if ( new_state == TCP_CA_Open ) {
		strcpy(w->current_state, "TCP_CA_Open");
	}
        else if ( new_state == TCP_CA_CWR ) {
		strcpy(w->current_state, "TCP_CA_CWR");
	}
        else if ( new_state == TCP_CA_Loss ) { 
		strcpy(w->current_state, "TCP_CA_Loss"); 
	}
        else if ( new_state == TCP_CA_Recovery ) { 
		strcpy(w->current_state, "TCP_CA_Recovery");
	}
        else if ( new_state == TCP_CA_Disorder ) {
		strcpy(w->current_state, "TCP_CA_Disorder");
	}

	printk(KERN_INFO "++++++++++++++++++++++++++++++ %s ++++++++++++++++++++++++++++++++\n", w->current_state);

}*/

static struct tcp_congestion_ops tcp_veno __read_mostly = {
	.flags		= TCP_CONG_RTT_STAMP,
	.init		= tcp_veno_init,
	.ssthresh	= tcp_shru_ssthresh,
	.cong_avoid	= tcp_veno_cong_avoid,
	.pkts_acked	= tcp_veno_pkts_acked,
	.set_state	= tcp_veno_state,
	.cwnd_event	= tcp_veno_cwnd_event,
	.min_cwnd       = tcp_shru_min_cwnd,
	.undo_cwnd	= tcp_undo_cwnd,

	.owner		= THIS_MODULE,
	.name		= "taggb",
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
