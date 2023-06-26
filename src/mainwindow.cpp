#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <algorithm>
#include <fstream>
#include <cstdint>
#include <utility>
#include <QKeyEvent>
#include <QFileDialog>
#include <QMimeData>
#include <QListWidgetItem>
#include <QSignalBlocker>
#include <QVariant>
#include "namedialog.h"
#include "setupdialog.h"
#include "aboutdialog.h"
#include "readtextdialog.hpp"
#include "text_conversion.hpp"
#include "tonetextdialog.h"
#include "io/file_io.hpp"
#include "io/file_io_error.hpp"

Q_DECLARE_METATYPE(TonePtr)

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
	{ Qt::Key_P, 28 }
};

const QString MainWindow::NOTE_NAME_TBL_[12] = {
	"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
};

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	settings_(),
	chip_(settings_.getEmulation(), 3993600 * 2, settings_.getRate()),
	audio_(chip_, chip_.getRate(), 40),
	octave_(4),
	pressedKeyNameFM_("FM: "),
	pressedKeyNamePSG_("PSG: "),
	jamKeyOnFunc_(&MainWindow::JamKeyOnFM),
	jamKeyOffFunc_(&MainWindow::JamKeyOffFM)
{
	ui->setupUi(this);

	setWindowTitle("YM2608 Tone Editor[*]");

	ui->statusBar->showMessage(pressedKeyNameFM_ + " | " + pressedKeyNamePSG_);

	ui->freqSlider->setText("FREQ.: ");
	ui->pmsSlider->setText("PMS: ");
	ui->amsSlider->setText("AMS: ");
	ui->alSlider->setText("AL: ");
	ui->fbSlider->setText("FB: ");

	ui->freqSlider->setMaximum(7);
	ui->pmsSlider->setMaximum(7);
	ui->amsSlider->setMaximum(3);
	ui->alSlider->setMaximum(7);
	ui->fbSlider->setMaximum(7);

	connect(ui->freqSlider, &LabeledHSlider::valueChanged, this, &MainWindow::onFreqChanged);
	connect(ui->pmsSlider, &LabeledHSlider::valueChanged, this, &MainWindow::onPMSChanged);
	connect(ui->amsSlider, &LabeledHSlider::valueChanged, this, &MainWindow::onAMSChanged);
	connect(ui->alSlider, &LabeledHSlider::valueChanged, this, &MainWindow::onALChanged);
	connect(ui->fbSlider, &LabeledHSlider::valueChanged, this, &MainWindow::onFBChanged);
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

	nameFontMet_ = std::make_unique<QFontMetrics>(ui->nameLabel->font());

	chip_.setRegister(0x29, 0x80);	// Init interrupt
	chip_.setRegister(0x07, 0xff);  // PSG mix
	chip_.setRegister(0x11, 0x3f);  // Drum total volume

	addToneTo(0);
	setWindowModified(false);

	ui->listWidget->installEventFilter(this);
}

MainWindow::~MainWindow()
{
	delete ui;
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
	if (obj == ui->listWidget) {
		switch (event->type()) {
		case QEvent::KeyPress:
		{
			auto ke = reinterpret_cast<QKeyEvent*>(event);
			keyPressEvent(ke);
			if (NOTE_NUM_MAP_.count(ke->key())) return true;
			switch (ke->key()) {
			case Qt::Key_Insert:
				on_newTonePushButton_clicked();
				return true;
            case Qt::Key_Delete:
            {
                auto items = ui->listWidget->selectedItems();
                if (!items.size()) {	// No selection
                    removeToneAt(ui->listWidget->currentRow());
                }
                else {
                    for (const auto& item : items)
                        removeToneAt(ui->listWidget->row(item));
                }
                return true;
            }
			default:
				break;
			}
			break;
		}
		case QEvent::KeyRelease:
			keyReleaseEvent(reinterpret_cast<QKeyEvent*>(event));
			break;
		default:
			break;
		}
	}

	return false;
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
	Q_UNUSED(event)
	ui->nameLabel->setText(modifyDisplayedToneName(ui->nameLabel->text()));
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
		if (octave_ > 0) --octave_;
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
		for (int i = 1; i <= 6; ++i) {
			SetFMTone(i);
		}
		// Clear key on table
		jamKeyOnTableFM_.clear();
		jamKeyOnTablePSG_.clear();
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
	if (mime->hasUrls()) {
		const auto urls = mime->urls();
		for (auto& url : urls) {
			switch (io::detectFileType(url.toLocalFile())) {
			case io::FileType::Unknown:
				return;
			default:
				break;
			}
		}
		event->acceptProposedAction();
	}
}

