# Rocrail - Model Railroad Software
#
#    Copyright (C) 2002-2012 Rob Versluis, Rocrail.net
#
#    Without an official permission commercial use is not permitted.
#    Forking this project is not permitted.  
#
# ------------------------------------------------------------
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
# ------------------------------------------------------------
# commandline:
#   make DEBUG=... MOUNTPOINT=... OUTDIR=...
#
# commandline for mingw cross compiling:
#   make PLATFORM=WIN32 clean
#   make TOOLPREFIX=i386-mingw32- PLATFORM=WIN32 all
#
# commandline for mingw native compiling:
#   make PLATFORM=WIN32 NATIVE=WIN32 clean
#   make TOOLPREFIX=i386-mingw32- PLATFORM=WIN32 NATIVE=WIN32 all
#
# commandline for osx compiling:
#   make PLATFORM=MACOSX clean all
#
FS=/
CS=;
COPY=cp
REMOVE=rm -f
GENSUFFIX=
MKDIR=mkdir -p

ifeq ($(NATIVE),WIN32)
	FS=$\\
	CS=&
	COPY=copy
	REMOVE=del /Q
	GENSUFFIX=.exe
	MKDIR=mkdir
endif


PLATFORM=LINUX
ARCH=
ifeq ($(PLATFORM),WIN32)
	LIBS=-liphlpapi -lmpr -lws2_32 -ladvapi32 -lwinmm -lm
	DIRPREFIX=win
	CC_EXTRA_FLAGS=-mthreads
	LNK_FLAGS=--def rocdigs.def -mthreads -static
	LNK=$(TOOLPREFIX)dllwrap
	SHAREDSUFFIX=.dll
else ifeq ($(PLATFORM),WIN64)
	DIRPREFIX=win
	LIBS=-liphlpapi -lmpr -lmswsock -lws2_32 -ladvapi32 -lwinmm -lm
	LNK=$(TOOLPREFIX)dllwrap
	LNK_FLAGS=--def rocdigs.def
	SHAREDSUFFIX=.dll
	CC_EXTRA_FLAGS=
else ifeq ($(PLATFORM),MACOSX)
	LIBS=-lpthread -ldl -lm
	LIBUSB=-lusb-1.0
	DIRPREFIX=unx
	CC_EXTRA_FLAGS=-fPIC
	LNK_FLAGS=-dynamiclib -flat_namespace -undefined suppress -L/usr/local/lib
	LNK=gcc
	SHAREDSUFFIX=.dylib
else ifeq ($(PLATFORM),OPENBSD)
	LIBS=-pthread
	DIRPREFIX=unx
	CC_EXTRA_FLAGS=-fPIC
	LNK_FLAGS=-shared
	LNK=gcc
	SHAREDSUFFIX=.so
else
	LIBS=-lpthread -ldl -lm
	LIBUSB=-lusb-1.0
	DIRPREFIX=unx
	CC_EXTRA_FLAGS=-fPIC
	LNK_FLAGS=-shared
	LNK=gcc
	SHAREDSUFFIX=.so
endif

MOUNTPOINT=..
OUTDIR=..$(FS)$(DIRPREFIX)bin
GENDIR=..$(FS)$(DIRPREFIX)gen
TMPOUTDIR=$(GENDIR)$(FS)rocdigs$(FS)bin
DEBUG=-g
DESTDIR=$(FS)opt$(FS)rocrail
BINDIR=..$(FS)rocs$(FS)bin

CPP=$(TOOLPREFIX)gcc
INCL_PATH=$(MOUNTPOINT)
CC_FLAGS=-c $(CC_EXTRA_FLAGS) $(DEBUG) -I$(INCL_PATH) -I$(GENDIR)
DDL_CC_FLAGS=-c $(CC_EXTRA_FLAGS) $(DEBUG) -O2 -I$(INCL_PATH) -I$(GENDIR)
RRLIBS=$(OUTDIR)$(FS)libwrapper.a $(OUTDIR)$(FS)librocutils.a $(OUTDIR)$(FS)librocs.a

