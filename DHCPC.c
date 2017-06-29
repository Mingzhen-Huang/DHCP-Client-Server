/*
	contributer: Wang Guanrui, Huang Mingzhen




*/
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
#define MAX 512

uint32_t clientIPAdd;
int length = 0;
void seteth1(int sock);
int st;

struct in_addr GetIpAdd();
enum dhcp_msg_type {
     DHCP_DISCOVER = 1,
     DHCP_OFFER    = 2,
     DHCP_REQUEST  = 3,
     DHCP_DECLINE  = 4,
     DHCP_ACK      = 5,
     DHCP_NAK      = 6,
     DHCP_RELEASE  = 7,
     DHCP_INFORM   = 8,
     DHCP_RREQUEST = 9,
};

enum op_types {
    BOOTREQUEST = 1,
    BOOTREPLY   = 2,   
};

enum hardware_address_types {
    ETHERNET     = 0x01,
    ETHERNET_LEN = 0x06,
};

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


void init_header(struct dhcp_msg *packet, uint8_t type, int time)
{
		

	length = 0;
	switch (type) {
	 
		case DHCP_DISCOVER:
			memset(packet, 0, sizeof(struct dhcp_msg));
			packet->hdr.xid=0xaaaaaaaa;
			packet->hdr.htype = ETHERNET;
			packet->hdr.hlen = ETHERNET_LEN;
			packet->hdr.hops= 0;
			packet->hdr.ciaddr=0;
			packet->hdr.yiaddr=0;
			packet->hdr.flags=0x0080;
			packet->hdr.secs=0x0000;
		    packet->hdr.siaddr=0x00000000;
		    packet->hdr.giaddr=0;  
		    GetLocalMacAddr(packet->hdr.chaddr);
		    packet->hdr.dhcp_magic=0x63538263;
		    packet->hdr.op = BOOTREQUEST;
		    addopt53(packet,type);
		    addopt60(packet);
		    addopt255(packet);
		    break;
		case DHCP_REQUEST:
			memset(packet->option, 0, sizeof(packet->option));
			//packet->hdr.xid=0x1111111a;
			packet->hdr.htype = ETHERNET;
			packet->hdr.hlen = ETHERNET_LEN;
			packet->hdr.hops= 0;
			//packet->hdr.ciaddr=0;
			//packet->hdr.yiaddr=0;
			packet->hdr.flags=0x0080;
			packet->hdr.secs=0x0000;
		    packet->hdr.siaddr=0x00000000;
		    packet->hdr.giaddr=0;  
		    GetLocalMacAddr(packet->hdr.chaddr);
		    packet->hdr.dhcp_magic=0x63538263;
		    packet->hdr.op = BOOTREQUEST;
		    addopt53(packet,type);
		    //addopt3(packet);
		    addopt54(packet);
		    addopt255(packet);
			break;
		case DHCP_RELEASE:
			memset(packet, 0, sizeof(struct dhcp_msg));
			//memset(packet->option, 0, sizeof(packet->option));
			packet->hdr.xid=0x11111111;
			packet->hdr.htype = ETHERNET;
			packet->hdr.hlen = ETHERNET_LEN;
			packet->hdr.hops= 0;
			packet->hdr.ciaddr=GetIpAdd().s_addr;
			packet->hdr.yiaddr=GetIpAdd().s_addr;
			packet->hdr.flags=0x0000;
			packet->hdr.secs=0x0000;
		    packet->hdr.siaddr=0x00000000;
		    packet->hdr.giaddr=0;  
		    GetLocalMacAddr(packet->hdr.chaddr);
		    packet->hdr.dhcp_magic=0x63538263;
		    packet->hdr.op = BOOTREQUEST;
		    addopt53(packet,type);
		     addopt55(packet);
		    //addopt3(packet);
		    addopt255(packet);
		    break;
		case DHCP_INFORM:
			memset(packet, 0, sizeof(struct dhcp_msg));
			packet->hdr.xid=0xbbbbbbbb;
			packet->hdr.htype = ETHERNET;
			packet->hdr.hlen = ETHERNET_LEN;
			packet->hdr.hops= 0;
			packet->hdr.ciaddr=GetIpAdd().s_addr;
			packet->hdr.yiaddr=GetIpAdd().s_addr;
			packet->hdr.flags=0x0080;
			packet->hdr.secs=0x0000;
		    packet->hdr.siaddr=0x00000000;
		    packet->hdr.giaddr=0;  
		    GetLocalMacAddr(packet->hdr.chaddr);
		    packet->hdr.dhcp_magic=0x63538263;
		    packet->hdr.op = BOOTREQUEST;
		    addopt53(packet,type);
		    addopt55(packet);
		    //addopt3(packet);
		    addopt255(packet);
			break;
		case DHCP_RREQUEST:
			memset(packet->option, 0, sizeof(packet->option));
			//packet->hdr.xid=0x1111111a;
			packet->hdr.htype = ETHERNET;
			packet->hdr.hlen = ETHERNET_LEN;
			packet->hdr.hops= 0;
			//packet->hdr.ciaddr=0;
			//packet->hdr.yiaddr=0;
			packet->hdr.flags=0x0000;
			packet->hdr.secs=0x0000;
		    packet->hdr.siaddr=0x00000000;
		    packet->hdr.giaddr=0;  
		    GetLocalMacAddr(packet->hdr.chaddr);
		    packet->hdr.dhcp_magic=0x63538263;
		    packet->hdr.op = BOOTREQUEST;
		    addopt53(packet,DHCP_REQUEST);
		    //addopt3(packet);
		    addopt54(packet);
		    addopt255(packet);
			break;
		
	
	}

	 
}

