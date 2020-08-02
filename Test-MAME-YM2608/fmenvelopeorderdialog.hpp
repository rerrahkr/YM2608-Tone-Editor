#ifndef FMENVELOPEORDERDIALOG_HPP
#define FMENVELOPEORDERDIALOG_HPP

#include <QDialog>
#include <QComboBox>
#include <vector>
#include "tone_converter.hpp"

namespace Ui {
	class FmEnvelopeOrderDialog;
}

class FmEnvelopeOrderDialog : public QDialog
{
	Q_OBJECT

public:
	explicit FmEnvelopeOrderDialog(std::vector<FmEnvelopeTextType> set, QWidget *parent = nullptr);
	~FmEnvelopeOrderDialog();
	std::vector<FmEnvelopeTextType> getSet();

private slots:
	void on_upToolButton_clicked();
	void on_downToolButton_clicked();
	void on_addPushButton_clicked();
	void on_removePushButton_clicked();
	void on_treeWidget_itemSelectionChanged();

private:
	Ui::FmEnvelopeOrderDialog *ui;

	void swapset(int aboveRow, int belowRow);
	void insertRow(int row, FmEnvelopeTextType type);
	QComboBox* makeCombobox();
	void alignTreeOn1stItemChanged();
};

#endif // FMENVELOPEORDERDIALOG_HPP
