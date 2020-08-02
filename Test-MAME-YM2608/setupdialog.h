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
	QString outputFormat();
	std::vector<FmEnvelopeText> inputOrders();

private slots:
    void on_horizontalSlider_valueChanged(int value);

	void on_addEnvelopeSetPushButton_clicked();
	void on_removeEnvelopeSetpushButton_clicked();
	void on_editEnvelopeSetPushButton_clicked();
	void on_envelopeSetNameLineEdit_textChanged(const QString &arg1);
	void on_envelopeSetListWidget_currentRowChanged(int currentRow);
	void updateEnvelopeSetUi();

private:
    Ui::SetupDialog *ui;
	std::map<int, int> rateMap_;
	std::vector<FmEnvelopeText> fmEnvelopeTexts_;
};

#endif // SETUPDIALOG_H
