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
#include <time.h>
#define MAX 512


//int readippool(char ippool[][20]);
//int readiplease(struct lease iplease[20]);
//int writeiplease(struct lease iplease[20]);
//int removeiplease(struct lease iplease[20],char ip[20]);

struct lease
{

	char ipaddr[20];
	char macaddr[30];
	char ttime[30];
	
};

//read ip pool from file
int readippool(char ippool[][20]){

	FILE *fp;
	char StrLine[20];
	
	int i=0;
	if((fp = fopen("dhcp.config","r")) == NULL) //判断文件是否存在及可读
    { 
		printf("error!"); 
		return -1;
		
    }

    while(!feof(fp)){
        memset(ippool[i],0,sizeof(StrLine));
    	fgets(StrLine,1024,fp);
    	strcpy(ippool[i],StrLine);
    	i++;

    }
    fclose(fp);

    return 0;





}

//read ip lease from file
int readiplease(struct lease iplease[20]){

	FILE* fp;
	char str[100];
	int i=0;
	if((fp = fopen("dhcp.lease","r")) == NULL) //判断文件是否存在及可读
    { 
		printf("error!"); 
		return -1;
		
    }

    while(!feof(fp)){


        memset(str,0,sizeof(str));



    	if (fgets(str,1024,fp)!=0)
    	{
			strcpy(iplease[i].ipaddr,strtok(str," "));
    		strcpy(iplease[i].macaddr,strtok(NULL," "));
    		strcpy(iplease[i].ttime,strtok(NULL," "));
    	}
    	


    	


    	i++;

    }
    fclose(fp);

    return 0;






}

//write iplease to file
int writeiplease(struct lease iplease){


	FILE* fp;
	int i=0;
	if((fp = fopen("dhcp.lease","a+")) == NULL) 
    { 
		printf("error!"); 
		return -1;
		
    }
    time_t t;
	t=time(0);
	//iplease.ttime=t;
	sprintf(iplease.ttime,"%d",t);
	//printf("%d\n",t );
	//printf("%d\n",errno );

		fprintf(fp,"%s %s %s\n",iplease.ipaddr,iplease.macaddr,iplease.ttime);


    





    fclose(fp);
    return 0;




}

//remove iplease term according to given ip(need readiplease() first)
int removeiplease(struct lease iplease[20],char ip[20]){

	FILE* fp;
	int i=0;
	if((fp = fopen("dhcp.lease","w+")) == NULL) 
    { 
		printf("error!"); 
		return -1;
		
    }

    while(strlen(iplease[i].ipaddr) != 0){


		if (iplease[i].ipaddr[strlen(iplease[i].ipaddr)-1]=='\n')
		{
			iplease[i].ipaddr[strlen(iplease[i].ipaddr)-1]='\0';
		}
		if (ip[strlen(ip)-1]=='\n')
		{
			ip[strlen(ip)-1]='\0';
		}

    	if (strcmp(ip,iplease[i].ipaddr)!=0)
    	{
    		fprintf(fp,"%s %s %s",iplease[i].ipaddr,iplease[i].macaddr,iplease[i].ttime);
    	}

		
		i++;



    }

    





    fclose(fp);
    return 0;



}




struct in_addr GetIpAdd();
int length = 0;

enum dhcp_msg_type {
     DHCP_DISCOVER = 1,
     DHCP_OFFER    = 2,
     DHCP_REQUEST  = 3,
     DHCP_DECLINE  = 4,
     DHCP_ACK      = 5,
     DHCP_NAK      = 6,
     DHCP_RELEASE  = 7,
     DHCP_INFORM   = 8,
     DHCP_IACK     = 9,
};

enum op_types {
    BOOTREQUEST = 1,
    BOOTREPLY   = 2,   
};

