QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    authorization/database.cpp \
    chat.cpp \
    pathOperations/pathdialog.cpp \
    user.cpp \
    authorization/authorization.cpp \
    main.cpp \
    mainwindow.cpp \
    stylesheet.cpp

HEADERS += \
    authorization/database.h \
    chat.h \
    pathOperations/pathdialog.h \
    user.h \
    authorization/authorization.h \
    mainwindow.h \
    stylesheet.h

FORMS += \
    authorization/authorization.ui \
    mainwindow.ui \
    pathOperations/pathdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons.qrc
