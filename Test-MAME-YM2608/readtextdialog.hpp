#ifndef READTEXTDIALOG_HPP
#define READTEXTDIALOG_HPP

#include <vector>
#include <QDialog>
#include <QString>

namespace Ui {
	class ReadTextDialog;
}

class ReadTextDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ReadTextDialog(std::vector<QString> types, QWidget *parent = nullptr);
	~ReadTextDialog();
	QString type() const;
	QString text() const;

private:
	Ui::ReadTextDialog *ui;
};

#endif // READTEXTDIALOG_HPP
