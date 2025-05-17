#ifndef TONETEXTDIALOG_H
#define TONETEXTDIALOG_H

#include <QDialog>
#include "tone.hpp"
#include "tone_converter.hpp"

namespace Ui {
class ToneTextDialog;
}

class ToneTextDialog : public QDialog
{
    Q_OBJECT

public:
	ToneTextDialog(const Tone& tone, const ToneConverter& converter, QWidget *parent = 0);
    ~ToneTextDialog();

private slots:
	void on_comboBox_currentIndexChanged(int index);

private:
    Ui::ToneTextDialog *ui;
	const Tone& tone_;
	const ToneConverter& converter_;
};

#endif // TONETEXTDIALOG_H
