/*
 * TCP Westwood+: end-to-end bandwidth estimation for TCP
 *
 *      Angelo Dell'Aera: author of the first version of TCP Westwood+ in Linux 2.4
 *
 * Support at http://c3lab.poliba.it/index.php/Westwood
 * Main references in literature:
 *
 * - Mascolo S, Casetti, M. Gerla et al.
 *   "TCP Westwood: bandwidth estimation for TCP" Proc. ACM Mobicom 2001
 *
 * - A. Grieco, s. Mascolo
 *   "Performance evaluation of New Reno, Vegas, Westwood+ TCP" ACM Computer
 *     Comm. Review, 2004
 *
 * - A. Dell'Aera, L. Grieco, S. Mascolo.
 *   "Linux 2.4 Implementation of Westwood+ TCP with Rate-Halving :
 *    A Performance Evaluation Over the Internet" (ICC 2004), Paris, June 2004
 *
 * Westwood+ employs end-to-end bandwidth measurement to set cwnd and
 * ssthresh after packet loss. The probing phase is as the original Reno.
 */

#include <linux/mm.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/inet_diag.h>
#include <net/tcp.h>

#include <linux/types.h>
#include <linux/list.h>
#include <linux/gfp.h>

/* TCP Westwood structure */
struct peera {
    u32    bw_ns_est;        /* first bandwidth estimation..not too smoothed 8) */
    u32    bw_est;           /* bandwidth estimate */
    u32    rtt_win_sx;       /* here starts a new evaluation... */
    u32    bk;
    u32    snd_una;          /* used for evaluating the number of acked bytes */
    u32    cumul_ack;
    u32    accounted;
    u32    rtt;
    u32    rtt_min;          /* minimum observed RTT */
    u8     first_ack;        /* flag which infers that this is the first ack */
    u8     reset_rtt_min;    /* Reset RTT min to next RTT sample*/
    u8     ssthresh_tolerate;
};


/* TCP Westwood functions and constants */
#define TCP_WESTWOOD_RTT_MIN   (HZ/20)	/* 50ms */
#define TCP_WESTWOOD_INIT_RTT  (20*HZ)	/* maybe too conservative?! */

/*
 * @tcp_peera_create
 * This function initializes fields used in TCP Westwood+,
 * it is called after the initial SYN, so the sequence numbers
 * are correct but new passive connections we have no
 * information about RTTmin at this time so we simply set it to
 * TCP_WESTWOOD_INIT_RTT. This value was chosen to be too conservative
 * since in this way we're sure it will be updated in a consistent
 * way as soon as possible. It will reasonably happen within the first
 * RTT period of the connection lifetime.
 */
static void tcp_peera_init(struct sock *sk)
{
    struct tcp_sock *tp = tcp_sk(sk);
    struct peera *w = inet_csk_ca(sk);
    
    w->bk = 0;
    w->bw_ns_est = 0;
    w->bw_est = 0;
    w->accounted = 0;
    w->cumul_ack = 0;
    w->reset_rtt_min = 1;
    w->rtt_min = w->rtt = TCP_WESTWOOD_INIT_RTT;
    w->rtt_win_sx = tcp_time_stamp;
    w->snd_una = tcp_sk(sk)->snd_una;
    w->first_ack = 1;
    
    w->ssthresh_tolerate = 0;
    tp->snd_ssthresh = 9999U;
}

/*
 * @peera_do_filter
 * Low-pass filter. Implemented using constant coefficients.
 */
static inline u32 peera_do_filter(u32 a, u32 b)
{
    return ((7 * a) + b) >> 3;
}

static void peera_filter(struct peera *w, u32 delta)
{
    /* If the filter is empty fill it with the first sample of bandwidth  */
    if (w->bw_ns_est == 0 && w->bw_est == 0) {
        w->bw_ns_est = w->bk / delta;
        w->bw_est = w->bw_ns_est;
    } else {
        w->bw_ns_est = peera_do_filter(w->bw_ns_est, w->bk / delta);
        w->bw_est = peera_do_filter(w->bw_est, w->bw_ns_est);
    }
}

/*
 * @peera_pkts_acked
 * Called after processing group of packets.
 * but all peera needs is the last sample of srtt.
 */
static void tcp_peera_pkts_acked(struct sock *sk, u32 cnt, s32 rtt)
{
    struct peera *w = inet_csk_ca(sk);
    
    if (rtt > 0)
        w->rtt = usecs_to_jiffies(rtt);
}

/*
 * @peera_update_window
 * It updates RTT evaluation window if it is the right moment to do
 * it. If so it calls filter for evaluating bandwidth.
 */
