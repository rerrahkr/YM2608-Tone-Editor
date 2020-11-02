#include "tonetextdialog.h"
#include "ui_tonetextdialog.h"
#include "text_conversion.hpp"

ToneTextDialog::ToneTextDialog(const Tone& tone, const ToneConverter& converter, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ToneTextDialog),
	tone_(tone),
	converter_(converter)
{
	ui->setupUi(this);

	setWindowFlag(Qt::WindowContextHelpButtonHint, false);

	FmOutEnvelopeFormats formats = converter.getOutputFormats();
	for (const auto& pair : formats) {
		ui->comboBox->addItem(utf8ToQString(pair.first));
	}
	ui->comboBox->setCurrentIndex(0);
}

ToneTextDialog::~ToneTextDialog()
{
	delete ui;
}

void ToneTextDialog::on_comboBox_currentIndexChanged(const QString &arg1)
{
	ui->textEdit->setText(converter_.toneToText(tone_, arg1));
}
