#include "safedialog.h"
#include "ui_safedialog.h"
#include <QRandomGenerator>
#include <QDebug>
#include <QStyle>
#include <QPushButton>

SafeDialog::SafeDialog(const QString &message, QWidget *parent, SafeDialogType dialog_type) :
    QDialog(parent),
    ui(new Ui::SafeDialog)
{
    ui->setupUi(this);
    //初始化
    const QVector<QPushButton*> buttonList(
        {
            ui->pushButton_0,
            ui->pushButton_1,
            ui->pushButton_2,
            ui->pushButton_3,
            ui->pushButton_4,
            ui->pushButton_5,
            ui->pushButton_6,
            ui->pushButton_7,
            ui->pushButton_8
        }
    );
    ui->textLabel->setText(message);
    //随机选择一个按键
    int button_idx = (QRandomGenerator::global()->generate() & std::numeric_limits<int>::max()) % 9;
    qDebug() << "Button ID: " << button_idx << "\n";
    //根据dialogType设置按键类型
    for(int i=0; i<9; i++){
        if(i == button_idx){
            if(dialog_type == SafeDialogType::SAFE_CONFIRM){
                buttonList[i]->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogOkButton));
                connect(buttonList[i], &QPushButton::clicked, this, &QDialog::accept);
            }else{
                buttonList[i]->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogCancelButton));
                connect(buttonList[i], &QPushButton::clicked, this, &QDialog::reject);
            }
        }else{
            if(dialog_type == SafeDialogType::SAFE_CONFIRM){
                buttonList[i]->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogCancelButton));
                connect(buttonList[i], &QPushButton::clicked, this, &QDialog::reject);
            }else{
                buttonList[i]->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogOkButton));
                connect(buttonList[i], &QPushButton::clicked, this, &QDialog::accept);
            }
        }
    }
}

SafeDialog::~SafeDialog()
{
    delete ui;
}
