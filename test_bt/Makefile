all: simplescan rfcomm-server rfcomm-client
simplescan: simplescan.c
	gcc -o simplescan simplescan.c -lbluetooth
rfcomm-server: rfcomm-server.c
	gcc -o rfcomm-server rfcomm-server.c -lbluetooth
rfcomm-client: rfcomm-client.c
	gcc -o rfcomm-client rfcomm-client.c -lbluetooth
clean:
	rm -rf simplescan rfcomm-server rfcomm-client
