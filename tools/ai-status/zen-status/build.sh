#!/bin/sh
# Cross-compile zen-status.exe with mingw-w64 (gcc or llvm-mingw clang). No CRT: kernel32 + shell32 only.
set -e
cd "$(dirname "$0")"
CC="${CC:-x86_64-w64-mingw32-gcc}"
"$CC" -Os -s -std=c99 -Wall -nostdlib -ffreestanding -fno-builtin -fno-stack-protector -mno-stack-arg-probe \
  -fno-asynchronous-unwind-tables -e zs_entry -Wl,--subsystem,console -Wl,--no-insert-timestamp \
  zen-status.c -lkernel32 -lshell32 -o "${OUT:-zen-status.exe}"
ls -l "${OUT:-zen-status.exe}"
