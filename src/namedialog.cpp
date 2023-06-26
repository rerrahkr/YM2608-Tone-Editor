#include "namedialog.h"
#include "ui_namedialog.h"

NameDialog::NameDialog(const QString& name, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NameDialog)
{
    ui->setupUi(this);

    ui->lineEdit->setText(name);

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
