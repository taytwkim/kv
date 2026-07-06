# KV Store

Implement a simple KV store with the following features:

* ✅ Persistence via an append-only log and log replay at startup.
* ✅ Receive client requests over TCP.
* Support concurrent client connections.

## API

* Request `"GET key"` to get the value associated with a key.
* Request `"SET key value"` to set the value associated with a key.
* Request `"HAS key"` to check whether a key exists.
* Request `"REMOVE key"` to remove a key-value pair.

## Usage

1. Compile and run the server.

```shell
make server
make run-server
```

2. Open a second terminal and use `nc` to open a connection.

```shell
nc localhost 4000
```

## Platform

Currently tested on macOS.