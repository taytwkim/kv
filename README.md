# KV Store

Implement a simple KV store with the following features:

* ✅ Receive client requests over TCP.
* ✅ Support concurrent client connections.
* ✅ Persistence via an append-only log + log replay at server startup.

## API

* Request `GET key` to get the value associated with a key.
* Request `SET key value` to set the value associated with a key.
* Request `HAS key` to check whether a key exists.
* Request `REMOVE key` to remove a key-value pair.

## Usage

1. Compile and run the server.

```shell
make server
make run-server
```

* A `store.log` file will be created in the top-level directory to log `SET` and `REMOVE`.

2. Open a second terminal and use `nc` to open a connection.

```shell
nc localhost 4000
```

3. Type in requests.

```text
❯ nc localhost 4000
SET name joel
OK
GET name
OK: joel
...
```

## Platform

Tested on Linux and macOS. Built with C++17.
