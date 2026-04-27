`chatcore-client` is an interactive FTXUI terminal client for the chat server.

## Commands

| Command | Arguments | Description |
| --- | --- | --- |
| `/connect` | `<address> <port>` | Connect to the chat server |
| `/adduser` | `<user_name> <password>` | Create a new user |
| `/login` | `<user_name> <password>` | Authenticate and log in |
| `/logout` | none | Log out current user |
| `/addchannel` | `<channel_name>` | Create a new channel |
| `/channels` | none | List available channels |
| `/join` | `<channel_id>` | Join a channel and fetch recent messages |
| `/help` | none | Print all command usage |
| `/exit` | none | Quit the client |