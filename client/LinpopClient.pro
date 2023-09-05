QT       += core gui
QT       += network
QT += multimedia
QT += multimediawidgets
RC_FILE += res.rc
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

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
    autostart.cpp \
    chathistorywindow.cpp \
    chatrecord.cpp \
    chatrecordlist.cpp \
    chatwindow.cpp \
    configwindow.cpp \
    creategroupwindow.cpp \
    doubleclickedlabel.cpp \
    friend.cpp \
    friendinformation.cpp \
    friendlybufferhelper.cpp \
    indexwindow.cpp \
    loginwindow.cpp \
    main.cpp \
    mihoyolauncher.cpp \
    modifynicknamewindow.cpp \
    receivefilewindow.cpp \
    recvthread.cpp \
    registrationwindow.cpp \
    requesttoserver.cpp \
    scanner.cpp \
    sendthread.cpp \
    transferfilewindow.cpp \
    user.cpp \
    video.cpp

HEADERS += \
    addfriendswindow.h \
    autostart.h \
    chathistorywindow.h \
    chatrecord.h \
    chatrecordlist.h \
    chatwindow.h \
    configwindow.h \
    creategroupwindow.h \
    doubleclickedlabel.h \
    friend.h \
    friendinformation.h \
    friendlybufferhelper.h \
    indexwindow.h \
    loginwindow.h \
    mihoyolauncher.h \
    modifynicknamewindow.h \
    receivefilewindow.h \
    recvthread.h \
    registrationwindow.h \
    requesttoserver.h \
    scanner.h \
    sendthread.h \
    transferfilewindow.h \
    user.h \
    video.h

FORMS += \
    addfriendswindow.ui \
    chathistorywindow.ui \
    chatwindow.ui \
    configwindow.ui \
    creategroupwindow.ui \
    friendinformation.ui \
    indexwindow.ui \
    loginwindow.ui \
    modifynicknamewindow.ui \
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
    picture.qrc \
    video.qrc
