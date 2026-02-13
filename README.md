diff --git a/README.md b/README.md
index d4fb547f6017fa0b7408b759a17b6c40aa60c538..2793316eb9e6d39ec61b78e21a5f0d3c84f14f94 100644
--- a/README.md
+++ b/README.md
@@ -1 +1,158 @@
-# usocket
\ No newline at end of file
+# usocket
+
+Practical setup guide to get a C++ socket program working on **Windows**.
+
+> This repository is currently a starter repo, so this README provides a known-good Windows networking baseline using WinSock2.
+
+## 1) Tools you need on Windows
+
+You already have Git and GCC, so choose one of these toolchains:
+
+### Option A (recommended): Visual Studio 2022 (Community)
+- Install **Desktop development with C++** workload.
+- This gives you:
+  - MSVC compiler (`cl`)
+  - Windows SDK (WinSock headers/libs)
+  - CMake + debugger support in VS
+
+### Option B: MSYS2 + MinGW-w64 GCC
+- Install [MSYS2](https://www.msys2.org/).
+- In **MSYS2 UCRT64** terminal, install:
+  ```bash
+  pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-make git
+  ```
+- Build in that same UCRT64 terminal.
+
+## 2) Verify compiler quickly
+
+### MSVC
+Open **x64 Native Tools Command Prompt for VS 2022**:
+```bat
+cl
+```
+
+### MinGW
+In MSYS2 UCRT64 terminal:
+```bash
+g++ --version
+```
+
+## 3) Working C++ socket example (Windows)
+
+Create `example_tcp_client.cpp`:
+
+```cpp
+#include <winsock2.h>
+#include <ws2tcpip.h>
+#include <iostream>
+#include <string>
+
+#pragma comment(lib, "Ws2_32.lib")
+
+int main() {
+    WSADATA wsaData;
+    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
+        std::cerr << "WSAStartup failed\n";
+        return 1;
+    }
+
+    addrinfo hints{};
+    hints.ai_family = AF_UNSPEC;
+    hints.ai_socktype = SOCK_STREAM;
+    hints.ai_protocol = IPPROTO_TCP;
+
+    addrinfo* result = nullptr;
+    if (getaddrinfo("example.com", "80", &hints, &result) != 0) {
+        std::cerr << "getaddrinfo failed\n";
+        WSACleanup();
+        return 1;
+    }
+
+    SOCKET sock = INVALID_SOCKET;
+    for (addrinfo* ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
+        sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
+        if (sock == INVALID_SOCKET) continue;
+
+        if (connect(sock, ptr->ai_addr, static_cast<int>(ptr->ai_addrlen)) == 0) {
+            break;
+        }
+
+        closesocket(sock);
+        sock = INVALID_SOCKET;
+    }
+    freeaddrinfo(result);
+
+    if (sock == INVALID_SOCKET) {
+        std::cerr << "Unable to connect\n";
+        WSACleanup();
+        return 1;
+    }
+
+    const std::string req =
+        "GET / HTTP/1.1\r\n"
+        "Host: example.com\r\n"
+        "Connection: close\r\n\r\n";
+
+    int sent = send(sock, req.c_str(), static_cast<int>(req.size()), 0);
+    if (sent == SOCKET_ERROR) {
+        std::cerr << "send failed\n";
+        closesocket(sock);
+        WSACleanup();
+        return 1;
+    }
+
+    char buffer[2048];
+    int bytes = 0;
+    while ((bytes = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
+        buffer[bytes] = '\0';
+        std::cout << buffer;
+    }
+
+    closesocket(sock);
+    WSACleanup();
+    return 0;
+}
+```
+
+## 4) Build and run the example
+
+### MSVC
+```bat
+cl /EHsc example_tcp_client.cpp ws2_32.lib
+example_tcp_client.exe
+```
+
+### MinGW-w64 GCC
+```bash
+g++ -std=c++17 -O2 example_tcp_client.cpp -lws2_32 -o example_tcp_client.exe
+./example_tcp_client.exe
+```
+
+If it works, you should see HTTP response headers/body from `example.com`.
+
+## 5) Using this from Code::Blocks
+
+1. Create a new **Console application (C++)** project.
+2. Replace generated source with the example above.
+3. Add linker library: `ws2_32`.
+   - Project -> Build options -> Linker settings -> Add -> `ws2_32`
+4. Build and Run.
+
+## 6) If you want to migrate to VS Code
+
+Install extensions:
+- **C/C++** (Microsoft)
+- **CMake Tools** (if using CMake)
+
+Use either:
+- MSVC from Developer Command Prompt, or
+- MSYS2/MinGW terminal as integrated shell.
+
+## Common Windows socket errors
+
+- `undefined reference to WSAStartup` / `socket` / `connect`:
+  - You forgot to link `ws2_32`.
+- `winsock2.h not found`:
+  - Windows SDK (MSVC route) or correct MinGW/MSYS2 env is missing.
+- Program exits immediately:
+  - Run from terminal so you can see output/errors.