void MainWindow::dropEvent(QDropEvent* event)
{
	const auto urls = event->mimeData()->urls();
	for (const auto& url : urls) {
		QString file = url.toLocalFile();
		switch (io::detectFileType(file)) {
		case io::FileType::SingleTone:	loadSingleTone(file);	break;
		case io::FileType::ToneBank:	loadToneBank(file);		break;
		case io::FileType::SongFile:	loadSongFile(file);		break;
		default:	break;
		}
	}
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	if (isWindowModified()) {
		switch (showSaveWarning()) {
		case QMessageBox::Yes:
			if (!saveTone()) event->ignore();
			break;
		case QMessageBox::No: break;
		default: event->ignore(); break;
		}
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
		int oct = octave_ + jamKeyNumber / 12;
		if (7 < oct) return;
		SetFMKey(ch, oct, (jamKeyNumber % 12));
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
		int oct = octave_ + jamKeyNumber / 12;
		if (7 < oct) return;
		SetPSGKey(ch, oct, (jamKeyNumber % 12));
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
	auto tone = getCurrentTone();
    if (tone == nullptr) return;
	data = tone->FB << 3;
	data += tone->AL;
	chip_.setRegister(0xb0 + bch, data);

	for (int i = 0; i < 4; i++) {
		uint32_t offset;
		switch (i) {
		case 0: offset = 0;  break;
		case 1: offset = 8;  break;
		case 2: offset = 4;  break;
		case 3: offset = 12; break;
		}

		data = tone->op[i].DT << 4;
		data |= tone->op[i].ML;
		chip_.setRegister(0x30 + bch + offset, data);

		data = tone->op[i].TL;
		chip_.setRegister(0x40 + bch + offset, data);

		data = tone->op[i].KS << 6;
		data |= tone->op[i].AR;
		chip_.setRegister(0x50 + bch + offset, data);

		data = tone->op[i].AM << 7;
		data |= tone->op[i].DR;
		chip_.setRegister(0x60 + bch + offset, data);

		data = tone->op[i].SR;
		chip_.setRegister(0x70 + bch + offset, data);

		data = tone->op[i].SL << 4;
		data |= tone->op[i].RR;
		chip_.setRegister(0x80 + bch + offset, data);

		data = tone->op[i].SSGEG;
		chip_.setRegister(0x90 + bch + offset, data);
	}

	data = tone->FREQ_LFO;
	chip_.setRegister(0x22, data);
	data = 0xc0 | (tone->AMS_LFO << 4) | tone->PMS_LFO;
	chip_.setRegister(0xb4 + bch, data);
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

QString MainWindow::keyNumberToNameString(int jamKeyNumber)
{
	return NOTE_NAME_TBL_[jamKeyNumber % 12];
}

QString MainWindow::modifyDisplayedToneName(const QString& src) const
{
	return nameFontMet_->elidedText(src, Qt::ElideRight, ui->nameLabel->width());
}

void MainWindow::on_lfoGroupBox_clicked(bool checked)
{
	auto tone = getCurrentTone();
    if (tone == nullptr) return;
	tone->FREQ_LFO = (checked ? 8 : 0) | ui->freqSlider->value();
	chip_.setRegister(0x22, tone->FREQ_LFO);

	setWindowModified(true);
}

void MainWindow::onFreqChanged(int value)
{
	auto tone = getCurrentTone();
    if (tone == nullptr) return;
	tone->FREQ_LFO = (ui->lfoGroupBox->isChecked() ? 8 : 0) | value;
	chip_.setRegister(0x22, tone->FREQ_LFO);

	setWindowModified(true);
}

void MainWindow::onPMSChanged(int value)
{
	auto tone = getCurrentTone();
    if (tone == nullptr) return;
	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 3; ++j) {
			uint32_t chOffset = 0x100 * i + j;
			tone->PMS_LFO = value;
			uint8_t reg = 0xc0 | (tone->AMS_LFO << 4) | value;
			chip_.setRegister(0xb4 + chOffset, reg);
		}
	}

	setWindowModified(true);
}

