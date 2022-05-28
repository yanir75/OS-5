#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_LENGTH 256

int main(int argc, char const *argv[])
{

	FILE *server, *c1, *c2, *c3;
	int server_status, c1_status, c2_status, c3_status;
	char buffer[MAX_LENGTH];

	// open client proccesses
	c1 = popen("./client 127.0.0.1", "w");
	if (c1 == NULL)
	{
		perror("Failed to open server proccess!\n");
		exit(1);
	}

	// open client proccesses
	c2 = popen("./client 127.0.0.1", "w");
	if (c2 == NULL)
	{
		perror("Failed to open server proccess!\n");
		exit(1);
	}

	// open client proccesses
	c3 = popen("./client 127.0.0.1", "w");
	if (c3 == NULL)
	{
		perror("Failed to open server proccess!\n");
		exit(1);
	}

	printf("All Clients Connected Successfuly!\n");

	sleep(1);

	// send some commands
	fputs("PUSH c1\n", c1);
	fputs("PUSH c1\n", c1);
	fputs("POP\n", c1);
	fputs("PUSH c2\n", c2);
	fputs("POP\n", c2);
	fputs("POP\n", c2);
	fputs("PUSH c3\n", c3);
	fputs("POP\n", c3);

	return 0;
}
