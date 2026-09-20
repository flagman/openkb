/* Test harness: inject scripted keys into SDL_PollEvent, dump distinct frames from SDL_UpdateTexture. */
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *script = NULL, *cursor = NULL;
static const char *outdir = ".";
static Uint32 last_inject = 0, interval = 500;
static int pending_up = 0; static SDL_Keycode pending_key = 0;
static int done_script = 0; static int quit_sent = 0;
static unsigned char *lastframe = NULL; static size_t lastsize = 0; static int nframes = 0;

static void init(void) {
	static int inited = 0; if (inited) return; inited = 1;
	const char *k = getenv("OPENKB_TEST_KEYS"); if (k) { script = strdup(k); cursor = script; }
	const char *d = getenv("OPENKB_TEST_DIR"); if (d) outdir = d;
	const char *i = getenv("OPENKB_TEST_INTERVAL"); if (i) interval = atoi(i);
	last_inject = SDL_GetTicks() + 1000; /* let the first screen settle */
}

int my_PollEvent(SDL_Event *ev) {
	init();
	int r = SDL_PollEvent(ev);
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
		SDL_Keycode key = SDL_GetKeyFromName(cursor);
		fprintf(stderr, "[harness] key '%s' -> %d\n", cursor, (int)key);
		cursor = comma ? comma + 1 : cursor + strlen(cursor);
		last_inject = now;
		memset(ev, 0, sizeof *ev); ev->type = SDL_KEYDOWN; ev->key.keysym.sym = key;
		ev->key.keysym.scancode = SDL_GetScancodeFromKey(key); ev->key.state = SDL_PRESSED;
		pending_up = 1; pending_key = key;
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
		if (nframes < 200) {
			char path[1024]; snprintf(path, sizeof path, "%s/frame-%03d.bmp", outdir, nframes);
			SDL_Surface *s = SDL_CreateRGBSurfaceFrom((void*)pixels, w, h, 32, pitch, 0x00FF0000, 0x0000FF00, 0x000000FF, 0);
			SDL_SaveBMP(s, path); SDL_FreeSurface(s);
		}
		nframes++;
	}
	return SDL_UpdateTexture(t, rect, pixels, pitch);
}

#define INTERPOSE(new, old) __attribute__((used)) static const struct { const void *n, *o; } _ip_##old __attribute__((section("__DATA,__interpose"))) = { (const void*)new, (const void*)old }
INTERPOSE(my_PollEvent, SDL_PollEvent);
INTERPOSE(my_UpdateTexture, SDL_UpdateTexture);

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
	int r = SDL_OpenAudio(desired, obtained);
	if (obtained) fprintf(stderr, "[harness] audio: fmt=0x%x ch=%d freq=%d samples=%d\n", obtained->format, obtained->channels, obtained->freq, obtained->samples);
	return r;
}
INTERPOSE(my_OpenAudio, SDL_OpenAudio);
