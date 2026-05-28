import socket
import sys

# Przyciski dotykowe są na 192.168.1.220 port 100
HOST, PORT = "192.168.1.220", 100
data = " ".join(sys.argv[1:]).strip() or "OFF"

# Create a socket (SOCK_STREAM means a TCP socket)
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
    # Connect to server and send data
    sock.connect((HOST, PORT))
    sock.sendall(bytes(data + "\n", "utf-8"))

    # Receive data from the server and shut down
    received = str(sock.recv(1024), "utf-8")

print("Sent:     {}".format(data))
print("Received: {}".format(received))