This project implements a TCP-based Client–Server architecture that allows multiple clients to remotely manage files on a server-controlled directory. It demonstrates key concepts of C programming, Linux OS internals, socket programming, concurrency, and synchronization.

The system supports listing files, uploading/downloading, deleting files, and basic directory management — all performed over a custom application protocol.

🏗 System Architecture
Server Runs as a background service

Listens on a fixed TCP port
Handles multiple clients concurrently using threads or forked processes
Manages a dedicated directory for all file operations
Ensures safe access using synchronization (mutex/file-locks)

Client
Command-line utility Connects to the server over TCP Sends commands and receives responses
Supports file transfer operations with validation

🔑 Features
✔ 1. File Listing
Lists all files/subdirectories
Includes metadata (size, permissions, timestamps)
Optionally exclude hidden files

Client Usage:
✔ 2. File Upload
Client sends local file → server stores it
Handles overwrite/rename conflicts
Supports integrity verification

Client Usage:
client upload <local_filename> [remote_filename]

✔ 3. File Download
Server streams file to client in chunks
Resume capability support (optional) Handles concurrent reads

Client Usage:
client download <remote_filename> [local_filename]

✔ 4. File Deletion
Safe deletion with existence checks
Returns status to client
Client Usage:

client delete <filename>

✔ 5. Directory Management (Bonus)
Create directories
Remove directories (with optional recursive mode)

Client Usage:

client mkdir <dirname>
client rmdir <dirname> [--recursive]

🧵 Concurrency Support

The server handles multiple clients via:

Thread-per-client model, or

Process-per-client model (fork)

Synchronization mechanisms ensure safe access during uploads, downloads, and deletions.

🔌 Communication Protocol

Built on TCP for reliable data transfer

Custom command/response protocol

Handles disconnections gracefully

🛠 Implementation Summary
Server

Initialize socket (bind, listen)
Accept connections in loop
Parse and process client commands
Spawn thread/process for each client
Perform file/directory operations
Send formatted responses

Client Parse CLI arguments
Connect to server
Send user command
Handle server responses
Download/upload files as required
