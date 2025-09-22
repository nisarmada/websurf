#!/usr/bin/env python3
import socket
import time

# Server's IP and port
HOST = '127.0.0.1'
PORT = 8080

# The request headers you want to send
# Notice that the Content-Length is set, but we won't send the full body.
request = (
    "POST /upload HTTP/1.1\r\n"
    "Host: localhost:8080\r\n"
    "Content-Length: 10000000\r\n"
    "Content-Type: application/octet-stream\r\n"
    "Connection: close\r\n"
    "\r\n"
)

try:
    # Create a socket
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # Connect to the server
    s.connect((HOST, PORT))

    print(f"Connected to {HOST}:{PORT}. Sending headers...")
    s.sendall(request.encode('utf-8'))

    print("Headers sent. Now sending a small part of the body...")
    # Send only a fraction of the expected body
    s.sendall(b'This is a small part of the body...')
    
    print("Sent partial body. Closing the connection immediately.")
    s.close()

except socket.error as e:
    print(f"Socket error: {e}")
except Exception as e:
    print(f"An error occurred: {e}")

