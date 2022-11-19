from socket import *

sock = socket(family=AF_INET, type=SOCK_DGRAM)
sock.bind(("0.0.0.0", 8887))
sock.sendto(bytes("Hello", "utf-8"), ("192.168.1.27", 9001))
