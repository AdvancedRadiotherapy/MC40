# --------------------------------------------------------------
# GNUmakefile beamline attempt 1 tprice 21/03/13
# --------------------------------------------------------------

name := bhamBeamline
G4TARGET := $(name)
G4EXLIB := true
G4VIS_USE := true

ifndef G4INSTALL
  G4INSTALL = ../../..
endif

#for ROOT
ROOTCONFIG := root-config
EXTRALIBS += $(shell ${ROOTCONFIG} --libs)
CPPFLAGS += $(shell ${ROOTCONFIG} --cflags)

CPPFLAGS += -fPIC

.PHONY: all
all: lib bin

include $(G4INSTALL)/config/binmake.gmk

visclean:
	rm -f g4*.prim g4*.eps g4*.wrl
	rm -f .DAWN_*
