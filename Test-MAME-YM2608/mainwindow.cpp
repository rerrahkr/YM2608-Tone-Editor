#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QKeyEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <algorithm>
#include <fstream>
#include "chips/chip_def.hpp"
#include "tone_file.hpp"
#include "namedialog.h"
#include "setupdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    settings_(),
    #ifdef SINC_INTERPOLATION
    chip_(3993600 * 2, settings_.getRate(), settings_.getDuration()),
    #else
    chip_(3993600 * 2, settings_.getRate()),
    #endif
    audio_(chip_, chip_.getRate(), 40),
    octaveFM_(3),
    octavePSG_(3),
    pressedKeyNameFM_("FM: "),
    pressedKeyNamePSG_("PSG: "),
    isEdit_(false)
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

    sliders_[0] = ui->op1Sliders;
    sliders_[1] = ui->op2Sliders;
    sliders_[2] = ui->op3Sliders;
    sliders_[3] = ui->op4Sliders;

    for (int i = 0; i < 4; ++i) {
        sliders_[i]->setOperatorNumber(i + 1);
    }

    tone_ = std::make_unique<Tone>();
    {
        tone_->name = "name";
        tone_->path = "./";
        tone_->AL = 4;
        for (int i = 0; i < 4; ++i) {
            tone_->op[i].AR = 31;
            tone_->op[i].RR = 7;
            if (i % 2 == 0) tone_->op[i].TL = 32;
        }
    }

    chip_.setRegister(0x29, 0x80);	// Init interrupt
    InitPan();
    SetFMTone(1);

    std::fill_n(jamKeyOnTableFM_, 17, false);
    std::fill_n(jamKeyOnTablePSG_, 17, false);

    converter_.loadFormat("format.conf");
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
        SetFMTone(1);
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
        SetFMKey(1, (octaveFM_ + jamKeyNumber / 12), (jamKeyNumber % 12));
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
        SetPSGKey(1, (octavePSG_ + jamKeyNumber / 12), (jamKeyNumber % 12));
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


void MainWindow::SetFMTone(int channel)
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

    data = tone_->FB << 3;
    data += tone_->AL;
    chip_.setRegister(0xb0 + bch, data);

    for (int i = 0; i < 4; i++) {
        uint32 offset;
        switch (i) {
        case 0: offset = 0;  break;
        case 1: offset = 8;  break;
        case 2: offset = 4;  break;
        case 3: offset = 12; break;
        }

        data = tone_->op[i].DT << 4;
        data |= tone_->op[i].ML;
        chip_.setRegister(0x30 + bch + offset, data);

        data = tone_->op[i].TL;
        chip_.setRegister(0x40 + bch + offset, data);

        data = tone_->op[i].KS << 6;
        data |= tone_->op[i].AR;
        chip_.setRegister(0x50 + bch + offset, data);

        data = tone_->op[i].AM << 7;
        data |= tone_->op[i].DR;
        chip_.setRegister(0x60 + bch + offset, data);

        data = tone_->op[i].SR;
        chip_.setRegister(0x70 + bch + offset, data);

        data = tone_->op[i].SL << 4;
        data |= tone_->op[i].RR;
        chip_.setRegister(0x80 + bch + offset, data);
    }

    ui->nameLabel->setText(QString::fromStdString(tone_->name));
    ui->alSlider->setValue(tone_->AL);
    ui->fbSlider->setValue(tone_->FB);
    for (int i = 0; i< 4; ++i) {
        sliders_[i]->setParameterValue(ParameterState::AR, tone_->op[i].AR);
        sliders_[i]->setParameterValue(ParameterState::DR, tone_->op[i].DR);
        sliders_[i]->setParameterValue(ParameterState::SR, tone_->op[i].SR);
        sliders_[i]->setParameterValue(ParameterState::RR, tone_->op[i].RR);
        sliders_[i]->setParameterValue(ParameterState::SL, tone_->op[i].SL);
        sliders_[i]->setParameterValue(ParameterState::TL, tone_->op[i].TL);
        sliders_[i]->setParameterValue(ParameterState::KS, tone_->op[i].KS);
        sliders_[i]->setParameterValue(ParameterState::ML, tone_->op[i].ML);
        sliders_[i]->setParameterValue(ParameterState::DT, tone_->op[i].DT);
        sliders_[i]->setParameterValue(ParameterState::AM, tone_->op[i].AM);
    }

    isEdit_ = false;
    setWindowTitle("YM2608 Tone Editor - " + QString::fromStdString(tone_->name));
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

