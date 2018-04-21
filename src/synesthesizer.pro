HEADERS       = adsr.h \
                aseqdump.h \
                channel.h \
                k8062.h \
                synesthesizer.h \

SOURCES       = main.cpp \
                adsr.cpp \
                aseqdump.c \
                channel.cpp \
                k8062.cpp \
                synesthesizer.cpp \

LIBS += -lm -lasound
