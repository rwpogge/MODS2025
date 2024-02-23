TEMPLATE = app
TARGET = imsTool
MOC_DIR = moc
OBJECTS_DIR = obj
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += client.h
SOURCES += client.cpp main.cpp
RESOURCES += ims.qrc

QT += network
QT += widgets