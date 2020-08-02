#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QKeyEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <algorithm>
#include <fstream>
#include <cstdint>
#include <utility>
#include "tone_file.hpp"
#include "namedialog.h"
#include "setupdialog.h"
#include "aboutdialog.h"
#include "readtextdialog.hpp"
#include "text_conversion.hpp"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	settings_(),
	chip_(3993600 * 2, settings_.getRate()),
	audio_(chip_, chip_.getRate(), 40),
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
	chip_.setRegister(0x07, 0xff);  // PSG mix
	chip_.setRegister(0x11, 0x3f);  // Drum total volume
	InitPan();
	for (int i = 1; i <= 6; ++i) {
		SetFMTone(i);
	}
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
	case Qt::Key_F5:    // Bass drum
		chip_.setRegister(0x18, 0xdf);
		chip_.setRegister(0x10, 0x01);
		break;
	case Qt::Key_F6:    // Snare drum
		chip_.setRegister(0x19, 0xdf);
		chip_.setRegister(0x10, 0x02);
		break;
	case Qt::Key_F7:    // Top cymbal
		chip_.setRegister(0x1a, 0xdf);
		chip_.setRegister(0x10, 0x04);
		break;
	case Qt::Key_F8:    // Hi hat
		chip_.setRegister(0x1b, 0xdf);
		chip_.setRegister(0x10, 0x08);
		break;
	case Qt::Key_F9:    // Tomtom
		chip_.setRegister(0x1c, 0xdf);
		chip_.setRegister(0x10, 0x10);
		break;
	case Qt::Key_F10:    // Rim shot
		chip_.setRegister(0x1d, 0xdf);
		chip_.setRegister(0x10, 0x20);
		break;
	case Qt::Key_F12:   // Reset sound
		chip_.reset();
		chip_.setRegister(0x29, 0x80);	// Init interrupt
		chip_.setRegister(0x07, 0xff);  // PSG mix
		chip_.setRegister(0x11, 0x3f);  // Drum total volume
		InitPan();
		for (int i = 1; i <= 6; ++i) {
			SetFMTone(i);
		}
		// Clear key on table
		jamKeyOnTableFM_.clear();
		jamKeyOnTablePSG_.clear();
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


void MainWindow::JamKeyOnFM(int jamKeyNumber, bool isRepeat)
{
	if (!isRepeat) {
		int ch = 1;

		if (jamKeyOnTableFM_.size()) {
			// Search playable channel
			int flags = 0x00;
			for (auto&& it : jamKeyOnTableFM_) {
				flags |= (it >> 8);
			}

			for (; ch <= 6; ++ch) {
				if (!(flags & 0x01)) break;
				flags >>= 1;
			}

			if (ch > 6) {   // In using all channel
				int delFlag = jamKeyOnTableFM_.front() >> 8;
				int delCh = 1;
				for (; delCh <= 6; ++delCh) {
					if (delFlag & 0x01) break;
					delFlag >>= 1;
				}
				SetFMKeyOff(delCh);
				jamKeyOnTableFM_.erase(jamKeyOnTableFM_.begin());
				ch = delCh;
			}
		}

		// Set key on
		SetFMKey(ch, (octaveFM_ + jamKeyNumber / 12), (jamKeyNumber % 12));
		SetFMKeyOn(ch);
		int value = (1 << (ch + 7)) | (octaveFM_ * 12 + jamKeyNumber);
		jamKeyOnTableFM_.push_back(value);

		pressedKeyNameFM_ = "FM: " + keyNumberToNameString(jamKeyNumber);
		pressedKeyNameFM_ += QString::number(octaveFM_ + jamKeyNumber / 12);
		ui->statusBar->showMessage(pressedKeyNameFM_ + " | " + pressedKeyNamePSG_);
	}
}

