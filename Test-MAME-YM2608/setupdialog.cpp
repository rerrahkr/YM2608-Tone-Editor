#include "setupdialog.h"
#include "ui_setupdialog.h"
#include <QRadioButton>
#include <QListWidgetItem>
#include "sliderstyle.hpp"
#include "fmenvelopeorderdialog.hpp"
#include "text_conversion.hpp"

SetupDialog::SetupDialog(const Settings& settings, const ToneConverter& converter, QWidget *parent)
	: QDialog(parent),
	  ui(new Ui::SetupDialog)
{
	ui->setupUi(this);

	setWindowFlag(Qt::WindowContextHelpButtonHint, false);

	// Stream
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
	//	ui->outFormatPlainTextEdit->setPlainText(QString::fromStdString(converter.getOutputFormat()));
	updateOutEnvelopeSetUi();
}

SetupDialog::~SetupDialog()
{
	delete ui;
}

unsigned int SetupDialog::duration()
{
	return static_cast<unsigned int>(ui->horizontalSlider->value());
}

unsigned int SetupDialog::rate()
{
	return rateMap_[ui->buttonGroup->checkedId()];
}

QString SetupDialog::outputFormat()
{
	return "";//ui->outFormatPlainTextEdit->toPlainText();
}

FmInEnvelopeFormats SetupDialog::inputFormats()
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
	auto name = QString("Set %1").arg(fmInEnvFormats_.size() + 1);
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

}

void SetupDialog::on_addOutEnvelopeSetPushButton_clicked()
{

}

void SetupDialog::on_removeOutEnvelopeSetpushButton_clicked()
{

}

void SetupDialog::on_outEnvelopeSetNameLineEdit_textChanged(const QString &arg1)
{

}

void SetupDialog::on_editOutEnvelopeSetPushButton_clicked()
{

}

void SetupDialog::on_outEnvelopeSetListWidget_currentRowChanged(int currentRow)
{

}
