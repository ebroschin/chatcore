import socket
import struct
import threading
import sys

HOST = "127.0.0.1"
PORT = 1338

def read_bytes(socket, size):
    data = bytearray()

    while len(data) < size:
        chunk = socket.recv(size - len(data))
        if not chunk:
            raise ConnectionError("Server disconnected")
        data.extend(chunk)

    return bytes(data)

def handle_responses(socket):
    try:
        while True:
            header = read_bytes(socket, 4)
            size = struct.unpack("!I", header)[0]
            payload = read_bytes(socket, size)

            #TODO use callback handlers for incoming payloads
            print(f"\nreceived {size} bytes")
            print(f"hex: {payload.hex(' ')}")
            print(f"text: {payload.decode('utf-8', 'replace')}")
    except (ConnectionError, OSError) as error:
        print(f"\nconnection closed: {error}")
        sys.exit(1)

print("Commands: /text MESSAGE, /raw HEX, /quit")

with socket.create_connection((HOST, PORT)) as socket:
    threading.Thread(
        target = handle_responses,
        args = (socket,),
        daemon = True
    ).start()

    while True:
        command = input("cli> ")

        if command == "/quit":
            break

        if command.startswith("/raw "):
            print(command[5:])
            socket.sendall(bytes.fromhex(command[5:]))
            continue

        if command.startswith("/text "):
            payload = command[6:].encode("utf-8")
            packet = struct.pack("!I", len(payload)) + payload
            socket.send(packet)
            continue

