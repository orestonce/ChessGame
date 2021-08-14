#-------------------------------------------------
#
# Project created by QtCreator 2016-04-10T15:01:59
#
#-------------------------------------------------

QT       += core gui websockets
QT       += network sql
QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ChessClient
TEMPLATE = app


SOURCES += main.cpp\
        login.cpp \
	setserverdialog.cpp \
    registerdialog.cpp \
    database.cpp \
    gamepanel.cpp \
    gamecore.cpp \
    rpchub.cpp \
    CSProtocol.cpp

HEADERS  += login.h \
	setserverdialog.h \
    registerdialog.h \
    database.h \
    gamepanel.h \
    gamecore.h \
    rpchub.h \
    CSProtocol.h

FORMS    += login.ui \
    setserverdialog.ui \
    registerdialog.ui \
    gamepanel.ui

RESOURCES += \
    game.qrc \

RC_ICONS = images/icon.ico

DISTFILES +=
