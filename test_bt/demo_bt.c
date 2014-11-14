#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/rfcomm.h>

#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

int server();
int client(const char *);
const char * scan(int);

int use_rfcomm = 0;


int main(int argc, char *argv[])
{
    if (argc > 2)
    {
        if (0 == strcmp(argv[2], "rfcomm"))
        {
            use_rfcomm = 1;
            puts("Using rfcomm");
        }
    }

    if (argc > 1)
    {
        if (0 == strcmp(argv[1], "client"))
        {
            puts("Starting client...");
            client(scan(1));
        }   
        else if (0 == strcmp(argv[1], "server"))
        {
            puts("Starting server...");
            server();
        }
        else if (0 == strcmp(argv[1], "scan"))
        {
            puts("Scanning...");
            scan(0);
        }
        else
        {
            puts("Unknown command");
            return 1;
        }
    }
    else
    {
        puts("Usage: demo_bt <server|client|scan> [rfcomm]");
        return 1;
    }
    return 0;
}

int client(const char *dest)
{
    struct sockaddr_l2 addr_l2 = {0};
    struct sockaddr_rc addr_rc = {0};
    int s, status;
    //char dest[18] = {0};

    // allocate a socket
    if (use_rfcomm)
    {
        s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    }
    else
    {
        s = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
    }

    // set the connection parameters (who to connect to)
    addr_l2.l2_family = AF_BLUETOOTH;
    addr_l2.l2_psm = htobs(0x1001);
    str2ba( dest, &addr_l2.l2_bdaddr );

    addr_rc.rc_family = AF_BLUETOOTH;
    addr_rc.rc_channel = (uint8_t) 1;
    str2ba(dest, &addr_rc.rc_bdaddr);

    int is_bad_send = 0;
    // connect to server
    char buf[1024] = {0};
    int error;
    while (1)
    {
        if (use_rfcomm)
        {
            status = connect(s, (struct sockaddr *)&addr_rc, sizeof(addr_rc));
        }
        else
        {
            status = connect(s, (struct sockaddr *)&addr_l2, sizeof(addr_l2));
        }

        if (status != 0) {
            perror("fail to connect");
            close(s);
            if (use_rfcomm)
            {
                s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
            }
            else
            {
                s = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
            }    
            continue;
        }

        if (is_bad_send)
        {
            error = send(s, buf, strlen(buf), 0);
            
            if( error < 0 )
            {
                continue;
            }
            else
            {
                is_bad_send = 0;
            }
        }

        
        while (1) 
        {
            puts("Enter string to send:");
            scanf("%s", buf);
            error = send(s, buf, strlen(buf), 0);
            
            if( error < 0 )
            {
                perror("uh oh");
                is_bad_send = 1;
                break;
            }
            else
            {
                printf("Sent message %s\n", buf);
            }            
        }
    }

    close(s);
}

int server()
{
    struct sockaddr_l2 loc_addr_l2 = { 0 }, rem_addr_l2 = { 0 };
    struct sockaddr_rc loc_addr_rc = {0}, rem_addr_rc = {0};
    char buf[1024] = { 0 };
    int s, client, bytes_read;
    socklen_t opt_l2 = sizeof(rem_addr_l2);
    socklen_t opt_rc = sizeof(rem_addr_rc);

    // allocate socket
    if (use_rfcomm)
    {
        s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    }   
    else
    { 
        s = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
    }

    // bind socket to port 0x1001 of the first available 
    // bluetooth adapter
    loc_addr_l2.l2_family = AF_BLUETOOTH;
    loc_addr_l2.l2_bdaddr = *BDADDR_ANY;
    loc_addr_l2.l2_psm = htobs(0x1001);
    

    loc_addr_rc.rc_family = AF_BLUETOOTH;
    loc_addr_rc.rc_bdaddr = *BDADDR_ANY;
    loc_addr_rc.rc_channel = (uint8_t) 1;
    

    if (use_rfcomm)
    {
        bind(s, (struct sockaddr *)&loc_addr_rc, sizeof(loc_addr_rc));
    }
    else 
    {
        bind(s, (struct sockaddr *)&loc_addr_l2, sizeof(loc_addr_l2));
    }


    // put socket into listening mode
    listen(s, 1);

    // accept one connection
    while (1)
    {
        if (use_rfcomm)
        {
            client = accept(s, (struct sockaddr *)&rem_addr_rc, &opt_rc);
        }
        else
        {
            client = accept(s, (struct sockaddr *)&rem_addr_l2, &opt_l2);
        }

        if (client < 0)
        {
            perror("failed to accept connection");
            continue;
        }

        if (use_rfcomm)
        {
            ba2str(&rem_addr_rc.rc_bdaddr, buf);
        }
        else
        {
            ba2str( &rem_addr_l2.l2_bdaddr, buf );
        }

        fprintf(stderr, "accepted connection from %s\n", buf);

     
        while (1) 
        {
            memset(buf, 0, sizeof(buf));

            // read data from the client
            bytes_read = recv(client, buf, sizeof(buf), 0);
            if( bytes_read > 0 ) 
            {
                printf("received [%s]\n", buf);
            }
            else
            {
                break;
            }
        }
    }

    // close connection
    close(client);
    close(s);
}

const char *scan(int do_ask)
{
    inquiry_info *ii = NULL;
    int max_rsp, num_rsp;
    int dev_id, sock, len, flags;
    int i;
    static char addr[19] = {0};
    char name[248] = {0};

    //Pass NULL to hci_get_route to get the first available bt adapter
    dev_id = hci_get_route(NULL);

    //open the adapter with given id
    sock = hci_open_dev(dev_id);

    //check that there is an adapter
    if (dev_id < 0 || sock < 0)
    {
        perror("opening socket");
        return NULL;
    }

    puts("Scanning for bluetooth devices...");

    len = 8;
    max_rsp = 256;

    //IREQ_CACHE_FLUSH requires that the cache of previously deteced devices is flushed before the current inquiry.
    flags = IREQ_CACHE_FLUSH;
    ii = (inquiry_info *) malloc(max_rsp * sizeof(inquiry_info));
    
    //perform bt device discovery
    //return -1 on error
    num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
    if (num_rsp < 0)
    {
        perror("hci_inquiry");
    }

    for (i = 0; i < num_rsp; i++)
    {
        //bdaddr_t contains a 6 part 8-bit integer address
        //str2ba(const char *str, bdaddr_t *ba);
        //ba2str(const bdaddr_t *ba, char *str);
        ba2str(&(ii+i)->bdaddr, addr);
        memset(name, 0, sizeof(name));
        if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name), name, 0) < 0)
        {
            strcpy(name, "[unknown]");
        }
        
        printf("[%d]\t%s\t%s\n", i, addr, name);
    }

    if (do_ask)
    {
        puts("Enter the number of the device you wish to connect with");
        scanf("%d", &i);
        ba2str(&(ii+i)->bdaddr, addr);
    }

    free(ii);
    close(sock);
    return addr;
}