#!/bin/bash

# PortMaster preamble
XDG_DATA_HOME=${XDG_DATA_HOME:-$HOME/.local/share}
if [ -d "/opt/system/Tools/PortMaster/" ]; then
  controlfolder="/opt/system/Tools/PortMaster"
elif [ -d "/opt/tools/PortMaster/" ]; then
  controlfolder="/opt/tools/PortMaster"
elif [ -d "$XDG_DATA_HOME/PortMaster/" ]; then
  controlfolder="$XDG_DATA_HOME/PortMaster"
else
  controlfolder="/roms/ports/PortMaster"
fi
source $controlfolder/control.txt
[ -f "${controlfolder}/mod_${CFW_NAME}.txt" ] && source "${controlfolder}/mod_${CFW_NAME}.txt"
get_controls

GAMEDIR=/$directory/ports/openkb
CONFDIR="$GAMEDIR/conf"

# Logging
> "$GAMEDIR/log.txt" && exec > >(tee "$GAMEDIR/log.txt") 2>&1

# The game keeps its config and saves in $HOME/.openkb; keep that inside the port
mkdir -p "$CONFDIR"
export HOME="$CONFDIR"

cd $GAMEDIR
$ESUDO chmod +x openkb.aarch64

export SDL_GAMECONTROLLERCONFIG="$sdl_controllerconfig"

$GPTOKEYB2 "openkb.aarch64" -c "$GAMEDIR/openkb.gptk" &
pm_platform_helper "$GAMEDIR/openkb.aarch64"
./openkb.aarch64 --fullscreen --gamepad --stretch --rootdir "$GAMEDIR/data"
pm_finish
