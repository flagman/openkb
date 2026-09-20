## Notes

OpenKB is an open source reimplementation of **King's Bounty** (New World Computing, 1990), the turn-based fantasy strategy game that later grew into Heroes of Might and Magic. Explore four continents, recruit an army, hunt down villains and recover the Sceptre of Order before your days run out.

The port ships with the game's free art set. Owners of the DOS original can copy `416.CC`, `256.CC` and `KB.EXE` into `openkb/data/dos/` and pick **DOS (VGA)** in the module menu at start to play with the original graphics.

Thanks to **Vitaly Driedfruit** for openkb, to **missandei** and **Santiago Iborra** for the free graphics, and to Jon Van Caneghem for the original game.

Source of this build: https://github.com/flagman/openkb (SDL2 port with gamepad-navigable menus, GPLv3). Config and saves live in `openkb/conf/.openkb/`.

## Controls

| Control       | Action                                        |
|---------------|-----------------------------------------------|
| D-pad         | Move the hero / move the cursor in menus      |
| L1 + D-pad    | Diagonal moves (d-pad turned 45° left)        |
| R1 + D-pad    | Diagonal moves (d-pad turned 45° right)       |
| East button (A on Anbernic) / Start | Confirm, continue (Enter) |
| South button (B on Anbernic) / Select | Back, close (Esc)   |
| X             | Options menu with every command               |
| Y             | View character                                |
| L2            | View army                                     |
| R2            | Use magic                                     |
| Left stick    | Same as d-pad                                 |
| Right stick   | Mouse pointer, click with L3 / R3             |

Every menu can be walked with the d-pad and picked with A. In "How many?" prompts Up/Down change the number by 1 and Left/Right by 10; leaving it empty means "all".

## Building

`scripts/build-aarch64.sh` in the source repository builds `openkb.aarch64` inside the PortMaster `portmaster-builder:aarch64-latest` container; the only runtime dependency is the system SDL2.
