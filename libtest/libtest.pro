TEMPLATE = app
LIBS += -L/home/genius/workspace/proj/libtest/libs/target/ \
         -ladd \
         -lsub \
         -lhello
SOURCES += \
    source/main.c
TARGET = libtest
