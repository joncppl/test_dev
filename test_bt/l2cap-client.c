#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>

int main(int argc, char **argv)
{
    struct sockaddr_l2 addr = { 0 };
    int s, status;
    char dest[18] = "DC:A9:71:0D:BF:64";

    // allocate a socket
    s = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);

    // set the connection parameters (who to connect to)
    addr.l2_family = AF_BLUETOOTH;
    addr.l2_psm = htobs(0x1001);
    str2ba( dest, &addr.l2_bdaddr );

    // connect to server
    status = connect(s, (struct sockaddr *)&addr, sizeof(addr));
    if (status != 0) {
        perror("fail to connect");
        exit(1);
    }

    char buf[56] = {0};
    int i = 0;
    while (1) 
    {
        // send a message
        if( status == 0 ) {
            sprintf(buf, "%d", i++);
            status = send(s, buf, strlen(buf), 0);
        }

        if( status < 0 ) perror("uh oh");
        else puts("sent message");

        sleep(1);
    }


    close(s);
}
