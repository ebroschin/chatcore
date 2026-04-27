`chatcore-server` is a multithreaded TCP server for chat features: user creation/login, channel creation/joining, message broadcast, and periodic persistence to SQLite.

## Application Arguments

| Flag | Default | Description |
| --- | --- | --- |
| `--ip` | `0.0.0.0` | Interface address to bind and accept connections on |
| `--port` | `1338` | TCP port to listen on |
| `--db` | `sqlite.db` | SQLite database file path |
| `--log` | `info` | Log level (`verbose`, `debug`, `info`, `warning`, `error`, `critical`) |

Example:

```bash
./chatcore-server --ip 0.0.0.0 --port 1338 --db sqlite.db --log info
```
