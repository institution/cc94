import socket

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
r = sock.sendto(bytes('Ala ma kota!'+chr(0), "utf-8"), ('127.0.0.1', 4567))
print(r)
