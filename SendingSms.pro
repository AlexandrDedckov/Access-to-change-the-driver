#-------------------------------------------------
#
# Project created by QtCreator 2015-01-16T09:46:52
#
#-------------------------------------------------

QT += core network
QT -= gui
#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = sendingSMS
TEMPLATE = app
CONFIG += console
CONFIG   += c++11

LIBS += -lz

SOURCES += main.cpp\
    mainwindow.cpp \
    model/car.cpp \
    model/driver.cpp \
    model/month.cpp \
    model/drivers.cpp \
    model/cars.cpp \
    model/request/request.cpp \
    model/request/queuerequests.cpp \
    model/autoparks.cpp \
    model/request/crypto.cpp \
    model/request/obtainingserver.cpp \
    model/request/qcompressor.cpp \
    model/request/serverconfig.cpp \
    model/request/signalingupdate.cpp \
    delivery.cpp \
    model/request/requestautomaticredial.cpp

HEADERS  += mainwindow.h \
    model/car.h \
    txt_constants.h \
    model/drivers.h \
    model/irequest.h \
    model/icollectionrequest.h \
    model/cars.h \
    model/request/request.h \
    model/request/queuerequests.h \
    model/autoparks.h \
    model/request/crypto.h \
    model/request/obtainingserver.h \
    model/request/qcompressor.h \
    model/request/serverconfig.h \
    model/request/md5.h \
    model/request/jsonparser.h \
    model/request/signalingupdate.h \
    delivery.h \
    model/request/requestautomaticredial.h

RESOURCES += \
    schedulingDrivers.qrc

RC_ICONS = icon.ico

#QMAKE_LFLAGS=-static -static-libgcc
