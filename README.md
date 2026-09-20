# openkb

Fork of **openkb**, an open source reimplementation of King's Bounty (1990)
by Vitaly Driedfruit and contributors.

Upstream: https://openkb.sourceforge.net/ (git: `https://git.code.sf.net/p/openkb/code`)

This fork exists to keep the game building on current toolchains (macOS /
Apple clang) and to prepare a PortMaster port. All changes are published
here under the same terms as upstream.

## License

- Source code: GNU GPL v3 or later, see `src/COPYING`.
- Free game assets in `data/free`: CC-BY-SA 4.0 / GPL v3 dual-licensed, see
  `data/free/COPYING`.

## Building on macOS

Requires Homebrew `sdl12-compat`, `libpng`, `autoconf`, `automake`, and an
SDL_image 1.2 built against sdl12-compat (not available in Homebrew; build
from `SDL_image-1.2.12` source with `-Wno-incompatible-function-pointer-types`).

```
vendor/drop.sh vendor/          # fetches vendored sources; see notes below
./autogen.sh
./configure CFLAGS="-g -I$HOME/.local/sdl_image-1.2/include/SDL -I/opt/homebrew/include" \
            LIBS="-L/opt/homebrew/lib -L$HOME/.local/sdl_image-1.2/lib"
make
./openkb --rootdir "$PWD/data"
```

Notes on `vendor/drop.sh`: its `git://` and `ftp://` URLs are dead. Fetch
manually instead:

- `SDL_inprint`, `SDL_SavePNG`: `https://github.com/driedfruit/<name>.git`
- `scale2x`: `contrib/sdl/scale2x.c` from `https://github.com/amadvance/scale2x`
- `hfsutils-3.2.6.tar.gz`: `ftp://ftp.mars.org/pub/hfs/` still answers to curl;
  copy `install-sh` into `libhfs/` and `librsrc/` and configure them with
  `CFLAGS="-Wno-implicit-int -Wno-implicit-function-declaration -Wno-int-conversion"`.
- `sha2` is listed but not used by the build.
