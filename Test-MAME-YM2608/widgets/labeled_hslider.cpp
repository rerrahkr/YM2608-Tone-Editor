#include "labeled_hslider.hpp"
#include "ui_labeled_hslider.h"
#include "sliderstyle.hpp"

LabeledHSlider::LabeledHSlider(QWidget *parent, const QString str) :
	QWidget(parent),
	ui(new Ui::LabeledHSlider),
	str_(str)
{
	ui->setupUi(this);
	ui->label->setText(str_ + QString::number(ui->horizontalSlider->value()));
	ui->horizontalSlider->setStyle(new SliderStyle());
}

LabeledHSlider::~LabeledHSlider()
{
	delete ui;
}

int LabeledHSlider::value() const
{
	return ui->horizontalSlider->value();
}

void LabeledHSlider::setValue(int value)
{
	ui->horizontalSlider->setValue(value);
}

int LabeledHSlider::maximum() const
{
	return ui->horizontalSlider->maximum();
}

void LabeledHSlider::setMaximum(int value)
{
	ui->horizontalSlider->setMaximum(value);
}

int LabeledHSlider::minimum() const
{
	return ui->horizontalSlider->minimum();
}

void LabeledHSlider::setMinimum(int value)
{
	ui->horizontalSlider->setMinimum(value);
}

QString LabeledHSlider::text() const
{
	return str_;
}

void LabeledHSlider::setText(const QString str)
{
	str_ = str;
	ui->label->setText(str_ + QString::number(ui->horizontalSlider->value()));
}

void LabeledHSlider::on_horizontalSlider_valueChanged(int value)
{
	ui->label->setText(str_ + QString::number(value));

	emit valueChanged(value);
}
