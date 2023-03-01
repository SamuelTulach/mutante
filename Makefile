DRVNAME = main
INCLUDES = -I/usr/i686-w64-mingw32/include/ddk
NATIVE = -nostartfiles -nostdlib -w -Wno-everything -Wl,--subsystem,nativem
CFLAGS =  $(INCLUDES)  -Iinclude -std=c++11

# The list of sub-components to build
OBJECTS = 
NTDLL = /usr/i686-w64-mingw32/lib/libntdll.a
SRCOBJS = $(OBJECTS:.o=.cpp)



# Kernel-mode libs:
#   libntoskrnl = basic kernel-mode environment
#   libhal = WRITE_PORT_UCHAR et al.
KRNLIBS = -lntoskrnl -lhal -lntdll

CC = i686-w64-mingw32-g++
DLLTOOL = i686-w64-mingw32-dlltool
STRIP = i686-w64-mingw32-strip

all: $(OBJECTS) $(DRVNAME).sys

.SUFFIXES: .sys 


.o.sys: 
	echo "1111111111"
	$(CC) $(OBJECTS) $*.o -Wl,--base-file,$*.base \
	-Wl,--entry,_DriverEntry@8 \
	-nostartfiles -nostdlib \
	-o junk.tmp \
	-L/usr/i686-w64-mingw32/include/lib/ \
	-lntoskrnl -lhal --enable-stdcall-fixup -lntdll ntdll.a -nostdlib 

#-rm -f junk.tmp

	echo "22222"
	$(DLLTOOL) --dllname $*.sys \
	--base-file $*.base --output-exp $*.exp 

	echo "333333"
	$(CC) $(OBJECTS)  -Wl,--subsystem,native \
	-Wl,--image-base,0x10000 \
	-Wl,--file-alignment,0x1000 \
	-Wl,--section-alignment,0x1000 \
	-Wl,--entry,_DriverEntry@8 \
	-Wl,$*.exp \
	-mdll -nostartfiles -L/usr/i686-w64-mingw32/include/lib/ -lntoskrnl -lhal --enable-stdcall-fixup -nostdlib -lntdll ntdll.a \
	-o mutante.sys \
	$*.o \
	$(KRNLIBS) 

	#$(STRIP) $*.sys *.exe
	rm $(OBJECTS) *.base *.exp

JUNK = *.base *.exp *.o *~ junk.tmp

	

clean:
	rm -f $(JUNK) *.sys *.exe

semiclean:
	rm -f $(JUNK) 

