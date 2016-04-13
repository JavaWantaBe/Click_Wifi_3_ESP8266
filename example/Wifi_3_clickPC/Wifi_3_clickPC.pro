TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++0x -pthread
LIBS += -pthread

INCLUDEPATH += \
            ../../library/include

SOURCES += main.c \
    ../../library/src/buffer.c \
    ../../library/src/esp8266.c \
    ../../library/src/esp8266_ll.c

HEADERS += \
    ../../library/include/buffer.h \
    ../../library/include/esp8266.h \
    ../../library/include/esp8266_ll.h \
    ../../library/include/esp8266_conf.h
