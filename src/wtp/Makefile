#include ../Defs.mak
#include ../Config.mak
#include ../WtpConfig.mak
#-include ../Config.local.mak
#-include ../WtpConfig.local.mak
#include ../Macros.mak
include ../Config.default.mak
-include ../Config.mak


INCL_DIRS=-I../

LIBDIR		:= ../../lib
LIBARCHDIR	:= $(LIBDIR)/$(ARCH)
OBJDIR		:= ../../obj/wtp/$(ARCH)
EXENAME		:= ./wtp

LIBS		:= -lcw -lnettle
#STATICLIBS	:= $(LIBDIR)/$(ARCH)/libcw.a
LDFLAGS		+= -L$(LIBARCHDIR) -L/usr/local/lib

#ifeq ($(SSL_LIBRARY),GNUTLS)
#CFLAGS+=-DWITH_GNUTLS
#CFLAGS+=$(GNUTLS_CFLAGS)
#LIBS+=$(GNUTLS_LIBS)
#LDFLAGS+=$(GNUTLS_LDFLAGS)
#endif

#ifeq ($(SSL_LIBRARY),OPENSSL)
CFLAGS+=-DWITH_OPENSSL
CFLAGS+=$(OPESSL_CFLAGS)
LDFLAGS+=$(OPENSSL_LDFLAGS)
LIBS+=$(OPENSSL_LIBS)
#endif

#SRC=$(wildcard *.c)
OBJS=\
     	wtp_main.o\
	discovery.o\
	join.o\
	configure.o\
	run.o\
	changestate.o\




#OBJS=$(patsubst %.c,%.o,$(SRC))
#OBJS:=$(patsubst %.o,$(OBJDIR)/%.o,$(OBJS))

ALL=$(EXENAME)

all: $(ALL)

#$(OBJDIR)/%.o:%.c

.c.o:
#	@mkdir -p $(OBJDIR)
	@echo "  $(CC) "$<
	@$(CC) $(INCL_DIRS) -c $(CFLAGS) $< -o $@

$(EXENAME): $(OBJS)
	echo "  $(CC) $(EXENAME)"
	$(CC) $(LDFLAGS) -o $(EXENAME) $(OBJS) $(LIBS) $(STATICLIBS)


clean: 
	rm -f *.o
#	rm  $(OBJDIR)/*
	rm -f $(EXENAME)

deps:  
	echo "# DO NOT DELETE THIS LINE -- make depend depends on it." > .depend
	makedepend $(SRC) -f.depend -p$(OBJDIR)/


-include .depend