void MainWindow::onAMSChanged(int value)
{
	auto tone = getCurrentTone();
    if (tone == nullptr) return;
	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 3; ++j) {
			uint32_t chOffset = 0x100 * i + j;
			tone->AMS_LFO = value;
			uint8_t reg = 0xc0 | (value << 4) | tone->PMS_LFO;
			chip_.setRegister(0xb4 + chOffset, reg);
		}
	}

	setWindowModified(true);
}

void MainWindow::onALChanged(int value)
{
	auto tone = getCurrentTone();
    if (tone == nullptr) return;
	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 3; ++j) {
			uint32_t chOffset = 0x100 * i + j;
			tone->AL = value;
			uint8_t reg = (tone->FB << 3) | value;
			chip_.setRegister(0xb0 + chOffset, reg);
		}
	}

	setWindowModified(true);
}

void MainWindow::onFBChanged(int value)
{
	auto tone = getCurrentTone();
    if (tone == nullptr) return;
	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 3; ++j) {
			uint32_t chOffset = 0x100 * i + j;
			tone->FB = value;
			uint8_t reg = (value << 3) | tone->AL;
			chip_.setRegister(0xb0 + chOffset, reg);
		}
	}

	setWindowModified(true);
}

void MainWindow::onParameterChanged(int op, OperatorParameter param, int value)
{
    auto tone = getCurrentTone();
    if (tone == nullptr) return;

	uint8_t v = static_cast<uint8_t>(value);
    Operator& slot = tone->op[op - 1];

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
			switch (param) {
			case OperatorParameter::AR:
				slot.AR = v;
				reg = (slot.KS << 6) | slot.AR;
				chip_.setRegister(0x50 + chOffset + opOffset, reg);
				break;
			case OperatorParameter::DR:
				slot.DR = v;
				reg = (slot.AM << 7) | slot.DR;
				chip_.setRegister(0x60 + chOffset + opOffset, reg);
				break;
			case OperatorParameter::SR:
				slot.SR = v;
				reg = slot.SR;
				chip_.setRegister(0x70 + chOffset + opOffset, reg);
				break;
			case OperatorParameter::RR:
				slot.RR = v;
				reg = (slot.SL << 4) | slot.RR;
				chip_.setRegister(0x80 + chOffset + opOffset, reg);
				break;
			case OperatorParameter::SL:
				slot.SL = v;
				reg = (slot.SL << 4) | slot.RR;
				chip_.setRegister(0x80 + chOffset + opOffset, reg);
				break;
			case OperatorParameter::TL:
				slot.TL = v;
				reg = slot.TL;
				chip_.setRegister(0x40 + chOffset + opOffset, reg);
				break;
			case OperatorParameter::KS:
				slot.KS = v;
				reg = (slot.KS << 6) | slot.AR;
				chip_.setRegister(0x50 + chOffset + opOffset, reg);
				break;
			case OperatorParameter::ML:
				slot.ML = v;
				reg = (slot.DT << 4) | slot.ML;
				chip_.setRegister(0x30 + chOffset + opOffset, reg);
				break;
			case OperatorParameter::DT:
				slot.DT = v;
				reg = (slot.DT << 4) | slot.ML;
				chip_.setRegister(0x30 + chOffset + opOffset, reg);
				break;
			case OperatorParameter::AM:
				slot.AM = v;
				reg = (slot.AM << 7) | slot.DR;
				chip_.setRegister(0x60 + chOffset + opOffset, reg);
				break;
			case OperatorParameter::SSGEG:
				slot.SSGEG = v;
				reg = slot.SSGEG;
				chip_.setRegister(0x90 + chOffset + opOffset, reg);
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
	try {
		Tone* t = io::loadSingleToneFrom(file);
		addToneTo(ui->listWidget->count(), t);
		ui->removeTonePushButton->setEnabled(true);
	}
	catch (FileIoError& e) {
		QMessageBox::critical(this, "Error", ("Failed to load the tone.\n\n") + QString(e.what()));
	}
}

void MainWindow::loadToneBank(const QString& file)
{
	try {
		std::vector<TonePtr> bank = io::loadToneBankFrom(file);
		for (TonePtr tone : bank) addToneTo(ui->listWidget->count(), tone);
		ui->removeTonePushButton->setEnabled(true);
	}
	catch (FileIoError& e) {
		QMessageBox::critical(this, "Error", ("Failed to load the bank.\n\n") + QString(e.what()));
	}
}

void MainWindow::loadSongFile(const QString& file)
{
	try {
		std::vector<TonePtr> list = io::loadSongFileFrom(file);
		for (TonePtr tone : list) addToneTo(ui->listWidget->count(), tone);
		ui->removeTonePushButton->setEnabled(true);
	}
	catch (FileIoError& e) {
		QMessageBox::critical(this, "Error", ("Failed to load the song.\n\n") + QString(e.what()));
	}
}

void MainWindow::addToneTo(int n)
{
	addToneTo(n, TonePtr(new Tone));
}

void MainWindow::addToneTo(int n, Tone* tone)
{
	addToneTo(n, TonePtr(tone));
}

void MainWindow::addToneTo(int n, TonePtr tone)
{
	auto item = new QListWidgetItem(ui->listWidget);
	QString name = utf8ToQString(tone->name);
	item->setText(name);
	item->setData(Qt::UserRole, QVariant::fromValue(tone));
	ui->nameLabel->setText(modifyDisplayedToneName(name));
	ui->listWidget->insertItem(n, item);
	setWindowModified(true);

	ui->listWidget->clearSelection();
	ui->listWidget->setCurrentRow(n);
}

void MainWindow::removeToneAt(int n)
{
	delete ui->listWidget->takeItem(n);
	setWindowModified(true);

	ui->listWidget->clearSelection();

    if (ui->listWidget->count() == 0) return;

	ui->listWidget->setCurrentRow(std::min(n, ui->listWidget->count() - 1));

	ui->removeTonePushButton->setEnabled(ui->listWidget->count() != 1);
}

TonePtr MainWindow::getCurrentTone() const
{
    if (auto* item = ui->listWidget->currentItem()) {
        return item->data(Qt::UserRole).value<TonePtr>();
    }
    else {
        return {};
    }
}

void MainWindow::on_actionOpen_O_triggered()
{
    static QString dir("./");
    if (auto tone = getCurrentTone()) {
        dir = utf8ToQString(tone->path);
    }

	QStringList filters = io::getSingleToneLoadFilter();
    QString file = QFileDialog::getOpenFileName(this, "Open tone", dir,
												filters.join(";;"), nullptr
											#if defined(Q_OS_LINUX) || (defined(Q_OS_BSD4) && !defined(Q_OS_DARWIN))
												, QFileDialog::DontUseNativeDialog
											#endif
												);
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
	auto tone = getCurrentTone();
    if (tone == nullptr) return false;
	if (tone->path != "./" && QFile::exists(utf8ToQString(tone->path))) {
		try {
			io::saveSingleToneFrom(utf8ToQString(tone->path), *tone);
		}
		catch (FileIoError& e) {
			QMessageBox::critical(this, "Error", ("Failed to save the tone.\n\n") + QString(e.what()));
		}
		setWindowModified(false);
	}
	else {
		return saveToneAs();
	}

	return true;
}

bool MainWindow::saveToneAs()
{
	auto tone = getCurrentTone();
    if (tone == nullptr) return false;
	QStringList filters = io::getSingleToneSaveFilter();
	QString file = QFileDialog::getSaveFileName(this, "Save tone", utf8ToQString(tone->path),
												filters.join(";;"), nullptr
											#if defined(Q_OS_LINUX) || (defined(Q_OS_BSD4) && !defined(Q_OS_DARWIN))
												, QFileDialog::DontUseNativeDialog
											#endif
												);
	if (file.isNull()) return false;
	//	if (!file.endsWith(".tone")) file += ".tone";   // For Linux

	tone->path = file.toUtf8().toStdString();
	try {
		io::saveSingleToneFrom(utf8ToQString(tone->path), *tone);
	}
	catch (FileIoError& e) {
		QMessageBox::critical(this, "Error", ("Failed to save the tone.\n\n") + QString(e.what()));
	}

	setWindowModified(false);

	return true;
}

void MainWindow::on_nameButton_clicked()
{
    auto tone = getCurrentTone();
    if (tone == nullptr) return;

    NameDialog dialog(utf8ToQString(tone->name), this);
	if (dialog.exec() == QDialog::Accepted) {
		ui->nameLabel->setText(modifyDisplayedToneName(dialog.toneName()));
		ui->listWidget->currentItem()->setText(dialog.toneName());
        tone->name = dialog.toneName().toUtf8().toStdString();
		setWindowModified(true);
	}
}

void MainWindow::on_actionConvert_To_Text_C_triggered()
{
    auto tone = getCurrentTone();
    if (tone == nullptr) return;

	if (converter_.getOutputFormats().empty())
		QMessageBox::critical(this, "Error", "No output format is saved.");

    ToneTextDialog diag(*tone, converter_);
	diag.exec();
}

void MainWindow::on_actionSetup_E_triggered()
{
	SetupDialog dialog(settings_, converter_, this);
	if (dialog.exec() == QDialog::Accepted) {
		converter_.setOutputFormats(dialog.outputFormats());
		converter_.setInputFormats(dialog.inputFormats());
		settings_.setEmulation(dialog.emulation());
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
			if (Tone* tone = converter_.textToTone(dialog.text(), dialog.type())) {
				addToneTo(ui->listWidget->count(), tone);
				setWindowModified(true);
				return;
			}
		}
		// Error
		QMessageBox::critical(this, "Error", "Failed to read tone data from text.");
	}
}

