#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QCloseEvent>
#include <memory>
#include "types.h"
#include "chips/opna.hpp"
#include "audio_stream.hpp"
#include "parameter_state.hpp"
#include "tone.hpp"
#include "widgets/operator_sliders.h"

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

    std::unique_ptr<Tone> tone_;

    int octaveFM_, octavePSG_;
    bool jamKeyOnTableFM_[17], jamKeyOnTablePSG_[17];
    QString pressedKeyNameFM_, pressedKeyNamePSG_;

    OperatorSliders* sliders_[4];
    bool isEdit_;

    void SetFMTone(int channel);
    void SetFMKeyOn(int channel);
    void SetFMKeyOff(int channel);
    void SetFMKey(int channel, uint32 octave, uint32 keynum);
    void SetPSGKey(int channel, uint32 octave, uint32 keynum);
    void InitPan();

    void JamKeyOnFM(uint32 jamKeyNumber, bool isRepeat);
    void JamKeyOnPSG(uint32 jamKeyNumber, bool isRepeat);
    void JamKeyOffFM(uint32 jamKeyNumber, bool isRepeat);
    void JamKeyOffPSG(uint32 jamKeyNumber, bool isRepeat);

    QString keyNumberToNameString(uint32 jamKeyNumber);

    void closeEvent(QCloseEvent* event) override;

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

private slots:
    void onALChanged(int value);
    void onFBChanged(int value);
    void onParameterChanged(int op, const ParameterState& state);
    void on_actionOpen_O_triggered();
    void on_actionSave_S_triggered();
    void on_actionExit_E_triggered();
    void on_actionSave_As_triggered();
    void on_nameButton_clicked();

private:
    bool saveTone();
    bool saveToneAs();
};

#endif // MAINWINDOW_H
