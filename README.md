# Client-Server Application for Monitoring Work Activity

## Explanation

This is a simple application to show the current work activity of all employees in an organization.

## Client (Windows)

- **Language:** C/C++
- **Features:**
  - Silent launch on user logon and operates in the background.
  - Interacts with the server via the TCP protocol.
  - Does not use third-party libraries like Boost or frameworks like Qt.

## Server

- **Language:** Python (Flask)
- **Features:**
  - List all connected clients: domain, machine, IP, user.
  - Show client’s last active time.
  - Ability to get screenshots from the client’s desktop.

## How to Run the Project

### Running the Server (Flask):

1. Install the required Python packages:

    ```bash
    pip install Flask Pillow
    ```

2. Run the server:

    ```bash
    py server.py
    ```

    The server will start running on [http://127.0.0.1:5000](http://127.0.0.1:5000). Open a browser and navigate to this URL to view the list of clients and their activity.

### Running the Client (C++):

1. Build the C++ client code in your preferred IDE (Visual Studio recommended).

2. If you prefer not to build the client yourself, you can find the pre-compiled executable in the `x64 -> Release` folder. You can launch the client by running the `Monitoring.exe` file from this folder.

3. The client will connect to the server and silently run in the background upon launch.

## Removing the Client from Startup

To manually remove the program from the Windows startup list, follow these steps:

1. **Open the Registry Editor**:
   - Press `Win + R` to open the Run dialog.
   - Type `regedit` and press Enter.

2. **Navigate to the Startup Key**:
   - In the Registry Editor, navigate to the following path:
     ```
     HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run
     ```

3. **Locate the Entry**:
   - Find the entry named `Monitoring`.

4. **Delete the Entry**:
   - Right-click on the `Monitoring` entry and select `Delete`.

5. **Close the Registry Editor**:
   - Exit the Registry Editor.
