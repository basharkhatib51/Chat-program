#include<stdio.h>
#include<winsock2.h>
#include<pthread.h>
#include<conio.h>
#pragma comment(lib,"ws2_32.lib")
void copy(char *ms){
	int x=strlen(ms);
	for(int i=1;i<x;i++){
		ms[i-1]=ms[i];
	}
	ms[x-1]=NULL;
}
void * read(void * s){
	SOCKET S=(SOCKET) s;
	char msg[10000];
	char file[10000];
	int msg_size,file_size;
	FILE *output;
	while(1){
		msg_size=recv(S,msg,10000,0);
		if(msg_size==SOCKET_ERROR){
			puts("the server disconnected");
			return NULL;
		}
		msg[msg_size]=NULL;
		if(msg[0]=='0'){
			copy(msg);
			puts(msg);
		}
		else if(msg[0]=='1'){
			copy(msg);
			char file_location[200];
			strcpy(file_location,"download/");
			strcat(file_location,msg);
			printf("opening file %s\n",file_location);
			output=fopen(file_location,"w");
			
		}
		else if(msg[0]=='2'){
			copy(msg);
			puts("writing to file");
			fprintf(output,"%s",msg);
			fclose(output);
		}
	}
}
void public_chat(SOCKET s){
	char msg[10000],temp[2000];
	char op;
	while(1){
		strcpy(msg,"");
		strcpy(temp,"");
		puts("to enter a message press 1 to send file press 2");
		op=getch();
		if(op=='1')
		{
			printf("enter your message : ");
			gets(temp);
			strcpy(msg,"1");
			strcat(msg,temp);
			send(s,msg,strlen(msg),0);
		}
		else if(op=='2')
		{
			printf("enter file location : ");
			gets(temp);
			char filename[200];
			char file[10000];
			printf("enter file name and extention : ");
			gets(filename);
			strcat(temp,filename);
			puts(temp);
			FILE *input;
			char get_char;
			input = fopen(temp, "rb");
			int i=0;
			strcpy(file,"");
			while((get_char=fgetc(input))!= EOF)
			   {
				   file[i++]=get_char;
			   }
			file[i]=NULL;
			fclose(input);
			strcpy(msg,"2");
			strcat(msg,filename);
			send(s,msg,strlen(msg),0);
			strcpy(msg,"3");
			strcat(msg,file);
			send(s,msg,strlen(msg),0);
		}

	}
}
void main()
{
	WSADATA wsa;
	SOCKET s;
	char ip[30];
	struct sockaddr_in server;
	char message[2000],msg[2000], server_reply[2000],name[20];
	int recv_size;
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		printf("Failed. Error Code : %d",WSAGetLastError());
	}
	if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d" , WSAGetLastError());
	}
	puts("enter the ip adress of server :");
	gets(ip);
	server.sin_addr.s_addr = inet_addr(ip);
	server.sin_family = AF_INET;
	server.sin_port = htons(1996);

	if (connect(s , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		puts("connect error");
	}
	puts("Connected");
	pthread_t th;
	while(1){
		puts("enter your name :");
		gets(name);
		strcpy(msg,"0");
		strcat(msg,name);
		send(s,msg,strlen(msg),0);
		int size=recv(s,msg,2000,0);
		msg[size]=NULL;
		if(strcmp(msg,"ok")==0){break;}
		else{puts(msg);}
	}
	pthread_create(&th,NULL,read,(void *)s);
	while(1){
		strcpy(msg,"");
		strcpy(message,"");
		public_chat(s);
	}
}
