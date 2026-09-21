# openkb

Fork of **openkb**, an open source reimplementation of King's Bounty (1990)
by Vitaly Driedfruit and contributors.

Upstream: https://openkb.sourceforge.net/ (git: `https://git.code.sf.net/p/openkb/code`)

This fork, maintained by **[Pavel Malai (flagman)](https://github.com/flagman)**,
ports the game to SDL2, fixes engine and gameplay bugs, and adds handheld controls
and a PortMaster port. SDL2 is the only external library dependency. All changes
are published here under the same terms as upstream.

## What changed from upstream

- **SDL 1.2 → SDL2.** Rendering still happens on a software surface; `KB_flip()`
  uploads it to a streaming texture. Logical size is 320x200 or 640x400, so the
  window is freely resizable and fullscreen letterboxes correctly. Optional
  `--stretch` scales the picture to 4:3 (640x480 at 2x zoom).
- **SDL_image and libpng dropped.** PNGs are decoded by the vendored
  [lodepng](https://github.com/lvandeve/lodepng) (zlib license) in
  `src/lib/kbpng.c`, which keeps indexed palettes and tRNS color keys intact,
  as the rest of the code expects.
- **Autotools dropped.** A plain `Makefile` and a hand-written `src/config.h`.
- **libhfs / librsrc dropped.** They were fetched and linked but never called.
- **vendor/drop.sh dropped.** `scale2x.c` and `inprint.c` are committed as-is.
- **Handheld controls.** D-pad navigation, controller-based name and number
  entry, diagonal movement with L1/R1 modifiers and on-screen hints, and saved
  game deletion with confirmation. No analog sticks are required.
- **Engine stability and audio fixes.** Fixes for heap buffer overflows, sounds
  freed while still playing, arm64 crashes, and distorted PC-speaker audio.
- **Gameplay fixes.** Correct leadership gains, immediate troop-count updates
  after attacks, archer AI in close combat, DOS map dwelling entry, and weekly
  upkeep and desertion based on the original DOS game.
- **PortMaster packaging.** An aarch64 build script, launcher, controller mapping
  and free game assets. See the [port README](portmaster/openkb/README.md) for
  controls and instructions for using the original DOS assets.

`src/tools/` (asset tools) is still SDL 1.2 and is not built by default.

## Building

Needs a C compiler, GNU make, and SDL2 with `sdl2-config` on `PATH`.

```
make
./openkb --rootdir "$PWD/data"
```

macOS: `brew install sdl2`. Debian/Ubuntu: `apt install libsdl2-dev`.

`make install PREFIX=/usr/local` installs the binary, `data/` and the man page.
When installed, `--rootdir` is not needed. Config lives in `~/.openkb/openkb.ini`.

Useful flags: `--fullscreen`, `--gamepad`, `--stretch`, `--nosound`,
`--config <file>`, `--rootdir <dir>`, `--datadir <dir>`, `--savedir <dir>`.

## Credits

- **Vitaly Driedfruit and contributors** — the original openkb engine.
- **[Pavel Malai (flagman)](https://github.com/flagman)** — maintenance of this
  fork, SDL2 migration, engine and gameplay bug fixes, handheld controls, and
  the PortMaster port.
- **missandei and Santiago Iborra** — the free graphics.
- **Jon Van Caneghem / New World Computing** — the original King's Bounty.

## License

- Source code: GNU GPL v3 or later, see `src/COPYING`.
- Free game assets in `data/free`: CC-BY-SA 4.0 / GPL v3 dual-licensed, see
  `data/free/COPYING`.
- `vendor/lodepng.*`: zlib license (Lode Vandevenne).
- `vendor/scale2x.c`: GPL v2+ (Andrea Mazzoleni).
- `vendor/inprint.c`: from SDL_inprint by driedfruit.
