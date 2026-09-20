## Notes

OpenKB is an open source reimplementation of **King's Bounty** (New World Computing, 1990), the turn-based fantasy strategy game that later grew into Heroes of Might and Magic. Explore four continents, recruit an army, hunt down villains and recover the Sceptre of Order before your days run out.

The port ships with the game's free art set. Owners of the DOS original can copy `416.CC`, `256.CC` and `KB.EXE` into `openkb/data/dos/` and pick **DOS (VGA)** in the module menu at start to play with the original graphics.

Thanks to **Vitaly Driedfruit** for openkb, to **missandei** and **Santiago Iborra** for the free graphics, and to Jon Van Caneghem for the original game.

Source of this build: https://github.com/flagman/openkb (SDL2 port with gamepad-navigable menus, GPLv3). Config and saves live in `openkb/conf/.openkb/`.

## Controls

Buttons are listed by position, because the letters printed on them differ between devices: Anbernic, Miyoo, TrimUI and Powkiddy use the Nintendo layout, Retroid, Odin and AYN the Xbox one. The in-game legend (Options menu) uses the Xbox/SDL letters, the same convention as PortMaster itself.

| Control       | Nintendo label | Xbox label | Action                                        |
|---------------|----------------|------------|-----------------------------------------------|
| D-pad / Left stick | | | Move the hero, move the cursor in menus  |
| L1 + Up/Down  | | | Move up-left / down-left                      |
| R1 + Up/Down  | | | Move up-right / down-right                    |
| East button / Start | A | B | Confirm, continue (Enter)             |
| South button  | B | A | Back, close (Esc)                             |
| North button  | X | Y | Wait (skip the unit's turn in combat, end week on the map); in the Load menu: delete the highlighted saved game after a confirmation |
| West button   | Y | X | Options menu with every command               |
| L2            | | | View army                                     |
| R2            | | | Use magic                                     |
| Right stick   | | | Mouse pointer, click with L3 / R3             |
| Select + Start | | | Quit the port (use Options > Quit and Save first to keep progress) |

The Options menu (west button) lists every command with its button; every menu can be walked with the d-pad and picked with the confirm button. Name entry: Up/Down pick a letter, Right moves to the next one, Left erases, confirm accepts (an empty name becomes the class name). In "How many?" prompts Up/Down change the number by 1 and Left/Right by 10; leaving it empty means "all".

## Building

`scripts/build-aarch64.sh` in the source repository builds `openkb.aarch64` inside the PortMaster `portmaster-builder:aarch64-latest` container; the only runtime dependency is the system SDL2.
