#include<stdio.h>
#include<stdlib.h>
#include<WinSock2.h>
#include<pthread.h>
#include<conio.h>
#pragma comment(lib,"ws2_32.lib")
#define max_clients_num 10
typedef struct clients {
	int sk;
	char name[20];
	char pub_message[2000];
	char filename[100];
	char file[10000];
	char rec_pub_message[max_clients_num][20];
	char rec_pub_file[max_clients_num][20];

};
clients cl[10];


void copy(char *ms){
	int x=strlen(ms);
	for(int i=1;i<x;i++){
		ms[i-1]=ms[i];
	}
	ms[x-1]=NULL;
}
bool cheak_name(char *ms){
	for(int i=0;i<max_clients_num;i++){
		if(cl[i].sk!=0 && strcmp(ms,cl[i].name)==0){return false;}
	}
	return true;
}
void * reciv(void * s){
	puts("thread created");
	SOCKET new_clinet=(SOCKET) s;
	int reciv_size,i=0;
	for(i=0;i<max_clients_num;i++){
		if(cl[i].sk==new_clinet){
			break;
		}
	}
	char message[2000];
	while(1){
		reciv_size = recv(new_clinet , message , 2000 , 0);
		if(reciv_size==SOCKET_ERROR){
			printf("the client %s disconnected\n",cl[i].name);
			cl[i].sk=0;
			strcpy(cl[i].name,"");
			return NULL;
		}
		message[reciv_size]=NULL;
		if(message[0] == '0'){
			copy(message);
			if(cheak_name(message)){
				strcpy(cl[i].name,message);
				printf("the client_id=%d choosed his name : %s\n",cl[i].sk,cl[i].name);
				char *temp="ok";
				send(new_clinet,temp,strlen(temp),0);
			}
			else{
				char *temp="sorry this name is already in use choose another one";
				send(new_clinet,temp,strlen(temp),0);
			}
		}
		else if(message[0]=='1'){
			copy(message);
			strcpy(cl[i].pub_message,message);
			for(int z=0;z<max_clients_num;z++){
				strcpy(cl[i].rec_pub_message[z],"");
			}
			strcpy(cl[i].rec_pub_message[i],cl[i].name);
			printf("the client %s send to public chat : %s\n",cl[i].name,cl[i].pub_message);
		}
		else if(message[0]=='2'){
			copy(message);
			strcpy(cl[i].filename,message);
			printf("the client %s set file name : %s\n",cl[i].name,cl[i].filename);
		}
		else if(message[0]=='3'){
			copy(message);
			strcpy(cl[i].file,message);
			for(int z=0;z<max_clients_num;z++){
				strcpy(cl[i].rec_pub_file[z],"");
			}
			strcpy(cl[i].rec_pub_file[i],cl[i].name);
			printf("the client %s send file \n",cl[i].name);
		}
		strcpy(message,"");
	}
}
void * send(void * s){
	SOCKET new_clinet=(SOCKET) s;
	int cx=0;
	char msg[2000];
	for(cx;cx<max_clients_num;cx++){
		if(new_clinet==cl[cx].sk){break;}
	}
	while(1){
		for(int i=0;i<max_clients_num;i++){
			if(strcmp(cl[i].pub_message,"")!=0 && strcmp(cl[i].rec_pub_message[cx],cl[cx].name)!=0 ){
				strcpy(msg,"0");
				strcat(msg,cl[i].name);
				strcat(msg," : ");
				strcat(msg,cl[i].pub_message);
				send(new_clinet,msg,strlen(msg),0);
				strcpy(cl[i].rec_pub_message[cx],cl[cx].name);
			}
			if(strcmp(cl[i].file,"")!=0 && strcmp(cl[i].rec_pub_file[cx],cl[cx].name)!=0 ){
				strcpy(msg,"1");
				strcat(msg,cl[i].filename);
				send(new_clinet,msg,strlen(msg),0);
				strcpy(msg,"2");
				strcat(msg,cl[i].file);
				send(new_clinet,msg,strlen(msg),0);
				strcpy(cl[i].rec_pub_file[cx],cl[cx].name);
			}
		}
	}
	return NULL;
}
void ini_cl(){
	for(int i=0;i<max_clients_num;i++){
		cl[i].sk=0;
	}
}
void main(){
	WSADATA wsa;
	SOCKET s , new_socket;
	struct sockaddr_in server , client;
	int c;
	char *message,server_reply[2000];
	ini_cl();
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		printf("Failed. Error Code : %d",WSAGetLastError());
		return;
	}

	if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d" , WSAGetLastError());
	}

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(1996);


	if( bind(s ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d" , WSAGetLastError());
	}

	listen(s , 5);
	puts("Waiting clients to connect...");

	c = sizeof(struct sockaddr_in);
	pthread_t thread;
	int i;
	while(1){
		new_socket = accept(s , (struct sockaddr *)&client, &c);
		puts("Connection accepted");
		for(i=0;i<max_clients_num;i++){
			if(cl[i].sk==0){cl[i].sk=new_socket;break;}
		}
		if(i==max_clients_num){
			char *msg="the server connected to the maximum number of clients pls reconnect later";
			send(new_socket,msg,strlen(msg),0);
		}
		else{
			pthread_create(&thread,NULL,reciv,(void *)new_socket);
			pthread_create(&thread,NULL,send,(void *)new_socket);
		}

	}
	closesocket(s);
	WSACleanup();

}