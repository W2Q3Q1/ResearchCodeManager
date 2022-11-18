#ifndef ADDRESEARCHDIALOG_H
#define ADDRESEARCHDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class AddResearchDialog;
}

class AddResearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddResearchDialog(QWidget *parent = nullptr);
    virtual ~AddResearchDialog();

    QString getNameInput();
    QString getInformationInput();

private:
    Ui::AddResearchDialog *ui;
    void accept() override;
};

#endif // ADDRESEARCHDIALOG_H