void MainWindow::on_newTonePushButton_clicked()
{
	addToneTo(ui->listWidget->currentRow());

	ui->removeTonePushButton->setEnabled(true);
}

void MainWindow::on_removeTonePushButton_clicked()
{
	int row = ui->listWidget->currentRow();
	if (row != -1) removeToneAt(row);
}

void MainWindow::on_listWidget_currentRowChanged(int currentRow)
{
	Q_UNUSED(currentRow)
	auto tone = getCurrentTone();
    if (tone == nullptr) return;
	ui->nameLabel->setText(modifyDisplayedToneName(utf8ToQString(tone->name)));
	QSignalBlocker blfo(ui->lfoGroupBox), bfreq(ui->freqSlider), bpms(ui->pmsSlider);
	QSignalBlocker bams(ui->amsSlider), bal(ui->alSlider), bfb(ui->fbSlider);
	ui->lfoGroupBox->setChecked((tone->FREQ_LFO & 0x08) != 0);
	ui->freqSlider->setValue(tone->FREQ_LFO & 0x07);
	ui->pmsSlider->setValue(tone->PMS_LFO);
	ui->amsSlider->setValue(tone->AMS_LFO);
	ui->alSlider->setValue(tone->AL);
	ui->fbSlider->setValue(tone->FB);
	for (int i = 0; i< 4; ++i) {
		QSignalBlocker bop(sliders_[i]);
		sliders_[i]->setParameterValue(OperatorParameter::AR, tone->op[i].AR);
		sliders_[i]->setParameterValue(OperatorParameter::DR, tone->op[i].DR);
		sliders_[i]->setParameterValue(OperatorParameter::SR, tone->op[i].SR);
		sliders_[i]->setParameterValue(OperatorParameter::RR, tone->op[i].RR);
		sliders_[i]->setParameterValue(OperatorParameter::SL, tone->op[i].SL);
		sliders_[i]->setParameterValue(OperatorParameter::TL, tone->op[i].TL);
		sliders_[i]->setParameterValue(OperatorParameter::KS, tone->op[i].KS);
		sliders_[i]->setParameterValue(OperatorParameter::ML, tone->op[i].ML);
		sliders_[i]->setParameterValue(OperatorParameter::DT, tone->op[i].DT);
		sliders_[i]->setParameterValue(OperatorParameter::AM, tone->op[i].AM);
		sliders_[i]->setParameterValue(OperatorParameter::SSGEG, tone->op[i].SSGEG);
	}

	for (int i = 1; i <= 6; ++i) {
		SetFMTone(i);
	}
}