enum hardware_address_types {
    ETHERNET     = 0x01,
    ETHERNET_LEN = 0x06,
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


void init_header(struct dhcp_msg *packet, uint8_t type, int ttime)
{
	length = 0;	

	//printf("%d\n",ttime );
	
	switch (type) {
	 	
		case DHCP_OFFER:
		//memset(packet, 0, sizeof(struct dhcp_msg));
			memset(packet->option, 0, sizeof(packet->option));

			packet->hdr.secs=ttime;
		    packet->hdr.op = BOOTREPLY;
		    addopt53(packet,type);
		    addopt51(packet,ttime);
		    addopt1(packet);
		    addopt3(packet);
		    addopt6(packet);
		    addopt54(packet);
		    addopt58(packet,ttime);
		    addopt59(packet,ttime);
		    addopt255(packet);
		    break;
		case DHCP_ACK:
			memset(packet->option, 0, sizeof(packet->option));
			packet->hdr.secs=ttime;
		    packet->hdr.op = BOOTREPLY;
		    addopt53(packet,type);
		    addopt51(packet,ttime);
		    addopt1(packet);
		    addopt3(packet);
		    addopt6(packet);
		    addopt54(packet);
		    addopt58(packet,ttime);
		    addopt59(packet,ttime);
		    addopt255(packet);
		    break;
		case DHCP_IACK:
			memset(packet->option, 0, sizeof(packet->option));
			packet->hdr.flags=0x0000;
		    packet->hdr.op = BOOTREPLY;
		    addopt53(packet,DHCP_ACK);
		    addopt1(packet);
		    addopt3(packet);
		    addopt6(packet);
		    addopt54(packet);
		    addopt58(packet);
		    addopt59(packet);
		    addopt255(packet);
		    break;
		case DHCP_NAK:
			memset(packet->option, 0, sizeof(packet->option));
			packet->hdr.ciaddr=0;
			packet->hdr.yiaddr=0;
			packet->hdr.flags=0x0080;
			packet->hdr.secs=0x0000;
		    packet->hdr.siaddr=0x00000000;
		    packet->hdr.giaddr=0;  
		    packet->hdr.dhcp_magic=0x63538263;
		    packet->hdr.op = BOOTREPLY;
		    addopt53(packet,type);
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
	packet->option[length+5]=0x00;
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
	


void addopt51(struct dhcp_msg *packet,int ttime){
	
	packet->option[length]=0x33;
	packet->option[length+1]=0x04;
	packet->option[length+2]=0;
	packet->option[length+3]=0;
	packet->option[length+4]=0;
	packet->option[length+5]=ttime;
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

void addopt58(struct dhcp_msg *packet,int ttime){
	
	packet->option[length]=0x3a;
	packet->option[length+1]=0x04;
	packet->option[length+2]=0;
	packet->option[length+3]=0;
	packet->option[length+4]=0;
	packet->option[length+5]=ttime/2;
	length += 6;
}
void addopt59(struct dhcp_msg *packet,int ttime){
	
	packet->option[length]=0x3b;
	packet->option[length+1]=0x04;
	packet->option[length+2]=0;
	packet->option[length+3]=0;
	packet->option[length+4]=0;
	packet->option[length+5]=ttime/8*7;
	length += 6;
}

void addopt255(struct dhcp_msg *packet){
	
	
	packet->option[length]=0xff ;
}
void addopt60(struct dhcp_msg *packet){
	
	
	packet->option[length]=0x3c;
	packet->option[length+1]=0x05;
	packet->option[length+2]=0x20;
	packet->option[length+3]=0x14;
	packet->option[length+4]=0x21;
	packet->option[length+5]=0x32;
	packet->option[length+6]=0x79;
	length += 7;
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
void addopt6(struct dhcp_msg *packet){
	
	
	packet->option[length]=0x06;
	packet->option[length+1]=0x08;
	packet->option[length+2]=0x08;
	packet->option[length+3]=0x08;
	packet->option[length+4]=0x08;
	packet->option[length+5]=0x08;
	packet->option[length+6]=0x08;
	packet->option[length+7]=0x08;
	packet->option[length+8]=0x08;
	packet->option[length+9]=0x09;
	
	length += 10;

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

void sendPacket(uint8_t type,int sock,dhcp_msg* message,int ttime){
	struct sockaddr_in severAddr;
	
	length = 0;
	//message=(dhcp_msg*)malloc(sizeof(dhcp_msg));
	init_header(message,type,ttime);	
	memset(&severAddr, 0, sizeof(severAddr));
	severAddr.sin_family = AF_INET;
	//if(type==DHCP_DISCOVER)  
	if(type==DHCP_IACK) {
		severAddr.sin_addr.s_addr = message->hdr.yiaddr;
	//	printf("%d\n", message->hdr.yiaddr);
	}
	else severAddr.sin_addr.s_addr = inet_addr("255.255.255.255");
	severAddr.sin_port = htons(68);
	//rintf("%dms\n", message->option[2]);
	if (( sendto(sock,message, sizeof(*message), 0, (struct sockaddr *) &severAddr, sizeof(severAddr)))<0) 	printf("send failed\n");
	//close(sock);
}

/*void sendPacketIP(uint8_t type,int sock,dhcp_msg* message,int ttime,char ipaddr[20]){
	struct sockaddr_in severAddr;
	
	length = 0;
	//message=(dhcp_msg*)malloc(sizeof(dhcp_msg));
	init_header(message,type,ttime);
	in_addr_t ip;
	ip = inet_addr(ipaddr);
	memcpy(message->hdr.yiaddr,&ip,strlen(ip));	
	memset(&severAddr, 0, sizeof(severAddr));
	severAddr.sin_family = AF_INET;
	//if(type==DHCP_DISCOVER)  
	severAddr.sin_addr.s_addr = ip;
	severAddr.sin_port = htons(68);
	//rintf("%dms\n", message->option[2]);

	if (( sendto(sock,message, sizeof(*message), 0, (struct sockaddr *) &severAddr, sizeof(severAddr)))<0) 	printf("send failed\n");
	//close(sock);
}*/



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
	clientAddr.sin_port = htons(67);

	if ((bind(sock, (struct sockaddr *) &clientAddr,sizeof(clientAddr))) < 0)	printf("bind() failed %d.\n",errno);



}

struct dhcp_msg* receivePacket(int sock){
	struct dhcp_msg* message;
	struct sockaddr_in clientAddr;
	unsigned int cliAddrLen,recvMsgSize;
	
	message=(dhcp_msg*)malloc(sizeof(dhcp_msg));
	memset(message, 0, sizeof(struct dhcp_msg));
	if ((recvMsgSize = recvfrom(sock,message,MAX,0,(struct sockaddr *) &clientAddr, &cliAddrLen)) < 0) printf("recvfrom() error.\n");
	//close(sock);
	return message;
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

int main(int argc, char const *argv[])
{
	int totaltime=atoi(argv[1]);
	printf("server start\n");
	dhcp_msg* Dmessage,*message;
	int sock = 0;

	sock=setsocket();
	seteth1(sock);
	setbind(sock);

	while(1){

		message=receivePacket(sock);
		

		if (message->option[2]==DHCP_INFORM)
		{
			if(message->hdr.op!=0){
					printf("receiver DHCP_INFORM\n");
				}
			sendPacket(DHCP_IACK,sock,message,0);
			if(message->hdr.op!=0){
					printf("send ack\n");
				}
		}
		else if (message->option[2]==DHCP_RELEASE)
		{
			if(message->hdr.op!=0){
					printf("receiver DHCP_RELEASE\n");
				}
			struct lease iplease[20];
			char b[20];
			memset(b,0,sizeof(b));
			memset(iplease,0,sizeof(iplease));
			readiplease(iplease);
			struct in_addr ippa;
			ippa.s_addr=message->hdr.ciaddr;

			strcpy(b,inet_ntoa(ippa));




			removeiplease(iplease,b);
			
		}
		else if (message->option[2]==DHCP_DISCOVER)
		{

			if(message->hdr.op!=0){
					printf("receiver DHCP_DISCOVER\n");
				}

			struct lease iplease[20];
			char ippool[20][20];
			char b[20];
			memset(ippool,0,sizeof(ippool));
			memset(b,0,sizeof(b));
			memset(iplease,0,sizeof(iplease));
			readiplease(iplease);
			readippool(ippool);
			int i = 0;
			int t = 0;
			int r = 0;
			while(1){

				t = 0;
				if (strlen(ippool[i])==0)
				{
					printf("No ip available\n");
					break;
				}




				while(1){

					if (strlen(iplease[t].ipaddr) == 0)
					{
						r = 1;
						break;
					}

					if (strcmp(ippool[i],iplease[t].ipaddr)==0)
					{
						break;
					}

					t++;

				}

				if (r == 1)
				{
					break;
				}

				i++;







			}

			uint32_t ip;

			ip = inet_addr(ippool[i]);
			message->hdr.yiaddr = ip;
				



			sendPacket(DHCP_OFFER,sock,message,totaltime);
		}
		else if (message->option[2]==DHCP_REQUEST)
		{
			printf("receive DHCP_REQUEST\n");

				if (message->hdr.yiaddr==0)
				{
					sendPacket(DHCP_ACK,sock,message,totaltime);
				}
				else{


					


					

					struct lease iplease[20];
					char ippool[20][20];
					char b[20];
					memset(ippool,0,sizeof(ippool));
					memset(b,0,sizeof(b));
					memset(iplease,0,sizeof(iplease));
					readiplease(iplease);
					readippool(ippool);
					int i = 0;
					int t = 0;

					while(1){

						if (strlen(ippool[i])==0)
						{
							printf("illegal ip\n");
							sendPacket(DHCP_NAK,sock,message,totaltime);
							break;
						}

						if (inet_addr(ippool[i])==message->hdr.yiaddr)
						{
							//printf("%d\n",inet_addr(ippool[i]) );
							removeiplease(iplease,ippool[i]);
							struct lease ipp;
							strcpy(ipp.ipaddr,ippool[i]);
							if (ipp.ipaddr[strlen(ipp.ipaddr)-1]=='\n')
							{
								ipp.ipaddr[strlen(ipp.ipaddr)-1]='\0';
							}
							sprintf(ipp.macaddr,"%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",message->hdr.chaddr[0],message->hdr.chaddr[1],message->hdr.chaddr[2],message->hdr.chaddr[3],message->hdr.chaddr[4],message->hdr.chaddr[5]);
							printf("allocate IP for %s\n",ipp.macaddr );
							strcpy(ipp.ttime,"");
							writeiplease(ipp);
							printf("logging\n");
							uint32_t ip;

							ip = inet_addr(ippool[i]);
							message->hdr.yiaddr = ip;


							sendPacket(DHCP_ACK,sock,message,totaltime);
							break;
						}


						i++;
					}




					}







				
			


		}
		



	}
	close(sock);
	return 0;
}
/*
problems:
1. message packet data struct error
*/
