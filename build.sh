#!/bin/bash
INCLUDE_DIR='C:/w64devkit/i686-w64-mingw32/include/ddk/'
LINK_DIR='C:/w64devkit/i686-w64-mingw32/include/lib/'
# build
i686-w64-mingw32-g++ \
  -c mutante/mutante/main.cpp \
  -o mutante.o \
  -I$INCLUDE_DIR \
  -Iinclude/ \
  -nostartfiles \
  -nostdlib \
  -w \
  -Wno-everything \
  -Wl,--subsystem,nativem \
  -std=c++11 \
  -lntdll \
  -DNTDDI_VERSION=NTDDI_WIN10_19H1
# ntdll .a
cp C:/Windows/System32/ntdll.dll .
gendef ntdll.dll
i686-w64-mingw32-dlltool --def ntdll.def --dllname ntdll.dll --output-lib ntdll.a
# link
i686-w64-mingw32-g++ mutante.o \
  -Wl,--base-file,main.base \
  -Wl,--entry,_DriverEntry@8 \
  -nostartfiles -nostdlib \
  -o mutante.sys \
  -L$LINK_DIR \
  -L. \
  -lntdll \
  -lntoskrnl -lhal --enable-stdcall-fixup -nostdlib \
  -Wl,--subsystem,native -Wl,--image-base,0x10000 \
  -Wl,--file-alignment,0x1000 -Wl,--section-alignment,0x1000 \
  -Wl,--entry,_DriverEntry@8 -mdll

