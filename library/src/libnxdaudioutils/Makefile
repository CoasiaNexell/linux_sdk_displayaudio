include ../../../env.mk

######################################################################
# Build options
INCLUDE += -I. -I../../include
INCLUDE += -I. -I../../prebuilt/include
LIBRARY	+= -lstdc++ -lpthread -lm -lsqlite3 -lasound -lnx_config

######################################################################
# Target
COBJS	:=
CXXOBJS	:=
CXXOBJS += CNX_Time.o
CXXOBJS += CNX_PostProcess.o
CXXOBJS += CNX_DAudioStatus.o
CXXOBJS += NX_SqliteUtils.o
CXXOBJS += NX_DAudioUtils.o

CXXFLAGS += -fPIC

TARGET := libnxdaudioutils.so

######################################################################
# Build
OBJS	:= $(COBJS) $(CXXOBJS)

all: $(TARGET)

$(TARGET):	depend $(OBJS)
	$(CC) -shared -Wl,-soname,$(TARGET) -o $@ $(OBJS) $(LDFLAGS) $(LIBRARY)

install:
	cp -av $(TARGET) ../../lib/
	cp -av CNX_DAudioStatus.h ../../include/
	cp -av NX_DAudioUtils.h ../../include/

clean:
	rm -rf $(TARGET) *.o .depend

distclean: clean
	rm -rf ../../lib/$(TARGET)
	rm -rf ../../include/CNX_DAudioStatus.h
	rm -rf ../../include/NX_DAudioUtils.h

#########################################################################
# Dependency
ifeq (.depend,$(wildcard .depend))
include .depend
endif

SRCS := $(COBJS:.o=.c) $(AOBJS:.o=.s) $(CXXOBJS:.o=.cpp)
INCS := $(INCLUDE)
depend dep:
	$(CC) -M $(CFLAGS) $(AFLAGS) $(CXXFLAGS) $(INCS) $(SRCS) > .depend
