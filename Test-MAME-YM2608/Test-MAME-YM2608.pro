#-------------------------------------------------
#
# Project created by QtCreator 2018-03-12T21:11:20
#
#-------------------------------------------------

QT       = core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = YM2608ToneEditor
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


CONFIG += c++17


SOURCES += \
    chips/nuked/nuke2608intf.c \
    chips/nuked/ym3438.c \
    fmenvelopeformatdialog.cpp \
    fmenvelopeorderdialog.cpp \
    io/binary_container.cpp \
    io/btb_io.cpp \
    io/bti_io.cpp \
    io/ff_io.cpp \
    io/file_io.cpp \
    io/file_io_error.cpp \
    io/original_bank_io.cpp \
    io/original_tone_io.cpp \
        main.cpp \
        mainwindow.cpp \
    chips/opna.cpp \
    chips/mame/2608intf.c \
    chips/mame/emu2149.c \
    chips/mame/fm.c \
    chips/mame/ymdeltat.c \
    audio_stream.cpp \
    audio_stream_mixier.cpp \
    widgets/labeled_hslider.cpp \
    widgets/labeled_vslider.cpp \
    widgets/operator_sliders.cpp \
    chips/chip.cpp \
    chips/resampler.cpp \
    namedialog.cpp \
    tone_converter.cpp \
    tonetextdialog.cpp \
    setupdialog.cpp \
    settings.cpp \
    aboutdialog.cpp \
    sliderstyle.cpp \
    readtextdialog.cpp \
    widgets/ssgeg_slider.cpp

HEADERS += \
    chips/chip_def.h \
    chips/nuked/nuke2608intf.h \
    chips/nuked/ym3438.h \
    fmenvelopeformatdialog.hpp \
    fmenvelopeorderdialog.hpp \
    io/binary_container.hpp \
    io/btb_io.hpp \
    io/bti_io.hpp \
    io/ff_io.hpp \
    io/file_io.hpp \
    io/file_io_error.hpp \
    io/original_bank_io.hpp \
    io/original_tone_io.hpp \
        mainwindow.h \
    chips/mame/2608intf.h \
    chips/mame/emu2149.h \
    chips/mame/emutypes.h \
    chips/mame/fm.h \
    chips/mame/mamedef.h \
    chips/mame/ymdeltat.h \
    chips/opna.hpp \
    text_conversion.hpp \
    types.h \
    audio_stream.hpp \
    audio_stream_mixier.hpp \
    widgets/labeled_hslider.hpp \
    widgets/labeled_vslider.h \
    widgets/operator_sliders.h \
    parameter_state.hpp \
    chips/chip.hpp \
    chips/chip_misc.h \
    chips/resampler.hpp \
    tone.hpp \
    namedialog.h \
    tone_converter.hpp \
    tonetextdialog.h \
    setupdialog.h \
    settings.hpp \
    aboutdialog.h \
    sliderstyle.hpp \
    readtextdialog.hpp \
    widgets/ssgeg_slider.hpp

FORMS += \
    fmenvelopeformatdialog.ui \
    fmenvelopeorderdialog.ui \
        mainwindow.ui \
    widgets/labeled_hslider.ui \
    widgets/labeled_vslider.ui \
    widgets/operator_sliders.ui \
    namedialog.ui \
    tonetextdialog.ui \
    setupdialog.ui \
    aboutdialog.ui \
    readtextdialog.ui \
    widgets/ssgeg_slider.ui