void addopt53(struct dhcp_msg *packet,int type){
	
	packet->option[length]=0x35;
	packet->option[length+1]=0x01;
	packet->option[length+2]=type;
	length += 3;

	

}

void addopt1(struct dhcp_msg *packet){
	//printf("%d\n",strlen(packet->option));
	
	packet->option[length]=0x01 ;
	packet->option[length+1]=0x04;
	packet->option[length+2]=0xff;
	packet->option[length+3]=0xff;
	packet->option[length+4]=0xff;
	packet->option[length+5]=0x80;
	length += 6;
}	

void addopt3(struct dhcp_msg *packet){
struct in_addr ip;
    

    packet->option[length]=0x03 ;

    packet->option[length+1]=0x04;
  //    ip=GetIpAdd();


ip=GetIpAdd();
    memcpy(packet->option+length+2,&ip,4);

    
    length += 6;
}
	


void addopt51(struct dhcp_msg *packet,int time){
	
	packet->option[length]=0x33;
	packet->option[length+1]=0x04;
	packet->option[length+2]=0;
	packet->option[length+3]=0;
	packet->option[length+4]=0;
	packet->option[length+5]=time;
	length += 6;
}

void addopt54(struct dhcp_msg *packet){

	
	//printf("%dlen\n",length );
	packet->option[length]=0x36;
	packet->option[length+1]=0x04;
	packet->option[length+2]=0xc0;
	packet->option[length+3]=0xa8;
	packet->option[length+4]=0x00;
	packet->option[length+5]=0x01;
	length += 6;
}

void addopt58(struct dhcp_msg *packet,int time){
	
	packet->option[length]=0x3a;
	packet->option[length+1]=0x04;
	packet->option[length+2]=0;
	packet->option[length+3]=0;
	packet->option[length+4]=0;
	packet->option[length+5]=time/2;
	length += 6;
}
void addopt59(struct dhcp_msg *packet,int time){
	
	packet->option[length]=0x3b;
	packet->option[length+1]=0x04;
	packet->option[length+2]=0;
	packet->option[length+3]=0;
	packet->option[length+4]=0;
	packet->option[length+5]=time/8*7;
	length += 6;
}

