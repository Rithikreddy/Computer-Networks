#include "networks.h"
#define ADDRESS "./unixsock"

int clients[100];
int curr_cli;

void* process(void* argt);

int main(int argc, char** argv){
	unlink(ADDRESS);
	int lfd, cfd;
	struct sockaddr_un servip;
	

	if((lfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0){
		perror("Socket error!"); exit(0);
	}
	bzero(&servip,sizeof(servip));
  	servip.sun_family = AF_UNIX;
   	strcpy(servip.sun_path, ADDRESS);

	if(bind(lfd, (struct sockaddr*)&servip, sizeof(servip)) < 0){
		perror("Bind error!"); exit(0);
	}
	listen(lfd, LISTENQ);

	while(1){
		cfd = accept(lfd, NULL, NULL);
		
		if(cfd > 0){
			if(fork()==0){
				int curr;
				read(cfd, &curr, sizeof(int));
				curr_cli = curr;

				for(int i=0; i<curr; i++){
					clients[i] = recv_fd(cfd);
				}
				pthread_t tid[curr];
				for(int i=0; i<curr; i++){
					pthread_create(&tid[i], NULL, process, (void*)&clients[i]);
				}
			}
			else{
				close(cfd);
			}
		}
	}
	return 0;
}

void* process(void* argt){
	int cfd = *((int*)argt);
	printf("%d\n", cfd);

	while(1){
		char buff[20];
		read(cfd, &buff, 20);
		printf("%s\n", buff);

		for(int i=0; i<curr_cli; i++){
			if(cfd!=clients[i])write(clients[i], buff, strlen(buff)+1);
		}
	}
}