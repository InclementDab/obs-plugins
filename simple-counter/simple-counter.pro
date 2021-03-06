# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Tools.
# ------------------------------------------------------

TEMPLATE = lib
TARGET = simple-counter
DESTDIR = ./build/Debug
QT += core xml svg widgets
CONFIG += debug
DEFINES += BASICTEXTVARIABLES_EXPORTS _WINDOWS _USRDLL
INCLUDEPATH += $(JSON11) \
    $(OBS) \
    $(OBS)/libobs \
    $(OBS)/build/UI \
    $(OBS)/UI/obs-frontend-api \
    $(OBS)/plugins/obs-browser/panel \
    $(CURL) \
    $(CURLPP) \
    $(SPDLOG) \
    ./source \
    ./source/window \
    ./source/ui \
    ./source/ui/forms
DEPENDPATH += .
MOC_DIR += $(ProjectDir)/source/forms/moc
OBJECTS_DIR += debug
UI_DIR += $(ProjectDir)/source/forms
RCC_DIR += $(ProjectDir)/source/forms
include(simple-counter.pri)
