_This project has been created as part of the 42 curriculum by engiusep, ynzue-es, nyousfi._

# FT_IRC — Internet Relay Chat Server

## Description

FT_IRC is an IRC (Internet Relay Chat) server implementation written in C++98, following the 42 school curriculum standards. The goal of this project is to recreate a functional IRC server that complies with the IRC protocol, allowing multiple clients to connect simultaneously and communicate through channels or private messages.

The server is built with a non-blocking architecture using `poll()` for I/O multiplexing, ensuring efficient handling of multiple connections in a single-threaded environment. It supports essential IRC features including user authentication, channel management, operator privileges, and various channel modes.

This project provides hands-on experience with network programming, socket management, protocol implementation, and concurrent I/O handling in C++.

## Instructions

### Compilation

The project uses a Makefile with the following targets:

```bash
make        # Compile the project
make clean  # Remove object files
make fclean # Remove object files and executable
make re     # Recompile from scratch
```

### Execution

To start the IRC server, run:

```bash
./ircserv <port> <password>
```

**Parameters:**
- `<port>`: The port number on which the server will listen (e.g., 6667)
- `<password>`: The server password required for client authentication

**Example:**
```bash
./ircserv 6667 mypassword
```

### Connection

You can connect to the server using any IRC client (HexChat, Irssi, WeeChat) or even with `netcat` for testing:

```bash
nc 127.0.0.1 6667
PASS mypassword
NICK nickname
USER username 0 * :Real Name
```

### Supported Commands

**Authentication:**
- `PASS` — Server password authentication
- `NICK` — Set nickname
- `USER` — Set username and real name

**Channel Management:**
- `JOIN` — Join a channel
- `PART` — Leave a channel
- `TOPIC` — View or set channel topic
- `NAMES` — List users in a channel

**Messaging:**
- `PRIVMSG` — Send private or channel messages
- `NOTICE` — Send notice messages

**Operator Commands:**
- `KICK` — Kick a user from a channel
- `INVITE` — Invite a user to a channel
- `MODE` — Manage channel modes

**Channel Modes:**
- `i` — Invite-only mode
- `t` — Topic restricted to operators
- `k` — Channel password
- `o` — Grant/revoke operator privilege
- `l` — Set user limit

## Resources

### IRC Protocol Documentation
- [RFC 1459](https://tools.ietf.org/html/rfc1459) — Internet Relay Chat Protocol (original specification)
- [RFC 2810](https://tools.ietf.org/html/rfc2810) — Internet Relay Chat: Architecture
- [RFC 2811](https://tools.ietf.org/html/rfc2811) — Internet Relay Chat: Channel Management
- [RFC 2812](https://tools.ietf.org/html/rfc2812) — Internet Relay Chat: Client Protocol
- [RFC 2813](https://tools.ietf.org/html/rfc2813) — Internet Relay Chat: Server Protocol

### Network Programming
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/) — Comprehensive guide to socket programming
- [poll() man page](https://man7.org/linux/man-pages/man2/poll.2.html) — Linux manual for poll() system call
- [TCP/IP Sockets in C](https://www.elsevier.com/books/tcp-ip-sockets-in-c/donahoo/978-0-12-374540-8) — Practical guide for TCP/IP programming

### AI Usage

AI tools (GitHub Copilot, ChatGPT) were used during the development of this project for the following tasks:

- **Code suggestions and autocompletion:** Assisted in writing boilerplate code and implementing standard patterns for socket programming and command parsing
- **Debugging assistance:** Helped identify potential bugs and memory leaks by reviewing code snippets
- **RFC interpretation:** Clarified ambiguous parts of the IRC protocol specifications
- **Documentation:** Helped structure and format this README file

All core logic, architecture decisions, and implementation were developed and understood by the project authors. AI was used as a productivity tool rather than a replacement for learning and understanding.