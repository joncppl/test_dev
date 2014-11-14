import bluetooth

sock=bluetooth.BluetoothSocket(bluetooth.L2CAP)

bd_addr = "DC:A9:71:0D:BF:64"
port = 0x1001

sock.connect((bd_addr, port))

for x in range(0, 500):
	sock.send(x)

sock.close()