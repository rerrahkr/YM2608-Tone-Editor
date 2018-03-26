#include "namedialog.h"
#include "ui_namedialog.h"

NameDialog::NameDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NameDialog)
{
    ui->setupUi(this);

	setFixedSize(size());
	setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);
	setWindowFlag(Qt::WindowContextHelpButtonHint, false);
}

NameDialog::~NameDialog()
{
    delete ui;
}

QString NameDialog::toneName()
{
    return ui->lineEdit->text();
}