static void peera_update_window(struct sock *sk)
{
    struct peera *w = inet_csk_ca(sk);
    s32 delta = tcp_time_stamp - w->rtt_win_sx;
    
    /* Initialize w->snd_una with the first acked sequence number in order
     * to fix mismatch between tp->snd_una and w->snd_una for the first
     * bandwidth sample
     */
    if (w->first_ack) {
        w->snd_una = tcp_sk(sk)->snd_una;
        w->first_ack = 0;
    }
    
    /*
     * See if a RTT-window has passed.
     * Be careful since if RTT is less than
     * 50ms we don't filter but we continue 'building the sample'.
     * This minimum limit was chosen since an estimation on small
     * time intervals is better to avoid...
     * Obviously on a LAN we reasonably will always have
     * right_bound = left_bound + WESTWOOD_RTT_MIN
     */
    if (w->rtt && delta > max_t(u32, w->rtt, TCP_WESTWOOD_RTT_MIN)) {
        peera_filter(w, delta);
        
        w->bk = 0;
        w->rtt_win_sx = tcp_time_stamp;
    }
}

static inline void update_rtt_min(struct peera *w)
{
    if (w->reset_rtt_min) {
        w->rtt_min = w->rtt;
        w->reset_rtt_min = 0;
    } else
        w->rtt_min = min(w->rtt, w->rtt_min);
}


/*
 * @peera_fast_bw
 * It is called when we are in fast path. In particular it is called when
 * header prediction is successful. In such case in fact update is
 * straight forward and doesn't need any particular care.
 */
static inline void peera_fast_bw(struct sock *sk)
{
    const struct tcp_sock *tp = tcp_sk(sk);
    struct peera *w = inet_csk_ca(sk);
    
    peera_update_window(sk);
    
    w->bk += tp->snd_una - w->snd_una;
    w->snd_una = tp->snd_una;
    update_rtt_min(w);
}

/*
 * @peera_acked_count
 * This function evaluates cumul_ack for evaluating bk in case of
 * delayed or partial acks.
 */
static inline u32 peera_acked_count(struct sock *sk)
{
    const struct tcp_sock *tp = tcp_sk(sk);
    struct peera *w = inet_csk_ca(sk);
    
    w->cumul_ack = tp->snd_una - w->snd_una;
    
    /* If cumul_ack is 0 this is a dupack since it's not moving
     * tp->snd_una.
     */
    if (!w->cumul_ack) {
        w->accounted += tp->mss_cache;
        w->cumul_ack = tp->mss_cache;
    }
    
    if (w->cumul_ack > tp->mss_cache) {
        /* Partial or delayed ack */
        if (w->accounted >= w->cumul_ack) {
            w->accounted -= w->cumul_ack;
            w->cumul_ack = tp->mss_cache;
        } else {
            w->cumul_ack -= w->accounted;
            w->accounted = 0;
        }
    }
    
    w->snd_una = tp->snd_una;
    
    return w->cumul_ack;
}


/*
 * TCP Westwood
 * Here limit is evaluated as Bw estimation*RTTmin (for obtaining it
 * in packets we use mss_cache). Rttmin is guaranteed to be >= 2
 * so avoids ever returning 0.
 */
static u32 tcp_peera_bw_rttmin(const struct sock *sk)
{
    const struct tcp_sock *tp = tcp_sk(sk);
    const struct peera *w = inet_csk_ca(sk);
    return max_t(u32, (w->bw_est * w->rtt_min) / tp->mss_cache, 2);
}

