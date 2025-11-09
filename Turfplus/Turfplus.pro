#-------------------------------------------------
#
# Project created by QtCreator 2019-03-30T16:43:14
#
#-------------------------------------------------

QT       += core gui

CONFIG   += c++11
QMAKE_CXXFLAGS += -std=c++11
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Turfplus
TEMPLATE = app


SOURCES += main.cpp\
		mainwindow.cpp \
	imodel.cpp \
    idelegate.cpp \
    itableview.cpp \
    historywindow.cpp \
    loadingdialog.cpp \
    duelswindow.cpp \
    duelwideget.cpp

HEADERS  += mainwindow.h \
	imodel.h \
    idelegate.h \
    itableview.h \
    historywindow.h \
    loadingdialog.h \
    duelswindow.h \
    duelwideget.h

FORMS    += mainwindow.ui \
    historywindow.ui \
    loadingdialog.ui \
    duelswindow.ui \
    duelwideget.ui
