#!/usr/bin/env bash
set -euo pipefail

WORKSPACE="${1:-"/workspace/src/"}"
SESSION=chatcore-demo
PORT="${CHAT_PORT:-1338}"

SERVER_BIN="${WORKSPACE}/build/linux-release-server/apps/server/chatcore-server"
CLIENT_BIN="${WORKSPACE}/build/linux-release-client/apps/client/chatcore-client"
LOAD_BIN="${WORKSPACE}/build/linux-release-load-tester/apps/load-tester/chatcore-load-tester"

tmux has-session -t "$SESSION" 2>/dev/null && tmux kill-session -t "$SESSION"
tmux new-session -d -s "$SESSION" -n "Navigate between windows by pressing Ctrl+B + Arrow Keys | Quit session: Ctrl+B + D"

P_TL="$(tmux display-message -p -t "$SESSION:0.0" "#{pane_id}")"
P_TR="$(tmux split-window -h -t "$P_TL" -P -F "#{pane_id}")" 
P_BL="$(tmux split-window -v -t "$P_TL" -P -F "#{pane_id}")"
P_BR="$(tmux split-window -v -t "$P_TR" -P -F "#{pane_id}")"

tmux set -t "$SESSION:0" pane-border-status top
tmux set -t "$SESSION:0" pane-border-format "#{pane_title}"

tmux select-pane -t "$P_BL" -T "Server (Running)"
tmux select-pane -t "$P_BR" -T "Load Tester (press Enter to run Load Test)"

tmux send-keys -t "$P_TL" "$CLIENT_BIN --ip localhost --port $PORT --log verbose" C-m
tmux send-keys -t "$P_TR" "$CLIENT_BIN --ip localhost --port $PORT --log verbose" C-m
tmux send-keys -t "$P_BL" "$SERVER_BIN --port $PORT --log verbose" C-m

tmux send-keys -t "$P_BR" "$LOAD_BIN --ip localhost --port $PORT"
tmux select-pane -t "$P_TL"

tmux attach -t "$SESSION"
