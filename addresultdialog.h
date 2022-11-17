#ifndef ADDRESULTDIALOG_H
#define ADDRESULTDIALOG_H

#include <QDialog>
#include <QMap>
#include <QTableWidget>

namespace Ui {
class AddResultDialog;
}

class AddResultDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddResultDialog(int code_idx, QWidget *parent = nullptr);
    ~AddResultDialog();

    QString getNameInput();
    QMap<QString, QString> getHPInput();
    QMap<QString, QString> getRInput();
    QString getConfigPathInput();
    QString getLogPathInput();

private slots:
    void configOpenFileButtonClicked();
    void configOpenDirButtonClicked();
    void logOpenFileButtonClicked();
    void logOpenDirButtonClicked();
    void addNewHPKV();
    void deleteCurrentHPKV();
    void addNewRKV();
    void deleteCurrentRKV();
    void refreshHPDeletable();
    void refreshRDeletable();

private:
    Ui::AddResultDialog *ui;
    const int code_idx;
    void accept() override;
    void showEvent(QShowEvent* event) override;
    void addNewKV(QTableWidget *widget);
    void deleteCurrentKV(QTableWidget *widget);
    void refreshDeletable(QPushButton *delete_button, QTableWidget* widget);
};

#endif // ADDRESULTDIALOG_H
