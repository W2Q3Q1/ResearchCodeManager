#ifndef ADDCODEDIALOG_H
#define ADDCODEDIALOG_H

#include <QDialog>

namespace Ui {
class AddCodeDialog;
}

class AddCodeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddCodeDialog(int research_idx, QWidget *parent = nullptr);
    ~AddCodeDialog();
    QString getNameInput();
    QString getPathInput();
    QString getInformationInput();

private slots:
    void openFileButtonClicked();
    void openDirButtonClicked();

private:
    Ui::AddCodeDialog *ui;
    const int research_idx;
    void accept() override;
};

#endif // ADDCODEDIALOG_H
