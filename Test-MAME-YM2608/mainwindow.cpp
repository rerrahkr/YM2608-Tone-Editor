#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QKeyEvent>
#include <algorithm>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    chip_(3993600 * 2, 0, 40),
    audio_(chip_, 40),
    octaveFM_(3),
    octavePSG_(3),
    pressedKeyNameFM_("FM: "),
    pressedKeyNamePSG_("PSG: ")
{
    ui->setupUi(this);

    ui->statusBar->showMessage(pressedKeyNameFM_ + " | " + pressedKeyNamePSG_);

    ui->alSlider->setText("AL: ");
    ui->fbSlider->setText("FB: ");

    ui->alSlider->setMaximum(7);
    ui->fbSlider->setMaximum(7);

    connect(ui->alSlider, &LabeledSlider::valueChanged, this, &MainWindow::onALChanged);
    connect(ui->fbSlider, &LabeledSlider::valueChanged, this, &MainWindow::onFBChanged);
    connect(ui->op1Sliders, &OperatorSliders::parameterChanged, this, &MainWindow::onParameterChanged);
    connect(ui->op2Sliders, &OperatorSliders::parameterChanged, this, &MainWindow::onParameterChanged);
    connect(ui->op3Sliders, &OperatorSliders::parameterChanged, this, &MainWindow::onParameterChanged);
    connect(ui->op4Sliders, &OperatorSliders::parameterChanged, this, &MainWindow::onParameterChanged);


    ui->op1Sliders->setOperatorNumber(1);
    ui->op2Sliders->setOperatorNumber(2);
    ui->op3Sliders->setOperatorNumber(3);
    ui->op4Sliders->setOperatorNumber(4);

    {
        voice_.name = "detune";
        voice_.AL = 5;
        voice_.FB = 5;
        voice_.op[0].AR = 31;
        voice_.op[0].DR = 5;
        voice_.op[0].SR = 0;
        voice_.op[0].RR = 7;
        voice_.op[0].SL = 12;
        voice_.op[0].TL = 22;
        voice_.op[0].KS = 0;
        voice_.op[0].ML = 0;
        voice_.op[0].DT = 0;
        voice_.op[0].AM = 0;
        voice_.op[1].AR = 31;
        voice_.op[1].DR = 5;
        voice_.op[1].SR = 0;
        voice_.op[1].RR = 7;
        voice_.op[1].SL = 6;
        voice_.op[1].TL = 12;
        voice_.op[1].KS = 0;
        voice_.op[1].ML = 2;
        voice_.op[1].DT = 3;
        voice_.op[1].AM = 0;
        voice_.op[2].AR = 31;
        voice_.op[2].DR = 5;
        voice_.op[2].SR = 0;
        voice_.op[2].RR = 7;
        voice_.op[2].SL = 7;
        voice_.op[2].TL = 21;
        voice_.op[2].KS = 0;
        voice_.op[2].ML = 3;
        voice_.op[2].DT = 0;
        voice_.op[2].AM = 0;
        voice_.op[3].AR = 31;
        voice_.op[3].DR = 0;
        voice_.op[3].SR = 0;
        voice_.op[3].RR = 7;
        voice_.op[3].SL = 0;
        voice_.op[3].TL = 0;
        voice_.op[3].KS = 0;
        voice_.op[3].ML = 0;
        voice_.op[3].DT = 0;
        voice_.op[3].AM = 0;
    }

    {
        ui->alSlider->setValue(voice_.AL);
        ui->fbSlider->setValue(voice_.FB);
        ui->op1Sliders->setParameterValue(ParameterState::AR, voice_.op[0].AR);
        ui->op1Sliders->setParameterValue(ParameterState::DR, voice_.op[0].DR);
        ui->op1Sliders->setParameterValue(ParameterState::SR, voice_.op[0].SR);
        ui->op1Sliders->setParameterValue(ParameterState::RR, voice_.op[0].RR);
        ui->op1Sliders->setParameterValue(ParameterState::SL, voice_.op[0].SL);
        ui->op1Sliders->setParameterValue(ParameterState::TL, voice_.op[0].TL);
        ui->op1Sliders->setParameterValue(ParameterState::KS, voice_.op[0].KS);
        ui->op1Sliders->setParameterValue(ParameterState::ML, voice_.op[0].ML);
        ui->op1Sliders->setParameterValue(ParameterState::DT, voice_.op[0].DT);
        ui->op1Sliders->setParameterValue(ParameterState::AM, voice_.op[0].AM);
        ui->op2Sliders->setParameterValue(ParameterState::AR, voice_.op[1].AR);
        ui->op2Sliders->setParameterValue(ParameterState::DR, voice_.op[1].DR);
        ui->op2Sliders->setParameterValue(ParameterState::SR, voice_.op[1].SR);
        ui->op2Sliders->setParameterValue(ParameterState::RR, voice_.op[1].RR);
        ui->op2Sliders->setParameterValue(ParameterState::SL, voice_.op[1].SL);
        ui->op2Sliders->setParameterValue(ParameterState::TL, voice_.op[1].TL);
        ui->op2Sliders->setParameterValue(ParameterState::KS, voice_.op[1].KS);
        ui->op2Sliders->setParameterValue(ParameterState::ML, voice_.op[1].ML);
        ui->op2Sliders->setParameterValue(ParameterState::DT, voice_.op[1].DT);
        ui->op2Sliders->setParameterValue(ParameterState::AM, voice_.op[1].AM);
        ui->op3Sliders->setParameterValue(ParameterState::AR, voice_.op[2].AR);
        ui->op3Sliders->setParameterValue(ParameterState::DR, voice_.op[2].DR);
        ui->op3Sliders->setParameterValue(ParameterState::SR, voice_.op[2].SR);
        ui->op3Sliders->setParameterValue(ParameterState::RR, voice_.op[2].RR);
        ui->op3Sliders->setParameterValue(ParameterState::SL, voice_.op[2].SL);
        ui->op3Sliders->setParameterValue(ParameterState::TL, voice_.op[2].TL);
        ui->op3Sliders->setParameterValue(ParameterState::KS, voice_.op[2].KS);
        ui->op3Sliders->setParameterValue(ParameterState::ML, voice_.op[2].ML);
        ui->op3Sliders->setParameterValue(ParameterState::DT, voice_.op[2].DT);
        ui->op3Sliders->setParameterValue(ParameterState::AM, voice_.op[2].AM);
        ui->op4Sliders->setParameterValue(ParameterState::AR, voice_.op[3].AR);
        ui->op4Sliders->setParameterValue(ParameterState::DR, voice_.op[3].DR);
        ui->op4Sliders->setParameterValue(ParameterState::SR, voice_.op[3].SR);
        ui->op4Sliders->setParameterValue(ParameterState::RR, voice_.op[3].RR);
        ui->op4Sliders->setParameterValue(ParameterState::SL, voice_.op[3].SL);
        ui->op4Sliders->setParameterValue(ParameterState::TL, voice_.op[3].TL);
        ui->op4Sliders->setParameterValue(ParameterState::KS, voice_.op[3].KS);
        ui->op4Sliders->setParameterValue(ParameterState::ML, voice_.op[3].ML);
        ui->op4Sliders->setParameterValue(ParameterState::DT, voice_.op[3].DT);
        ui->op4Sliders->setParameterValue(ParameterState::AM, voice_.op[3].AM);
    }

    chip_.setRegister(0x29, 0x80);	// Init interrupt
    InitPan();
    SetFMVoice(voice_, 1);

    std::fill_n(jamKeyOnTableFM_, 17, false);
    std::fill_n(jamKeyOnTablePSG_, 17, false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    bool isRepeat = event->isAutoRepeat();

    switch (event->key()) {
    case Qt::Key_Z:			JamKeyOnFM(0, isRepeat);            break;
    case Qt::Key_S:			JamKeyOnFM(1, isRepeat);            break;
    case Qt::Key_X:			JamKeyOnFM(2, isRepeat);            break;
    case Qt::Key_D:			JamKeyOnFM(3, isRepeat);            break;
    case Qt::Key_C:			JamKeyOnFM(4, isRepeat);            break;
    case Qt::Key_V:			JamKeyOnFM(5, isRepeat);            break;
    case Qt::Key_G:			JamKeyOnFM(6, isRepeat);            break;
    case Qt::Key_B:			JamKeyOnFM(7, isRepeat);            break;
    case Qt::Key_H:			JamKeyOnFM(8, isRepeat);            break;
    case Qt::Key_N:			JamKeyOnFM(9, isRepeat);            break;
    case Qt::Key_J:			JamKeyOnFM(10, isRepeat);           break;
    case Qt::Key_M:			JamKeyOnFM(11, isRepeat);           break;
    case Qt::Key_Comma:		JamKeyOnFM(12, isRepeat);           break;
    case Qt::Key_L:			JamKeyOnFM(13, isRepeat);           break;
    case Qt::Key_Period:	JamKeyOnFM(14, isRepeat);           break;
    case Qt::Key_Semicolon:	JamKeyOnFM(15, isRepeat);           break;
    case Qt::Key_Slash:		JamKeyOnFM(16, isRepeat);           break;
    case Qt::Key_Q:			JamKeyOnPSG(0, isRepeat);           break;
    case Qt::Key_2:			JamKeyOnPSG(1, isRepeat);           break;
    case Qt::Key_W:			JamKeyOnPSG(2, isRepeat);           break;
    case Qt::Key_3:			JamKeyOnPSG(3, isRepeat);           break;
    case Qt::Key_E:			JamKeyOnPSG(4, isRepeat);           break;
    case Qt::Key_R:			JamKeyOnPSG(5, isRepeat);           break;
    case Qt::Key_5:			JamKeyOnPSG(6, isRepeat);           break;
    case Qt::Key_T:			JamKeyOnPSG(7, isRepeat);           break;
    case Qt::Key_6:			JamKeyOnPSG(8, isRepeat);           break;
    case Qt::Key_Y:			JamKeyOnPSG(9, isRepeat);           break;
    case Qt::Key_7:			JamKeyOnPSG(10, isRepeat);          break;
    case Qt::Key_U:			JamKeyOnPSG(11, isRepeat);          break;
    case Qt::Key_I:			JamKeyOnPSG(12, isRepeat);          break;
    case Qt::Key_9:			JamKeyOnPSG(13, isRepeat);          break;
    case Qt::Key_O:			JamKeyOnPSG(14, isRepeat);          break;
    case Qt::Key_0:			JamKeyOnPSG(15, isRepeat);          break;
    case Qt::Key_P:			JamKeyOnPSG(16, isRepeat);          break;
    case Qt::Key_F1:		if (octaveFM_ > 1)	--octaveFM_;	break;
    case Qt::Key_F2:		if (octaveFM_ < 7)	++octaveFM_;	break;
    case Qt::Key_F3:		if (octavePSG_ > 1)	--octavePSG_;   break;
    case Qt::Key_F4:        if (octavePSG_ < 6)	++octavePSG_;	break;
    case Qt::Key_F12:   // Reset sound
        chip_.reset();
        chip_.setRegister(0x29, 0x80);	// Init interrupt
        InitPan();
        SetFMVoice(voice_, 1);
        break;
    case Qt::Key_Escape:    close();                            break;
    default:                                                    break;
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    bool isRepeat = event->isAutoRepeat();

    switch (event->key()) {
    case Qt::Key_Z:			JamKeyOffFM(0, isRepeat);	break;
    case Qt::Key_S:			JamKeyOffFM(1, isRepeat);	break;
    case Qt::Key_X:			JamKeyOffFM(2, isRepeat);	break;
    case Qt::Key_D:			JamKeyOffFM(3, isRepeat);	break;
    case Qt::Key_C:			JamKeyOffFM(4, isRepeat);	break;
    case Qt::Key_V:			JamKeyOffFM(5, isRepeat);	break;
    case Qt::Key_G:			JamKeyOffFM(6, isRepeat);	break;
    case Qt::Key_B:			JamKeyOffFM(7, isRepeat);	break;
    case Qt::Key_H:			JamKeyOffFM(8, isRepeat);	break;
    case Qt::Key_N:			JamKeyOffFM(9, isRepeat);	break;
    case Qt::Key_J:			JamKeyOffFM(10, isRepeat);	break;
    case Qt::Key_M:			JamKeyOffFM(11, isRepeat);	break;
    case Qt::Key_Comma:		JamKeyOffFM(12, isRepeat);  break;
    case Qt::Key_L:			JamKeyOffFM(13, isRepeat);  break;
    case Qt::Key_Period:	JamKeyOffFM(14, isRepeat);  break;
    case Qt::Key_Semicolon:	JamKeyOffFM(15, isRepeat);  break;
    case Qt::Key_Slash:		JamKeyOffFM(16, isRepeat);  break;
    case Qt::Key_Q:			JamKeyOffPSG(0, isRepeat);  break;
    case Qt::Key_2:			JamKeyOffPSG(1, isRepeat);  break;
    case Qt::Key_W:			JamKeyOffPSG(2, isRepeat);  break;
    case Qt::Key_3:			JamKeyOffPSG(3, isRepeat);  break;
    case Qt::Key_E:			JamKeyOffPSG(4, isRepeat);  break;
    case Qt::Key_R:			JamKeyOffPSG(5, isRepeat);  break;
    case Qt::Key_5:			JamKeyOffPSG(6, isRepeat);  break;
    case Qt::Key_T:			JamKeyOffPSG(7, isRepeat);  break;
    case Qt::Key_6:			JamKeyOffPSG(8, isRepeat);  break;
    case Qt::Key_Y:			JamKeyOffPSG(9, isRepeat);  break;
    case Qt::Key_7:			JamKeyOffPSG(10, isRepeat); break;
    case Qt::Key_U:			JamKeyOffPSG(11, isRepeat); break;
    case Qt::Key_I:			JamKeyOffPSG(12, isRepeat); break;
    case Qt::Key_9:			JamKeyOffPSG(13, isRepeat); break;
    case Qt::Key_O:			JamKeyOffPSG(14, isRepeat); break;
    case Qt::Key_0:			JamKeyOffPSG(15, isRepeat); break;
    case Qt::Key_P:			JamKeyOffPSG(16, isRepeat); break;
    default:                                            break;
    }
}


void MainWindow::JamKeyOnFM(uint32 jamKeyNumber, bool isRepeat)
{
    if (!isRepeat && !jamKeyOnTableFM_[jamKeyNumber]) {
        SetFMTone(1, (octaveFM_ + jamKeyNumber / 12), (jamKeyNumber % 12));
        SetFMKeyOn(1);
        jamKeyOnTableFM_[jamKeyNumber] = true;

        pressedKeyNameFM_ = "FM: " + keyNumberToNameString(jamKeyNumber);
        pressedKeyNameFM_ += QString::number(octaveFM_ + jamKeyNumber / 12);
        ui->statusBar->showMessage(pressedKeyNameFM_ + " | " + pressedKeyNamePSG_);
    }
}

void MainWindow::JamKeyOnPSG(uint32 jamKeyNumber, bool isRepeat)
{
    if (!isRepeat && !jamKeyOnTablePSG_[jamKeyNumber]) {
        SetPSGTone(1, (octavePSG_ + jamKeyNumber / 12), (jamKeyNumber % 12));
        chip_.setRegister(0x08, 0x0f);
        chip_.setRegister(0x07, 0x7e);
        jamKeyOnTablePSG_[jamKeyNumber] = true;

        pressedKeyNamePSG_ = "PSG: " + keyNumberToNameString(jamKeyNumber);
        pressedKeyNamePSG_ += QString::number(octavePSG_ + jamKeyNumber / 12);
        ui->statusBar->showMessage(pressedKeyNameFM_ + " | " + pressedKeyNamePSG_);
    }
}

void MainWindow::JamKeyOffFM(uint32 jamKeyNumber, bool isRepeat)
{
    if (!isRepeat && jamKeyOnTableFM_[jamKeyNumber]) {
        SetFMKeyOff(1);
        jamKeyOnTableFM_[jamKeyNumber] = false;

        pressedKeyNameFM_ = "FM: ";
        ui->statusBar->showMessage(pressedKeyNameFM_ + " | " + pressedKeyNamePSG_);
    }
}

void MainWindow::JamKeyOffPSG(uint32 jamKeyNumber, bool isRepeat)
{
    if (!isRepeat && jamKeyOnTablePSG_[jamKeyNumber]) {
        chip_.setRegister(0x08, 0x00);
        chip_.setRegister(0x07, 0x7f);
        jamKeyOnTablePSG_[jamKeyNumber] = false;

        pressedKeyNamePSG_ = "PSG: ";
        ui->statusBar->showMessage(pressedKeyNameFM_ + " | " + pressedKeyNamePSG_);
    }
}


void MainWindow::SetFMVoice(struct Voice voice, int channel)
{
    uint32 bch;	// Bank and channel offset
    switch (channel) {
    case 1:
    case 2:
    case 3:
        bch = channel - 1;
        break;
    case 4:
    case 5:
    case 6:
        bch = 0x100 + (channel % 3) - 1;
        break;
    }

    uint32 data;

    data = voice.FB << 3;
    data += voice.AL;
    chip_.setRegister(0xb0 + bch, data);

    for (int i = 0; i < 4; i++) {
        uint32 offset;
        switch (i) {
        case 0: offset = 0;  break;
        case 1: offset = 8;  break;
        case 2: offset = 4;  break;
        case 3: offset = 12; break;
        }

        data = voice.op[i].DT << 4;
        data |= voice.op[i].ML;
        chip_.setRegister(0x30 + bch + offset, data);

        data = voice.op[i].TL;
        chip_.setRegister(0x40 + bch + offset, data);

        data = voice.op[i].KS << 6;
        data |= voice.op[i].AR;
        chip_.setRegister(0x50 + bch + offset, data);

        data = voice.op[i].AM << 7;
        data |= voice.op[i].DR;
        chip_.setRegister(0x60 + bch + offset, data);

        data = voice.op[i].SR;
        chip_.setRegister(0x70 + bch + offset, data);

        data = voice.op[i].SL << 4;
        data |= voice.op[i].RR;
        chip_.setRegister(0x80 + bch + offset, data);
    }
}

void MainWindow::SetFMKeyOn(int channel)
{
    uint32 slot = 0x0f;
    uint32 ch;
    switch (channel) {
    case 1: ch = 0x00; break;
    case 2: ch = 0x01; break;
    case 3: ch = 0x02; break;
    case 4: ch = 0x04; break;
    case 5: ch = 0x05; break;
    case 6: ch = 0x06; break;
    }
    chip_.setRegister(0x28, (slot << 4) | ch);
}

void MainWindow::SetFMKeyOff(int channel)
{
    uint32 slot = 0x00;
    uint32 ch;
    switch (channel) {
    case 1: ch = 0x00; break;
    case 2: ch = 0x01; break;
    case 3: ch = 0x02; break;
    case 4: ch = 0x04; break;
    case 5: ch = 0x05; break;
    case 6: ch = 0x06; break;
    }
    chip_.setRegister(0x28, (slot << 4) | ch);
}

void MainWindow::SetFMTone(int channel, uint32 octave, uint32 keynum)
{
    uint32 bch;	// Bank and channel offset
    switch (channel) {
    case 1:
    case 2:
    case 3:
        bch = channel - 1;
        break;
    case 4:
    case 5:
    case 6:
        bch = 0x100 + (channel % 3) - 1;
        break;
    }

    // A4(440Hz)
    uint32 data;
    uint32 block = octave;	// Octave
    uint32 fnum = fm_tune_tbl[keynum];
    data = block << 3;
    data = data | (fnum >> 8);
    chip_.setRegister(0xa4 + bch, data);
    data = fnum & 0xff;
    chip_.setRegister(0xa0 + bch, data);
}

void MainWindow::SetPSGTone(int channel, uint32 octave, uint32 keynum)
{
    uint32 offset = 2 * (channel - 1);

    uint32 data = psg_tune_tbl[keynum];
    if (octave > 0) {
        data = (data + 1) >> octave;
    }

    chip_.setRegister(0x00 + offset, data & 0xff);
    chip_.setRegister(0x01 + offset, data >> 8);
}

void MainWindow::InitPan()
{
    chip_.setRegister(0xb4, 0xc0);
    chip_.setRegister(0xb5, 0xc0);
    chip_.setRegister(0xb6, 0xc0);
}

QString MainWindow::keyNumberToNameString(uint32 jamKeyNumber)
{
    switch (jamKeyNumber) {
    case 0:
    case 12:    return "C";
    case 1:
    case 13:    return "C#";
    case 2:
    case 14:    return "D";
    case 3:
    case 15:    return "D#";
    case 4:
    case 16:    return "E";
    case 5:     return "F";
    case 6:     return "F#";
    case 7:     return "G";
    case 8:     return "G#";
    case 9:     return "A";
    case 10:    return "A#";
    case 11:    return "B";
    default:    return "";
    }
}

void MainWindow::onALChanged(int value)
{
    voice_.AL = value;
    uint32 reg = (voice_.FB << 3) | voice_.AL;
    chip_.setRegister(0xb0, reg);
}

void MainWindow::onFBChanged(int value)
{
    voice_.FB = value;
    uint32 reg = (voice_.FB << 3) | voice_.AL;
    chip_.setRegister(0xb0, reg);
}

void MainWindow::onParameterChanged(int op, const ParameterState& state)
{
    uint32 value = state.getValue();
    MainWindow::Operator& slot = voice_.op[op - 1];

    uint32 opOffset;
    switch (op) {
    case 1: opOffset = 0x00; break;
    case 2: opOffset = 0x08; break;
    case 3: opOffset = 0x04; break;
    case 4: opOffset = 0x0c; break;
    default: opOffset = 0;   break;
    }

    uint32 reg;
    switch (state.getParametor()) {
    case ParameterState::AR:
        slot.AR = value;
        reg = (slot.KS << 6) | slot.AR;
        chip_.setRegister(0x50 + opOffset, reg);
        break;
    case ParameterState::DR:
        slot.DR = value;
        reg = (slot.AM << 7) | slot.DR;
        chip_.setRegister(0x60 + opOffset, reg);
        break;
    case ParameterState::SR:
        slot.SR = value;
        reg = slot.SR;
        chip_.setRegister(0x70 + opOffset, reg);
        break;
    case ParameterState::RR:
        slot.RR = value;
        reg = (slot.SL << 4) | slot.RR;
        chip_.setRegister(0x80 + opOffset, reg);
        break;
    case ParameterState::SL:
        slot.SL = value;
        reg = (slot.SL << 4) | slot.RR;
        chip_.setRegister(0x80 + opOffset, reg);
        break;
    case ParameterState::TL:
        slot.TL = value;
        reg = slot.TL;
        chip_.setRegister(0x40 + opOffset, reg);
        break;
    case ParameterState::KS:
        slot.KS = value;
        reg = (slot.KS << 6) | slot.AR;
        chip_.setRegister(0x50 + opOffset, reg);
        break;
    case ParameterState::ML:
        slot.ML = value;
        reg = (slot.DT << 4) | slot.ML;
        chip_.setRegister(0x30 + opOffset, reg);
        break;
    case ParameterState::DT:
        slot.DT = value;
        reg = (slot.DT << 4) | slot.ML;
        chip_.setRegister(0x30 + opOffset, reg);
        break;
    case ParameterState::AM:
        slot.AM = value;
        reg = (slot.AM << 7) | slot.DR;
        chip_.setRegister(0x60 + opOffset, reg);
        break;
    default:
        return;
    }
}
