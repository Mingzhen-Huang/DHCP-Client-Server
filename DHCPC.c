#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), sendto()and recvfrom() */
#include <arpa/inet.h> /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h> /* for atoi() and exit() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */
#include <sys/socket.h>
#include <net/if.h>
#include <errno.h>

enum dhcp_msg_type {
     DHCP_DISCOVER = 1,
     DHCP_OFFER    = 2,
     DHCP_REQUEST  = 3,
     DHCP_DECLINE  = 4,
     DHCP_ACK      = 5,
     DHCP_NAK      = 6,
     DHCP_RELEASE  = 7,
     DHCP_INFORM   = 8,
};

enum op_types {
    BOOTREQUEST = 1,
    BOOTREPLY   = 2,   
};

enum hardware_address_types {
    ETHERNET     = 0x01,
    ETHERNET_LEN = 0x06,
};

/* DHCP message */

enum {
    DHCP_HEADER_SIZE = 236 // without size of options
};


struct dhcp_option {
    uint8_t id;        // option id
    uint8_t len;       // option length
    uint8_t data[256]; // option data

    //STAILQ_ENTRY(dhcp_option) pointers; // pointers, see queue(3)
};

typedef struct dhcp_option dhcp_option_list;

struct dhcp_message {
    uint8_t op;      // message op code, message type
    uint8_t htype;   // hardware address type
    uint8_t hlen;    // hardware address length
    uint8_t hops;    // incremented by relay agents

    uint32_t xid;    // transaction ID

    uint16_t secs;   // seconds since address acquisition or renewal
    uint16_t flags;  // flags

    uint32_t ciaddr; // client IP address
    uint32_t yiaddr; // 'your' client IP address
    uint32_t siaddr; // IP address of the next server to use in bootstrap
    uint32_t giaddr; // relay agent IP address

    uint8_t chaddr[16]; // client hardware address

    uint8_t sname[64]; // server host name

    uint8_t file[128]; // boot file name

    uint8_t options[312]; // optional parameters field
};

typedef struct dhcp_message dhcp_message;

struct dhcp_msg {
    dhcp_message hdr;
    //dhcp_option_list opts;
    uint8_t data[256];
};

typedef struct dhcp_msg dhcp_msg;


void init_header(struct dhcp_msg *packet, uint8_t type)
{
 	
	memset(packet, 0, sizeof(struct dhcp_msg));
	switch (type) {
	 
		case DHCP_DISCOVER:
			packet->hdr.xid=0x11111111;
		case DHCP_REQUEST:
		case DHCP_RELEASE:
		case DHCP_INFORM:
		packet->hdr.op = BOOTREQUEST;
		break;
		case DHCP_OFFER:
		case DHCP_ACK:
		case DHCP_NAK:
		packet->hdr.op = BOOTREPLY;
		
	
	}
	packet->hdr.htype = ETHERNET;
	packet->hdr.hlen = ETHERNET_LEN;
	packet->hdr.hops= 0;
	
	packet->hdr.flags=0x0001;
	packet->hdr.secs=10;
	//packet->hdr.chaddr={0x0,0x1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
	
	//memcpy({1,1,1,1,1,1,1,1},packet->hdr.chaddr,sizeof({1,1,1,1,1,1,1,1}));
	//packet->cookie = htonl(DHCP_MAGIC);
	//packet->options[0] = DHCP_END;
	//packet->opts.len = 255;
	packet->data[255]=0xff;

	//add_simple_option(packet->options, DHCP_MESSAGE_TYPE, type);
	 
}


void sendPacket(uint8_t type,dhcp_msg* packet){
	init_header(packet,type);
}

int main(int argc, char const *argv[])
{
	int sock; /* Socket descriptor */
	const int optval=1;
	struct ifreq if_eth1;
	struct sockaddr_in severAddr;
	struct sockaddr_in clientAddr;
	dhcp_msg* message;
	message=(dhcp_msg*)malloc(sizeof(dhcp_msg));


	if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)	printf("socket() failed.\n");


	strcpy(if_eth1.ifr_name,"eth1");
	//printf("%s hhh\n",if_eth1.ifr_hwaddr );
	socklen_t len=sizeof(optval);
	setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &optval, len);
	
	if(setsockopt(sock, SOL_SOCKET, SO_BROADCAST,(char*)&if_eth1, sizeof(if_eth1))<0)		printf("bind eth1 error\n");
	
	
   
		
	memset(&severAddr, 0, sizeof(severAddr));
	severAddr.sin_family = AF_INET; 
	char *ip;
	severAddr.sin_addr.s_addr = inet_addr("255.255.255.255");
	severAddr.sin_port = htons(67);
	printf("%d\n", errno);

	clientAddr.sin_family = AF_INET;
    
	clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	clientAddr.sin_port = htons(68);
	printf("%djhjhf\n",clientAddr.sin_addr.s_addr);
	printf("%d\n", errno);
	//if ((bind(sock, (struct sockaddr *) &clientAddr,sizeof(clientAddr))) < 0)	printf("bind() failed.\n");
	printf("%d\n", errno);
	//if ((bind(sock, (struct sockaddr *) &severAddr,sizeof(severAddr)) < 0))   printf("bind() failed.\n");


	sendPacket(DHCP_ACK,message);
	printf("%d\n",message->hdr.op );
	if (( sendto(sock,message, sizeof(message), 0, (struct sockaddr *) &severAddr, sizeof(severAddr)))<0) 	printf("send failed\n");
	
	
	close(sock);

	return 0;
}
/*
problems:
1. cannot bind port 68  errno 98
2. how to deal with options
	



*/
