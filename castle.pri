!win32 {
    isEmpty(OUTPUT_DIR):OUTPUT_DIR=$$PWD/build
} else {
    isEmpty(OUTPUT_DIR):OUTPUT_DIR=$$PWD/build-windows
}

QT += core

CONFIG += silent

TOPLEVELDIR = $$PWD

PROJECT_SOURCE_DIR = $$TOPLEVELDIR/..

INCLUDEPATH += $$TOPLEVELDIR

LIBS += -L$$OUTPUT_DIR/lib

QMAKE_RPATHDIR += $$OUTPUT_DIR/lib
