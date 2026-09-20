# openkb

Fork of **openkb**, an open source reimplementation of King's Bounty (1990)
by Vitaly Driedfruit and contributors.

Upstream: https://openkb.sourceforge.net/ (git: `https://git.code.sf.net/p/openkb/code`)

This fork ports the game to SDL2 with no other external dependencies, keeps it
building on current toolchains (Apple clang, arm64), and is the base for a
PortMaster port. All changes are published here under the same terms as upstream.

## What changed from upstream

- **SDL 1.2 → SDL2.** Rendering still happens on a software surface; `KB_flip()`
  uploads it to a streaming texture. Logical size is 320x200 or 640x400, so the
  window is freely resizable and fullscreen letterboxes correctly. Mouse
  coordinates arrive already scaled.
- **SDL_image and libpng dropped.** PNGs are decoded by the vendored
  [lodepng](https://github.com/lvandeve/lodepng) (zlib license) in
  `src/lib/kbpng.c`, which keeps indexed palettes and tRNS color keys intact,
  as the rest of the code expects.
- **Autotools dropped.** A plain `Makefile` and a hand-written `src/config.h`.
- **libhfs / librsrc dropped.** They were fetched and linked but never called.
- **vendor/drop.sh dropped.** `scale2x.c` and `inprint.c` are committed as-is.
- Fixes found while porting: a premature `va_end` that crashed on arm64, and
  the PC-speaker synth writing unsigned samples into a signed 16-bit stream.

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

Useful flags: `--fullscreen`, `--nosound`, `--config <file>`, `--rootdir <dir>`,
`--datadir <dir>`, `--savedir <dir>`.

## License

- Source code: GNU GPL v3 or later, see `src/COPYING`.
- Free game assets in `data/free`: CC-BY-SA 4.0 / GPL v3 dual-licensed, see
  `data/free/COPYING`.
- `vendor/lodepng.*`: zlib license (Lode Vandevenne).
- `vendor/scale2x.c`: GPL v2+ (Andrea Mazzoleni).
- `vendor/inprint.c`: from SDL_inprint by driedfruit.
