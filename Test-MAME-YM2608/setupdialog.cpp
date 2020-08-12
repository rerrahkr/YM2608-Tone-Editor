#include "setupdialog.h"
#include "ui_setupdialog.h"
#include <QRadioButton>
#include <QListWidgetItem>
#include "sliderstyle.hpp"
#include "fmenvelopeorderdialog.hpp"
#include "fmenvelopeformatdialog.hpp"
#include "text_conversion.hpp"

SetupDialog::SetupDialog(const Settings& settings, const ToneConverter& converter, QWidget *parent)
	: QDialog(parent),
	  ui(new Ui::SetupDialog)
{
	ui->setupUi(this);

	setWindowFlag(Qt::WindowContextHelpButtonHint, false);

	// Stream
	ui->emuComboBox->addItem("MAME YM2608", static_cast<int>(chip::Emu::Mame));
	ui->emuComboBox->addItem("Nuked OPN-Mod", static_cast<int>(chip::Emu::Nuked));
	for (int i = 0; i < ui->emuComboBox->count(); ++i) {
		if (ui->emuComboBox->itemData(i).toInt() == static_cast<int>(settings.getEmulation()))
			ui->emuComboBox->setCurrentIndex(i);
	}

	rateMap_[ui->buttonGroup->id(ui->radioButton44100)] = 44100;
	rateMap_[ui->buttonGroup->id(ui->radioButton48000)] = 48000;
	rateMap_[ui->buttonGroup->id(ui->radioButton55466)] = 55466;

	ui->horizontalSlider->setValue(settings.getDuration());
	ui->horizontalSlider->setStyle(new SliderStyle());
	for (auto& pair : rateMap_) {
		if (pair.second == settings.getRate()) {
			ui->buttonGroup->button(pair.first)->setChecked(true);
		}
	}

	// Input
	fmInEnvFormats_ = converter.getInputFormats();
	updateInEnvelopeSetUi();

	// Output
	fmOutEnvFormats_ = converter.getOutputFormats();
	updateOutEnvelopeSetUi();
}

SetupDialog::~SetupDialog()
{
	delete ui;
}

chip::Emu SetupDialog::emulation() const
{
	return static_cast<chip::Emu>(ui->emuComboBox->currentData().toInt());
}

unsigned int SetupDialog::duration() const
{
	return static_cast<unsigned int>(ui->horizontalSlider->value());
}

unsigned int SetupDialog::rate() const
{
	return rateMap_.at(ui->buttonGroup->checkedId());
}

FmOutEnvelopeFormats SetupDialog::outputFormats() const
{
	return fmOutEnvFormats_;
}

FmInEnvelopeFormats SetupDialog::inputFormats() const
{
	return fmInEnvFormats_;
}

void SetupDialog::on_horizontalSlider_valueChanged(int value)
{
	ui->label->setText(QString::number(value) + "ms");
}

void SetupDialog::updateInEnvelopeSetUi()
{
	ui->inEnvelopeSetListWidget->clear();
	for (auto& pair : fmInEnvFormats_)
		ui->inEnvelopeSetListWidget->addItem(utf8ToQString(pair.first));
}

void SetupDialog::on_addInEnvelopeSetPushButton_clicked()
{
	auto name = QString("Format %1").arg(fmInEnvFormats_.size() + 1);
	fmInEnvFormats_[name.toUtf8().toStdString()] = FmInEnvelopeOrders();
	updateInEnvelopeSetUi();
	for (int i = ui->inEnvelopeSetListWidget->count() - 1; i >= 0; --i) {
		if (ui->inEnvelopeSetListWidget->item(i)->text() == name) {
			ui->inEnvelopeSetListWidget->setCurrentRow(i);
			break;
		}
	}
}

void SetupDialog::on_removeInEnvelopeSetPushButton_clicked()
{
	fmInEnvFormats_.erase(ui->inEnvelopeSetListWidget->currentItem()->text().toUtf8().toStdString());
	updateInEnvelopeSetUi();
}

