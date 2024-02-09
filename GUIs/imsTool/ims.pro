TEMPLATE = app
TARGET = imsTool
MOC_DIR = moc
OBJECTS_DIR = obj
HEADERS       = client.h
SOURCES       = client.cpp \
                main.cpp
RESOURCES     = ims.qrc
QT           += network
QT           += widgets
CONFIG       += warn_off

# install
target.path = [$$INSTALL_IMS_HOME]
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS ims.pro images
sources.path = [$$INSTALL_IMS_HOME]
INSTALLS += target sources
