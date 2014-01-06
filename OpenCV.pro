# -------------------------------------------------
# Project created by QtCreator 2009-12-15T23:07:40
# -------------------------------------------------
TARGET = OpenCV
TEMPLATE = app

# OpenCv Configuration
INCLUDEPATH += /usr/local/include/opencv
LIBS += -L/usr/local/lib
LIBS += -lopencv_core
LIBS += -lopencv_imgproc
LIBS += -lopencv_highgui
LIBS += -lopencv_ml
LIBS += -lopencv_video
LIBS += -lopencv_features2d
LIBS += -lopencv_calib3d
LIBS += -lopencv_objdetect
LIBS += -lopencv_contrib
LIBS += -lopencv_legacy
LIBS += -lopencv_flann
LIBS += -lopencv_nonfree
LIBS += `pkg-config opencv --libs`

# LIBS += "C:\OpenCV2.0\lib\libcvaux200.dll.a"
SOURCES += main.cpp \
    camerawindow.cpp \
    opencvwidget.cpp \
    camshift.cpp \
    camshiftdialog.cpp \
    facedetect.cpp \
    recognition.cpp
HEADERS += camerawindow.h \
    opencvwidget.h \
    camshift.h \
    version.h \
    camshiftdialog.h \
    facedetect.h \
    recognition.h
RESOURCES += resources.qrc
FORMS += camshiftdialog.ui

OTHER_FILES += \
    haarcascades/haarcascade_frontalface_alt2.xml
