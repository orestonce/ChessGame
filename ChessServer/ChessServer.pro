TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11
QMAKE_LFLAGS += -levent -lmysqlclient -L/usr/lib64/mysql/ -lpthread

SOURCES += main.cpp \
    RoNet/RoNet.cpp \
    RoNet/private/RoNetMgr.cpp \
    RoNet/private/RoNetUser.cpp \
    SrvUser/SrvUser.cpp \
    SrvUser/SrvUserMgr.cpp \
    ChessServer.cpp \
    SrvConfig/CSrvConfig.cpp \
    SrvConfig/inifile/inifile.cpp \
    Protol/CSProtol.cpp \
    DBThread/DBThreadMgr.cpp \
    DBThread/DBThread.cpp \
    DBThread/Mysql/mysqlquery.cpp \
    EventBuffer/EventBuffer.cpp \
    EventBuffer/EventBufferRecycle.cpp \
    SrvUser/SrvUser.Event.cpp \
    DBThread/DBThread.Event.cpp \
    ChessGame/Chess/Piece.cpp \
    ChessGame/Chess/Piece.Rules.cpp \
    ChessGame/Chess/Point.cpp \
    ChessGame/ChessGame.cpp \
    ChessGame/ChessGame.Net.cpp

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    RoSpace/RoTypes.h \
    RoNet/RoNet.h \
    RoNet/RoNetTypes.h \
    RoSpace/RoSingleton.h \
    RoNet/private/RoNetMgr.h \
    RoLog/RoLog.h \
    RoSpace/RoTools.h \
    RoNet/private/RoNetUser.h \
    SrvUser/SrvUser.h \
    SrvUser/SrvUserMgr.h \
    ChessServer.h \
    SrvConfig/CSrvConfig.h \
    SrvConfig/inifile/inifile.h \
    Protol/CSProtol.h \
    DBThread/DBThreadMgr.h \
    DBThread/DBThread.h \
    RoSpace/RoThreadPool.h \
    DBThread/Mysql/mysqlquery.h \
    EventBuffer/EventBuffer.h \
    EventBuffer/EventBufferRecycle.h \
    Protol/CSProtol.Serialize.hpp \
    Protol/CSProtol.Parser.hpp \
    Protol/Common.h \
    Protol/CSProtol.SendData.hpp \
    Protol/ServerInternal.h \
    ChessGame/Chess/Piece.h \
    ChessGame/Chess/Point.h \
    ChessGame/ChessGame.h

