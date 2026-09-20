/*
 *  config.h -- build configuration (hand-written, replaces autoconf output)
 *
 *  This file is part of openkb and is distributed under the GNU GPL v3
 *  or later, see COPYING.
 */
#ifndef _OPENKB_CONFIG_H
#define _OPENKB_CONFIG_H

#define PACKAGE_NAME "openkb"
#define PACKAGE_TARNAME "openkb"
#define PACKAGE_VERSION "0.0.3"
#define PACKAGE_STRING "openkb 0.0.3"
#define PACKAGE_BUGREPORT "https://github.com/flagman/openkb/issues"
#define PACKAGE_URL "https://github.com/flagman/openkb"

/* SDL2 is the only supported backend */
#define HAVE_LIBSDL 1

/* Standard C / POSIX bits every supported platform has */
#define HAVE_STRING_H 1
#define HAVE_STRINGS_H 1
#define HAVE_STDINT_H 1
#define HAVE_INTTYPES_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STDIO_H 1
#define HAVE_UNISTD_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_MALLOC 1
#define HAVE_ISASCII 1
#define HAVE_MKDIR 1
#define HAVE_GETCWD 1
#define HAVE_STRCASECMP 1
#define HAVE_STRDUP 1

/* strlcpy/strlcat: BSD libc and macOS have them, glibc only since 2.38.
 * Everything else uses the copies in vendor/. */
#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
#define HAVE_STRLCPY 1
#define HAVE_STRLCAT 1
#elif defined(__GLIBC__) && defined(__GLIBC_PREREQ)
#if __GLIBC_PREREQ(2, 38)
#define HAVE_STRLCPY 1
#define HAVE_STRLCAT 1
#endif
#endif

#if defined(__linux__)
#define HAVE_MALLOC_H 1
#define HAVE_ENDIAN_H 1
#endif

#endif /* _OPENKB_CONFIG_H */
