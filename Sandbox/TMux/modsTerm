#!/bin/bash
#
# modsTerm - start MODS binocular observing script session
#
# This script launches a named tmux session that will be used to execute
# MODS binocular science target acquisition and observation scripts.
#
# While it may be invoked by itself, if the session is not running
# it will be executed by the acqBinoMODS and execBinoMODS scripts.
#
# The modsTerm tmux session is configured with 3 window panes
# 
#      +---------------------+---------------------+
#      |        MODS1        |        MODS2        |
#      |         1.0         |         1.1         |
#      |                     |                     |
#      |                     |                     |
#      |                     |                     |
#      +---------------------+---------------------+
#      |                  command                  |
#      |                    1.2                    |
#      +---------------------+---------------------+
#
# Low-level setup is in the $configDir/tmux_mods.conf file.
# Also defines and exports aliases to the bash shell to make
# detachment by "detach" or "quit" and throttling "exit"
#
# R. Pogge (pogge.1@osu.edu)
# OSU Astronomy Department
# 2025 Oct 4 - Alpha version
#
# Modification History:
#   2025 Oct 13 - beta version, put tmaliases in configDir [rwp/osu]
#   2025 Oct 14 - resize on launch to a good size if needed [rwp/osu]
#
#---------------------------------------------------------------------------

tmuxID="modsTerm"

# LBTO Public executable path

modsDir="/lbt/lbto/mods"
configDir="/lbt/lbto/mods/.config"

# Parse the command-line options

tmux has-session -t $tmuxID 2> /dev/null
if [ $? != 0 ]; then
   tmux -f ${configDir}/tmux_mods.conf new-session -d -s $tmuxID
   tmux new-window -t ${tmuxID}:1 -n "MODS Binocular Observing"
   tmux split-window -v -p 5  
   tmux split-window -h -p 50 -t ${tmuxID}:1.0
   # pane IDs
   tmux set -g pane-border-status top
   tmux set -g pane-border-format " #{pane_title} "
   tmux select-pane -t ${tmuxID}:1.0 -T "MODS1"
   tmux select-pane -t ${tmuxID}:1.1 -T "MODS2"
   tmux select-pane -t ${tmuxID}:1.2 -T "Commands"
   # aliases
   tmux send-keys -t ${tmuxID}:1.0 "source ${configDir}/tmaliases" C-m
   tmux send-keys -t ${tmuxID}:1.1 "source ${configDir}/tmaliases" C-m
   tmux send-keys -t ${tmuxID}:1.2 "source ${configDir}/tmaliases" C-m
fi

# If we're already in the expected tmux session, do nothing
# otherwise attach to the command pane (1.2)

if [ -n "$TMUX" ]; then
   echo "Currently in the ${tmuxID} session"
else
   resize -2 42 128 &> /dev/null
   tmux select-pane -t ${tmuxID}:1.2
   tmux attach-session -t ${tmuxID}
fi

exit 0
