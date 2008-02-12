include($$PWD/../castle.pri)

QT += gui network svg xml

TEMPLATE = app
OBJECTS_DIR = tmp
MOC_DIR = tmp
TARGET = 960fen
DESTDIR = $$OUTPUT_DIR/bin

INCLUDEPATH += \
    tmp \
    $$OUTPUT_DIR/src/tmp \
    $$TOPLEVELDIR/src

SOURCES += \
    main.cpp
