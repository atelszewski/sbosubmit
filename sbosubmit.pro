# qmake project file for sbosubmit.
# Author: Andrzej Telszewski <atelszewski@gmail.com>

isEmpty(PREFIX) { PREFIX = /usr/local   }
isEmpty(MANDIR) { MANDIR = $$PREFIX/man }
isEmpty(DOCDIR) { DOCDIR = $$PREFIX/doc/sbosubmit }

man1.files = man/*.1
man1.path  = $$MANDIR/man1
doc.files  = doc/*
doc.path   = $$DOCDIR

target.path = $$PREFIX/bin
INSTALLS = target man1 doc

TARGET = sbosubmit
TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG -= declarative_debug
CONFIG += warn_off

DESTDIR = build
OBJECTS_DIR = build

# We'll strip on our own.
QMAKE_STRIP = /bin/true

QMAKE_LFLAGS           = $$(LDFLAGS)
QMAKE_LFLAGS_DEBUG     =
QMAKE_LFLAGS_RELEASE   =
QMAKE_CFLAGS           = -pipe -Wall -std=c11 $$(CFLAGS)
QMAKE_CFLAGS_DEBUG     =
QMAKE_CFLAGS_RELEASE   =
QMAKE_CXXFLAGS         = -pipe -Wall -std=c++11 $$(CXXFLAGS)
QMAKE_CXXFLAGS_DEBUG   =
QMAKE_CXXFLAGS_RELEASE =

LIBS = -larchive -lcurl
INCLUDEPATH = src
SOURCES = $$files(src/*.cpp)
HEADERS = $$files(src/*.h)
