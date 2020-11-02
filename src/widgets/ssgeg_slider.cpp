#include "ssgeg_slider.hpp"
#include "ui_ssgeg_slider.h"
#include <QSignalBlocker>
#include "sliderstyle.hpp"

SSGEGSlider::SSGEGSlider(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::SSGEGSlider),
	str_("EG: ")
{
	ui->setupUi(this);
	ui->label->setText(str_ + QString::number(value()));
	ui->verticalSlider->setStyle(new SliderStyle());
	ui->verticalSlider->setMaximum(7);
}

SSGEGSlider::~SSGEGSlider()
{
	delete ui;
}

int SSGEGSlider::value() const
{
	return (ui->checkBox->isChecked() ? 8 : 0) + ui->verticalSlider->value();
}

void SSGEGSlider::setValue(int value)
{
	QSignalBlocker b1(ui->checkBox), b2(ui->verticalSlider);
	ui->checkBox->setChecked(value & 8);
	ui->verticalSlider->setValue(value & 7);

	ui->label->setText(str_ + QString::number(value));
	emit valueChanged(value);
}

void SSGEGSlider::on_checkBox_stateChanged(int arg1)
{
	int v = ((arg1 == Qt::Checked) ? 8 : 0) + ui->verticalSlider->value();
	ui->label->setText(str_ + QString::number(v));
	emit valueChanged(v);
}

void SSGEGSlider::on_verticalSlider_valueChanged(int value)
{
	int v = (ui->checkBox->isChecked() ? 8 : 0) + value;
	ui->label->setText(str_ + QString::number(v));
	emit valueChanged(v);
}
