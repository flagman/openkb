# Test harness (macOS)

`harness.c` builds into a dylib that is injected with `DYLD_INSERT_LIBRARIES`.
It interposes three SDL2 calls so the game can be driven and inspected without
touching game code or needing screen-recording permissions:

- `SDL_PollEvent`: feeds key presses from `OPENKB_TEST_KEYS` (comma-separated
  SDL key names, e.g. `Return,A,Right`) every `OPENKB_TEST_INTERVAL` ms
  (default 500), then sends `SDL_QUIT`.
- `SDL_UpdateTexture`: writes every distinct frame to `OPENKB_TEST_DIR/frame-NNN.bmp`.
- `SDL_OpenAudio`: appends the mixer output as raw PCM to `OPENKB_TEST_AUDIO`.

```
cc -dynamiclib $(sdl2-config --cflags) harness.c $(sdl2-config --libs) -o harness.dylib
DYLD_INSERT_LIBRARIES=$PWD/harness.dylib OPENKB_TEST_DIR=/tmp/frames \
  OPENKB_TEST_KEYS="Return,Return,Return,Return,A,B,O,B,Return,Return,Right" \
  ./openkb --rootdir "$PWD/data"
python3 pcmstat.py /tmp/audio.raw   # counts full-scale discontinuities
```

On Linux the same idea works with `LD_PRELOAD` and `dlsym(RTLD_NEXT, ...)`
instead of the `__interpose` section.
