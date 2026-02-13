# usocket

Windows C++ quickstart for a **uSocket-style TCP server** (server sends data to client).

## 1) Tools you need on Windows

You already have Git and GCC. Use one of these:

- **Visual Studio 2022 (recommended)**
  - Install workload: **Desktop development with C++**
  - Includes MSVC, Windows SDK, debugger, CMake
- **MSYS2 + MinGW-w64 GCC**
  - Install MSYS2: https://www.msys2.org/
  - In **MSYS2 UCRT64** terminal:
    ```bash
    pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make mingw-w64-ucrt-x86_64-cmake git
    ```

## 2) Working example: `usocket_server.cpp`

This repo now includes a simple TCP server example in `usocket_server.cpp`.

Behavior:
- listens on `0.0.0.0:9001`
- accepts one client at a time
- sends a message from server -> client
- prints connection logs with `usocket` label

## 3) Build and run on Windows

### MSVC (Visual Studio Developer Command Prompt)
```bat
cl /EHsc usocket_server.cpp ws2_32.lib
usocket_server.exe
```

### MinGW-w64 (MSYS2 UCRT64)
```bash
g++ -std=c++17 -O2 usocket_server.cpp -lws2_32 -o usocket_server.exe
./usocket_server.exe
```

## 4) Test from another terminal

Use PowerShell (Windows has `curl` alias):

```powershell
$client = New-Object System.Net.Sockets.TcpClient("127.0.0.1",9001)
$stream = $client.GetStream()
$reader = New-Object System.IO.StreamReader($stream)
$reader.ReadToEnd()
$client.Close()
```

Expected output includes:

```text
hello from usocket server
```

## 5) Code::Blocks setup

1. Create/import a Console C++ project.
2. Add `usocket_server.cpp`.
3. Linker settings -> add `ws2_32`.
4. Build + Run.

## 6) Common issues

- `undefined reference to WSAStartup/socket/bind/listen/accept/send`
  - Missing linker lib: `ws2_32`.
- `winsock2.h not found`
  - Missing Windows SDK or not using MSYS2 UCRT64 environment.
- `bind failed`
  - Port already in use; change port in source.
