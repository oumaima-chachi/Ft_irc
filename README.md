This project has been created as part of the 42 curriculum by ochachi and aben-dri.

# ft_irc

## Description

ft_irc is an IRC server developed in C++98 as part of the 42 curriculum.

The goal of this project is to understand how IRC servers work by recreating the core features of the IRC protocol. The server handles multiple clients simultaneously using non-blocking I/O and allows users to communicate through channels and private messages.

Our implementation includes authentication, channel management, operator privileges, and the mandatory IRC commands required by the subject.

---

## Team Contributions

### ochachi
I was responsible for the commands implementation, including:

- Command parsing
- KICK
- INVITE
- TOPIC
- MODE
- Command validation and error handling

### aben-dri
Responsible for the server architecture and networking part, including:

- Socket management
- Client connections
- Poll loop
- Server communication handling
- Core server logic

---

## Features

### User Features

- Authentication with password
- Nickname registration
- Username registration
- Private messaging
- Channel messaging
- Multiple client support

### Operator Commands

- KICK
- INVITE
- TOPIC
- MODE

Supported channel modes:

- `i` : Invite-only channel
- `t` : Topic restricted to operators
- `k` : Channel password
- `o` : Operator privileges
- `l` : User limit

---

## Compilation

Build the project with:

```bash
make
```

Run the server:

```bash
./ircserv <port> <password>
```

Example:

```bash
./ircserv 6667 password
```

---

## Usage

Connect using an IRC client such as HexChat:

```text
/server localhost 6667 password
```

Register:

```text
PASS password
NICK user
USER user 0 * :Real Name
```

Join a channel:

```text
JOIN #42
```

Send a message:

```text
PRIVMSG #42 :Hello everyone!
```

---

## What We Learned

Through this project we learned:

- Socket programming
- TCP/IP communication
- Non-blocking I/O
- Event-driven programming using poll()
- IRC protocol fundamentals
- Team collaboration on a large C++ project

---

## Resources

- RFC 1459
- RFC 2812
- Beej's Guide to Network Programming
- Linux Man Pages

### AI Usage

AI tools were used to:

- Better understand the IRC protocol
- Research networking concepts
- Review implementation ideas
- Improve documentation

All code included in the final project was reviewed, tested, and fully understood by the team before integration.