void MainWindow::on_actionSave_Bank_As_triggered()
{
	auto items = ui->listWidget->selectedItems();
	if (items.empty()) {
		QMessageBox::information(this, "Information", "No tone saved as a bank is selected.");
		return;
	}

	QStringList filters = io::getToneBankSaveFilter();
	QString file = QFileDialog::getSaveFileName(this, "Save bank", ".", filters.join(";;"), nullptr
											#if defined(Q_OS_LINUX) || (defined(Q_OS_BSD4) && !defined(Q_OS_DARWIN))
												, QFileDialog::DontUseNativeDialog
											#endif
												);
	if (file.isNull()) return;

	std::vector<TonePtr> bank;
	bank.reserve(items.size());
	std::transform(items.begin(), items.end(), std::back_inserter(bank),
				   [](const QListWidgetItem* item) { return item->data(Qt::UserRole).value<TonePtr>(); });

	try {
		io::saveToneBankFrom(file, bank);
	}
	catch (FileIoError& e) {
		QMessageBox::critical(this, "Error", ("Failed to save the bank.\n\n") + QString(e.what()));
	}

	setWindowModified(false);
}

void MainWindow::on_actionO_pen_Bank_triggered()
{
	QStringList filters = io::getToneBankLoadFilter();
	QString file = QFileDialog::getOpenFileName(this, "Open bank", ".", filters.join(";;"), nullptr
											#if defined(Q_OS_LINUX) || (defined(Q_OS_BSD4) && !defined(Q_OS_DARWIN))
												, QFileDialog::DontUseNativeDialog
											#endif
												);
	if (!file.isNull()) {
		loadToneBank(file);
	}
}

void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
	for (int row = 0; row < ui->listWidget->count(); ++row) {
		QListWidgetItem* item = ui->listWidget->item(row);
		bool accept = arg1.isEmpty() || item->text().contains(arg1, Qt::CaseInsensitive);
		item->setHidden(!accept);
	}
}

void MainWindow::on_actionOpe_n_Song_triggered()
{
	QStringList filters = io::getSongFileLoadFilter();
	QString file = QFileDialog::getOpenFileName(this, "Open song", ".", filters.join(";;"), nullptr
											#if defined(Q_OS_LINUX) || (defined(Q_OS_BSD4) && !defined(Q_OS_DARWIN))
												, QFileDialog::DontUseNativeDialog
											#endif
												);
	if (!file.isNull()) {
		loadSongFile(file);
	}
}