static void tcp_peera_event(struct sock *sk, enum tcp_ca_event event)
{
    struct tcp_sock *tp = tcp_sk(sk);
    struct peera *w = inet_csk_ca(sk);
    struct inet_connection_sock *icsk = inet_csk(sk);
    
    if (event == CA_EVENT_FAST_ACK)          printk(KERN_INFO "************* CA_EVENT_FAST_ACK     cwnd=%5u.%u ssthresh=%u\n", tp->snd_cwnd, tp->snd_cwnd_cnt, tp->snd_ssthresh);
    else if (event == CA_EVENT_SLOW_ACK)     printk(KERN_INFO "************* CA_EVENT_SLOW_ACK     cwnd=%5u.%u ssthresh=%u\n", tp->snd_cwnd, tp->snd_cwnd_cnt, tp->snd_ssthresh);
    else if (event == CA_EVENT_TX_START)     printk(KERN_INFO "************* CA_EVENT_TX_START     cwnd=%5u.%u ssthresh=%u\n", tp->snd_cwnd, tp->snd_cwnd_cnt, tp->snd_ssthresh);
    else if (event == CA_EVENT_CWND_RESTART) printk(KERN_INFO "************* CA_EVENT_CWND_RESTART cwnd=%5u.%u ssthresh=%u\n", tp->snd_cwnd, tp->snd_cwnd_cnt, tp->snd_ssthresh);
    else if (event == CA_EVENT_COMPLETE_CWR) printk(KERN_INFO "************* CA_EVENT_COMPLETE_CWR cwnd=%5u.%u ssthresh=%u\n", tp->snd_cwnd, tp->snd_cwnd_cnt, tp->snd_ssthresh);
    else if (event == CA_EVENT_LOSS)         printk(KERN_INFO "************* CA_EVENT_LOSS         cwnd=%5u.%u ssthresh=%u\n", tp->snd_cwnd, tp->snd_cwnd_cnt, tp->snd_ssthresh);
    else if (event == CA_EVENT_FRTO)         printk(KERN_INFO "************* CA_EVENT_FRTO         cwnd=%5u.%u ssthresh=%u\n", tp->snd_cwnd, tp->snd_cwnd_cnt, tp->snd_ssthresh);
    else printk(KERN_INFO "************* else\n");
    
    switch (event) {
        case CA_EVENT_FAST_ACK:
            peera_fast_bw(sk);
            break;
            
        case CA_EVENT_COMPLETE_CWR:
            //tp->snd_cwnd = tcp_peera_bw_rttmin(sk);
            //tp->snd_ssthresh = tcp_peera_bw_rttmin(sk);
            break;
            
        case CA_EVENT_FRTO:
            //tp->snd_ssthresh = tcp_peera_bw_rttmin(sk);
            /* Update RTT_min when next ack arrives */
            w->reset_rtt_min = 1;
            break;
            
        case CA_EVENT_SLOW_ACK:
            //peera_update_window(sk);
            w->bk += peera_acked_count(sk);
            update_rtt_min(w);
            break;
            
        default:
            /* don't care */
            break;
    }
    
    if (event == CA_EVENT_FAST_ACK)          printk(KERN_INFO "    ********* CA_EVENT_FAST_ACK     cwnd=%5u.%u ssthresh=%u\n", tp->snd_cwnd, tp->snd_cwnd_cnt, tp->snd_ssthresh);
    else if (event == CA_EVENT_SLOW_ACK)     printk(KERN_INFO "    ********* CA_EVENT_SLOW_ACK     cwnd=%5u.%u ssthresh=%u\n", tp->snd_cwnd, tp->snd_cwnd_cnt, tp->snd_ssthresh);
    else if (event == CA_EVENT_TX_START)     printk(KERN_INFO "    ********* CA_EVENT_TX_START     cwnd=%5u.%u ssthresh=%u\n", tp->snd_cwnd, tp->snd_cwnd_cnt, tp->snd_ssthresh);
    else if (event == CA_EVENT_CWND_RESTART) printk(KERN_INFO "    ********* CA_EVENT_CWND_RESTART cwnd=%5u.%u ssthresh=%u\n", tp->snd_cwnd, tp->snd_cwnd_cnt, tp->snd_ssthresh);
    else if (event == CA_EVENT_COMPLETE_CWR) printk(KERN_INFO "    ********* CA_EVENT_COMPLETE_CWR cwnd=%5u.%u ssthresh=%u\n", tp->snd_cwnd, tp->snd_cwnd_cnt, tp->snd_ssthresh);
    else if (event == CA_EVENT_LOSS)         printk(KERN_INFO "    ********* CA_EVENT_LOSS         cwnd=%5u.%u ssthresh=%u\n", tp->snd_cwnd, tp->snd_cwnd_cnt, tp->snd_ssthresh);
    else if (event == CA_EVENT_FRTO)         printk(KERN_INFO "    ********* CA_EVENT_FRTO         cwnd=%5u.%u ssthresh=%u\n", tp->snd_cwnd, tp->snd_cwnd_cnt, tp->snd_ssthresh);
    
    printk(KERN_INFO "    ********* BW_RTTMIN=%u BW=%u RTT_MIN=%u RTT=%u UNA=%u SRTT=%u rto=%u icsk_backoff=%u\n", tcp_peera_bw_rttmin(sk), w->bw_est, w->rtt_min, w->rtt, tp->snd_una, tp->srtt, icsk->icsk_rto, icsk->icsk_backoff);
    
}

/* Slow start threshold is half the congestion window (min 2) */
u32 tcp_peera_ssthresh(struct sock *sk)
{
    const struct tcp_sock *tp = tcp_sk(sk);
    struct peera *w = inet_csk_ca(sk);
    
    w->ssthresh_tolerate += 1;
    if (w->ssthresh_tolerate >= 1000U) {
        printk(KERN_INFO "++ ssthresh has been changed\n");
        w->ssthresh_tolerate = 0;
        return max(tp->snd_cwnd >> 1U, 2U);
    }
    else {
        printk(KERN_INFO "++ ssthresh\n");
        return tp->snd_ssthresh;
    }
    
}


