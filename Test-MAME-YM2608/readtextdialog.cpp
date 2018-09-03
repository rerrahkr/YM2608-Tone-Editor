#include "readtextdialog.hpp"
#include "ui_readtextdialog.h"

ReadTextDialog::ReadTextDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ReadTextDialog)
{
	ui->setupUi(this);

	setWindowFlag(Qt::WindowContextHelpButtonHint, false);
}

ReadTextDialog::~ReadTextDialog()
{
	delete ui;
}

QString ReadTextDialog::text() const
{
	return ui->plainTextEdit->toPlainText();
}
