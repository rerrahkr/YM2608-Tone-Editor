#include "fmenvelopeorderdialog.hpp"
#include "ui_fmenvelopeorderdialog.h"

FmEnvelopeOrderDialog::FmEnvelopeOrderDialog(FmInEnvelopeOrders order, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::FmEnvelopeOrderDialog)
{
	ui->setupUi(this);

	setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);

	for (size_t i = 0; i < order.size(); ++i) {
		insertRow(static_cast<int>(i), order.at(i));
	}
}

FmEnvelopeOrderDialog::~FmEnvelopeOrderDialog()
{
	delete ui;
}

FmInEnvelopeOrders FmEnvelopeOrderDialog::getOrder()
{
	std::vector<FmEnvelopeTextType> order;
	for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i) {
		order.push_back(static_cast<FmEnvelopeTextType>(
							qobject_cast<QComboBox*>(ui->treeWidget->itemWidget(
														 ui->treeWidget->topLevelItem(i), 1))->currentData().toInt()));
	}
	return order;
}

void FmEnvelopeOrderDialog::swapset(int aboveRow, int belowRow)
{
	auto* tree = ui->treeWidget;
	QComboBox* belowBox = makeCombobox();
	belowBox->setCurrentIndex(qobject_cast<QComboBox*>(tree->itemWidget(tree->topLevelItem(belowRow), 1))->currentIndex());
	QTreeWidgetItem* below = tree->takeTopLevelItem(belowRow);
	if (tree->topLevelItemCount() > 2) {
		QComboBox* aboveBox = makeCombobox();
		aboveBox->setCurrentIndex(qobject_cast<QComboBox*>(tree->itemWidget(tree->topLevelItem(aboveRow), 1))->currentIndex());
		QTreeWidgetItem* above = tree->takeTopLevelItem(aboveRow);
		tree->insertTopLevelItem(aboveRow, below);
		tree->insertTopLevelItem(belowRow, above);
		tree->setItemWidget(below, 1, belowBox);
		tree->setItemWidget(above, 1, aboveBox);
	}
	else {
		tree->insertTopLevelItem(aboveRow, below);
		tree->setItemWidget(below, 1, belowBox);
	}

	if (!aboveRow || !belowRow) alignTreeOn1stItemChanged();	// Dummy set and delete to align

	for (int i = aboveRow; i < ui->treeWidget->topLevelItemCount(); ++i) {
		ui->treeWidget->topLevelItem(i)->setText(0, QString::number(i));
	}
}

void FmEnvelopeOrderDialog::insertRow(int row, FmEnvelopeTextType type)
{
	if (row == -1) row = 0;
	auto item = new QTreeWidgetItem();
	item->setText(0, QString::number(row));
	QComboBox* box = makeCombobox();
	for (int i = 0; i < box->count(); ++i) {
		if (static_cast<FmEnvelopeTextType>(box->itemData(i).toInt()) == type) {
			box->setCurrentIndex(i);
			break;
		}
	}
	ui->treeWidget->insertTopLevelItem(row, item);
	ui->treeWidget->setItemWidget(item, 1, box);

	if (!row) alignTreeOn1stItemChanged();	// Dummy set and delete to align

	for (int i = row + 1; i < ui->treeWidget->topLevelItemCount(); ++i) {
		ui->treeWidget->topLevelItem(i)->setText(0, QString::number(i));
	}
}

