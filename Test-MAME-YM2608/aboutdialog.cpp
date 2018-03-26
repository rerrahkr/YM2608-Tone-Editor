#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::AboutDialog)
{
	ui->setupUi(this);

	setFixedSize(size());
	setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);
	setWindowFlag(Qt::WindowContextHelpButtonHint, false);

	ui->label->setText(
				"YM2608 Tone Editor\n"
				"Version 0.1.0\n"
				"Copyright © 2018 Rerrah"
				);
}

AboutDialog::~AboutDialog()
{
	delete ui;
}
