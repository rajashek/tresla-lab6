sudo sysctl -w net.ipv4.tcp_congestion_control=cubic
sudo rmmod tcp_mili
make clean >/dev/null