void MainWindow::JamKeyOnPSG(int jamKeyNumber, bool isRepeat)
{
	if (!isRepeat) {
		int ch = 1;

		if (jamKeyOnTablePSG_.size()) {
			// Search playable channel
			int flags = 0x00;
			for (auto&& it : jamKeyOnTablePSG_) {
				flags |= (it >> 8);
			}

			for (; ch <= 3; ++ch) {
				if (!(flags & 0x01)) break;
				flags >>= 1;
			}

			if (ch > 3) {   // In using all channel
				int delFlag = jamKeyOnTablePSG_.front() >> 8;
				int delCh = 1;
				for (; delCh <= 3; ++delCh) {
					if (delFlag & 0x01) break;
					delFlag >>= 1;
				}
				SetPSGKeyOff(delCh);
				jamKeyOnTablePSG_.erase(jamKeyOnTablePSG_.begin());
				ch = delCh;
			}
		}

		// Set key on
		SetPSGKey(ch, (octavePSG_ + jamKeyNumber / 12), (jamKeyNumber % 12));
		SetPSGKeyOn(ch);
		int value = (1 << (ch + 7)) | (octavePSG_ * 12 + jamKeyNumber);
		jamKeyOnTablePSG_.push_back(value);

		pressedKeyNamePSG_ = "PSG: " + keyNumberToNameString(jamKeyNumber);
		pressedKeyNamePSG_ += QString::number(octavePSG_ + jamKeyNumber / 12);
		ui->statusBar->showMessage(pressedKeyNameFM_ + " | " + pressedKeyNamePSG_);
	}
}

void MainWindow::JamKeyOffFM(int jamKeyNumber, bool isRepeat)
{
	if (!isRepeat) {
		int value = octaveFM_ * 12 + jamKeyNumber;
		for (size_t i = 0; i < jamKeyOnTableFM_.size(); ++i) {
			if ((jamKeyOnTableFM_[i] & 0x0ff) == value) {
				int flag = jamKeyOnTableFM_[i] >> 8;
				int ch = 1;
				for (; ch <= 6; ++ch) {
					if (flag & 0x01) break;
					flag >>= 1;
				}

				SetFMKeyOff(ch);
				jamKeyOnTableFM_.erase(jamKeyOnTableFM_.begin() + i);

				pressedKeyNameFM_ = "FM: ";
				ui->statusBar->showMessage(pressedKeyNameFM_ + " | " + pressedKeyNamePSG_);
				break;
			}
		}
	}
}

void MainWindow::JamKeyOffPSG(int jamKeyNumber, bool isRepeat)
{
	if (!isRepeat) {
		int value = octavePSG_ * 12 + jamKeyNumber;
		for (size_t i = 0; i < jamKeyOnTablePSG_.size(); ++i) {
			if ((jamKeyOnTablePSG_[i] & 0x0ff) == value) {
				int flag = jamKeyOnTablePSG_[i] >> 8;
				int ch = 1;
				for (; ch <= 3; ++ch) {
					if (flag & 0x01) break;
					flag >>= 1;
				}
				SetPSGKeyOff(ch);
				jamKeyOnTablePSG_.erase(jamKeyOnTablePSG_.begin() + i);

				pressedKeyNamePSG_ = "PSG: ";
				ui->statusBar->showMessage(pressedKeyNameFM_ + " | " + pressedKeyNamePSG_);
				break;
			}
		}
	}
}


