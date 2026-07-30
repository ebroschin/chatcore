# chatcore | Multi-Threaded Chat System in C++23

[![CI Builds and Tests (Linux, Windows)](https://github.com/ebroschin/chatcore/actions/workflows/ci.yml/badge.svg)](https://github.com/ebroschin/chatcore/actions/workflows/ci.yml)

**chatcore** is a multi-threaded C++23 chat system: a TCP server with a length-prefixed JSON protocol, a terminal client (FTXUI), and a load tester for latency under concurrent clients.

Built and tested in CI on Linux and Windows (GCC, Clang, MSVC/clang-cl, MinGW; x64, x86, and arm64).

Shared infrastructure (networking, scheduling, persistence, logging) lives in the [`ebroschin-sdk`](ebroschin-sdk) submodule.

## Demo
![Demo screenshot](docs/images/demo.png)

### Docker Quick-Start
The source code can be built and run using Docker, including a `demo.sh` script that starts two clients, the server, and the load tester in a tmux session.

#### Linux

```bash
sudo -E docker build -t chatcore-demo -f scripts/docker/Dockerfile.linux "https://github.com/ebroschin/chatcore.git#master" \
&& sudo docker run --rm -it -p 1338:1338 chatcore-demo bash -lc '/workspace/src/scripts/docker/demo.sh'
```

#### Windows

```pwsh
docker build -t chatcore-demo -f scripts/docker/Dockerfile.linux "https://github.com/ebroschin/chatcore.git#master" ; if ($?) { docker run --rm -it -p 1338:1338 chatcore-demo bash -lc "/workspace/src/scripts/docker/demo.sh" }
```

### Manual Build (Linux)
Manual build instructions are currently provided for Linux only.
Windows builds are validated in CI (MSVC, clang-cl, MinGW).

#### Install required dependencies

```bash
sudo apt-get update
sudo apt-get install -y \
  git curl ca-certificates zip unzip \
  build-essential pkg-config \
  cmake ninja-build \
  autoconf autoconf-archive automake libtool \
  tmux
```

#### Clone and Build

```bash
git clone --recurse-submodules https://github.com/ebroschin/chatcore.git
cd chatcore
./scripts/linux/build-all.sh
```

`build-all.sh` defaults to the `linux-gcc-release` presets. Pass another shared prefix to override, for example `./scripts/linux/build-all.sh linux-clang-release`.

#### Run Demo (tmux session)
```bash
./scripts/docker/demo.sh .
```

#### Run individual applications

```bash
./build/linux-gcc-release-server/apps/server/chatcore-server --ip 0.0.0.0 --port 1338 --db sqlite.db --log info
./build/linux-gcc-release-client/apps/client/chatcore-client --ip localhost --port 1338 --log info
./build/linux-gcc-release-load-tester/apps/load-tester/chatcore-load-tester --ip localhost --port 1338 --clients 100 --log info
```

## Features

The system allows a user to:
- Create users and authenticate with username and password to establish a session
- Create chat channels
- Join chat channels and receive the latest chat messages
- Receive/Write chat messages to all users in a channel
- Use the bundled FTXUI terminal client to interact with the chat server
- Use the bundled Load Tester to evaluate latency percentiles (p50, p95, p99, max) by spawning a configurable amount of clients with a steady stream of chat messages

The server supports:
- Persistent data throughout server restarts (using sqlite for simplicity)
- Concurrent client connections

The feature scope of the project is intentionally constrained with known [limitations](#Limitations).

## Project Structure

The repository is structured into `apps/` (executables) and the [`ebroschin-sdk`](ebroschin-sdk) git submodule (shared library modules). Third-party dependencies are managed with **vcpkg** (also a submodule).

Core dependencies: `boost-asio`, `boost-stacktrace`, `boost-bimap`, `boost-multi-index`, `nlohmann-json`, `sqlitecpp`, `spdlog`

Optional vcpkg features: `ftxui` (client), `gtest` (tests)

### Applications

| Path | Purpose |
| --- | --- |
| [`apps/server`](apps/server) | TCP chat server: connections, users, chat channels, and messages |
| [`apps/client`](apps/client) | FTXUI terminal client for interactive use of the chat protocol |
| [`apps/load-tester`](apps/load-tester) | CLI load generator with latency/error reporting (p50, p95, p99, max) |

### ebroschin-sdk

The applications are built on libraries from my reusable codebase, which provide:
- application lifecycle
- asynchronous networking with TCP/RPC
- task scheduling
- data persistence with SQLite
- logging with spdlog
- utilities and helper classes/datastructures

Shared chat protocol DTOs currently live in `ebroschin-sdk/chatcore-api`. (will move to separate repository later)

See [`ebroschin-sdk/README.md`](ebroschin-sdk/README.md) for the full module list and usage.

## Threading/Concurrency Model

Each application uses the same threading model and thread-affine systems.

- **Main Thread**: bootstraps the application and its systems, some of which may spawn their own dedicated thread
  - Afterwards, the main thread sleeps until it receives a signal to quit the application, which in turn runs the shutdown routine, tearing down its systems and exiting the application process safely
- **Network I/O Thread**: spawned and owned by `TcpSystem` from `ebroschin-network` using either `BoostTcpResolver` (client) or `BoostTcpAcceptor` (server) from `ebroschin-network-modules`
  - These "connector" modules are implemented using `boost-asio`'s `io_context` and asynchronously create connection instances
  - Once a `TcpConnection` is created and registered to the `TcpSystem`, it can send and receive unstructured bytes
  - The concrete implementation of said `TcpConnection` (`BoostTcpConnection`) effectively defines the network protocol by interpreting these bytes
- **Application Thread**: spawned and owned by `ApplicationSystem`
  - Runs the message processor of the `TcpSystem` in a dedicated domain/business logic thread
  - The `MessageProcessor` acts as a thread-serialization boundary, it receives incoming network messages from `TcpConnection` instances and stores them in an internal queue
- **Scheduler Thread**: spawned and owned by `SchedulingSystem`
  - Accepts callbacks which can be executed after a certain amount of time or in intervals
  - The application developer must ensure that the scheduled callbacks access data in a thread-safe manner

## Networking Details

The transport protocol and wire format are defined by the application-specific modules that are instantiated with the `TcpSystem`.

- **Transport framing**: First 4 bytes represent the payload length (big-endian `uint32_t`, uses `htonl(), ntohl()`), while remaining bytes represent the payload
- **Wire format**: JSON (`nlohmann-json`)

`[apps/server] chat_tcp_system.hpp`
```cpp  
using ChatServerTcpSystem = network::tcp::TcpSystemBuilder<    
  network::modules::BoostTcpAcceptor, //uses boost-asio and defines how incoming/outgoing bytes from sockets are interpreted  
  network::modules::JsonNetworkCodec, //decodes/encodes bytes from/to json    
  network::modules::DirectMessageHandler, //defines how incoming network messages are handled    
  api::MessageTypes //compile-time registered list of message types
>::Type;  
```  
### Network Messages

The `TcpSystem` provides an abstraction for sending and receiving network messages as application-specific data transfer objects (DTO).

`[ebroschin-sdk/chatcore-api] api.hpp`
```cpp  
struct ReceiveChatMessage {    
  static constexpr std::uint64_t TypeId = 103;    
    
  PersistenceId user_id;    
  PersistenceId channel_id;    
  std::string content;  
};    
    
struct WriteChatMessage {    
  static constexpr std::uint64_t TypeId = 104;    
    
  std::string content;  
};  
```  

In the provided implementation, the `BoostTcpConnection` starts reading bytes asynchronously after its instantiation via `boost::asio::async_read`.

When the system **receives** a network message:
1. The `io_context (boost-asio)` asynchronously calls back to `BoostTcpConnection`
2. The `BoostTcpConnection` reads the first 4 bytes (`sizeof(std::uint32_t)`) and interprets them as `std::uint32_t payload_length` via `ntohl()`
3. The `BoostTcpConnection` reads the next `payload_length` bytes and passes these to the `TcpMessageProcessor`
4. The `TcpMessageProcessor` uses the provided codec implementation (`JsonNetworkCodec`) to:
  5. Decode the bytes into the application-specific wire format (JSON)
  6. Deserialize the wire format into a C++ data transfer object (network message DTO)
7. The `TcpMessageProcessor` passes the DTO to the MessageHandler which calls the application-specific handler logic

```mermaid  
sequenceDiagram  
    participant IO as io_context    
    participant Conn as BoostTcpConnection    
    participant Proc as TcpMessageProcessor    
    participant Codec as JsonNetworkCodec    
    participant Handler as MessageHandler  
    IO->>Conn: async callback
    Conn->>Conn: read 4 bytes (length)
    Conn->>Conn: read payload
    Conn->>Proc: pass bytes
    Proc->>Codec: decode + deserialize
    Codec-->>Proc: DTO
    Proc->>Handler: handle message
```  

When the system **sends** a network message:
1. The network message DTO is passed to the `TcpSystem` along with a `connection_id`
2. The TcpSystem uses the provided codec implementation (`JsonNetworkCodec`) to:
  3. Serialize the C++ data transfer object to the wire format (JSON)
  4. Encode the wire format to bytes
5. The `TcpConnection` that corresponds to the `connection_id` receives said bytes and:
  6. Calculates the `payload_length` as `std::uint32_t` via `htonl()`
  7. Prepends the `payload_length` (`sizeof(std::uint32_t) = 4`) to the payload bytes
  8. Sends the resulting bytes via `boost::asio::async_write`

```mermaid  
sequenceDiagram  
    participant App as Application    
    participant Sys as TcpSystem    
    participant Codec as JsonNetworkCodec    
    participant Conn as TcpConnection    
    participant Net as Network  
    App->>Sys: DTO + connection_id    
    Sys->>Codec: serialize DTO    
    Codec-->>Sys: JSON (wire format)    
    Sys->>Codec: encode JSON    
    Codec-->>Sys: byte buffer  
    Sys->>Conn: send bytes    
    Conn->>Conn: calculate payload_length (htonl)    
    Conn->>Conn: prepend length (4 bytes)  
    Conn->>Net: async_write(buffer)  
```  

### Message Handling

On the lowest level, the `TcpMessageProcessor` receives incoming network messages and calls the registered handlers based on its `TypeId`. On the highest level, the application defines which "actions" are performed by registering callbacks for each network message type.
#### Signals/Subscriptions

The client uses a 1:X message handling model. Each incoming network message triggers one or more callbacks in the application.

The client therefore uses the `ObservableMessageHandler` from `ebroschin-network-modules` which offers signals/subscription-based callback registration.
#### Direct Callbacks

The server uses a 1:1 message handling model. Each incoming network message has exactly one outcome:
- perform the necessary operations on the data
- return a response to sender

To facilitate higher throughput, the server therefore uses the much faster `DirectMessageHandler` from `ebroschin-network-modules` which offers only a single callback per network message type and avoids signals/subscriptions overhead.

## Software Design

Each application is structured using the same pattern: a composition of sub-systems within a system context owned by the application (composition root). Each system implements a single responsibility, manages its own data, and exposes public functions for other sub-systems.

`[apps/server] chat_server_application.cpp`
```cpp  
ctx_.Register<scheduling::SchedulingSystem>(); //runs & manages scheduling thread  
ctx_.Register<ChatServerTcpSystem>(); //runs & manages network thread  

auto* persistence_system = ctx_.Register<ChatPersistenceSystem>(arguments_.GetSqliteFilename()); //manages persistence adapters (business logic)  
persistence_system->Register<ChatPersistenceAdapter, SqliteChatPersistenceAdapter>(); //chat persistence functions  
persistence_system->Register<UserPersistenceAdapter, SqliteUserPersistenceAdapter>(); //user persistence functions  

ctx_.Register<ApplicationSystem>(*this); //runs & manages application thread  
ctx_.Register<UserServerSystem>(); //manages users, sessions and auth  
ctx_.Register<ChatServerSystem>(); //manages chat channels and chat messages  
```  

The order of system registration matters. Low-level systems are registered/initialized first, high-level systems are registered/initialized last. This keeps a clean dependency graph between systems without circular dependencies.

Each thread is managed by its own system (thread-affinity). The codebase uses modern STL concurrency primitives (`std::jthread`, `std::scoped_lock`, ...).

SDK libraries follow the same application pattern from `ebroschin-core`. Abstract libraries stay free of third-party dependencies; concrete backends live in `*-modules` packages (for example Boost.Asio networking or SQLite persistence).

## Benchmarks
Server Test Device Hardware:
- CPU: Intel(R) Core(TM) i7-6700K CPU @ 4.00GHz
- RAM: 31Gi
- OS: Linux Mint 22.3

Test Case:
- Create a configurable number of clients
- Each client joins the same test-channel
- Each client sends one message per second
  - the server broadcasts the message to all clients in the test-channel
  - when the sender receives its own message from the server, it marks said message as "Complete"
- Test runs for 10 seconds

| Clients | Sent | Failed | p50 (μs) | p95 (μs) | p99 (μs) | max (μs) |
| ------: | ---: | -----: | -------: | -------: | -------: | -------: |
|       5 |   45 |      0 |      279 |      334 |      351 |      369 |
|      25 |  225 |      0 |      356 |      553 |      582 |     2537 |
|     100 |  900 |      0 |      813 |     1424 |     2306 |    42439 |
|     200 | 1800 |      0 |     1376 |     2600 |    42209 |    43089 |
|     350 | 3150 |      0 |    65710 |   124434 |   130640 |   131121 |
|     450 | 4050 |   1603 |  1779904 |  3347632 |  3579313 |  3624184 |

Failed messages are those that could not complete a roundtrip before the end of the test duration.
The server handles a moderate load well. However, since the server does not implement a backpressure strategy, latency starts to rise as soon as the message queues begin to fill faster than the server can drain them.

## Limitations

The chat system should not be used in production as is. The following essential features were intentionally left out due to project scope:

- passwords are persisted in plain text (no hashing)
- network traffic is not encrypted (no TLS)
- no auth token/session renewal model
- no backpressure strategy for message queues
- no fairness strategy for incoming messages (payloads are processed sequentially in the order of arrival)
- no cache eviction strategy
- no session heartbeat (crashed clients stay logged in until server shutdown)
- the JSON wire format can be replaced with a binary format (for example protobuf) to improve performance and throughput of the chat server
- all chat messages that were sent 1 second before an unexpected server shutdown are lost (the server flushes to the database in intervals of 1 second)
- `std::pmr` data structures (preallocated memory) in hot paths would further improve chat server throughput
- running a load test with a high number of clients (e.g. 1000) will likely cause some authentication calls to time out (all clients attempt to log in at once, server message queues fill faster than they drain)
