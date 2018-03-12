#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "types.h"
#include "chips/opna.hpp"
#include "audio_stream.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private:
    Ui::MainWindow *ui;
    chip::OPNA chip_;
    AudioStream audio_;

    struct Operator {
        uint8 AR;
        uint8 DR;
        uint8 SR;
        uint8 RR;
        uint8 SL;
        uint8 TL;
        uint8 KS;
        uint8 ML;
        uint8 DT;
        uint8 AM;
    };

    struct Voice {
        std::string name;
        uint8 AL;
        uint8 FB;
        Operator op[4];
    };

    Voice voice_;

    int octaveFM_, octavePSG_;
    bool jamKeyOnTableFM_[17], jamKeyOnTablePSG_[17];

    void SetFMVoice(struct Voice voice, int channel);
    void SetFMKeyOn(int channel);
    void SetFMKeyOff(int channel);
    void SetFMTone(int channel, uint32 octave, uint32 keynum);
    void SetPSGTone(int channel, uint32 octave, uint32 keynum);
    void InitPan();

    void JamKeyOnFM(uint32 jamKeyNumber, bool isRepeat);
    void JamKeyOnPSG(uint32 jamKeyNumber, bool isRepeat);
    void JamKeyOffFM(uint32 jamKeyNumber, bool isRepeat);
    void JamKeyOffPSG(uint32 jamKeyNumber, bool isRepeat);

    const uint32 fm_tune_tbl[12] = {
        0x026a,	// C
        0x028f,	// C#
        0x02b6,	// D
        0x02df,	// D#
        0x030b,	// E
        0x0339,	// F
        0x036a,	// F#
        0x039e,	// G
        0x03d5,	// G#
        0x0410,	// A
        0x044e,	// A#
        0x048f	// B
    };

    const int psg_tune_tbl[12] = {
        0x0ee8,	// C
        0x0e12,	// C#
        0x0d48,	// D
        0x0c89,	// D#
        0x0bd5,	// E
        0x0b2b,	// F
        0x0a8a,	// F#
        0x09f3,	// G
        0x0964,	// G#
        0x08dd,	// A
        0x085e,	// A#
        0x07e6,	// B
    };
};

#endif // MAINWINDOW_H