void MainWindow::SetFMTone(int channel)
{
	uint32_t bch;	// Bank and channel offset
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

	uint8_t data;

	data = tone_->FB << 3;
	data += tone_->AL;
	chip_.setRegister(0xb0 + bch, data);

	for (int i = 0; i < 4; i++) {
		uint32_t offset;
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

	setWindowModified(false);
	setWindowTitle("YM2608 Tone Editor - " + QString::fromStdString(tone_->name) + "[*]");
}

void MainWindow::SetFMKeyOn(int channel)
{
	uint32_t slot = 0x0f;
	uint32_t ch;
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

void MainWindow::SetPSGKeyOn(int channel)
{
	int chOffset = channel - 1;
	chip_.setRegister(0x08 + chOffset, 0x0f);
	uint8_t reg = chip_.getRegister(0x07);
	reg &= ~(0x01 << chOffset);
	chip_.setRegister(0x07, reg);
}

void MainWindow::SetFMKeyOff(int channel)
{
	uint8_t slot = 0x00;
	uint8_t ch;
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

void MainWindow::SetPSGKeyOff(int channel)
{
	uint32_t chOffset = channel - 1;
	chip_.setRegister(0x08 + chOffset, 0x00);
	uint8_t reg = chip_.getRegister(0x07);
	reg |= (0x01 << chOffset);
	chip_.setRegister(0x07, reg);
}

void MainWindow::SetFMKey(int channel, int octave, int keynum)
{
	uint32_t bch;	// Bank and channel offset
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
	uint8_t data;
	uint32_t block = octave;	// Octave
	uint32_t fnum = fm_tune_tbl[keynum];
	data = block << 3;
	data = data | (fnum >> 8);
	chip_.setRegister(0xa4 + bch, data);
	data = fnum & 0xff;
	chip_.setRegister(0xa0 + bch, data);
}

void MainWindow::SetPSGKey(int channel, int octave, int keynum)
{
	uint32_t offset = 2 * (channel - 1);

	uint32_t data = psg_tune_tbl[keynum];
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

QString MainWindow::keyNumberToNameString(int jamKeyNumber)
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
	if (isWindowModified()) {
		QMessageBox mbox(QMessageBox::Warning, "Warning", "Do you want to save changes?", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, this);
		switch (mbox.exec()) {
		case QMessageBox::Yes:
			if (!saveTone()) event->ignore();
			break;
		case QMessageBox::No: break;
		default: event->ignore(); break;
		}
	}
}

void MainWindow::onALChanged(int value)
{
	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 3; ++j) {
			uint32_t chOffset = 0x100 * i + j;
			tone_->AL = value;
			uint8_t reg = (tone_->FB << 3) | tone_->AL;
			chip_.setRegister(0xb0 + chOffset, reg);
		}
	}

	setWindowModified(true);
}

void MainWindow::onFBChanged(int value)
{
	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 3; ++j) {
			uint32_t chOffset = 0x100 * i + j;
			tone_->FB = value;
			uint8_t reg = (tone_->FB << 3) | tone_->AL;
			chip_.setRegister(0xb0 + chOffset, reg);
		}
	}

	setWindowModified(true);
}

void MainWindow::onParameterChanged(int op, const ParameterState& state)
{
	uint8_t value = state.getValue();
	Operator& slot = tone_->op[op - 1];

	uint32_t opOffset;
	switch (op) {
	case 1: opOffset = 0x00; break;
	case 2: opOffset = 0x08; break;
	case 3: opOffset = 0x04; break;
	case 4: opOffset = 0x0c; break;
	default: opOffset = 0;   break;
	}

	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 3; ++j) {
			uint32_t chOffset = 0x100 * i + j;
			uint8_t reg;
			switch (state.getParametor()) {
			case ParameterState::AR:
				slot.AR = value;
				reg = (slot.KS << 6) | slot.AR;
				chip_.setRegister(0x50 + chOffset + opOffset, reg);
				break;
			case ParameterState::DR:
				slot.DR = value;
				reg = (slot.AM << 7) | slot.DR;
				chip_.setRegister(0x60 + chOffset + opOffset, reg);
				break;
			case ParameterState::SR:
				slot.SR = value;
				reg = slot.SR;
				chip_.setRegister(0x70 + chOffset + opOffset, reg);
				break;
			case ParameterState::RR:
				slot.RR = value;
				reg = (slot.SL << 4) | slot.RR;
				chip_.setRegister(0x80 + chOffset + opOffset, reg);
				break;
			case ParameterState::SL:
				slot.SL = value;
				reg = (slot.SL << 4) | slot.RR;
				chip_.setRegister(0x80 + chOffset + opOffset, reg);
				break;
			case ParameterState::TL:
				slot.TL = value;
				reg = slot.TL;
				chip_.setRegister(0x40 + chOffset + opOffset, reg);
				break;
			case ParameterState::KS:
				slot.KS = value;
				reg = (slot.KS << 6) | slot.AR;
				chip_.setRegister(0x50 + chOffset + opOffset, reg);
				break;
			case ParameterState::ML:
				slot.ML = value;
				reg = (slot.DT << 4) | slot.ML;
				chip_.setRegister(0x30 + chOffset + opOffset, reg);
				break;
			case ParameterState::DT:
				slot.DT = value;
				reg = (slot.DT << 4) | slot.ML;
				chip_.setRegister(0x30 + chOffset + opOffset, reg);
				break;
			case ParameterState::AM:
				slot.AM = value;
				reg = (slot.AM << 7) | slot.DR;
				chip_.setRegister(0x60 + chOffset + opOffset, reg);
				break;
			default:
				return;
			}
		}
	}

	setWindowModified(true);
}