void SetupDialog::on_inEnvelopeSetNameLineEdit_textChanged(const QString &arg1)
{
	auto node = fmInEnvFormats_.extract(ui->inEnvelopeSetListWidget->currentItem()->text().toUtf8().toStdString());
	node.key() = arg1.toUtf8().toStdString();
	fmInEnvFormats_.insert(std::move(node));
	ui->inEnvelopeSetListWidget->currentItem()->setText(arg1);
}

void SetupDialog::on_editInEnvelopeSetPushButton_clicked()
{
	QString name = ui->inEnvelopeSetListWidget->currentItem()->text();
	FmEnvelopeOrderDialog diag(fmInEnvFormats_[name.toUtf8().toStdString()]);
	diag.setWindowTitle(diag.windowTitle() + ": " + name);
	if (diag.exec() == QDialog::Accepted) {
		fmInEnvFormats_[name.toUtf8().toStdString()] = diag.getOrder();
	}
}

void SetupDialog::on_inEnvelopeSetListWidget_currentRowChanged(int currentRow)
{
	bool cond = currentRow != -1;
	ui->editInEnvelopeSetPushButton->setEnabled(cond);
	ui->removeInEnvelopeSetPushButton->setEnabled(cond);
	ui->inEnvelopeSetNameLineEdit->setEnabled(cond);
	if (cond)
		ui->inEnvelopeSetNameLineEdit->setText(ui->inEnvelopeSetListWidget->item(currentRow)->text());
}

void SetupDialog::updateOutEnvelopeSetUi()
{
	ui->outEnvelopeSetListWidget->clear();
	for (auto& pair : fmOutEnvFormats_)
		ui->outEnvelopeSetListWidget->addItem(utf8ToQString(pair.first));
}

void SetupDialog::on_addOutEnvelopeSetPushButton_clicked()
{
	auto name = QString("Format %1").arg(fmOutEnvFormats_.size() + 1);
	fmOutEnvFormats_[name.toUtf8().toStdString()] = "";
	updateOutEnvelopeSetUi();
	for (int i = ui->outEnvelopeSetListWidget->count() - 1; i >= 0; --i) {
		if (ui->outEnvelopeSetListWidget->item(i)->text() == name) {
			ui->outEnvelopeSetListWidget->setCurrentRow(i);
			break;
		}
	}
}

void SetupDialog::on_removeOutEnvelopeSetPushButton_clicked()
{
	fmOutEnvFormats_.erase(ui->outEnvelopeSetListWidget->currentItem()->text().toUtf8().toStdString());
	updateOutEnvelopeSetUi();
}

void SetupDialog::on_outEnvelopeSetNameLineEdit_textChanged(const QString &arg1)
{
	auto node = fmOutEnvFormats_.extract(ui->outEnvelopeSetListWidget->currentItem()->text().toUtf8().toStdString());
	node.key() = arg1.toUtf8().toStdString();
	fmOutEnvFormats_.insert(std::move(node));
	ui->outEnvelopeSetListWidget->currentItem()->setText(arg1);
}

void SetupDialog::on_editOutEnvelopeSetPushButton_clicked()
{
	QString name = ui->outEnvelopeSetListWidget->currentItem()->text();
	FmEnvelopeFormatDialog diag(utf8ToQString(fmOutEnvFormats_[name.toUtf8().toStdString()]));
	diag.setWindowTitle(diag.windowTitle() + ": " + name);
	if (diag.exec() == QDialog::Accepted) {
		fmOutEnvFormats_[name.toUtf8().toStdString()] = diag.getText().toUtf8().toStdString();
	}
}

void SetupDialog::on_outEnvelopeSetListWidget_currentRowChanged(int currentRow)
{
	bool cond = currentRow != -1;
	ui->editOutEnvelopeSetPushButton->setEnabled(cond);
	ui->removeOutEnvelopeSetPushButton->setEnabled(cond);
	ui->outEnvelopeSetNameLineEdit->setEnabled(cond);
	if (cond)
		ui->outEnvelopeSetNameLineEdit->setText(ui->outEnvelopeSetListWidget->item(currentRow)->text());
}
