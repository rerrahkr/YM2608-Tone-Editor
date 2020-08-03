#include "fmenvelopeformatdialog.hpp"
#include "ui_fmenvelopeformatdialog.h"

FmEnvelopeFormatDialog::FmEnvelopeFormatDialog(const QString& text, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::FmEnvelopeFormatDialog)
{
	ui->setupUi(this);

	ui->plainTextEdit->setPlainText(text);
}

FmEnvelopeFormatDialog::~FmEnvelopeFormatDialog()
{
	delete ui;
}

QString FmEnvelopeFormatDialog::getText() const
{
	return ui->plainTextEdit->toPlainText();
}
