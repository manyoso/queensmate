include($$PWD/../queensmate.pri)

QT += gui network svg xml

TEMPLATE = app
TARGET = 960fen

INCLUDEPATH += \
    $$TOPLEVELDIR/src

SOURCES += \
    main.cpp
