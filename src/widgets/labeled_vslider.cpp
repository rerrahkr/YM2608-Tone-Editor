#include "labeled_vslider.h"
#include "ui_labeled_vslider.h"
#include "sliderstyle.hpp"

LabeledVSlider::LabeledVSlider(QWidget *parent, const QString str) :
	QWidget(parent),
	ui(new Ui::LabeledVSlider),
	str_(str)
{
	ui->setupUi(this);
	ui->label->setText(str_ + QString::number(ui->verticalSlider->value()));
	ui->verticalSlider->setStyle(new SliderStyle());
}

LabeledVSlider::~LabeledVSlider()
{
	delete ui;
}

int LabeledVSlider::value() const
{
	return ui->verticalSlider->value();
}

void LabeledVSlider::setValue(int value)
{
	ui->verticalSlider->setValue(value);
}

int LabeledVSlider::maximum() const
{
	return ui->verticalSlider->maximum();
}

void LabeledVSlider::setMaximum(int value)
{
	ui->verticalSlider->setMaximum(value);
}

int LabeledVSlider::minimum() const
{
	return ui->verticalSlider->minimum();
}

void LabeledVSlider::setMinimum(int value)
{
	ui->verticalSlider->setMinimum(value);
}

QString LabeledVSlider::text() const
{
	return str_;
}

void LabeledVSlider::setText(const QString str)
{
	str_ = str;
	ui->label->setText(str_ + QString::number(ui->verticalSlider->value()));
}

void LabeledVSlider::on_verticalSlider_valueChanged(int value)
{
	ui->label->setText(str_ + QString::number(value));

	emit valueChanged(value);
}
