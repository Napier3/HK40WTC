QT       += core gui

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

INCLUDEPATH += $$PWD/SoftKeyboard

SOURCES += \
    SoftKeyboard/Keyboard/Keyboard.cpp \
    SoftKeyboard/Keyboard/googlepinyin.cpp \
    SoftKeyboard/Keyboard/googlepinyin/dictbuilder.cpp \
    SoftKeyboard/Keyboard/googlepinyin/dictlist.cpp \
    SoftKeyboard/Keyboard/googlepinyin/dicttrie.cpp \
    SoftKeyboard/Keyboard/googlepinyin/lpicache.cpp \
    SoftKeyboard/Keyboard/googlepinyin/matrixsearch.cpp \
    SoftKeyboard/Keyboard/googlepinyin/mystdlib.cpp \
    SoftKeyboard/Keyboard/googlepinyin/ngram.cpp \
    SoftKeyboard/Keyboard/googlepinyin/pinyinime.cpp \
    SoftKeyboard/Keyboard/googlepinyin/searchutility.cpp \
    SoftKeyboard/Keyboard/googlepinyin/spellingtable.cpp \
    SoftKeyboard/Keyboard/googlepinyin/spellingtrie.cpp \
    SoftKeyboard/Keyboard/googlepinyin/splparser.cpp \
    SoftKeyboard/Keyboard/googlepinyin/sync.cpp \
    SoftKeyboard/Keyboard/googlepinyin/userdict.cpp \
    SoftKeyboard/Keyboard/googlepinyin/utf16char.cpp \
    SoftKeyboard/Keyboard/googlepinyin/utf16reader.cpp \
    SoftKeyboard/SoftKeyBoard.cpp \
    cttest.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    SoftKeyboard/Keyboard/Keyboard.h \
    SoftKeyboard/Keyboard/googlepinyin.h \
    SoftKeyboard/SoftKeyBoard.h \
    SoftKeyboard/Keyboard/googlepinyin/atomdictbase.h \
    SoftKeyboard/Keyboard/googlepinyin/dictbuilder.h \
    SoftKeyboard/Keyboard/googlepinyin/dictdef.h \
    SoftKeyboard/Keyboard/googlepinyin/dictlist.h \
    SoftKeyboard/Keyboard/googlepinyin/dicttrie.h \
    SoftKeyboard/Keyboard/googlepinyin/lpicache.h \
    SoftKeyboard/Keyboard/googlepinyin/matrixsearch.h \
    SoftKeyboard/Keyboard/googlepinyin/mystdlib.h \
    SoftKeyboard/Keyboard/googlepinyin/ngram.h \
    SoftKeyboard/Keyboard/googlepinyin/pinyinime.h \
    SoftKeyboard/Keyboard/googlepinyin/searchutility.h \
    SoftKeyboard/Keyboard/googlepinyin/spellingtable.h \
    SoftKeyboard/Keyboard/googlepinyin/spellingtrie.h \
    SoftKeyboard/Keyboard/googlepinyin/splparser.h \
    SoftKeyboard/Keyboard/googlepinyin/sync.h \
    SoftKeyboard/Keyboard/googlepinyin/userdict.h \
    SoftKeyboard/Keyboard/googlepinyin/utf16char.h \
    SoftKeyboard/Keyboard/googlepinyin/utf16reader.h \
    cttest.h \
    mainwindow.h

FORMS += \
    SoftKeyboard/Keyboard/Keyboard.ui \
    cttest.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
