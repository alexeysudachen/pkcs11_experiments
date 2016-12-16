
TOPDIR  =.
_INCLUDE = $(XTERNAL)\include;$(INCLUDE)
EXECUTABLE = pkcs11

CFLAGS=-DUSE_OPENSSL

!include $(XTERNAL)\Make.rules.mak

SRCDIR  = .
OBJECTS = \
    $(OBJDIR)\main.obj \

LIBRARIES=libeay$(LIBSFX).lib

!include $(XTERNAL)\Make.exe.mak



