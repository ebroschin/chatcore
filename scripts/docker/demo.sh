#!/usr/bin/env bash
set -euo pipefail

SESSION=chatcore-demo
PORT="${CHAT_PORT:-1338}"

SERVER_BIN="/workspace/src/build/linux-release-server/apps/server/chatcore-server"
CLIENT_BIN="/workspace/src/build/linux-release-client/apps/client/chatcore-client"
LOAD_BIN="/workspace/src/build/linux-release-load-tester/apps/load-tester/chatcore-load-tester"

tmux has-session -t "$SESSION" 2>/dev/null && tmux kill-session -t "$SESSION"
tmux new-session -d -s "$SESSION" -n "Navigate between windows by pressing Ctrl+B + Arrow Keys | Quit session: Ctrl+B + D"

P_TL="$(tmux display-message -p -t "$SESSION:0.0" "#{pane_id}")"        # top-left
P_TR="$(tmux split-window -h -t "$P_TL" -P -F "#{pane_id}")"             # top-right
P_BL="$(tmux split-window -v -t "$P_TL" -P -F "#{pane_id}")"             # bottom-left
P_BR="$(tmux split-window -v -t "$P_TR" -P -F "#{pane_id}")"             # bottom-right

# Show pane titles above each pane
tmux set -t "$SESSION:0" pane-border-status top
tmux set -t "$SESSION:0" pane-border-format "#{pane_title}"

tmux select-pane -t "$P_TL" -T "Client A (Ready to connect)"
tmux select-pane -t "$P_TR" -T "Client B (Ready to connect)"
tmux select-pane -t "$P_BL" -T "Server (Running)"
tmux select-pane -t "$P_BR" -T "Load Tester (press Enter to run Load Test)"

# Start commands
tmux send-keys -t "$P_TL" "$CLIENT_BIN --ip localhost --port $PORT --log verbose" C-m
tmux send-keys -t "$P_TR" "$CLIENT_BIN --ip localhost --port $PORT --log verbose" C-m
tmux send-keys -t "$P_BL" "$SERVER_BIN --port $PORT --log verbose" C-m

# Prefill only, do not execute
tmux send-keys -t "$P_BR" "$LOAD_BIN --host localhost --port $PORT"

# Focus top-left client on attach
tmux select-pane -t "$P_TL"

tmux attach -t "$SESSION"