void tcp_peera_slow_start(struct tcp_sock *tp)
{
    //int cnt; /* increase in packets */
    
    /* RFC3465: ABC Slow start
     * Increase only after a full MSS of bytes is acked
     *
     * TCP sender SHOULD increase cwnd by the number of
     * previously unacknowledged bytes ACKed by each incoming
     * acknowledgment, provided the increase is not more than L
     */
    //if (sysctl_tcp_abc && tp->bytes_acked < tp->mss_cache)
    //       return;
    
    //if (sysctl_tcp_max_ssthresh > 0 && tp->snd_cwnd > sysctl_tcp_max_ssthresh)
    //        cnt = sysctl_tcp_max_ssthresh >> 1;     /* limited slow start */
    //else
    //        cnt = tp->snd_cwnd;                     /* exponential increase */
    
    
    /* RFC3465: ABC
     * We MAY increase by 2 if discovered delayed ack
     */
    //if (sysctl_tcp_abc > 1 && tp->bytes_acked >= 2*tp->mss_cache)
    //        cnt <<= 1;
    
    tp->bytes_acked = 0;
    
    /*tp->snd_cwnd_cnt += cnt;
     while (tp->snd_cwnd_cnt >= tp->snd_cwnd) {
     tp->snd_cwnd_cnt -= tp->snd_cwnd;
     if (tp->snd_cwnd < tp->snd_cwnd_clamp)
     tp->snd_cwnd++;
     }*/
    
    tp->snd_cwnd_cnt = 0;
    tp->snd_cwnd = min_t(u32, tp->snd_cwnd + 20, tp->snd_cwnd_clamp); //upper limit
    //tp->snd_cwnd = max_t(u32, tp->snd_cwnd, 100U);			  //lower limit
}

void tcp_peera_cong_avoid_ai(struct tcp_sock *tp, u32 w)
{
    if (tp->snd_cwnd_cnt >= w) {
        if (tp->snd_cwnd < tp->snd_cwnd_clamp)
            tp->snd_cwnd++;
        tp->snd_cwnd_cnt = 0;
    } else {
        tp->snd_cwnd_cnt++;
    }
}


void tcp_peera_cong_avoid(struct sock *sk, u32 ack, u32 in_flight)
{
    struct tcp_sock *tp = tcp_sk(sk);
    
    printk(KERN_INFO "ACK\n");
    
    if (!tcp_is_cwnd_limited(sk, in_flight)) {
        printk(KERN_INFO "LIMITED !!!!!!!!!!!!!!!!!\n");
        return;
    }
    
    /* In "safe" area, increase. */
    if (tp->snd_cwnd <= tp->snd_ssthresh)
        tcp_peera_slow_start(tp);
    
    /* In dangerous area, increase slowly. */
    else {
        tcp_peera_cong_avoid_ai(tp, tp->snd_cwnd);
    }
}



/* Extract info for Tcp socket info provided via netlink. */
static void tcp_peera_info(struct sock *sk, u32 ext,
                           struct sk_buff *skb)
{
    const struct peera *ca = inet_csk_ca(sk);
    if (ext & (1 << (INET_DIAG_VEGASINFO - 1))) {
        struct tcpvegas_info info = {
            .tcpv_enabled = 1,
            .tcpv_rtt = jiffies_to_usecs(ca->rtt),
            .tcpv_minrtt = jiffies_to_usecs(ca->rtt_min),
        };
        
        nla_put(skb, INET_DIAG_VEGASINFO, sizeof(info), &info);
    }
}


static struct tcp_congestion_ops tcp_peera __read_mostly = {
    .init		= tcp_peera_init,
    .ssthresh	= tcp_peera_ssthresh,
    .cong_avoid	= tcp_peera_cong_avoid,
    .min_cwnd	= tcp_peera_bw_rttmin,
    .cwnd_event	= tcp_peera_event,
    .get_info	= tcp_peera_info,
    .pkts_acked	= tcp_peera_pkts_acked,
    
    .owner		= THIS_MODULE,
    .name		= "peera"
};

static int __init tcp_peera_register(void)
{
    BUILD_BUG_ON(sizeof(struct peera) > ICSK_CA_PRIV_SIZE);
    return tcp_register_congestion_control(&tcp_peera);
}

static void __exit tcp_peera_unregister(void)
{
    tcp_unregister_congestion_control(&tcp_peera);
}

module_init(tcp_peera_register);
module_exit(tcp_peera_unregister);

MODULE_AUTHOR("Peera Yoodee");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("TCP Peera Westwood+");