# FT_IRC — Internet Relay Chat Server

## Description
FT_IRC is an IRC (Internet Relay Chat) server written in C++98. The project reproduces the behavior of a standard IRC server, supporting multiple simultaneous clients, chat channels, and private messages. The server is non-blocking and uses poll() for I/O multiplexing in a single main thread.

## Compilation
The project uses a Makefile.

Commands:
```bash
make
make clean
make fclean
make re
```

## Usage
Start the server with:
```bash
./ircserv <port> <password>
```
- port: listening port (e.g., 6667)
- password: server password

Example:
```bash
./ircserv 6667 mypassword
```

## Connection
You can connect with HexChat, Irssi, or Netcat. Example using Netcat:
```bash
nc 127.0.0.1 6667
PASS mypassword
NICK nickname
USER username 0 * :Real Name
```

## Features
- Authentication: PASS, NICK, USER  
- Channel management: JOIN, PART, TOPIC, NAMES  
- Private and public messaging: PRIVMSG, NOTICE  
- Operator commands: KICK, INVITE, MODE  
- Supported channel modes:  
  - i — invite-only  
  - t — topic-restricted  
  - k — password-protected  
  - o — operator privilege  
  - l — user limit

## Technical details
- Language: C++98  
- Networking: TCP/IP sockets, non-blocking I/O with poll()  
- Single-threaded main loop using poll()  
- Memory-conscious design (aimed at no leaks)

## Authors
engiusep
ynzue-es
nyousfi