#ifndef TONETEXTDIALOG_H
#define TONETEXTDIALOG_H

#include <QDialog>

namespace Ui {
class ToneTextDialog;
}

class ToneTextDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ToneTextDialog(QWidget *parent = 0);
    ~ToneTextDialog();
    void setText(QString text);

private:
    Ui::ToneTextDialog *ui;
};

#endif // TONETEXTDIALOG_H
