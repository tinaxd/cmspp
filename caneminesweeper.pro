TEMPLATE = app

CONFIG += c++17

SOURCES += main.cpp \
           ai.cpp \
	   board.cpp \
    boardconfigview.cpp \
    boardgenerationprogress.cpp \
    boardview.cpp \
    guimain.cpp

HEADERS += board.h \
           ai.h \
           boardconfigview.h \
           boardgenerationprogress.h \
           boardview.h \
           guimain.h

QT += widgets core

FORMS += \
    boardconfigview.ui \
    boardgenerationprogress.ui \
    mainwindow.ui

