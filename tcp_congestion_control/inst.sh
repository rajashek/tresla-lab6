sudo sysctl -w net.core.rmem_max=403886080
sudo sysctl -w net.core.wmem_max=403886080
sudo sysctl -w net.core.rmem_default=200553600
sudo sysctl -w net.core.wmem_default=200553600
sudo sysctl -w net.ipv4.tcp_rmem='80388608 200553600 403886080'
sudo sysctl -w net.ipv4.tcp_wmem='80388608 200553600 403886080'
sudo sysctl -w net.ipv4.tcp_mem='80388608 200553600 403886080'
sudo sysctl -w net.ipv4.tcp_retries2=4000
sudo sysctl -w net.ipv4.tcp_frto=1
sudo sysctl -w net.ipv4.tcp_frto_response=2
make clean && make
sudo insmod ./tcp_mili.ko
sudo sysctl -w net.ipv4.tcp_congestion_control=mili

