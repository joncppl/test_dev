#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

int main(int argc, char *argv[])
{
	struct sockaddr_rc addr = {0};
	int s, status;
	char dest[18] = "DC:A9:71:0D:BF:64";

	//allocate a socket
	s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

	//set the connection parameters (who to connect to)
	addr.rc_family = AF_BLUETOOTH;
	addr.rc_channel = (uint8_t) 1;
	str2ba(dest, &addr.rc_bdaddr);

	//connect to server
	status = connect(s, (struct sockaddr *)&addr, sizeof(addr));

	//send a message
	if (status == 0)
	{
		status = write(s, "hello!", 6);
		printf("Sent %d bytes\n", status);	
	}
	else
	{
		printf("Failed to connect\n");
	}	

	if (status != 6)
	{
		perror("fail");
	}

	close(s);
	return 0;
}
