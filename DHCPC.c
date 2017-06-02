#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), sendto()and recvfrom() */
#include <arpa/inet.h> /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h> /* for atoi() and exit() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */
#include <sys/socket.h>
#include <net/if.h>
#include <errno.h>
#include <sys/ioctl.h>

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



struct dhcp_option {
    uint8_t id;        // option id
    uint8_t len;       // option length
    uint8_t *data; 	// option data

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

    //dhcp_option_list list[10];
    uint8_t option[256];
   
};

typedef struct dhcp_msg dhcp_msg;


void init_header(struct dhcp_msg *packet, uint8_t type)
{
		


printf("in\n");
	switch (type) {
	 
		case DHCP_DISCOVER:
			memset(packet, 0, sizeof(struct dhcp_msg));
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
		    GetLocalMacAddr(packet->hdr.chaddr);
		    packet->hdr.dhcp_magic=0x63538263;
		    packet->hdr.op = BOOTREQUEST;
		    addopt53(packet);
		    addopt1(packet);
		    addopt3(packet);
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
	printf("%dxxx\n",strlen(packet->option));
	int length=strlen(packet->option);
	packet->option[length]=0x35;
	packet->option[length+1]=0x01;
	packet->option[length+2]=0x01;


}

void addopt1(struct dhcp_msg *packet){
	//printf("%d\n",strlen(packet->option));
	int length=strlen(packet->option);
	packet->option[length]=0x01 ;
	packet->option[length+1]=0x04;
	packet->option[length+2]=0xff;
	packet->option[length+3]=0xff;
	packet->option[length+4]=0xff;
	packet->option[length+5]=0x00;
	//printf("%d\n",strlen(packet->option) );
}	

void addopt3(struct dhcp_msg *packet){

	int length=strlen(packet->option)+1;
	//printf("%dlen\n",length );
	packet->option[length]=0x03 ;
	packet->option[length+1]=0x04;
	packet->option[length+2]=0x0a;
	packet->option[length+3]=0xc3;
	packet->option[length+4]=0x37;
	packet->option[length+5]=0x04;

}

void GetLocalMacAddr(uint8_t*szMac)
{
  
  int   sock;   
  struct   sockaddr_in   sin;   
  struct   sockaddr   sa;   
  struct   ifreq   ifr;   
  unsigned   char   mac[6];  


  sock=socket(AF_INET,SOCK_DGRAM,0);   
  if (sock==-1)   
  {   
    perror("socket");     }   

  strncpy(ifr.ifr_name,"eth1",sizeof(ifr.ifr_name));   
  ifr.ifr_name[IFNAMSIZ-1]   =   0;   

  memset(mac,0,sizeof(mac));   
  if (ioctl(sock,SIOCGIFHWADDR,&ifr)< 0)   
  {   
    perror("ioctl");   
  }   

  memcpy(&sa,&ifr.ifr_addr,sizeof(sin));   
  memcpy(mac,sa.sa_data,sizeof(mac));   
  int curmacstr[64];
  memset(curmacstr,0,sizeof(curmacstr));
  sprintf(curmacstr,"%.2X:%.2X:%.2X:%.2X:%.2X:%.2X",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
 
  strcpy(szMac,curmacstr);

  sscanf(curmacstr, "%02x:%02x:%02x:%02x:%02x:%02x ", &szMac[0], &szMac[1], &szMac[2], &szMac[3], &szMac[4], &szMac[5]); 

}

void sendPacket(uint8_t type,int sock,dhcp_msg* message){
	struct sockaddr_in severAddr;
	
	
	message=(dhcp_msg*)malloc(sizeof(dhcp_msg));
	init_header(message,type);	
	memset(&severAddr, 0, sizeof(severAddr));
	severAddr.sin_family = AF_INET; 
	char *ip;
	severAddr.sin_addr.s_addr = inet_addr("255.255.255.255");
	severAddr.sin_port = htons(67);
	//rintf("%dms\n", message->option[2]);
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

/*void receivePacket(){
	dhcp_msg* message;
	struct sockaddr_in clientAddr;
	unsigned int cliAddrLen;
	if ((recvMsgSize = recvfrom(sock, echoBuffer,1000,0,(struct sockaddr *) &clientAddr, &cliAddrLen)) < 0) printf("recvfrom() error.\n");
}*/
int main(int argc, char const *argv[])
{
	dhcp_msg* Dmessage;
	int sock; /* Socket descriptor */
	/*int time;
	if(argv[1]=="sleep"){
		time=argv[2]-'0';
	}*/
	

	sock=setnbind();
	seteth1(sock);
	sendPacket(DHCP_DISCOVER,sock,Dmessage);
	return 0;
}
/*
problems:
no error;
*/
