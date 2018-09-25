TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
TARGET = da_proc

SOURCES += \
        main.cpp \
    membership.cpp \
    udpsender.cpp \
    udpreceiver.cpp \
    perfectsender.cpp \
    sender.cpp \
    receiver.cpp \
    common.cpp \
    perfectreceiver.cpp

HEADERS += \
    membership.h \
    udpsender.h \
    udpreceiver.h \
    perfectsender.h \
    sender.h \
    receiver.h \
    common.h \
    perfectreceiver.h
