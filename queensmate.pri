QT += core

TOPLEVELDIR = $$PWD

PROJECT_SOURCE_DIR = $$TOPLEVELDIR/..

INCLUDEPATH += $$TOPLEVELDIR

LIBS += -L$$OUTPUT_DIR/lib

QMAKE_RPATHDIR += $$OUTPUT_DIR/lib
