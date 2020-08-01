#include "setupdialog.h"
#include "ui_setupdialog.h"
#include <QRadioButton>
#include <QListWidgetItem>
#include "sliderstyle.hpp"

SetupDialog::SetupDialog(const Settings& settings, const ToneConverter& converter, QWidget *parent)
	: QDialog(parent),
	  ui(new Ui::SetupDialog)
{
    ui->setupUi(this);

	setWindowFlag(Qt::WindowContextHelpButtonHint, false);

	// Stream
    rateMap_[ui->buttonGroup->id(ui->radioButton44100)] = 44100;
    rateMap_[ui->buttonGroup->id(ui->radioButton48000)] = 48000;
    rateMap_[ui->buttonGroup->id(ui->radioButton55466)] = 55466;

    ui->horizontalSlider->setValue(settings.getDuration());
	ui->horizontalSlider->setStyle(new SliderStyle());
    for (auto& pair : rateMap_) {
        if (pair.second == settings.getRate()) {
            ui->buttonGroup->button(pair.first)->setChecked(true);
        }
    }

	// Input
	for (auto& n : settings.getInputOrder()) {
		QString name;
		switch (n) {
		case 0:		name = "NO";	break;
		case 1:		name = "AL";	break;
		case 2:		name = "FB";	break;
		default:
		{
			int t = n - 3;
			switch (t % 10) {
			case 0:	name = "AR";	break;
			case 1:	name = "DR";	break;
			case 2:	name = "SR";	break;
			case 3:	name = "RR";	break;
			case 4:	name = "SL";	break;
			case 5:	name = "TL";	break;
			case 6:	name = "KS";	break;
			case 7:	name = "ML";	break;
			case 8:	name = "DT";	break;
			case 9:	name = "AM";	break;
			}
			name += QString::number(t / 10 + 1);
			break;
		}
		}
		auto item = new QListWidgetItem(name);
		item->setData(Qt::UserRole, n);
		ui->readOrderListWidget->addItem(item);
	}
	ui->readOrderListWidget->setCurrentRow(0);

	// Output
	ui->outFormatPlainTextEdit->setPlainText(QString::fromStdString(converter.getOutputFormat()));
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

QString SetupDialog::outputFormat()
{
	return ui->outFormatPlainTextEdit->toPlainText();
}

std::vector<int> SetupDialog::inputOrder()
{
	std::vector<int> order;
	for (int i = 0; i < ui->readOrderListWidget->count(); ++i) {
		order.push_back(ui->readOrderListWidget->item(i)->data(Qt::UserRole).toInt());
	}
	return order;
}

void SetupDialog::on_horizontalSlider_valueChanged(int value)
{
    ui->label->setText(QString::number(value) + "ms");
}

void SetupDialog::on_upToolButton_clicked()
{
	int row = ui->readOrderListWidget->currentRow();
	if (row > 0) {
		QListWidgetItem* tmp = ui->readOrderListWidget->takeItem(row - 1);
		ui->readOrderListWidget->insertItem(row, tmp);
	}
}

void SetupDialog::on_downToolButton_clicked()
{
	int row = ui->readOrderListWidget->currentRow();
	if (row < ui->readOrderListWidget->count() - 1) {
		QListWidgetItem* tmp = ui->readOrderListWidget->takeItem(row + 1);
		ui->readOrderListWidget->insertItem(row, tmp);
	}
}
