#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <algorithm>
#include <fstream>
#include <cstdint>
#include <utility>
#include <QKeyEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QMimeData>
#include "namedialog.h"
#include "setupdialog.h"
#include "aboutdialog.h"
#include "readtextdialog.hpp"
#include "text_conversion.hpp"
#include "tonetextdialog.h"
#include "io/file_io.hpp"

const std::unordered_map<int, int> MainWindow::NOTE_NUM_MAP_ = {
	{ Qt::Key_Z, 0 },
	{ Qt::Key_S, 1 },
	{ Qt::Key_X, 2 },
	{ Qt::Key_D, 3 },
	{ Qt::Key_C, 4 },
	{ Qt::Key_V, 5 },
	{ Qt::Key_G, 6 },
	{ Qt::Key_B, 7 },
	{ Qt::Key_H, 8 },
	{ Qt::Key_N, 9 },
	{ Qt::Key_J, 10 },
	{ Qt::Key_M, 11 },
	{ Qt::Key_Comma, 12 },
	{ Qt::Key_L, 13 },
	{ Qt::Key_Period, 14 },
	{ Qt::Key_Semicolon, 15 },
	{ Qt::Key_Slash, 16 },
	{ Qt::Key_Q, 12 },
	{ Qt::Key_2, 13 },
	{ Qt::Key_W, 14 },
	{ Qt::Key_3, 15 },
	{ Qt::Key_E, 16 },
	{ Qt::Key_R, 17 },
	{ Qt::Key_5, 18 },
	{ Qt::Key_T, 19 },
	{ Qt::Key_6, 20 },
	{ Qt::Key_Y, 21 },
	{ Qt::Key_7, 22 },
	{ Qt::Key_U, 23 },
	{ Qt::Key_I, 24 },
	{ Qt::Key_9, 25 },
	{ Qt::Key_O, 26 },
	{ Qt::Key_0, 27 },
	{ Qt::Key_P, 28 },
};

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	settings_(),
	chip_(3993600 * 2, settings_.getRate()),
	audio_(chip_, chip_.getRate(), 40),
	octave_(4),
	pressedKeyNameFM_("FM: "),
	pressedKeyNamePSG_("PSG: "),
	jamKeyOnFunc_(&MainWindow::JamKeyOnFM),
	jamKeyOffFunc_(&MainWindow::JamKeyOffFM)
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
	auto key = event->key();

	if (NOTE_NUM_MAP_.count(key)) {
		(this->*jamKeyOnFunc_)(NOTE_NUM_MAP_.at(key), isRepeat);
		return;
	}

	switch (event->key()) {
	case Qt::Key_F1:
		if (octave_ > 1) --octave_;
		break;
	case Qt::Key_F2:
		if (octave_ < 7) ++octave_;
		break;
	case Qt::Key_F3:
		if (jamKeyOnFunc_ == &MainWindow::JamKeyOnFM) {
			jamKeyOnFunc_ = &MainWindow::JamKeyOnPSG;
			jamKeyOffFunc_ = &MainWindow::JamKeyOffPSG;
		}
		else {
			jamKeyOnFunc_ = &MainWindow::JamKeyOnFM;
			jamKeyOffFunc_ = &MainWindow::JamKeyOffFM;
		}
		break;
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
	case Qt::Key_Escape:
		close();
		break;
	default:
		break;
	}
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
	bool isRepeat = event->isAutoRepeat();
	auto key = event->key();

	if (NOTE_NUM_MAP_.count(key)) {
		(this->*jamKeyOffFunc_)(NOTE_NUM_MAP_.at(key), isRepeat);
	}
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
	auto mime = event->mimeData();
	if (mime->hasUrls() && mime->urls().length() == 1) {
		switch (FileIo::getInstance().detectFileType(mime->urls().first().toLocalFile())) {
		case FileIo::FileType::Unknown:
			break;
		default:
			event->acceptProposedAction();
			break;
		}
	}
}

void MainWindow::dropEvent(QDropEvent* event)
{
	QString file = event->mimeData()->urls().first().toLocalFile();

	switch (FileIo::getInstance().detectFileType(file)) {
	case FileIo::FileType::SingleTone:
		loadSingleTone(file);
		break;
	case FileIo::FileType::ToneBank:
		// TODO
		break;
	default:
		break;
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
		SetFMKey(ch, (octave_ + jamKeyNumber / 12), (jamKeyNumber % 12));
		SetFMKeyOn(ch);
		int value = (1 << (ch + 7)) | (octave_ * 12 + jamKeyNumber);
		jamKeyOnTableFM_.push_back(value);

		pressedKeyNameFM_ = "FM: " + keyNumberToNameString(jamKeyNumber);
		pressedKeyNameFM_ += QString::number(octave_ + jamKeyNumber / 12);
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
		SetPSGKey(ch, (octave_ + jamKeyNumber / 12), (jamKeyNumber % 12));
		SetPSGKeyOn(ch);
		int value = (1 << (ch + 7)) | (octave_ * 12 + jamKeyNumber);
		jamKeyOnTablePSG_.push_back(value);

		pressedKeyNamePSG_ = "PSG: " + keyNumberToNameString(jamKeyNumber);
		pressedKeyNamePSG_ += QString::number(octave_ + jamKeyNumber / 12);
		ui->statusBar->showMessage(pressedKeyNameFM_ + " | " + pressedKeyNamePSG_);
	}
}

void MainWindow::JamKeyOffFM(int jamKeyNumber, bool isRepeat)
{
	if (!isRepeat) {
		int value = octave_ * 12 + jamKeyNumber;
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
		int value = octave_ * 12 + jamKeyNumber;
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
	uint32_t fnum = FM_FNUM_TBL_[keynum];
	data = block << 3;
	data = data | (fnum >> 8);
	chip_.setRegister(0xa4 + bch, data);
	data = fnum & 0xff;
	chip_.setRegister(0xa0 + bch, data);
}

void MainWindow::SetPSGKey(int channel, int octave, int keynum)
{
	uint32_t offset = 2 * (channel - 1);

	uint32_t data = PSG_PITCH_TBL_[keynum];
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

void MainWindow::loadSingleTone(const QString& file)
{
	if (Tone* t = FileIo::getInstance().loadSingleToneFrom(file)) {
		tone_.reset(t);
		for (int i = 1; i <= 6; ++i) {
			SetFMTone(i);
		}
	}
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

	QStringList filters {
		"FM tone file (*.tone)"
	};
	QString file = QFileDialog::getOpenFileName(this, "Open tone", QString::fromStdString(tone_->path), filters.join(";;"));
	if (!file.isNull()) {
		loadSingleTone(file);
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
		FileIo::getInstance().saveSingleToneFrom(utf8ToQString(tone_->path), *tone_);
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
	QStringList filters {
		"FM tone file (*.tone)"
	};
	QString file = QFileDialog::getSaveFileName(this, "Save tone", QString::fromStdString(tone_->path), filters.join(";;"));
	if (file.isNull()) return false;
	if (!file.endsWith(".tone")) file += ".tone";   // For Linux

	tone_->path = file.toStdString();
	FileIo::getInstance().saveSingleToneFrom(utf8ToQString(tone_->path), *tone_);

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
	if (converter_.getOutputFormats().empty())
		QMessageBox::critical(this, "Error", "No output format is saved.");

	ToneTextDialog diag(*tone_, converter_);
	diag.exec();
}

void MainWindow::on_actionSetup_E_triggered()
{
	SetupDialog dialog(settings_, converter_, this);
	if (dialog.exec() == QDialog::Accepted) {
		converter_.setOutputFormats(dialog.outputFormats());
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
