# set nesemu version
VERSION=0.2.3

# set something else...
WHAT=ISIT?

# blargg apu is c++ code, this is a hack
ifeq ($(BLARGGAPU),1)
	CC = $(CXX)
endif

all: version.h $(TARGET)

version.h:
	echo "#ifndef __version_h__" > version.h
	echo "#define __version_h__" >> version.h
	echo "#define VERSION \"$(VERSION)\"" >> version.h
	echo "#endif" >> version.h

packed: all $(TARGETP)

release: all
	$(TAR) nesemu-src-$(VERSION).tar.gz $(SOURCES) README hle_fds.bin nsf_bios.bin
	$(TAR) nesemu-$(BUILDTARGET)-$(VERSION).tar.gz $(TARGET) README hle_fds.bin nsf_bios.bin

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

$(TARGETP): $(TARGET)
	$(PACKER) $(PACKERFLAGS)

clean:
	rm -f $(OBJS) version.h $(TARGET) $(TARGETP)

rebuild: clean all

%.c: $(BMP_DIR)/%.bmp
	$(BMP2C) $< $(*F) > $@
