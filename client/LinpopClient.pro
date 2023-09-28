QT += core gui widgets network multimedia multimediawidgets websockets
RC_FILE += res.rc
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    addfriendswindow.cpp \
    chathistorywindow.cpp \
    chatwindow.cpp \
    clientsendthread.cpp \
    configwindow.cpp \
    creategroupwindow.cpp \
    doubleclickedlabel.cpp \
    fontselectorwindow.cpp \
    friendinformation.cpp \
    groupfilewindow.cpp \
    groupmemberwindow.cpp \
    indexwindow.cpp \
    loginwindow.cpp \
    main.cpp \
    mihoyolauncher.cpp \
    network.cpp \
    receivefilewindow.cpp \
    registrationwindow.cpp \
    scanner.cpp \
    serverrecvthread.cpp \
    serversendthread.cpp \
    transferfilewindow.cpp \
    video.cpp

HEADERS += \
    addfriendswindow.h \
    chathistorywindow.h \
    chatwindow.h \
    clientsendthread.h \
    configwindow.h \
    creategroupwindow.h \
    doubleclickedlabel.h \
    fontselectorwindow.h \
    friendinformation.h \
    groupfilewindow.h \
    groupmemberwindow.h \
    indexwindow.h \
    loginwindow.h \
    mihoyolauncher.h \
    network.h \
    receivefilewindow.h \
    registrationwindow.h \
    scanner.h \
    serverrecvthread.h \
    serversendthread.h \
    transferfilewindow.h \
    video.h

FORMS += \
    addfriendswindow.ui \
    chathistorywindow.ui \
    chatwindow.ui \
    configwindow.ui \
    creategroupwindow.ui \
    fontselectorwindow.ui \
    friendinformation.ui \
    groupfilewindow.ui \
    groupmemberwindow.ui \
    indexwindow.ui \
    loginwindow.ui \
    receivefilewindow.ui \
    registrationwindow.ui \
    transferfilewindow.ui \
    video.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    res.rc \
    uac.manifest

RESOURCES += \
    icon.qrc \
    video.qrc
