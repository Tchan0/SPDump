# Put the filename of the output binary here
TARGET = SPDump.elf

# List all of your C files here, but change the extension to ".o"
# Include "romdisk.o" if you want a rom disk.
OBJS = SPDump.o

all: rm-elf $(TARGET)

include $(KOS_BASE)/Makefile.rules

asm: 
	${KOS_CC_BASE}/bin/${KOS_CC_PREFIX}-objdump -d -S $(TARGET) > SPDump.asm

clean: rm-elf
	-rm -f $(OBJS)

rm-elf:
	-rm -f $(TARGET)

$(TARGET): $(OBJS)
	kos-cc -o $(TARGET) $(OBJS)

run: $(TARGET)
	$(KOS_LOADER) $(TARGET)
# /opt/toolchains/dc/bin/dc-tool-ser -t /dev/ttyUSB0 -b 520833 -x ./minimal.elf

dist: $(TARGET)
	-rm -f $(OBJS) romdisk.img
	$(KOS_STRIP) $(TARGET)