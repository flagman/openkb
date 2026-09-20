/* Test harness: inject scripted keys into SDL_PollEvent, dump distinct frames from SDL_UpdateTexture.
 * macOS: DYLD_INSERT_LIBRARIES + __interpose.  Linux: LD_PRELOAD + dlsym(RTLD_NEXT). */
#ifndef __APPLE__
#define _GNU_SOURCE
#include <dlfcn.h>
#endif
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __APPLE__
#define REAL_PollEvent SDL_PollEvent
#define REAL_UpdateTexture SDL_UpdateTexture
#define REAL_OpenAudio SDL_OpenAudio
#else
static int (*REAL_PollEvent)(SDL_Event *);
static int (*REAL_UpdateTexture)(SDL_Texture *, const SDL_Rect *, const void *, int);
static int (*REAL_OpenAudio)(SDL_AudioSpec *, SDL_AudioSpec *);
#endif
static char *script = NULL, *cursor = NULL;
static const char *outdir = NULL;	/* frames are dumped only when OPENKB_TEST_DIR is set */
static Uint32 last_inject = 0, interval = 500;
static int pending_up = 0; static SDL_Keycode pending_key = 0;
static int done_script = 0; static int quit_sent = 0;
static int nframes = 0;
static unsigned char *lastframe = NULL; static size_t lastsize = 0;

static void init(void) {
	static int inited = 0; if (inited) return; inited = 1;
#ifndef __APPLE__
	REAL_PollEvent = dlsym(RTLD_NEXT, "SDL_PollEvent");
	REAL_UpdateTexture = dlsym(RTLD_NEXT, "SDL_UpdateTexture");
	REAL_OpenAudio = dlsym(RTLD_NEXT, "SDL_OpenAudio");
#endif
	const char *k = getenv("OPENKB_TEST_KEYS"); if (k) { script = strdup(k); cursor = script; }
	const char *d = getenv("OPENKB_TEST_DIR"); if (d) outdir = d;
	const char *i = getenv("OPENKB_TEST_INTERVAL"); if (i) interval = atoi(i);
	last_inject = SDL_GetTicks() + 1000; /* let the first screen settle */
}

int my_PollEvent(SDL_Event *ev) {
	init();
	int r = REAL_PollEvent(ev);
	if (r) return r;
	if (!ev) return 0;
	Uint32 now = SDL_GetTicks();
	if (pending_up) {
		pending_up = 0;
		memset(ev, 0, sizeof *ev); ev->type = SDL_KEYUP; ev->key.keysym.sym = pending_key;
		ev->key.keysym.scancode = SDL_GetScancodeFromKey(pending_key); ev->key.state = SDL_RELEASED;
		return 1;
	}
	if (now - last_inject < interval) return 0;
	if (cursor && *cursor) {
		char *comma = strchr(cursor, ','); if (comma) *comma = 0;
		int hold = 0, release = 0;
		if (!strcmp(cursor, "LShift")) hold = 1;
		if (!strcmp(cursor, "-LShift")) { release = 1; cursor++; }
		SDL_Keycode key = SDL_GetKeyFromName(!strcmp(cursor, "LShift") ? "Left Shift" : cursor);
		fprintf(stderr, "[harness] key '%s' -> %d (after frame %d)\n", cursor, (int)key, nframes - 1);
		cursor = comma ? comma + 1 : cursor + strlen(cursor);
		last_inject = now;
		if (hold) SDL_SetModState(KMOD_LSHIFT);
		if (release) SDL_SetModState(KMOD_NONE);
		memset(ev, 0, sizeof *ev); ev->type = release ? SDL_KEYUP : SDL_KEYDOWN; ev->key.keysym.sym = key;
		ev->key.keysym.mod = SDL_GetModState();
		ev->key.keysym.scancode = SDL_GetScancodeFromKey(key); ev->key.state = release ? SDL_RELEASED : SDL_PRESSED;
		if (!hold && !release) { pending_up = 1; pending_key = key; }
		return 1;
	}
	if (!done_script) { done_script = 1; last_inject = now; return 0; }
	if (!quit_sent && now - last_inject > 1500) {
		quit_sent = 1; memset(ev, 0, sizeof *ev); ev->type = SDL_QUIT; fprintf(stderr, "[harness] quit\n"); return 1;
	}
	return 0;
}

int my_UpdateTexture(SDL_Texture *t, const SDL_Rect *rect, const void *pixels, int pitch) {
	init();
	int w, h; SDL_QueryTexture(t, NULL, NULL, &w, &h);
	size_t size = (size_t)pitch * h;
	if (!lastframe || lastsize != size || memcmp(lastframe, pixels, size) != 0) {
		lastframe = realloc(lastframe, size); lastsize = size; memcpy(lastframe, pixels, size);
		if (outdir && nframes < 200) {
			char path[1024]; snprintf(path, sizeof path, "%s/frame-%03d.bmp", outdir, nframes);
			SDL_Surface *s = SDL_CreateRGBSurfaceFrom((void*)pixels, w, h, 32, pitch, 0x00FF0000, 0x0000FF00, 0x000000FF, 0);
			SDL_SaveBMP(s, path); SDL_FreeSurface(s);
		}
		nframes++;
	}
	return REAL_UpdateTexture(t, rect, pixels, pitch);
}

#ifdef __APPLE__
#define INTERPOSE(new, old) __attribute__((used)) static const struct { const void *n, *o; } _ip_##old __attribute__((section("__DATA,__interpose"))) = { (const void*)new, (const void*)old }
INTERPOSE(my_PollEvent, SDL_PollEvent);
INTERPOSE(my_UpdateTexture, SDL_UpdateTexture);
#else
#define REAL(name) ((__typeof__(&name))dlsym(RTLD_NEXT, #name))
int SDL_PollEvent(SDL_Event *ev) { return my_PollEvent(ev); }
int SDL_UpdateTexture(SDL_Texture *t, const SDL_Rect *r, const void *p, int pitch) { return my_UpdateTexture(t, r, p, pitch); }
#endif

/* ---- audio capture: wrap the app's callback and append raw PCM to OPENKB_TEST_AUDIO ---- */
static SDL_AudioCallback user_cb = NULL; static void *user_ud = NULL; static FILE *audio_fp = NULL;
static void cap_cb(void *ud, Uint8 *stream, int len) {
	user_cb(user_ud, stream, len);
	if (audio_fp) { fwrite(stream, 1, len, audio_fp); fflush(audio_fp); }
}
int my_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained) {
	const char *path = getenv("OPENKB_TEST_AUDIO");
	if (path && desired && desired->callback) {
		user_cb = desired->callback; user_ud = desired->userdata;
		desired->callback = cap_cb; desired->userdata = NULL;
		audio_fp = fopen(path, "wb");
	}
	int r = REAL_OpenAudio(desired, obtained);
	if (obtained) fprintf(stderr, "[harness] audio: fmt=0x%x ch=%d freq=%d samples=%d\n", obtained->format, obtained->channels, obtained->freq, obtained->samples);
	return r;
}
#ifdef __APPLE__
INTERPOSE(my_OpenAudio, SDL_OpenAudio);
#else
int SDL_OpenAudio(SDL_AudioSpec *d, SDL_AudioSpec *o) { init(); return my_OpenAudio(d, o); }
#endif
