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
	fmEnvelopeTexts_ = converter.getInputFormats();
	updateEnvelopeSetUi();

	// Output
	ui->outFormatPlainTextEdit->setPlainText(QString::fromStdString(converter.getOutputFormat()));
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
	return ui->outFormatPlainTextEdit->toPlainText();
}

std::vector<FmEnvelopeText> SetupDialog::inputOrders()
{
	return fmEnvelopeTexts_;
}

void SetupDialog::on_horizontalSlider_valueChanged(int value)
{
    ui->label->setText(QString::number(value) + "ms");
}

void SetupDialog::on_addEnvelopeSetPushButton_clicked()
{
	auto name = QString("Set %1").arg(fmEnvelopeTexts_.size() + 1);
	fmEnvelopeTexts_.push_back({ name.toUtf8().toStdString(), std::vector<FmEnvelopeTextType>() });
	updateEnvelopeSetUi();
	for (int i = ui->envelopeSetListWidget->count() - 1; i >= 0; --i) {
		if (ui->envelopeSetListWidget->item(i)->text() == name) {
			ui->envelopeSetListWidget->setCurrentRow(i);
			break;
		}
	}
}

void SetupDialog::on_removeEnvelopeSetpushButton_clicked()
{
	fmEnvelopeTexts_.erase(fmEnvelopeTexts_.begin() + ui->envelopeSetListWidget->currentRow());
	updateEnvelopeSetUi();
}

void SetupDialog::on_editEnvelopeSetPushButton_clicked()
{
	size_t row = static_cast<size_t>(ui->envelopeSetListWidget->currentRow());
	FmEnvelopeOrderDialog diag(fmEnvelopeTexts_.at(row).texts);
	diag.setWindowTitle(diag.windowTitle() + ": " + ui->envelopeSetNameLineEdit->text());
	if (diag.exec() == QDialog::Accepted) {
		fmEnvelopeTexts_.at(row).texts = diag.getSet();
	}
}

void SetupDialog::on_envelopeSetNameLineEdit_textChanged(const QString &arg1)
{
	fmEnvelopeTexts_.at(static_cast<size_t>(ui->envelopeSetListWidget->currentRow())).name = arg1.toStdString();
	ui->envelopeSetListWidget->currentItem()->setText(arg1);
}

void SetupDialog::on_envelopeSetListWidget_currentRowChanged(int currentRow)
{
	if (currentRow == -1) {
		ui->editEnvelopeSetPushButton->setEnabled(false);
		ui->removeEnvelopeSetpushButton->setEnabled(false);
		ui->envelopeSetNameLineEdit->setEnabled(false);
	}
	else {
		ui->editEnvelopeSetPushButton->setEnabled(true);
		ui->removeEnvelopeSetpushButton->setEnabled(true);
		ui->envelopeSetNameLineEdit->setEnabled(true);
		ui->envelopeSetNameLineEdit->setText(ui->envelopeSetListWidget->item(currentRow)->text());
	}
}

void SetupDialog::updateEnvelopeSetUi()
{
	std::sort(fmEnvelopeTexts_.begin(), fmEnvelopeTexts_.end(),
			  [](const FmEnvelopeText& a, const FmEnvelopeText& b) -> bool { return (a.name < b.name); });

	ui->envelopeSetListWidget->clear();
	for (auto& texts : fmEnvelopeTexts_)
		ui->envelopeSetListWidget->addItem(utf8ToQString(texts.name));
}
