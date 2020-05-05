TEMPLATE = app

CONFIG += c++17

SOURCES += main.cpp \
           ai.cpp \
	   board.cpp \
    boardconfigview.cpp \
    boardview.cpp \
    guimain.cpp

HEADERS += board.h \
           ai.h \
           boardconfigview.h \
           boardview.h \
           guimain.h

QT += widgets core

FORMS += \
    boardconfigview.ui \
    mainwindow.ui

