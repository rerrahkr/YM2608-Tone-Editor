#include "tonetextdialog.h"
#include "ui_tonetextdialog.h"

ToneTextDialog::ToneTextDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ToneTextDialog)
{
    ui->setupUi(this);

	setWindowFlag(Qt::WindowContextHelpButtonHint, false);
}

void ToneTextDialog::setText(QString text)
{
    ui->textEdit->setText(text);
}

ToneTextDialog::~ToneTextDialog()
{
    delete ui;
}
