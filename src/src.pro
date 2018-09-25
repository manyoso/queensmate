include($$PWD/../queensmate.pri)

QT += gui network svg xml

TEMPLATE = app
TARGET = queensmate

#FIXME the paths aren't properly quoted under windows...
!win32 {
    THEMES_FILE = .   # Need to give some bogus input
    themes.input = THEMES_FILE
    themes.output = themes.h
    themes.commands = $(COPY_DIR) $$TOPLEVELDIR/themes $$OUT_PWD/../
    themes.name = themes
    themes.CONFIG = target_predeps no_link
    QMAKE_EXTRA_COMPILERS += themes
}

SOURCES += \
    aboutdialog.cpp \
    application.cpp \
    bitboard.cpp \
    board.cpp \
    boardpiece.cpp \
    boardsquare.cpp \
    boardview.cpp \
    captured.cpp \
    changetheme.cpp \
    clock.cpp \
    configuredialog.cpp \
    dataloader.cpp \
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
    pgn.cpp \
    pgnlexer.cpp \
    pgnparser.cpp \
    player.cpp \
    resource.cpp \
    rules.cpp \
    scratchview.cpp \
    square.cpp \
    tableview.cpp \
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
    changetheme.h \
    chess.h \
    clock.h \
    configuredialog.h \
    dataloader.h \
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
    pgn.h \
    pgnlexer.h \
    pgnparser.h \
    player.h \
    resource.h \
    rules.h \
    scratchview.h \
    square.h \
    tableview.h \
    tabwidget.h \
    theme.h \
    uciengine.h

FORMS += \
    ui/aboutdialog.ui \
    ui/changetheme.ui \
    ui/configuredialog.ui \
    ui/gameview.ui \
    ui/mainwindow.ui \
    ui/newgamedialog.ui

RESOURCES += \
    resources/resources.qrc
