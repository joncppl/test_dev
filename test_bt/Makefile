all: simplescan rfcomm-server rfcomm-client l2cap-server l2cap-client demo
demo: demo.c
	gcc -g -o demo demo.c -lbluetooth
l2cap-server: l2cap-server.c
	gcc -g -o l2cap-server l2cap-server.c -lbluetooth
l2cap-client: l2cap-client.c
	gcc -g -o l2cap-client l2cap-client.c -lbluetooth
simplescan: simplescan.c
	gcc -g -o simplescan simplescan.c -lbluetooth
rfcomm-server: rfcomm-server.c
	gcc -g -o rfcomm-server rfcomm-server.c -lbluetooth
rfcomm-client: rfcomm-client.c
	gcc -g -o rfcomm-client rfcomm-client.c -lbluetooth
clean:
	rm -rf simplescan rfcomm-server rfcomm-client l2cap-server l2cap-client
