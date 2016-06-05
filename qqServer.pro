#-------------------------------------------------
#
# Project created by QtCreator 2016-05-15T20:16:59
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qqServer
TEMPLATE = app


SOURCES += main.cpp \
    server_cpp/socketthread.cpp \
    server_cpp/tcpserver.cpp \
    model_cpp/SQL.cpp \
    server_cpp/dataformat.cpp \
    serverwidget.cpp

HEADERS  += \
    server_h/socketthread.h \
    server_h/tcpserver.h \
    model_h/SQL.h \
    model_h/sql_statement.h \
    server_h/loginobj.h \
    server_h/dataformat.h \
    serverwidget.h \
    model_h/datatype.h

FORMS    += widget.ui

RESOURCES += \
    image.qrc
