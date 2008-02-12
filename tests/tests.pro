include($$PWD/../castle.pri)

CONFIG += qtestlib
TEMPLATE = app
OBJECTS_DIR = tmp
MOC_DIR = tmp
TARGET = runtests
DESTDIR = $$OUTPUT_DIR/bin

INCLUDEPATH += \
    tmp \
    $$TOPLEVELDIR/src \
    $$OUTPUT_DIR/src/tmp

SOURCES += \
    main.cpp \
    testobject.cpp \

HEADERS += \
    testobject.h \
