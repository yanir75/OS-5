/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <err.h>

//#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#define SA struct sockaddr
#define PORT 3605 // the port users will be connecting to
#define MAX 1024
#define BACKLOG 10 // how many pending connections queue will hold
size_t n_connected = 0;
int fd;
struct flock lock;

void *handle_client(void *args,int **ind,char **stack)
{
	int new_fd = *(int *)(args);
	char buff[MAX];
	// infinite loop for chat
	bzero(buff, MAX);
	while (read(new_fd, buff, sizeof(buff)))
	{
		printf("%s\n", buff);
		if (strncmp("PUSH ", buff, 5) == 0)
		{
			while(fcntl (fd, F_SETLKW, &lock)){

			}
			lock.l_pid = getpid();
			strcpy(*stack+**ind, buff + 5);
			//printf("%s\n",*stack+**ind);
			**ind = **ind +strlen(buff+5)+1;
			//printf("%d",**ind);
//			pthread_mutex_lock(&lock);
			//push(&head, t);
//			pthread_mutex_unlock(&lock);
			printf("was pushed successfully \n");
			char *buffer = "success";
			write(new_fd, buffer, sizeof(buffer));
			//sleep(10);
			lock.l_type = F_UNLCK;
 			fcntl (fd, F_SETLKW, &lock);
		}
		else if (strncmp("POP", buff, 3) == 0)
		{
			while(fcntl (fd, F_SETLKW, &lock)){

			}
			fcntl (fd, F_SETLKW, &lock);
			if(**ind !=0){
//			pthread_mutex_lock(&lock);
			//char *send_to_client = pop(&head);
			//printf("popping2\n");
			//printf("%d\n",**ind);
			int count=0;
			while (**ind!=0 && count<2)
			{
				//printf("IN LOOP\n");
				**ind = **ind -1;
				//printf("%d\n",**ind);

				if(*(*stack+**ind)=='\0'){
					//printf("test %d\n",**ind);
					count++;
				}
				//printf("test2 %d\n",**ind);
			}
			//printf("%d\n",**ind);
//			pthread_mutex_unlock(&lock);
			//char t[1024];
			if(**ind==0){
				char sender[1024];
				strcpy(sender,*stack+**ind);
				//printf("%s i j\n",sender);
			write(new_fd,sender, strlen(sender) + 1);
			}
			else{
				char sender[1024];
				strcpy(sender,*stack+**ind+1);
				//printf("%s hey\n",sender);
				write(new_fd, sender, strlen(sender) + 1);
			}
			}
			else{
			char *send_to_client2 = "empty stack";
			write(new_fd, send_to_client2, strlen(send_to_client2) + 1);
			}
			lock.l_type = F_UNLCK;
 			fcntl (fd, F_SETLKW, &lock);
			//me_free(send_to_client);
		}
		else if (strncmp("TOP", buff, 3) == 0)
		{
			if(**ind!=0){
			//("popping3\n");
			int tmp = **ind;
			int count=0;
			while (tmp!=0 && count<2)
			{
				//printf("IN LOOP\n");
				tmp = tmp -1;
				//printf("%d\n",**ind);

				if(*(*stack+tmp)=='\0'){
					//printf("test %d\n",**ind);
					count++;
				}
				//printf("test2 %d\n",**ind);
			}
			//printf("%d\n",tmp);
//			pthread_mutex_unlock(&lock);
			//char t[1024];
			if(tmp==0){
				char sender[1024];
				strcpy(sender,*stack+tmp);
				//printf("%s i j\n",sender);
			write(new_fd,sender, strlen(sender) + 1);
			}
			else{
				char sender[1024];
				strcpy(sender,*stack+tmp+1);
				//printf("%s hey\n",sender);
				write(new_fd, sender, strlen(sender) + 1);
			}
			}
			else{
			char *send_to_client2 = "stack empty";
			write(new_fd, send_to_client2, strlen(send_to_client2) + 1);
			}
		}
		else if (strcmp("EXIT", buff) == 0)
		{
			n_connected--;
			printf("%ld Users Connected!\n", n_connected);
			if (n_connected == 0)
			{
				printf("No more clients are connected, dumping stack memory\n");
			}
			break;
		}
		else
		{
			char *send_to_client2 = "illegal command";
			write(new_fd, send_to_client2, strlen(send_to_client2) + 1);
		}
		bzero(buff, MAX);
	}

	close(new_fd);
//	char ref[] = "closed socket";
//	pthread_exit(ref);
}

int main(int argc, char const *argv[])
{	
	memset (&lock, 0, sizeof(lock));
	lock.l_type = F_WRLCK;
	FILE *fp = fopen("lock.file","w+");
	fclose(fp);
	fd = open("lock.file",O_RDWR, 0);//fileno(fp);
	
	int *ind = (int *)mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, 0, 0);
	if(ind==MAP_FAILED){
		exit(1);
	}
	char *stack =(char *)mmap(NULL, 1024*1000, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, 0, 0);
	if(stack==MAP_FAILED){
		printf("exit");
		exit(1);
	}

	*ind = 0;

	 int sockfd, new_fd, len;
    struct sockaddr_in servaddr, cli;
   
    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));
   
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
   
    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");
   
    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);
	// if (pthread_mutex_init(&lock, NULL) != 0)
	// {
	// 	printf("\n mutex init failed\n");
	// 	return 1;
	// }
	while (1)
	{ // main accept() loop
		new_fd = accept(sockfd, (SA*)&cli, &len);
		if (new_fd == -1)
		{
			perror("accept");
			continue;
		}
		n_connected++;
		printf("server: got connection\n");

		// if (!fork()) { // this is the child process
		//     close(sockfd); // child doesn't need the listener
		// if (send(new_fd, "Hello, world!", 13, 0) == -1)
		// perror("send");
		// close(new_fd);
		// exit(0);
		// }
		// pthread_t thr;
		// pthread_create(&thr, NULL, handle_client, (void *)&new_fd);
		// close(new_fd);  // parent doesn't need this
		switch (fork())
		{
		case -1:
			exit(0);
			break;
		case 0:
			handle_client((void *)&new_fd,&ind,&stack);
			break;
		default:
			break;
		}
	}
	//pthread_mutex_destroy(&lock);

	return 0;
}