#-------------------------------------------------
#
# Project created by QtCreator 2018-03-12T21:11:20
#
#-------------------------------------------------

QT       = core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

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

win32:target.path = C:/YM2608-Tone-Editor
else:target.path = /usr/local/bin
INSTALLS += target


CONFIG += c++17


SOURCES += \
    chips/emu2149/emu2149.c \
    chips/mame/fmopn.c \
    chips/mame/mame_2608.cpp \
    chips/mame/ymdeltat.c \
    chips/nuked/nuked_2608.cpp \
    chips/nuked/ym3438.c \
    fmenvelopeformatdialog.cpp \
    fmenvelopeorderdialog.cpp \
    io/abstract_file_handler.cpp \
    io/binary_container.cpp \
    io/bnk_io.cpp \
    io/btb_io.cpp \
    io/bti_io.cpp \
    io/dmp_io.cpp \
    io/ff_io.cpp \
    io/file_io.cpp \
    io/file_io_error.cpp \
    io/fmp_io.cpp \
    io/fui_io.cpp \
    io/gyb_io.cpp \
    io/ins_io.cpp \
    io/mucom88_io.cpp \
    io/original_bank_io.cpp \
    io/original_tone_io.cpp \
    io/pmd_io.cpp \
    io/register_recorder.cpp \
    io/s98_io.cpp \
    io/tfi_io.cpp \
    io/vgi_io.cpp \
    io/vgm_io.cpp \
    io/wopn/opni_io.cpp \
    io/wopn/wopn_file.c \
    io/wopn/wopn_io.cpp \
    io/y12_io.cpp \
    main.cpp \
    mainwindow.cpp \
    chips/opna.cpp \
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
    chips/2608_interface.hpp \
    chips/chip_def.h \
    chips/emu2149/emu2149.h \
    chips/mame/fmopn.h \
    chips/mame/fmopn_2608rom.h \
    chips/mame/mame_2608.hpp \
    chips/mame/mamedefs.h \
    chips/mame/ymdeltat.h \
    chips/nuked/nuked_2608.hpp \
    chips/nuked/ym3438.h \
    fmenvelopeformatdialog.hpp \
    fmenvelopeorderdialog.hpp \
    io/abstract_file_handler.hpp \
    io/binary_container.hpp \
    io/bnk_io.hpp \
    io/btb_io.hpp \
    io/bti_io.hpp \
    io/dmp_io.hpp \
    io/ff_io.hpp \
    io/file_io.hpp \
    io/file_io_error.hpp \
    io/fmp_io.hpp \
    io/fui_io.hpp \
    io/gyb_io.hpp \
    io/ins_io.hpp \
    io/io_helper.hpp \
    io/mucom88_io.hpp \
    io/original_bank_io.hpp \
    io/original_tone_io.hpp \
    io/pmd_io.hpp \
    io/register_recorder.hpp \
    io/s98_io.hpp \
    io/tfi_io.hpp \
    io/vgi_io.hpp \
    io/vgm_io.hpp \
    io/wopn/opni_io.hpp \
    io/wopn/wopn_common.hpp \
    io/wopn/wopn_file.h \
    io/wopn/wopn_io.hpp \
    io/y12_io.hpp \
    mainwindow.h \
    chips/opna.hpp \
    text_conversion.hpp \
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