OBJS=$(patsubst impl/%.c,$(TMPOUTDIR)/%.o,$(wildcard impl/*.c))

DDXOBJS=$(patsubst impl/ddx/%.c,$(TMPOUTDIR)/ddx/%.o,$(wildcard impl/ddx/*.c))
LNOBJS=$(patsubst impl/loconet/%.c,$(TMPOUTDIR)/loconet/%.o,$(wildcard impl/loconet/*.c))
ECOSOBJS=$(patsubst impl/ecos/%.c,$(TMPOUTDIR)/ecos/%.o,$(wildcard impl/ecos/*.c))
COMMONOBJS=$(patsubst impl/common/%.c,$(TMPOUTDIR)/common/%.o,$(wildcard impl/common/*.c))
NMRAOBJS=$(patsubst impl/nmra/%.c,$(TMPOUTDIR)/nmra/%.o,$(wildcard impl/nmra/*.c))
ROCNETOBJS=$(patsubst impl/rocnet/%.c,$(TMPOUTDIR)/rocnet/%.o,$(wildcard impl/rocnet/*.c))
OPENDCCOBJS=$(patsubst impl/opendcc/%.c,$(TMPOUTDIR)/opendcc/%.o,$(wildcard impl/opendcc/*.c))
XPRESSNETOBJS=$(patsubst impl/xpressnet/%.c,$(TMPOUTDIR)/xpressnet/%.o,$(wildcard impl/xpressnet/*.c))
BIDIBOBJS=$(patsubst impl/bidib/%.c,$(TMPOUTDIR)/bidib/%.o,$(wildcard impl/bidib/*.c))
CBUSOBJS=$(patsubst impl/cbus/%.c,$(TMPOUTDIR)/cbus/%.o,$(wildcard impl/cbus/*.c))
SRCPOBJS=$(patsubst impl/srcp/%.c,$(TMPOUTDIR)/srcp/%.o,$(wildcard impl/srcp/*.c))
ZIMOCANOBJS=$(patsubst impl/zimocan/%.c,$(TMPOUTDIR)/zimocan/%.o,$(wildcard impl/zimocan/*.c))

all: $(OBJS) $(NMRAOBJS) $(COMMONOBJS) $(OUTDIR)$(FS)opendcc$(SHAREDSUFFIX) $(OUTDIR)$(FS)p50$(SHAREDSUFFIX) $(OUTDIR)$(FS)p50x$(SHAREDSUFFIX) $(OUTDIR)$(FS)hsi88$(SHAREDSUFFIX) $(OUTDIR)$(FS)srcp$(SHAREDSUFFIX) \
	$(OUTDIR)$(FS)roco$(SHAREDSUFFIX) $(OUTDIR)$(FS)zimo$(SHAREDSUFFIX) $(OUTDIR)$(FS)dinamo$(SHAREDSUFFIX) $(OUTDIR)$(FS)om32$(SHAREDSUFFIX) \
	$(OUTDIR)$(FS)ddx$(SHAREDSUFFIX) $(OUTDIR)$(FS)slx$(SHAREDSUFFIX) $(OUTDIR)$(FS)ecos$(SHAREDSUFFIX) $(OUTDIR)$(FS)rfid12$(SHAREDSUFFIX) $(OUTDIR)$(FS)rclink$(SHAREDSUFFIX) \
	$(OUTDIR)$(FS)barjut$(SHAREDSUFFIX) $(OUTDIR)$(FS)loconet$(SHAREDSUFFIX) $(OUTDIR)$(FS)zimobin$(SHAREDSUFFIX) $(OUTDIR)$(FS)xpressnet$(SHAREDSUFFIX) \
	$(OUTDIR)$(FS)virtual$(SHAREDSUFFIX) $(OUTDIR)$(FS)rocnet$(SHAREDSUFFIX) $(OUTDIR)$(FS)mcs2$(SHAREDSUFFIX) $(OUTDIR)$(FS)nce$(SHAREDSUFFIX) $(OUTDIR)$(FS)mttmfcc$(SHAREDSUFFIX) \
	$(OUTDIR)$(FS)clock$(SHAREDSUFFIX) $(OUTDIR)$(FS)lrc135$(SHAREDSUFFIX) $(OUTDIR)$(FS)sprog$(SHAREDSUFFIX) $(OUTDIR)$(FS)dcc232$(SHAREDSUFFIX) \
	$(OUTDIR)$(FS)rmx$(SHAREDSUFFIX) $(OUTDIR)$(FS)massoth$(SHAREDSUFFIX) $(OUTDIR)$(FS)tamsmc$(SHAREDSUFFIX) $(OUTDIR)$(FS)muet$(SHAREDSUFFIX) $(OUTDIR)$(FS)bidib$(SHAREDSUFFIX) \
	$(OUTDIR)$(FS)zs2$(SHAREDSUFFIX) $(OUTDIR)$(FS)cbus$(SHAREDSUFFIX) $(OUTDIR)$(FS)perir$(SHAREDSUFFIX) $(OUTDIR)$(FS)editspro$(SHAREDSUFFIX) \
	$(OUTDIR)$(FS)infracar$(SHAREDSUFFIX) $(OUTDIR)$(FS)cti$(SHAREDSUFFIX) $(OUTDIR)$(FS)easydcc$(SHAREDSUFFIX) $(OUTDIR)$(FS)z21$(SHAREDSUFFIX) \
	$(OUTDIR)$(FS)xnmm$(SHAREDSUFFIX) $(OUTDIR)$(FS)rocomp$(SHAREDSUFFIX) $(OUTDIR)$(FS)zimocan$(SHAREDSUFFIX) $(OUTDIR)$(FS)ucons88$(SHAREDSUFFIX) $(OUTDIR)$(FS)got$(SHAREDSUFFIX) \
	$(OUTDIR)$(FS)esunavi$(SHAREDSUFFIX) $(OUTDIR)$(FS)spl$(SHAREDSUFFIX) $(OUTDIR)$(FS)inter10$(SHAREDSUFFIX) $(OUTDIR)$(FS)mgbox$(SHAREDSUFFIX) \
	$(OUTDIR)$(FS)hue$(SHAREDSUFFIX) $(OUTDIR)$(FS)dccar$(SHAREDSUFFIX) $(OUTDIR)$(FS)dmxartnet$(SHAREDSUFFIX)

# ------------------------------------------------------------
# The Digital Interfaces as shared libraries.
# ------------------------------------------------------------
$(OUTDIR)$(FS)opendcc$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)opendcc.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)opendcc$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)opendcc.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)p50$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)p50.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)p50$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)p50.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)p50x$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)p50x.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)p50x$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)p50x.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)srcp$(SHAREDSUFFIX): $(SRCPOBJS) $(TMPOUTDIR)$(FS)srcp.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)srcp$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)srcp.o $(SRCPOBJS) $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)hsi88$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)hsi88.o $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)hsi88$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)hsi88.o $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)roco$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)roco.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)roco$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)roco.o $(COMMONOBJS) $(RRLIBS) $(LIBS)	
$(OUTDIR)$(FS)virtual$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)virtual.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)virtual$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)virtual.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)zimo$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)zimo.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)zimo$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)zimo.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)dinamo$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)dinamo.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)dinamo$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)dinamo.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)om32$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)om32.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)om32$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)om32.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)slx$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)slx.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)slx$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)slx.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)barjut$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)barjut.o $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)barjut$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)barjut.o $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)loconet$(SHAREDSUFFIX): $(LNOBJS) $(COMMONOBJS) $(TMPOUTDIR)$(FS)loconet.o $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)loconet$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)loconet.o $(LNOBJS) $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)ecos$(SHAREDSUFFIX): $(ECOSOBJS) $(COMMONOBJS) $(TMPOUTDIR)$(FS)ecos.o $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)ecos$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)ecos.o $(ECOSOBJS) $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)ddx$(SHAREDSUFFIX): $(DDXOBJS) $(COMMONOBJS) $(TMPOUTDIR)$(FS)ddx.o $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)ddx$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)ddx.o $(DDXOBJS) $(COMMONOBJS) $(RRLIBS) $(LIBS)	
$(OUTDIR)$(FS)rocnet$(SHAREDSUFFIX): $(ROCNETOBJS) $(COMMONOBJS) $(TMPOUTDIR)$(FS)rocnet.o $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)rocnet$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)rocnet.o $(ROCNETOBJS) $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)mcs2$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)mcs2.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)mcs2$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)mcs2.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)clock$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)clock.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)clock$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)clock.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)lrc135$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)lrc135.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)lrc135$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)lrc135.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)sprog$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)sprog.o $(COMMONOBJS) $(NMRAOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)sprog$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)sprog.o $(COMMONOBJS) $(NMRAOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)dcc232$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)dcc232.o $(COMMONOBJS) $(NMRAOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)dcc232$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)dcc232.o $(COMMONOBJS) $(NMRAOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)nce$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)nce.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)nce$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)nce.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)zimobin$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)zimobin.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)zimobin$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)zimobin.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)rfid12$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)rfid12.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)rfid12$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)rfid12.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)rclink$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)rclink.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)rclink$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)rclink.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)mttmfcc$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)mttmfcc.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)mttmfcc$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)mttmfcc.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)xpressnet$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)xpressnet.o $(COMMONOBJS) $(RRLIBS) $(XPRESSNETOBJS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)xpressnet$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)xpressnet.o $(XPRESSNETOBJS) $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)rmx$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)rmx.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)rmx$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)rmx.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)massoth$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)massoth.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)massoth$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)massoth.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)tamsmc$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)tamsmc.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)tamsmc$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)tamsmc.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)muet$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)muet.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)muet$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)muet.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)bidib$(SHAREDSUFFIX):  $(BIDIBOBJS) $(TMPOUTDIR)$(FS)bidib.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)bidib$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)bidib.o $(BIDIBOBJS) $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)zs2$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)zs2.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)zs2$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)zs2.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)cbus$(SHAREDSUFFIX): $(CBUSOBJS) $(TMPOUTDIR)$(FS)cbus.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)cbus$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)cbus.o $(CBUSOBJS) $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)perir$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)perir.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)perir$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)perir.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)editspro$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)editspro.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)editspro$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)editspro.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)infracar$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)infracar.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)infracar$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)infracar.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)cti$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)cti.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)cti$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)cti.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)easydcc$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)easydcc.o $(COMMONOBJS) $(NMRAOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)easydcc$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)easydcc.o $(COMMONOBJS) $(NMRAOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)z21$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)z21.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)z21$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)z21.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)xnmm$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)xnmm.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)xnmm$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)xnmm.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)rocomp$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)rocomp.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)rocomp$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)rocomp.o $(COMMONOBJS) $(RRLIBS) $(LIBS) $(LIBUSB)
$(OUTDIR)$(FS)zimocan$(SHAREDSUFFIX): $(ZIMOCANOBJS) $(TMPOUTDIR)$(FS)zimocan.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)zimocan$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)zimocan.o $(ZIMOCANOBJS) $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)ucons88$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)ucons88.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)ucons88$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)ucons88.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)got$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)got.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)got$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)got.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)esunavi$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)esunavi.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)esunavi$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)esunavi.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)spl$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)spl.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)spl$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)spl.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)inter10$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)inter10.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)inter10$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)inter10.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)mgbox$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)mgbox.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)mgbox$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)mgbox.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)hue$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)hue.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)hue$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)hue.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)dccar$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)dccar.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)dccar$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)dccar.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)$(FS)dmxartnet$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)dmxartnet.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)dmxartnet$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)dmxartnet.o $(COMMONOBJS) $(RRLIBS) $(LIBS)

# ------------------------------------------------------------
# The RocDigs objects.
# ------------------------------------------------------------
$(TMPOUTDIR)/%.o: impl/%.c
	$(CPP) $(CC_FLAGS) $< -o $@

$(TMPOUTDIR)/ddx/%.o: impl/ddx/%.c
	$(CPP) $(DDL_CC_FLAGS) $< -o $@

$(TMPOUTDIR)/loconet/%.o: impl/loconet/%.c
	$(CPP) $(CC_FLAGS) $< -o $@

$(TMPOUTDIR)/ecos/%.o: impl/ecos/%.c
	$(CPP) $(CC_FLAGS) $< -o $@

$(TMPOUTDIR)/common/%.o: impl/common/%.c
	$(CPP) $(CC_FLAGS) $< -o $@

$(TMPOUTDIR)/nmra/%.o: impl/nmra/%.c
	$(CPP) $(CC_FLAGS) $< -o $@

$(TMPOUTDIR)/rocnet/%.o: impl/rocnet/%.c
	$(CPP) $(CC_FLAGS) $< -o $@
	
$(TMPOUTDIR)/opendcc/%.o: impl/opendcc/%.c
	$(CPP) $(CC_FLAGS) $< -o $@

$(TMPOUTDIR)/xpressnet/%.o: impl/xpressnet/%.c
	$(CPP) $(CC_FLAGS) $< -o $@

$(TMPOUTDIR)/bidib/%.o: impl/bidib/%.c
	$(CPP) $(CC_FLAGS) $< -o $@
	
$(TMPOUTDIR)/srcp/%.o: impl/srcp/%.c
	$(CPP) $(CC_FLAGS) $< -o $@
	
$(TMPOUTDIR)/zimocan/%.o: impl/zimocan/%.c
	$(CPP) $(CC_FLAGS) $< -o $@
	
clean:
	-$(MKDIR) $(OUTDIR)
	-$(MKDIR) $(TMPOUTDIR)$(FS)ddx
	-$(MKDIR) $(TMPOUTDIR)$(FS)loconet
	-$(MKDIR) $(TMPOUTDIR)$(FS)rocnet
	-$(MKDIR) $(TMPOUTDIR)$(FS)ecos
	-$(MKDIR) $(TMPOUTDIR)$(FS)common
	-$(MKDIR) $(TMPOUTDIR)$(FS)nmra
	-$(MKDIR) $(TMPOUTDIR)$(FS)opendcc
	-$(MKDIR) $(TMPOUTDIR)$(FS)xpressnet
	-$(MKDIR) $(TMPOUTDIR)$(FS)bidib
	-$(MKDIR) $(TMPOUTDIR)$(FS)cbus
	-$(MKDIR) $(TMPOUTDIR)$(FS)srcp
	-$(MKDIR) $(TMPOUTDIR)$(FS)zimocan
	-$(MKDIR) $(GENDIR)$(FS)rocdigs
	-$(REMOVE) $(TMPOUTDIR)$(FS)*.o
	-$(REMOVE) $(TMPOUTDIR)$(FS)ddx$(FS)*.o
	-$(REMOVE) $(TMPOUTDIR)$(FS)loconet$(FS)*.o
	-$(REMOVE) $(TMPOUTDIR)$(FS)rocnet$(FS)*.o
	-$(REMOVE) $(TMPOUTDIR)$(FS)ecos$(FS)*.o
	-$(REMOVE) $(TMPOUTDIR)$(FS)common$(FS)*.o
	-$(REMOVE) $(TMPOUTDIR)$(FS)opendcc$(FS)*.o
	-$(REMOVE) $(TMPOUTDIR)$(FS)xpressnet$(FS)*.o
	-$(REMOVE) $(TMPOUTDIR)$(FS)bidib$(FS)*.o
	-$(REMOVE) $(TMPOUTDIR)$(FS)cbus$(FS)*.o
	-$(REMOVE) $(TMPOUTDIR)$(FS)nmra$(FS)*.o
	-$(REMOVE) $(TMPOUTDIR)$(FS)srcp$(FS)*.o
	-$(REMOVE) $(TMPOUTDIR)$(FS)zimocan$(FS)*.o
	-$(REMOVE) $(OUTDIR)$(FS)p50$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)p50x$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)srcp$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)hsi88$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)lenz$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)roco$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)zimo$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)dinamo$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)om32$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)ddx$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)slx$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)barjut$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)ecos$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)loconet$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)rocnet$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)virtual$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)mcs2$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)mgbox$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)clock$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)lrc135$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)sprog$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)dcc232$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)nce$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)zimobin$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)rfid12$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)rclink$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)mttmfcc$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)rmx$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)massoth$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)tamsmc$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)muet$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)bidib$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)zs2$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)cbus$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)opendcc$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)xpressnet$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)perir$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)editspro$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)infracar$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)cti$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)easydcc$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)z21$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)xnmm$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)rocomp$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)zimocan$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)ucons88$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)got$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)esunavi$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)spl$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)hue$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)inter10$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)dccar$(SHAREDSUFFIX)
	-$(REMOVE) $(OUTDIR)$(FS)dmxartnet$(SHAREDSUFFIX)
	cd $(GENDIR)$(FS)rocdigs$(CS) ..$(FS)$(BINDIR)$(FS)ogen$(GENSUFFIX) ..$(FS)..$(FS)rocdigs$(FS)rocdigs.xml ..$(FS)
	cd $(GENDIR)$(FS)rocdigs$(CS) ..$(FS)$(BINDIR)$(FS)xml2cstr$(GENSUFFIX) ..$(FS)..$(FS)common$(FS)version.xml impl$(FS)version.c svnLog

install:
	-cp -p $(OUTDIR)$(FS)p50$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)p50x$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)hsi88$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)srcp$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)roco$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)zimo$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)dinamo$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)om32$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)ddx$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)slx$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)barjut$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)ecos$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)loconet$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)rocnet$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)virtual$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)mcs2$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)mgbox$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)clock$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)lrc135$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)sprog$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)dcc232$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)nce$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)zimobin$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)rfid12$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)rclink$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)mttmfcc$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)rmx$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)massoth$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)muet$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)zs2$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)cbus$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)bidib$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)tamsmc$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)opendcc$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)xpressnet$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)perir$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)editspro$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)infracar$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)cti$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)easydcc$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)z21$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)xnmm$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)rocomp$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)zimocan$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)ucons88$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)got$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)esunavi$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)spl$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)hue$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)inter10$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)dccar$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)dmxartnet$(SHAREDSUFFIX) $(DESTDIR)
	
