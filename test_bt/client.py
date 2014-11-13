import bluetooth

sock=bluetooth.BluetoothSocket(bluetooth.L2CAP)

bd_addr = "DC:A9:71:0D:BF:64"
port = 0x1001

sock.connect((bd_addr, port))

sock.send("hello!!")

sock.close()