void addopt255(struct dhcp_msg *packet){
	
	
	packet->option[length]=0xff ;
}
void addopt60(struct dhcp_msg *packet){
	
	
	packet->option[length]=0x3c;
	packet->option[length+1]=0x0a;
	packet->option[length+2]=0x20;
	packet->option[length+3]=0x14;
	packet->option[length+4]=0x21;
	packet->option[length+5]=0x32;
	packet->option[length+6]=0x79;
	packet->option[length+7]=0x20;
	packet->option[length+8]=0x14;
	packet->option[length+9]=0x21;
	packet->option[length+10]=0x33;
	packet->option[length+11]=0x03;
	length += 12;
}
void addopt55(struct dhcp_msg *packet){
	
	
	packet->option[length]=0x37;
	packet->option[length+1]=0x0c;
	packet->option[length+2]=0x01;
	packet->option[length+3]=0x0f;
	packet->option[length+4]=0x03;
	packet->option[length+5]=0x06;
	packet->option[length+6]=0x2c;
	packet->option[length+7]=0x2e;
	packet->option[length+8]=0x2f;
	packet->option[length+9]=0x1f;
	packet->option[length+10]=0x21;
	packet->option[length+11]=0x79;
	packet->option[length+12]=0xf9;
	packet->option[length+13]=0x2b;
	length += 14;

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
struct in_addr GetIpAdd()    
{          
      int sock;
        struct sockaddr_in sin;
        struct ifreq ifr;

        sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock == -1)
        {
                perror("socket");
                exit(1);
        }

        strncpy(ifr.ifr_name, "eth1", IFNAMSIZ);
        ifr.ifr_name[IFNAMSIZ - 1] = 0;

        if (ioctl(sock, SIOCGIFADDR, &ifr) < 0)
        {
                perror("ioctl");
                exit(1);
        }

        memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
        return sin.sin_addr;
} 
void sendPacket(uint8_t type,int sock,dhcp_msg* message,int time){
	struct sockaddr_in severAddr;
	
	
	//message=(dhcp_msg*)malloc(sizeof(dhcp_msg));
	length = 0;
	init_header(message,type,time);	
	memset(&severAddr, 0, sizeof(severAddr));
	severAddr.sin_family = AF_INET; 
	char *ip;
	if(type==DHCP_RREQUEST||type==DHCP_RELEASE) {severAddr.sin_addr.s_addr = inet_addr("192.168.0.1");
	//printf("hh\n");
	}

	else severAddr.sin_addr.s_addr = inet_addr("255.255.255.255");
	severAddr.sin_port = htons(67);
	if (( sendto(sock,message, sizeof(*message), 0, (struct sockaddr *) &severAddr, sizeof(severAddr)))<0) 	printf("send failed%d.\n",errno);
	//close(sock);
}

void seteth1(int sock){
	int optval=1;
	struct ifreq if_eth1;
	strcpy(if_eth1.ifr_name,"eth1");
	//printf("%s hhh\n",if_eth1.ifr_hwaddr );
	socklen_t len=sizeof(optval);
	setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &optval, len);
	
	if(setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE,(char*)&if_eth1, sizeof(if_eth1))<0)		printf("bind eth1 error\n");

}
int setsocket(){
	int sock;
	

	if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)	printf("socket() failed.\n");


	
	

	return sock;

}

void setbind(int sock){
	struct sockaddr_in clientAddr;


	clientAddr.sin_family = AF_INET;
    
	clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	//clientAddr.sin_addr.s_addr =inet_addr("0.0.0.0");
	clientAddr.sin_port = htons(68);

	if ((bind(sock, (struct sockaddr *) &clientAddr,sizeof(clientAddr))) < 0)	printf("bind() failed %d.\n",errno);



}

struct dhcp_msg* receivePacket(int sock){
	struct dhcp_msg* message;
	struct sockaddr_in clientAddr;
	unsigned int cliAddrLen,recvMsgSize,status=0;
	
	message=(dhcp_msg*)malloc(sizeof(dhcp_msg));
	memset(message, 0, sizeof(struct dhcp_msg));
	struct timeval tv_out;
	tv_out.tv_sec = 3;//等待3秒
	tv_out.tv_usec = 0;
	setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,&tv_out, sizeof(tv_out));
	if ((recvMsgSize = recvfrom(sock,message,MAX,0,(struct sockaddr *) &clientAddr, &cliAddrLen)) < 0) st=0;
	else st=1;
	clientIPAdd=clientAddr.sin_addr.s_addr;
	//close(sock);
	return message;
}
void setIP(char *ifname,char *Ipaddr){
	int fd;
    int rc;
    struct ifreq ifr; 
    struct sockaddr_in *sin;
   // struct rtentry  rt;
    
	fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0)
    {
            perror("socket   error");     
               
    }
    memset(&ifr,0,sizeof(ifr)); 
    strcpy(ifr.ifr_name,ifname); 
    sin = (struct sockaddr_in*)&ifr.ifr_addr;     
    sin->sin_family = AF_INET;  
   
    //ipaddr
    if(inet_aton(Ipaddr,&(sin->sin_addr)) < 0)   
    {     
        perror("inet_aton   error");     
           
    }    
    
    if(ioctl(fd,SIOCSIFADDR,&ifr) < 0)   
    {     
        perror("ioctl   SIOCSIFADDR   error");     
           
    }
   // char mask[13]="255.255.255.0";
  // mask ="255.255.255.0";
    if(strcmp(Ipaddr,"0.0.0.0")!=0){
	    if(inet_aton("255.255.255.0",&(sin->sin_addr)) < 0)   
	    {     
	        perror("inet_pton   error");     
	       // return -4;     
	    }    
	    if(ioctl(fd, SIOCSIFNETMASK, &ifr) < 0)
	    {
	       perror("ioctl");
	        //return -5;
	    }
	}
}


