#include "readtextdialog.hpp"
#include "ui_readtextdialog.h"

ReadTextDialog::ReadTextDialog(std::vector<QString> types, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ReadTextDialog)
{
	ui->setupUi(this);

	setWindowFlag(Qt::WindowContextHelpButtonHint, false);

	for (const QString& type : types) ui->comboBox->addItem(type);
}

ReadTextDialog::~ReadTextDialog()
{
	delete ui;
}

QString ReadTextDialog::type() const
{
	return ui->comboBox->currentText();
}

QString ReadTextDialog::text() const
{
	return ui->plainTextEdit->toPlainText();
}
