#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LENGTH 256

int main(int argc, char const *argv[])
{

	FILE *server, *c1, *c2, *c3;
	FILE *c3_read;
	int server_status, c1_status, c2_status, c3_status;
	char buffer[MAX_LENGTH];
	server = popen("./server", "r");
	sleep(1);
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

	char buff[101];
	// test long strings
	for (int i = 0; i < 5; i++)
	{
		buff[i] = 'M';
		buff[i + 1] = '\0';
		fprintf(c1, "PUSH %s-C1\n", buff);
	}
	for (int i = 0; i < 5; i++)
	{
		buff[i] = 'M';
		buff[i + 1] = '\0';
		fprintf(c2, "PUSH %s-C2\n", buff);
	}
	char *long_str = "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Donec quam felis, ultricies nec, pellentesque eu, pretium quis, sem. Nulla consequat massa quis enim. Donec pede justo, fringilla vel, aliquet nec, vulputate eget, arcu. In enim justo, rhoncus ut, imperdiet a, venenatis vitae, justo. Nullam dictum felis eu pede mollis pretium. Integer tincidunt. Cras dapibus. Vivamus elementum semper nisi. Aenean vulputate eleifend tellus. Aenean leo ligula, porttitor eu, consequat vitae, eleifend ac, enim. Aliquam lorem ante, dapibus in, viverra quis, feugiat a, tellus. Phasellus viverra nulla ut metus varius laoreet. Quisque rutrum. Aenean imperdiet. Etiam ultricies nisi vel augue. Curabitur ullamcorper ultricies nisi. Nam eget dui. Etiam rhoncus. Maecenas tempus, tellus eget condimentum rhoncus, sem quam semper libero, sit amet adipiscing sem neque sed ipsum.";
	fprintf(c2, "PUSH %s\n", long_str);
	for (int i = 0; i < 11; i++)
	{
		fputs("POP\n", c3);
	}
	/**
	 * should print:
	 *
	 * Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Donec quam felis, ultricies nec, pellentesque eu, pretium quis, sem. Nulla consequat massa quis enim. Donec pede justo, fringilla vel, aliquet nec, vulputate eget, arcu. In enim justo, rhoncus ut, imperdiet a, venenatis vitae, justo. Nullam dictum felis eu pede mollis pretium. Integer tincidunt. Cras dapibus. Vivamus elementum semper nisi. Aenean vulputate eleifend tellus. Aenean leo ligula, porttitor eu, consequat vitae, eleifend ac, enim. Aliquam lorem ante, dapibus in, viverra quis, feugiat a, tellus. Phasellus viverra nulla ut metus varius laoreet. Quisque rutrum. Aenean imperdiet. Etiam ultricies nisi vel augue. Curabitur ullamcorper ultricies nisi. Nam eget dui. Etiam rhoncus. Maecenas tempus, tellus eget condimentum rhoncus, sem quam semper libero, sit amet adipiscing sem neque sed ipsum.
	 * MMMMM
	 * MMMM
	 * MMM
	 * MM
	 * M
	 *
	 */
	pclose(c1);
	pclose(c2);
	pclose(c3);

	printf("\n----------------------------------------------");
	printf("\nTests Passed\n");
	printf("----------------------------------------------\n");
	exit(0);
	return 0;
}
