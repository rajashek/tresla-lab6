sudo sysctl -w net.core.rmem_max=803886080
sudo sysctl -w net.core.wmem_max=803886080
sudo sysctl -w net.core.rmem_default=600553600
sudo sysctl -w net.core.wmem_default=600553600
sudo sysctl -w net.ipv4.tcp_rmem='600388608 600553600 803886080'
sudo sysctl -w net.ipv4.tcp_wmem='600388608 600553600 803886080'
sudo sysctl -w net.ipv4.tcp_mem='600388608 600553600 803886080'
sudo sysctl -w net.ipv4.tcp_retries2=4000
sudo sysctl -w net.ipv4.tcp_frto=0
sudo sysctl -w net.ipv4.tcp_frto_response=2
sudo sysctl -w net.ipv4.tcp_syn_retries=20
sudo sysctl -w net.ipv4.tcp_synack_retries=20
make clean && make
sudo insmod ./tcp_mili.ko
sudo sysctl -w net.ipv4.tcp_congestion_control=mili

