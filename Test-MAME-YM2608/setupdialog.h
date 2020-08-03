#ifndef SETUPDIALOG_H
#define SETUPDIALOG_H

#include <QDialog>
#include <QString>
#include <vector>
#include <map>
#include "settings.hpp"
#include "tone_converter.hpp"

namespace Ui {
	class SetupDialog;
}

class SetupDialog : public QDialog
{
	Q_OBJECT

public:
	SetupDialog(const Settings& settings, const ToneConverter& converter, QWidget *parent = nullptr);
	~SetupDialog() override;

	unsigned int rate();
	unsigned int duration();
	FmOutEnvelopeFormats outputFormats();
	FmInEnvelopeFormats inputFormats();

private slots:
	void on_horizontalSlider_valueChanged(int value);

	void updateInEnvelopeSetUi();
	void on_addInEnvelopeSetPushButton_clicked();
	void on_removeInEnvelopeSetPushButton_clicked();
	void on_inEnvelopeSetNameLineEdit_textChanged(const QString &arg1);
	void on_editInEnvelopeSetPushButton_clicked();
	void on_inEnvelopeSetListWidget_currentRowChanged(int currentRow);

	void updateOutEnvelopeSetUi();
	void on_addOutEnvelopeSetPushButton_clicked();
	void on_removeOutEnvelopeSetPushButton_clicked();
	void on_outEnvelopeSetNameLineEdit_textChanged(const QString &arg1);
	void on_editOutEnvelopeSetPushButton_clicked();
	void on_outEnvelopeSetListWidget_currentRowChanged(int currentRow);

private:
	Ui::SetupDialog *ui;
	std::map<int, int> rateMap_;
	FmInEnvelopeFormats fmInEnvFormats_;
	FmOutEnvelopeFormats fmOutEnvFormats_;
};

#endif // SETUPDIALOG_H
