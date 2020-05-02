TEMPLATE = app

CONFIG += c++17

SOURCES += main.cpp \
           ai.cpp \
	   board.cpp \
    boardview.cpp \
    guimain.cpp

HEADERS += board.h \
           ai.h \
           boardview.h \
           guimain.h

QT += widgets core

