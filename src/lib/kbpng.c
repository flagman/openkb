/*
 *  kbpng.c -- PNG loader (via lodepng) that keeps palettes intact
 *  Copyright (C) 2026 Pavel Malai
 *
 *  This file is part of openkb.
 *
 *  openkb is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  openkb is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with openkb.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <SDL.h>

#include "kbres.h"
#include "kbstd.h"

#include "../../vendor/lodepng.h"

/*
 * Decode a PNG held in memory into an SDL_Surface.
 *
 * Paletted PNGs (1/2/4/8-bit) become 8-bit indexed surfaces with the
 * original palette, and the first fully transparent palette entry (tRNS)
 * becomes the color key. This mirrors what SDL_image 1.2 used to do and
 * is what the rest of the code relies on (it pokes at palettes directly).
 *
 * Anything else is decoded to 32-bit RGBA.
 */
static SDL_Surface* KB_DecodePNG(const unsigned char *data, size_t size, const char *what)
{
	LodePNGState state;
	unsigned char *raw = NULL;
	unsigned w, h, err;
	SDL_Surface *surf = NULL;

	lodepng_state_init(&state);
	state.decoder.color_convert = 0; /* give us the file's own format */

	err = lodepng_decode(&raw, &w, &h, &state, data, size);
	if (err) {
		KB_errlog("PNG '%s': %s\n", what, lodepng_error_text(err));
		lodepng_state_cleanup(&state);
		free(raw);
		return NULL;
	}

	if (state.info_png.color.colortype == LCT_PALETTE) {
		unsigned bd = state.info_png.color.bitdepth;
		LodePNGColorMode *cm = &state.info_png.color;
		SDL_Color pal[256];
		size_t n = cm->palettesize > 256 ? 256 : cm->palettesize;
		size_t i;
		int key = -1;
		Uint32 x, y;
		size_t stride = (w * bd + 7) / 8;

		surf = SDL_CreateRGBSurface(0, w, h, 8, 0, 0, 0, 0);
		if (!surf) goto done;

		for (i = 0; i < n; i++) {
			pal[i].r = cm->palette[i * 4 + 0];
			pal[i].g = cm->palette[i * 4 + 1];
			pal[i].b = cm->palette[i * 4 + 2];
			pal[i].a = 255;
			if (key < 0 && cm->palette[i * 4 + 3] == 0) key = (int)i;
		}
		if (n) SDL_SetPaletteColors(surf->format->palette, pal, 0, (int)n);

		SDL_LockSurface(surf);
		for (y = 0; y < h; y++) {
			const unsigned char *row = raw + y * stride;
			Uint8 *dst = (Uint8 *)surf->pixels + y * surf->pitch;
			for (x = 0; x < w; x++) {
				unsigned bit = x * bd;
				unsigned byte = row[bit / 8];
				unsigned shift = 8 - bd - (bit % 8);
				dst[x] = (byte >> shift) & ((1u << bd) - 1);
			}
		}
		SDL_UnlockSurface(surf);

		if (key >= 0) SDL_SetColorKey(surf, SDL_TRUE, (Uint32)key);
	} else {
		/* Not paletted: ask lodepng for plain RGBA8 and wrap it */
		unsigned char *rgba = NULL;
		free(raw); raw = NULL;
		err = lodepng_decode32(&rgba, &w, &h, data, size);
		if (err) {
			KB_errlog("PNG '%s': %s\n", what, lodepng_error_text(err));
			goto done;
		}
		surf = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA32);
		if (surf) {
			SDL_LockSurface(surf);
			memcpy(surf->pixels, rgba, (size_t)w * h * 4);
			SDL_UnlockSurface(surf);
		}
		free(rgba);
	}

done:
	free(raw);
	lodepng_state_cleanup(&state);
	return surf;
}

/* Load a PNG from an SDL_RWops. Does not close it. */
SDL_Surface* KB_LoadPNG_RW(SDL_RWops *rw)
{
	Sint64 start, size;
	unsigned char *buf;
	SDL_Surface *surf;

	if (!rw) return NULL;

	start = SDL_RWtell(rw);
	size = SDL_RWsize(rw);
	if (size <= 0 || start < 0) return NULL;
	size -= start;

	buf = malloc((size_t)size);
	if (!buf) return NULL;

	if (SDL_RWread(rw, buf, 1, (size_t)size) != (size_t)size) {
		free(buf);
		return NULL;
	}

	surf = KB_DecodePNG(buf, (size_t)size, "<rwops>");
	free(buf);
	return surf;
}

/* Load a PNG from a file on disk. */
SDL_Surface* KB_LoadPNG(const char *filename)
{
	unsigned char *buf = NULL;
	size_t size = 0;
	SDL_Surface *surf;

	if (lodepng_load_file(&buf, &size, filename)) {
		KB_errlog("PNG '%s': can't read file\n", filename);
		free(buf);
		return NULL;
	}
	surf = KB_DecodePNG(buf, size, filename);
	free(buf);
	return surf;
}
