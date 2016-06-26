#-------------------------------------------------
#
# Project created by QtCreator 2016-04-10T15:01:59
#
#-------------------------------------------------

QT       += core gui
QT       += network
QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ChessClient
TEMPLATE = app


SOURCES += main.cpp\
        login.cpp \
    setserver.cpp \
    register.cpp \
    playgame.cpp \
    Protol/CSProtol.cpp

HEADERS  += login.h \
    setserver.h \
    register.h \
    playgame.h \
    Protol/CSProtol.h \
    RoTypes.h \
    Protol/CSProtol.Serialize.hpp \
    Protol/Common.h \
    Protol/CSProtol.Parser.hpp \
    Protol/ServerInternal.h \
    RoSpace/RoSingleton.h \
    RoSpace/RoThreadPool.h \
    RoSpace/RoTools.h \
    RoSpace/RoTypes.h

FORMS    += login.ui \
    setserver.ui \
    register.ui \
    playgame.ui

RESOURCES += \
    game.qrc

DISTFILES +=
