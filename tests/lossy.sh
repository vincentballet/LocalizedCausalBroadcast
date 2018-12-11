#!/bin/bash
echo "This script will add loss to ICMP and UDP traffic on the lo interface"
sudo iptables -t mangle -A POSTROUTING -o lo -p icmp -j CLASSIFY --set-class 1:3
sudo iptables -t mangle -A POSTROUTING -o lo -p udp -j CLASSIFY --set-class 1:3
sudo tc qdisc del dev lo root
sudo tc qdisc add dev lo root handle 1: prio
sudo tc qdisc add dev lo parent 1:3 handle 30: netem delay 50ms 200ms loss 10% 25% reorder 25% 50%
