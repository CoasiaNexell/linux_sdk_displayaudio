include ../../env.mk

#########################################################################
#	Toolchain.
#########################################################################
INCLUDE	+= -I./ -I../../library/prebuilt/include/

LIBRARY	+= -lstdc++

################################################################################
# Target
CXXOBJS	:=
CXXOBJS	+= main.o

CXXFLAGS += -fPIC

TARGET	= nxlogcat

################################################################################
# Build
OBJS	:= $(COBJS) $(CXXOBJS)

all: $(TARGET)

$(TARGET):	depend $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS) $(LIBRARY)

install:
	cp -a $(TARGET) ../../bin/
	ln -sf $(TARGET) logcat
	cp -a logcat ../../bin/

clean:
	rm -f $(TARGET) *.o .depend logcat

distclean: clean
	rm -rf ../../bin/$(TARGET)
	rm -rf ../../bin/logcat

#########################################################################
# Dependency
ifeq (.depend,$(wildcard .depend))
include .depend
endif

SRCS := $(COBJS:.o=.c) $(AOBJS:.o=.s) $(CXXOBJS:.o=.cpp)
INCS := $(INCLUDE)
depend dep:
	$(CC) -M $(CFLAGS) $(AFLAGS) $(CXXFLAGS) $(INCS) $(SRCS) > .depend