QComboBox* FmEnvelopeOrderDialog::makeCombobox()
{
	auto box = new QComboBox();
	box->addItem("Skip", static_cast<int>(FmEnvelopeTextType::Skip));
	box->addItem("AL", static_cast<int>(FmEnvelopeTextType::AL));
	box->addItem("FB", static_cast<int>(FmEnvelopeTextType::FB));
	box->addItem("AR1", static_cast<int>(FmEnvelopeTextType::AR1));
	box->addItem("DR1", static_cast<int>(FmEnvelopeTextType::DR1));
	box->addItem("SR1", static_cast<int>(FmEnvelopeTextType::SR1));
	box->addItem("RR1", static_cast<int>(FmEnvelopeTextType::RR1));
	box->addItem("SL1", static_cast<int>(FmEnvelopeTextType::SL1));
	box->addItem("TL1", static_cast<int>(FmEnvelopeTextType::TL1));
	box->addItem("KS1", static_cast<int>(FmEnvelopeTextType::KS1));
	box->addItem("ML1", static_cast<int>(FmEnvelopeTextType::ML1));
	box->addItem("DT1", static_cast<int>(FmEnvelopeTextType::DT1));
	box->addItem("AM1", static_cast<int>(FmEnvelopeTextType::AM1));
	box->addItem("SSGEG1", static_cast<int>(FmEnvelopeTextType::SSGEG1));
	box->addItem("AR2", static_cast<int>(FmEnvelopeTextType::AR2));
	box->addItem("DR2", static_cast<int>(FmEnvelopeTextType::DR2));
	box->addItem("SR2", static_cast<int>(FmEnvelopeTextType::SR2));
	box->addItem("RR2", static_cast<int>(FmEnvelopeTextType::RR2));
	box->addItem("SL2", static_cast<int>(FmEnvelopeTextType::SL2));
	box->addItem("TL2", static_cast<int>(FmEnvelopeTextType::TL2));
	box->addItem("KS2", static_cast<int>(FmEnvelopeTextType::KS2));
	box->addItem("ML2", static_cast<int>(FmEnvelopeTextType::ML2));
	box->addItem("DT2", static_cast<int>(FmEnvelopeTextType::DT2));
	box->addItem("AM2", static_cast<int>(FmEnvelopeTextType::AM2));
	box->addItem("SSGEG2", static_cast<int>(FmEnvelopeTextType::SSGEG2));
	box->addItem("AR3", static_cast<int>(FmEnvelopeTextType::AR3));
	box->addItem("DR3", static_cast<int>(FmEnvelopeTextType::DR3));
	box->addItem("SR3", static_cast<int>(FmEnvelopeTextType::SR3));
	box->addItem("RR3", static_cast<int>(FmEnvelopeTextType::RR3));
	box->addItem("SL3", static_cast<int>(FmEnvelopeTextType::SL3));
	box->addItem("TL3", static_cast<int>(FmEnvelopeTextType::TL3));
	box->addItem("KS3", static_cast<int>(FmEnvelopeTextType::KS3));
	box->addItem("ML3", static_cast<int>(FmEnvelopeTextType::ML3));
	box->addItem("DT3", static_cast<int>(FmEnvelopeTextType::DT3));
	box->addItem("AM3", static_cast<int>(FmEnvelopeTextType::AM3));
	box->addItem("SSGEG3", static_cast<int>(FmEnvelopeTextType::SSGEG3));
	box->addItem("AR4", static_cast<int>(FmEnvelopeTextType::AR4));
	box->addItem("DR4", static_cast<int>(FmEnvelopeTextType::DR4));
	box->addItem("SR4", static_cast<int>(FmEnvelopeTextType::SR4));
	box->addItem("RR4", static_cast<int>(FmEnvelopeTextType::RR4));
	box->addItem("SL4", static_cast<int>(FmEnvelopeTextType::SL4));
	box->addItem("TL4", static_cast<int>(FmEnvelopeTextType::TL4));
	box->addItem("KS4", static_cast<int>(FmEnvelopeTextType::KS4));
	box->addItem("ML4", static_cast<int>(FmEnvelopeTextType::ML4));
	box->addItem("DT4", static_cast<int>(FmEnvelopeTextType::DT4));
	box->addItem("AM4", static_cast<int>(FmEnvelopeTextType::AM4));
	box->addItem("SSGEG4", static_cast<int>(FmEnvelopeTextType::SSGEG4));
	return box;
}

/// Dummy set and delete to align
void FmEnvelopeOrderDialog::alignTreeOn1stItemChanged()
{
	auto tmp = new QTreeWidgetItem();
	ui->treeWidget->insertTopLevelItem(1, tmp);
	delete ui->treeWidget->takeTopLevelItem(1);
}

void FmEnvelopeOrderDialog::on_upToolButton_clicked()
{
	int curRow = ui->treeWidget->currentIndex().row();
	if (!curRow) return;

	swapset(curRow - 1, curRow);
	ui->treeWidget->setCurrentItem(ui->treeWidget->topLevelItem(curRow - 1));
}

void FmEnvelopeOrderDialog::on_downToolButton_clicked()
{
	int curRow = ui->treeWidget->currentIndex().row();
	if (curRow == ui->treeWidget->topLevelItemCount() - 1) return;

	swapset(curRow, curRow + 1);
	ui->treeWidget->setCurrentItem(ui->treeWidget->topLevelItem(curRow + 1));
}

void FmEnvelopeOrderDialog::on_addPushButton_clicked()
{
	int row = ui->treeWidget->currentIndex().row();
	insertRow(row, FmEnvelopeTextType::Skip);

	ui->treeWidget->setCurrentItem(ui->treeWidget->topLevelItem((row == -1) ? 0 : row));
	ui->upToolButton->setEnabled(true);
	ui->downToolButton->setEnabled(true);
	ui->removePushButton->setEnabled(true);
}

void FmEnvelopeOrderDialog::on_removePushButton_clicked()
{
	int row = ui->treeWidget->currentIndex().row();
	delete ui->treeWidget->takeTopLevelItem(row);

	for (int i = row; i < ui->treeWidget->topLevelItemCount(); ++i) {
		ui->treeWidget->topLevelItem(i)->setText(0, QString::number(i));
	}

	if (!ui->treeWidget->topLevelItemCount()) {
		ui->upToolButton->setEnabled(false);
		ui->downToolButton->setEnabled(false);
		ui->removePushButton->setEnabled(false);
	}
}

void FmEnvelopeOrderDialog::on_treeWidget_itemSelectionChanged()
{
	if (ui->treeWidget->currentIndex().row() == -1) {
		ui->upToolButton->setEnabled(false);
		ui->downToolButton->setEnabled(false);
		ui->removePushButton->setEnabled(false);
	}
	else {
		ui->upToolButton->setEnabled(true);
		ui->downToolButton->setEnabled(true);
		ui->removePushButton->setEnabled(true);
	}
}
