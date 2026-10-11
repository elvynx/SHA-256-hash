# SHA-256 Hash Calculator

A simple C++ desktop application built with raylib and OpenSSL for calculating file SHA-256 hashes.

## Features

- Drag and drop a file to calculate its SHA-256 hash.
- Display the hash and copy it to the clipboard.
- Reset using the “Calculate new hash” button.

Files are processed locally and never uploaded.

## Dependencies — CachyOS / Arch Linux

```bash
sudo pacman -Syu --needed base-devel raylib openssl
```

## Build and Run

```bash
g++ -std=c++17 -O2 main.cpp -o sha256 -lraylib -lcrypto
./sha256
```

The interface may temporarily become unresponsive while hashing large files.
