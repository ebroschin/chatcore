`chatcore-load-tester` simulates concurrent clients against the server and prints a latency report.

## Application Arguments

| Flag | Default | Description |
| --- | --- | --- |
| `--ip` | `localhost` | Target server address |
| `--port` | `1338` | Target server port |
| `--clients` | `30` | Number of simulated clients |
| `--log` | `info` | Log level (`verbose`, `debug`, `info`, `warning`, `error`, `critical`) |

Example:

```bash
./chatcore-load-tester --ip localhost --port 1338 --clients 100 --log info
```