void MainWindow::SetFMKey(int channel, uint32 octave, uint32 keynum)
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

void MainWindow::SetPSGKey(int channel, uint32 octave, uint32 keynum)
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

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (isEdit_) {
       QMessageBox mbox(QMessageBox::Warning, "Warning", "Do you want to save changes?", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, this);
       switch (mbox.exec()) {
       case QMessageBox::Yes:
           if (saveTone()) break;
           else event->ignore(); break;
       case QMessageBox::No: break;
       default: event->ignore(); break;
       }
    }
}

void MainWindow::onALChanged(int value)
{
    tone_->AL = value;
    uint32 reg = (tone_->FB << 3) | tone_->AL;
    chip_.setRegister(0xb0, reg);

    if (!isEdit_) {
        isEdit_ = true;
        setWindowTitle(windowTitle() + "*");
    }
}

void MainWindow::onFBChanged(int value)
{
    tone_->FB = value;
    uint32 reg = (tone_->FB << 3) | tone_->AL;
    chip_.setRegister(0xb0, reg);

    if (!isEdit_) {
        isEdit_ = true;
        setWindowTitle(windowTitle() + "*");
    }
}

void MainWindow::onParameterChanged(int op, const ParameterState& state)
{
    uint32 value = state.getValue();
    Operator& slot = tone_->op[op - 1];

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

    if (!isEdit_) {
        isEdit_ = true;
        setWindowTitle(windowTitle() + "*");
    }
}


void MainWindow::on_actionOpen_O_triggered()
{
    if (isEdit_) {
       QMessageBox mbox(QMessageBox::Warning, "Warning", "Do you want to save changes?", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, this);
       switch (mbox.exec()) {
       case QMessageBox::Yes:
           if (saveTone()) break;
           else return;
       case QMessageBox::No: break;
       default: return;
       }
    }

    QString file = QFileDialog::getOpenFileName(this, "Open tone", QString::fromStdString(tone_->path), "FM tone file (*.fmt)");
    if (!file.isNull()) {
        tone_ = ToneFile::read(file.toStdString());
        SetFMTone(1);
    }
}

void MainWindow::on_actionSave_S_triggered()
{
    saveTone();
}

void MainWindow::on_actionExit_X_triggered()
{
    close();
}

void MainWindow::on_actionSave_As_triggered()
{
    saveToneAs();
}

bool MainWindow::saveTone()
{
    bool isExist;
    {
        std::ifstream check(tone_->path, std::ios::in | std::ios::binary);
        isExist = check.is_open();
    }

    if (isExist) {
        ToneFile::write(tone_->path, tone_.get());
        isEdit_ = false;
        setWindowTitle("YM2608 Tone Editor - " + QString::fromStdString(tone_->name));
    }
    else {
        return saveToneAs();
    }

    return true;
}

bool MainWindow::saveToneAs()
{
    QString file = QFileDialog::getSaveFileName(this, "Save tone", QString::fromStdString(tone_->path), "FM tone file (*.fmt)");
    if (file.isNull()) return false;

    tone_->path = file.toStdString();
    ToneFile::write(tone_->path, tone_.get());

    isEdit_ = false;
    setWindowTitle("YM2608 Tone Editor - " + QString::fromStdString(tone_->name));

    return true;
}

void MainWindow::on_nameButton_clicked()
{
    NameDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        ui->nameLabel->setText(dialog.toneName());
        tone_->name = dialog.toneName().toStdString();
        if (!isEdit_) isEdit_ = true;
        setWindowTitle("YM2608 Tone Editor - " + dialog.toneName() + "*");
    }
}

void MainWindow::on_actionConvert_To_Text_C_triggered()
{
    QString str = QString::fromStdString(converter_.convert(tone_.get()));
    textDialog_.setText(str);
    if (textDialog_.isHidden()) {
        textDialog_.show();
    }
    else {
        textDialog_.activateWindow();
    }
}

void MainWindow::on_actionSetup_E_triggered()
{
    SetupDialog dialog(settings_, this);
    if (dialog.exec() == QDialog::Accepted) {
        settings_.setDuration(dialog.duration());
        settings_.setRate(dialog.rate());

        audio_.setDuration(settings_.getDuration());
        #ifdef SINC_INTERPOLATION
        chip_.setDuration(settings_.getDuration());
        #endif
        audio_.setRate(settings_.getRate());
        chip_.setRate(settings_.getRate());
    }
}
