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
#define PORT 3606 // the port users will be connecting to
#define MAX 1024
#define BACKLOG 10 // how many pending connections queue will hold
size_t *n_connected;
int fd;
struct flock lock;

void *handle_client(void *args, int **ind, char **stack)
{
	int new_fd = *(int *)(args);
	char buff[MAX];

	// infinite loop for chat
	bzero(buff, MAX);
	while (read(new_fd, buff, sizeof(buff)))
	{
		printf("%s\n", buff);

		// proccess commands
		if (strncmp("PUSH ", buff, 5) == 0)
		{
			while (fcntl(fd, F_SETLKW, &lock))
			{
			}
			lock.l_pid = getpid();
			strcpy(*stack + **ind, buff + 5);
			**ind = **ind + strlen(buff + 5) + 1;
			printf("was pushed successfully \n");
			char *buffer = "success";
			write(new_fd, buffer, sizeof(buffer));
			lock.l_type = F_UNLCK;
			fcntl(fd, F_SETLKW, &lock);
		}
		else if (strncmp("POP", buff, 3) == 0)
		{
			while (fcntl(fd, F_SETLKW, &lock))
			{
			}
			fcntl(fd, F_SETLKW, &lock);
			if (**ind != 0)
			{
				int count = 0;
				while (**ind != 0 && count < 2)
				{
					**ind = **ind - 1;

					if (*(*stack + **ind) == '\0')
					{
						count++;
					}
				}
				if (**ind == 0)
				{
					char sender[1024];
					strcpy(sender, *stack + **ind);
					write(new_fd, sender, strlen(sender) + 1);
				}
				else
				{
					char sender[1024];
					strcpy(sender, *stack + **ind + 1);
					write(new_fd, sender, strlen(sender) + 1);
				}
			}
			else
			{
				char *send_to_client2 = "empty stack";
				write(new_fd, send_to_client2, strlen(send_to_client2) + 1);
			}
			lock.l_type = F_UNLCK;
			fcntl(fd, F_SETLKW, &lock);
			if (**ind > 0)
			{
				**ind = **ind + 1;
			}
		}
		else if (strncmp("TOP", buff, 3) == 0)
		{
			if (**ind != 0)
			{
				int tmp = **ind;
				int count = 0;
				while (tmp != 0 && count < 2)
				{
					tmp = tmp - 1;

					if (*(*stack + tmp) == '\0')
					{
						count++;
					}
				}
				if (tmp == 0)
				{
					char sender[1024];
					strcpy(sender, *stack + tmp);
					write(new_fd, sender, strlen(sender) + 1);
				}
				else
				{
					char sender[1024];
					strcpy(sender, *stack + tmp + 1);
					write(new_fd, sender, strlen(sender) + 1);
				}
			}
			else
			{
				char *send_to_client2 = "stack empty";
				write(new_fd, send_to_client2, strlen(send_to_client2) + 1);
			}
		}
		else if (strcmp("EXIT", buff) == 0)
		{
			write(new_fd, "Quitting Session!\n", 20);
			(*n_connected)--;
			printf("%ld Users Connected!\n", *n_connected);
			if (*n_connected == 0)
			{
				printf("No more clients are connected\n");
				break;
			}
		}
		else
		{
			char *send_to_client2 = "illegal command";
			write(new_fd, send_to_client2, strlen(send_to_client2) + 1);
		}
		bzero(buff, MAX);
	}

	close(new_fd);
}

int main(int argc, char const *argv[])
{
	memset(&lock, 0, sizeof(lock));
	lock.l_type = F_WRLCK;
	FILE *fp = fopen("lock.file", "w+");
	fclose(fp);
	fd = open("lock.file", O_RDWR, 0); // fileno(fp);

	// put index as a shared pointer to memory
	int *ind = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0);
	if (ind == MAP_FAILED)
	{
		exit(1);
	}
	*ind = 0;

	// put stack as a pointer to shared memory
	char *stack = (char *)mmap(NULL, 1024 * 1000, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0);
	if (stack == MAP_FAILED)
	{
		printf("exit");
		exit(1);
	}

	// put user count as pointer in shared memory
	n_connected = (size_t *)mmap(NULL, sizeof(size_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0);
	if (n_connected == MAP_FAILED)
	{
		printf("n_conneted couldnt be allocated in shared memory!\n");
		exit(1);
	}
	*n_connected = 0;

	int sockfd, new_fd, len;
	struct sockaddr_in servaddr, cli;

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
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
	if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0)
	{
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	// Now server is ready to listen and verification
	if ((listen(sockfd, 5)) != 0)
	{
		printf("Listen failed...\n");
		exit(0);
	}
	else
	{
		printf("Server listening..\n");
	}

	len = sizeof(cli);

	// accept loop
	while (1)
	{
		new_fd = accept(sockfd, (SA *)&cli, &len);
		if (new_fd == -1)
		{
			perror("accept");
			continue;
		}
		(*n_connected)++;
		printf("server: got connection\n");
		printf("Currently there are %ld clients connected\n", *n_connected);

		switch (fork())
		{
		case -1:
			exit(0);
			break;
		case 0:
			handle_client((void *)&new_fd, &ind, &stack);
			break;
		default:
			break;
		}
	}

	return 0;
}