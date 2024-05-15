#!/usr/bin/env python3

import socket

HOST = "0.0.0.0"  # The server's hostname or IP address
PORT = 8080  # The port used by the server

request1 = """POST /upload HTTP/1.1\r\nTransfer-Encoding: chunked\r\nAccept: */*\r\nContent-Type: text/plain\r\nUser-Agent: ur mom\r\n\r\n5\r\nhello\r\n"""

request2 = """6\r\n world\r\n"""

request3 = """E\r\n, how are you?\r\n""" # size in hex

request4 = "0\r\n"

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    s.sendall(request1.encode()) # encode() sends the string as bytes
    s.sendall(request2.encode())
    s.sendall(request3.encode())
    s.sendall(request4.encode())
    data = s.recv(1024)

print(f"Received {data!r}")
