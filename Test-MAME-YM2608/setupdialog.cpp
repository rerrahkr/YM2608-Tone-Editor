#include "setupdialog.h"
#include "ui_setupdialog.h"
#include <QString>
#include <QRadioButton>

SetupDialog::SetupDialog(Settings& settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetupDialog)
{
    ui->setupUi(this);

	setFixedSize(size());
	setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);
	setWindowFlag(Qt::WindowContextHelpButtonHint, false);

    rateMap_[ui->buttonGroup->id(ui->radioButton44100)] = 44100;
    rateMap_[ui->buttonGroup->id(ui->radioButton48000)] = 48000;
    rateMap_[ui->buttonGroup->id(ui->radioButton110933)] = 110933;

    ui->horizontalSlider->setValue(settings.getDuration());
    for (auto& pair : rateMap_) {
        if (pair.second == settings.getRate()) {
            ui->buttonGroup->button(pair.first)->setChecked(true);
        }
    }
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

void SetupDialog::on_horizontalSlider_valueChanged(int value)
{
    ui->label->setText(QString::number(value) + "ms");
}
