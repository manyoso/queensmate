include($$PWD/../castle.pri)

QT += gui network svg xml

TEMPLATE = app
OBJECTS_DIR = tmp
MOC_DIR = tmp
TARGET = castle 
DESTDIR = $$OUTPUT_DIR/bin


INCLUDEPATH += \
    tmp \
    $$OUTPUT_DIR/src/tmp

SOURCES += \
    aboutdialog.cpp \
    application.cpp \
    bitboard.cpp \
    board.cpp \
    boardpiece.cpp \
    boardsquare.cpp \
    boardview.cpp \
    captured.cpp \
    clock.cpp \
    configuredialog.cpp \
    engine.cpp \
    game.cpp \
    gameview.cpp \
    inlinetableview.cpp \
    main.cpp \
    mainwindow.cpp \
    move.cpp \
    movesmodel.cpp \
    newgamedialog.cpp \
    notation.cpp \
    piece.cpp \
    player.cpp \
    resource.cpp \
    rules.cpp \
    square.cpp \
    tabwidget.cpp \
    theme.cpp \
    uciengine.cpp

HEADERS += \
    aboutdialog.h \
    application.h \
    bitboard.h \
    board.h \
    boardpiece.h \
    boardsquare.h \
    boardview.h \
    captured.h \
    chess.h \
    clock.h \
    configuredialog.h \
    engine.h \
    game.h \
    gameview.h \
    inlinetableview.h \
    mainwindow.h \
    move.h \
    movesmodel.h \
    newgamedialog.h \
    notation.h \
    piece.h \
    player.h \
    resource.h \
    rules.h \
    square.h \
    tabwidget.h \
    theme.h \
    uciengine.h

FORMS += \
    ui/aboutdialog.ui \
    ui/configuredialog.ui \
    ui/gameview.ui \
    ui/mainwindow.ui \
    ui/newgamedialog.ui

RESOURCES += \
    resources/resources.qrc
