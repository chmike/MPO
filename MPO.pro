TEMPLATE = app
CONFIG += console
CONFIG -= qt

//QMAKE_CXXFLAGS += -std=c++0x

SOURCES += main.cpp \
    Message.cpp \
    Link.cpp \
    Signal.cpp \
    Slot.cpp \
    Action.cpp

HEADERS += \
    Type.hpp \
    Message.hpp \
    Action.hpp \
    Link.hpp \
    Signal.hpp \
    Slot.hpp \
    MPO.hpp

OTHER_FILES += \
    .gitignore \
    Requirements.md \
    README.dox