void MainWindow::on_actionOpen_O_triggered()
{
	if (isWindowModified()) {
		QMessageBox mbox(QMessageBox::Warning, "Warning", "Do you want to save changes?", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, this);
		switch (mbox.exec()) {
		case QMessageBox::Yes:
			if (saveTone()) break;
			else return;
		case QMessageBox::No: break;
		default: return;
		}
	}

	QString file = QFileDialog::getOpenFileName(this, "Open tone", QString::fromStdString(tone_->path), "FM tone file (*.tone)");
	if (!file.isNull()) {
		tone_ = ToneFile::read(file.toStdString());
		for (int i = 1; i <= 6; ++i) {
			SetFMTone(i);
		}
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
	bool isExist = false;
	if (tone_->path != "./") {
		std::ifstream check(tone_->path, std::ios::in | std::ios::binary);
		isExist = check.is_open();
	}

	if (isExist) {
		ToneFile::write(tone_->path, tone_.get());
		setWindowModified(false);
		setWindowTitle("YM2608 Tone Editor - " + QString::fromStdString(tone_->name) + "[*]");
	}
	else {
		return saveToneAs();
	}

	return true;
}

bool MainWindow::saveToneAs()
{
	QString file = QFileDialog::getSaveFileName(this, "Save tone", QString::fromStdString(tone_->path), "FM tone file (*.tone)");
	if (file.isNull()) return false;
	if (!file.endsWith(".tone")) file += ".tone";   // For Linux

	tone_->path = file.toStdString();
	ToneFile::write(tone_->path, tone_.get());

	setWindowModified(false);
	setWindowTitle("YM2608 Tone Editor - " + QString::fromStdString(tone_->name) + "[*]");

	return true;
}

void MainWindow::on_nameButton_clicked()
{
	NameDialog dialog(this);
	if (dialog.exec() == QDialog::Accepted) {
		ui->nameLabel->setText(dialog.toneName());
		tone_->name = dialog.toneName().toStdString();
		setWindowModified(true);
		setWindowTitle("YM2608 Tone Editor - " + dialog.toneName() + "[*]");
	}
}

void MainWindow::on_actionConvert_To_Text_C_triggered()
{
	QString str = QString::fromStdString(converter_.toneToText(tone_.get()));
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
	SetupDialog dialog(settings_, converter_, this);
	if (dialog.exec() == QDialog::Accepted) {
		converter_.setOutputFormat(dialog.outputFormat().toStdString());
		converter_.setInputFormats(dialog.inputFormats());
		settings_.setDuration(dialog.duration());
		settings_.setRate(dialog.rate());

		audio_.setDuration(settings_.getDuration());
		audio_.setRate(settings_.getRate());
		chip_.setRate(settings_.getRate());
	}
}

void MainWindow::on_actionAbout_A_triggered()
{
	AboutDialog dialog;
	dialog.exec();
}

void MainWindow::on_actionRead_Text_R_triggered()
{
	FmInEnvelopeFormats formats = converter_.getInputFormats();
	std::vector<QString> types;
	types.reserve(formats.size());
	std::transform(formats.begin(), formats.end(), std::back_inserter(types),
				   [](const auto& set) { return utf8ToQString(set.first); });
	ReadTextDialog dialog(types, this);
	if (dialog.exec() == QDialog::Accepted) {
		if (!formats.empty()){
			if (std::unique_ptr<Tone> tone = converter_.textToTone(dialog.text(), dialog.type())) {
				tone_ = std::move(tone);
				for (int i = 1; i <= 6; ++i) {
					SetFMTone(i);
				}
				setWindowModified(true);
				setWindowTitle("YM2608 Tone Editor - [*]");
				return;
			}
		}
		// Error
		QMessageBox::critical(this, "Error", "Failed to read tone data from text.");
	}
}
