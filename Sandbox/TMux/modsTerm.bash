#!/bin/bash

tmuxID="modsTerm"
modsHost="${HOSTNAME^^}"

# Check if the session already exists

tmux has-session -t $tmuxID 2>/dev/null

if [ $? != 0 ]; then
  # Create a new detached session

  tmux -f ~/.tmux.conf.mods new-session -d -s $tmuxID

  # Create a new window (if needed) and name it

  tmux new-window -t $tmuxID:1 -n "${modsHost} Server Admin"

  # Split the window into panes (optional, adjust as needed)

  tmux split-window -v -p 5  # split bottom pane shorter
  tmux split-window -h -p 50 -t $tmuxID:1.0 # Split horizontally

  # Send commands to the panes to start programs

  # tmux send-keys -t $tmuxID:1.0 "./modsStatus" C-m # run the ISIS server in the first pane

  # Select the pane you want active initially

  tmux select-pane -t $tmuxID:1.2 # attach the command pane

fi

# Attach to the session

tmux attach-session -t $tmuxID
