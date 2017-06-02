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
    uint8_t data; 	// option data

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

  	uint32_t dhcp_magic;

  	
};

typedef struct dhcp_message dhcp_message;

struct dhcp_msg {
    dhcp_message hdr;

    //dhcp_option_list list[32];
    uint8_t *option;
   
};

typedef struct dhcp_msg dhcp_msg;


void init_header(struct dhcp_msg *packet, uint8_t type)
{
	
 	memset(packet, 0, sizeof(struct dhcp_msg));
printf("in\n");
	switch (type) {
	 
		case DHCP_DISCOVER:
			packet->hdr.xid=0x11111111;
			packet->hdr.htype = ETHERNET;
			packet->hdr.hlen = ETHERNET_LEN;
			packet->hdr.hops= 0;
			packet->hdr.ciaddr=0;
			packet->hdr.yiaddr=0;
			packet->hdr.flags=0x8000;
			packet->hdr.secs=0x0000;
		    packet->hdr.siaddr=0x00000000;
		    packet->hdr.giaddr=0;  
		    packet->hdr.dhcp_magic=0x63538263;  
		    packet->option=(uint8_t*)malloc(11*sizeof(uint8_t)); 
		    printf("%dsize\n",sizeof(packet->option) );
		    addopt53(packet);
		    packet->hdr.op = BOOTREQUEST;
		    break;
		case DHCP_REQUEST:
		case DHCP_RELEASE:
		case DHCP_INFORM:
		
		break;
		case DHCP_OFFER:
		case DHCP_ACK:
		case DHCP_NAK:
		packet->hdr.op = BOOTREPLY;
		
	
	}

	
	 
}

void addopt53(struct dhcp_msg *packet){
	printf("%d\n",strlen(packet->option));
	memset(packet->option, 0, sizeof(packet->option));
	int length=strlen(packet->option);
	packet->option[0]=0x35;
	packet->option[1]=0x01;
	packet->option[2]=0x05;
	packet->option[3]=0x05;
	packet->option[4]=0x05;
	packet->option[5]=0x05;
	packet->option[6]=0x05;
	packet->option[7]=0x05;
	packet->option[8]=0x05;
	packet->option[9]=0x05;
	packet->option[10]=0x05;
	


	/*int length=1;
    packet->list[0].len=length;

    packet->list[0].id=53;
   	packet->list[0].data=(uint8_t*)malloc(sizeof(uint8_t)*length);
   	packet->list[0].data[0]=0x05;*/
}


void sendPacket(uint8_t type,int sock){
	struct sockaddr_in severAddr;
	dhcp_msg* message;
	
	message=(dhcp_msg*)malloc(sizeof(dhcp_msg));
	init_header(message,type);	
	memset(&severAddr, 0, sizeof(severAddr));
	severAddr.sin_family = AF_INET; 
	char *ip;
	severAddr.sin_addr.s_addr = inet_addr("255.255.255.255");
	severAddr.sin_port = htons(67);
	printf("%dms\n", message->option[2]);
	if (( sendto(sock,message, sizeof(*message), 0, (struct sockaddr *) &severAddr, sizeof(severAddr)))<0) 	printf("send failed\n");
	close(sock);
}

void seteth1(int sock){
	int optval=1;
	struct ifreq if_eth1;
	strcpy(if_eth1.ifr_name,"eth1");
	//printf("%s hhh\n",if_eth1.ifr_hwaddr );
	socklen_t len=sizeof(optval);
	setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &optval, len);
	
	if(setsockopt(sock, SOL_SOCKET, SO_BROADCAST,(char*)&if_eth1, sizeof(if_eth1))<0)		printf("bind eth1 error\n");

}
int setnbind(){
	int sock;
	struct sockaddr_in clientAddr;

	if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)	printf("socket() failed.\n");
	clientAddr.sin_family = AF_INET;
    
	//clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	clientAddr.sin_addr.s_addr =inet_addr("0.0.0.0");
	clientAddr.sin_port = htons(17777);
	
	if ((bind(sock, (struct sockaddr *) &clientAddr,sizeof(clientAddr))) < 0)	printf("bind() failed.\n");
	return sock;

}

void receivePacket(){
	dhcp_msg* message;
	struct sockaddr_in clientAddr;
	unsigned int cliAddrLen;
	if ((recvMsgSize = recvfrom(sock, echoBuffer,1000,0,(struct sockaddr *) &clientAddr, &cliAddrLen)) < 0) printf("recvfrom() error.\n");
}
int main(int argc, char const *argv[])
{
	int sock; /* Socket descriptor */
	
	
	
	

	sock=setnbind();
	seteth1(sock);
	sendPacket(DHCP_DISCOVER,sock);
	printf("%d\n",errno);
	return 0;
}
/*
problems:
1. message packet data struct error
*/
