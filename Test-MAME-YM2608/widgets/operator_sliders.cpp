#include "operator_sliders.h"
#include "ui_operator_sliders.h"
#include "widgets/labeled_vslider.h"

OperatorSliders::OperatorSliders(QWidget *parent, int number) :
	QWidget(parent),
	ui(new Ui::OperatorSliders),
	number_(number)
{
	ui->setupUi(this);

	ui->operatorNumberLabel->setText("Op" + QString::number(number_));

	ui->arSlider->setText("AR: ");
	ui->drSlider->setText("DR: ");
	ui->srSlider->setText("SR: ");
	ui->rrSlider->setText("RR: ");
	ui->slSlider->setText("SL: ");
	ui->tlSlider->setText("TL: ");
	ui->ksSlider->setText("KS: ");
	ui->mlSlider->setText("ML: ");
	ui->dtSlider->setText("DT: ");
	ui->amSlider->setText("AM: ");

	ui->arSlider->setMaximum(31);
	ui->drSlider->setMaximum(31);
	ui->srSlider->setMaximum(31);
	ui->rrSlider->setMaximum(15);
	ui->slSlider->setMaximum(15);
	ui->tlSlider->setMaximum(127);
	ui->ksSlider->setMaximum(3);
	ui->mlSlider->setMaximum(15);
	ui->dtSlider->setMaximum(7);
	ui->amSlider->setMaximum(1);

	connect(ui->arSlider, &LabeledVSlider::valueChanged, this,
			[&](int v) { emit parameterChanged(number_, OperatorParameter::AR, v); });
	connect(ui->drSlider, &LabeledVSlider::valueChanged, this,
			[&](int v) { emit parameterChanged(number_, OperatorParameter::DR, v); });
	connect(ui->srSlider, &LabeledVSlider::valueChanged, this,
			[&](int v) { emit parameterChanged(number_, OperatorParameter::SR, v); });
	connect(ui->rrSlider, &LabeledVSlider::valueChanged, this,
			[&](int v) { emit parameterChanged(number_, OperatorParameter::RR, v); });
	connect(ui->slSlider, &LabeledVSlider::valueChanged, this,
			[&](int v) { emit parameterChanged(number_, OperatorParameter::SL, v); });
	connect(ui->tlSlider, &LabeledVSlider::valueChanged, this,
			[&](int v) { emit parameterChanged(number_, OperatorParameter::TL, v); });
	connect(ui->ksSlider, &LabeledVSlider::valueChanged, this,
			[&](int v) { emit parameterChanged(number_, OperatorParameter::KS, v); });
	connect(ui->mlSlider, &LabeledVSlider::valueChanged, this,
			[&](int v) { emit parameterChanged(number_, OperatorParameter::ML, v); });
	connect(ui->dtSlider, &LabeledVSlider::valueChanged, this,
			[&](int v) { emit parameterChanged(number_, OperatorParameter::DT, v); });
	connect(ui->amSlider, &LabeledVSlider::valueChanged, this,
			[&](int v) { emit parameterChanged(number_, OperatorParameter::AM, v); });
	connect(ui->ssgegSlider, &SSGEGSlider::valueChanged, this,
			[&](int v) { emit parameterChanged(number_, OperatorParameter::SSGEG, v); });
}

OperatorSliders::~OperatorSliders()
{
	delete ui;
}

int OperatorSliders::operatorNumber() const
{
	return number_;
}

void OperatorSliders::setOperatorNumber(int number)
{
	number_ = number;
	ui->operatorNumberLabel->setText("Op" + QString::number(number_));
}

int OperatorSliders::parameterValue(OperatorParameter param) const
{
	switch (param) {
	case OperatorParameter::AR: return ui->arSlider->value();
	case OperatorParameter::DR: return ui->drSlider->value();
	case OperatorParameter::SR: return ui->srSlider->value();
	case OperatorParameter::RR: return ui->rrSlider->value();
	case OperatorParameter::SL: return ui->slSlider->value();
	case OperatorParameter::TL: return ui->tlSlider->value();
	case OperatorParameter::KS: return ui->ksSlider->value();
	case OperatorParameter::ML: return ui->mlSlider->value();
	case OperatorParameter::DT: return ui->dtSlider->value();
	case OperatorParameter::AM: return ui->amSlider->value();
	case OperatorParameter::SSGEG:	return ui->ssgegSlider->value();
	default:                 return -1; // Error
	}
}

void OperatorSliders::setParameterValue(OperatorParameter param, int value)
{
	switch (param) {
	case OperatorParameter::AR: ui->arSlider->setValue(value); break;
	case OperatorParameter::DR: ui->drSlider->setValue(value); break;
	case OperatorParameter::SR: ui->srSlider->setValue(value); break;
	case OperatorParameter::RR: ui->rrSlider->setValue(value); break;
	case OperatorParameter::SL: ui->slSlider->setValue(value); break;
	case OperatorParameter::TL: ui->tlSlider->setValue(value); break;
	case OperatorParameter::KS: ui->ksSlider->setValue(value); break;
	case OperatorParameter::ML: ui->mlSlider->setValue(value); break;
	case OperatorParameter::DT: ui->dtSlider->setValue(value); break;
	case OperatorParameter::AM: ui->amSlider->setValue(value); break;
	case OperatorParameter::SSGEG:	ui->ssgegSlider->setValue(value);	break;
	default:                 break; // Error
	}
}
