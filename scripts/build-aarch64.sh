#!/bin/bash
# Builds openkb for aarch64 inside the PortMaster builder container.
# Run from the repository root:
#
#   docker run --rm --platform=linux/arm64 -v "$PWD":/workspace -w /workspace \
#     ghcr.io/monkeyx-net/portmaster-build-templates/portmaster-builder:aarch64-latest \
#     scripts/build-aarch64.sh
#
# Output: build-aarch64/openkb.aarch64
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD="$ROOT/build-aarch64"
mkdir -p "$BUILD"

command -v sdl2-config >/dev/null || { apt-get update -qq && apt-get install -y -qq libsdl2-dev >/dev/null; }

# The container's sdl2-config adds an rpath to /usr/local/lib; the port must not carry it
SDL_LIBS="$(sdl2-config --libs | sed 's/-Wl,-rpath,[^ ]*//; s/-Wl,--enable-new-dtags//')"

make -C "$ROOT" clean >/dev/null
make -C "$ROOT" -j"$(nproc)" CC=gcc SDL_LIBS="$SDL_LIBS"
strip -o "$BUILD/openkb.aarch64" "$ROOT/openkb"
make -C "$ROOT" clean >/dev/null

echo "== result =="
file "$BUILD/openkb.aarch64"
echo "-- NEEDED:"; readelf -d "$BUILD/openkb.aarch64" | grep -E "NEEDED|RPATH|RUNPATH" || true
echo "-- max GLIBC symbol version:"; objdump -T "$BUILD/openkb.aarch64" | grep -o 'GLIBC_[0-9.]*' | sort -Vu | tail -1