int main(int argc, char const *argv[])
{
	dhcp_msg* message;
		int sock; /* Socket descriptor */
		int time=16,i=0;
		sock=setsocket();
		seteth1(sock);
		setbind(sock);
		struct in_addr ipp;
		
	while(1){
		if(strcmp(argv[1], "default") == 0){
			//time=atoi(argv[2]);
			//time=16;	
			
			sendPacket(DHCP_DISCOVER,sock,message,0);
			printf("Send DHCP_DISCOVER\n");
			
			message=receivePacket(sock);
			if(message->hdr.op==0){
				printf("unreceive DHCP_OFFER\n" );
				continue;
			}
			printf("receive DHCP_OFFER\n");
			sendPacket(DHCP_REQUEST,sock,message,0);
			printf("Send DHCP_REQUEST\n");
			
			message=receivePacket(sock);

			ipp.s_addr = message->hdr.yiaddr;
			if(message->hdr.op!=0){
					printf("receiver ACK\n");
				}else{
					printf("unreceive ACK\n");
				}
			setIP("eth1",inet_ntoa(ipp));
			printf("get ip : %s\n", inet_ntoa(ipp));
			time=message->option[8];
			printf("time use %d seconds\n", time);
			while(1){
				st=0;
				i=0;
				while(i<time/2){
					i++;
					sleep(1);
				}
				sendPacket(DHCP_RREQUEST,sock,message,0);
				printf("send DHCP_REQUEST\n");
				//memset(message, 0, sizeof(struct dhcp_msg));
				st=0;
				message=receivePacket(sock);
				if(message->hdr.op!=0){
					printf("receive DHCP_ACK\n");
					continue;
				}

 
				i=0;
				while(i<time/8*3){
					i++;
					sleep(1);
				}
				message->hdr.xid=0x66666666;
				sendPacket(DHCP_REQUEST,sock,message,0);
				printf("send DHCP_REQUEST\n");
				st=0;
				message=receivePacket(sock);
				if(message->hdr.op!=0){
					printf("receive DHCP_ACK\n");
					continue;
				}
				i=0;
				while(i<time/8){
					i++;
					sleep(1);
				}
				setIP("eth1","0.0.0.0");
				if(1) break;
				
			}	
		}else if(strcmp(argv[1], "release")==0){
				time=50;
				
				
				sendPacket(DHCP_RELEASE,sock,message,0);
				printf("send DHCP_RELEASE\n");
				setIP("eth1","0.0.0.0");
				//printf("hhhh\n");
				break;

		}else if(strcmp(argv[1], "request")==0){
			memset(message, 0, sizeof(struct dhcp_msg));
			message->hdr.yiaddr=inet_addr(argv[2]);
			message->hdr.xid=0x99999999;
			sendPacket(DHCP_RREQUEST,sock,message,0);
			printf("send DHCP_REQUEST\n");
			message=receivePacket(sock);
			if(message->option[2]==DHCP_NAK){
				printf("illegal ip\n");
				//argv[2]="hhh";
				//continue;
				break;
			}
			 setIP("eth1",argv[2]);      
			break;
		}else if(strcmp(argv[1], "inform")==0){
			sendPacket(DHCP_INFORM,sock,message,0);
			sleep(1);
			break;
		}
		

	}
	
	close(sock);




	return 0;
}
