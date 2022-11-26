#include "checkanddownload.h"
#include "ui_checkanddownload.h"
#include <QDir>
#include <QIcon>
#include <QStyle>
#include <QCloseEvent>
#include <QMessageBox>
#include "enumeration.h"

CheckAndDownload::CheckAndDownload(int now_major, int now_minor, int now_fix, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CheckAndDownload),
    updateChecker(now_major, now_minor, now_fix),
    nowMajor(now_major),
    nowMinor(now_minor),
    nowFix(now_fix),
    tmpFilePath(QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("update_tmp/update.zip")),
    onProcessing(false),
    updateStatus(Checker::Status::ALPHA | Checker::Status::BETA | Checker::Status::RELEASE | Checker::Status::LTS)
    //,proxy(QNetworkProxy::ProxyType::Socks5Proxy, "localhost", 7890)
{
    ui->setupUi(this);
    ui->refreshButton->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));

    //connect
    void (QPushButton::*push_button_clicked) (bool) = &QPushButton::clicked;
    void (CheckAndDownload::*check_and_download_refreshClicked) () = &CheckAndDownload::refreshClicked;
    connect(ui->refreshButton, push_button_clicked, this, check_and_download_refreshClicked);
}

CheckAndDownload::~CheckAndDownload()
{
    delete ui;
}

void CheckAndDownload::refreshClicked(){
    //设置onProcessing状态
    this->setOnProcessing(true);
    //绑定升级检测
    this->connectChecker();
    //检测是否有可升级版本
    this->updateChecker.startChecking_Async(this->updateStatus, this->proxy);
}

void CheckAndDownload::checkerProgressing(qint64 now, qint64 max){
    if(max == 0){
        ui->progressBar->setValue(100);
    }else{
        ui->progressBar->setValue(100 * now / max);
    }
}

void CheckAndDownload::checkerSucceeded(int major, int minor, int fix, Checker::Status status, const QUrl &url, const QString &SHA512){
    //无论如何，解绑checker的全部消息
    this->disconnectChecker();
    this->setOnProcessing(false);
    if(major == -1){
        //无升级
        QMessageBox message_box;
        message_box.setText(tr("No Update Found"));
        message_box.setStandardButtons(QMessageBox::Ok);
        message_box.exec();
        return;
    }

    QString status_str;
    switch(status){
    case Checker::Status::ALPHA:
        status_str = "Alpha";
        break;
    case Checker::Status::BETA:
        status_str = "Beta";
        break;
    case Checker::Status::RELEASE:
        status_str = "Release";
        break;
    case Checker::Status::LTS:
        status_str = "LTS";
        break;
    }

    //提示用户是否升级
    QMessageBox message_box;
    message_box.setText(
        QString(tr("New Version Found: %1V%2.%3.%4, Download?")).arg(status_str, QString::number(major), QString::number(minor), QString::number(fix))
    );
    message_box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    int user_choose = message_box.exec();
    if(user_choose == QMessageBox::Cancel){
        return;
    }
    //开始更新
    this->setOnProcessing(true);
    this->connectDownloader();
    this->updateDownloader.startDownload_Async(url, QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("update_temp/update.zip"), SHA512, this->proxy);
}

void CheckAndDownload::checkerFailed(const QString &reason){
    //无论如何，解绑checker所有信息
    this->disconnectChecker();
    this->setOnProcessing(false);
    QMessageBox::critical(this, tr("Critical Error"), tr("Update Checking Failed: ") + reason);
}

void CheckAndDownload::downloaderProgressing(qint64 now, qint64 max){
    if(max == 0){
        ui->progressBar->setValue(100);
    }else{
        ui->progressBar->setValue(100 * now / max);
    }
}

void CheckAndDownload::downloaderSucceeded(){
    //解绑消息并设置OnProcessing
    this->disconnectDownloader();
    this->setOnProcessing(false);
    QMessageBox message_box;
    message_box.setText(tr("Download Successfully, Install?"));
    message_box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    int user_choose = message_box.exec();
    if(user_choose == QMessageBox::Cancel){
        return;
    }
    //开始解压
    this->setOnProcessing(true);
    this->connectUnzipper();
    this->updateUnzipper.startUnzipping_Async(QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("update_temp/update.zip"));
}

void CheckAndDownload::downloaderFailed(const QString &reason){
    //解绑消息并设置OnProcessing
    this->disconnectDownloader();
    this->setOnProcessing(false);
    QMessageBox::critical(this, tr("Critical Error"), tr("Update Downloading Failed: ") + reason);
}

void CheckAndDownload::unzipperSucceeded(){
    //解绑消息并设置OnProcessing
    this->disconnectUnzipper();
    this->setOnProcessing(false);
    //调用新版本升级程序
    QString new_updater_path = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("update_temp/updater.exe");
    QProcess new_updater_process;
    new_updater_process.setProgram(new_updater_path);
    new_updater_process.setArguments(
        {
            "--UpdateType",
            QString::number(UpdateType::OVERRIDE_OLD),
             "--MajorVersion",
            QString::number(this->nowMajor),
            "--MinorVersion",
            QString::number(this->nowMinor),
            "--FixVersion",
            QString::number(this->nowFix)
        }
    );
    new_updater_process.startDetached();
    close();
}

void CheckAndDownload::unzipperFailed(const QString &reason){
    //解绑消息并设置OnProcessing
    this->disconnectUnzipper();
    this->setOnProcessing(false);
}

void CheckAndDownload::setOnProcessing(bool on_processing){
    if(on_processing){
        assert(!this->onProcessing);
        //设置显示
        ui->progressBar->setFormat("Processing: %p%");
        ui->progressBar->setValue(0);
        //设置onProcessing
        this->onProcessing = true;
        //设置不可点击refresh
        ui->refreshButton->setEnabled(false);
        //设置不可点击菜单
        ui->menubar->setEnabled(false);
    }else{
        assert(this->onProcessing);
        ui->progressBar->setFormat("");
        ui->progressBar->setValue(0);
        this->onProcessing = false;
        ui->refreshButton->setEnabled(true);
        ui->menubar->setEnabled(true);
    }
}

void CheckAndDownload::closeEvent(QCloseEvent *event){
    if(this->onProcessing){
        event->ignore();
    }else{
        event->accept();
    }
}

void CheckAndDownload::connectChecker(){
    void (Checker::*checker_progressing) (qint64, qint64) = &Checker::progressing;
    void (Checker::*checker_succeeded) (int, int, int, Checker::Status, const QUrl &, const QString &) = &Checker::succeeded;
    void (Checker::*checker_failed) (const QString &) = &Checker::failed;
    void (CheckAndDownload::*check_and_download_checkerProgressing) (qint64, qint64) = &CheckAndDownload::checkerProgressing;
    void (CheckAndDownload::*check_and_download_checkerSucceeded) (int, int, int, Checker::Status, const QUrl &, const QString &) = &CheckAndDownload::checkerSucceeded;
    void (CheckAndDownload::*check_and_download_checkerFailed) (const QString &) = &CheckAndDownload::checkerFailed;
    connect(&(this->updateChecker), checker_progressing, this, check_and_download_checkerProgressing);
    connect(&(this->updateChecker), checker_succeeded, this, check_and_download_checkerSucceeded);
    connect(&(this->updateChecker), checker_failed, this, check_and_download_checkerFailed);
}

void CheckAndDownload::disconnectChecker(){
    void (Checker::*checker_progressing) (qint64, qint64) = &Checker::progressing;
    void (Checker::*checker_succeeded) (int, int, int, Checker::Status, const QUrl &, const QString &) = &Checker::succeeded;
    void (Checker::*checker_failed) (const QString &) = &Checker::failed;
    void (CheckAndDownload::*check_and_download_checkerProgressing) (qint64, qint64) = &CheckAndDownload::checkerProgressing;
    void (CheckAndDownload::*check_and_download_checkerSucceeded) (int, int, int, Checker::Status, const QUrl &, const QString &) = &CheckAndDownload::checkerSucceeded;
    void (CheckAndDownload::*check_and_download_checkerFailed) (const QString &) = &CheckAndDownload::checkerFailed;
    disconnect(&(this->updateChecker), checker_progressing, this, check_and_download_checkerProgressing);
    disconnect(&(this->updateChecker), checker_succeeded, this, check_and_download_checkerSucceeded);
    disconnect(&(this->updateChecker), checker_failed, this, check_and_download_checkerFailed);
}

void CheckAndDownload::connectDownloader(){
    void (Downloader::*downloader_progressing) (qint64, qint64) = &Downloader::progressing;
    void (Downloader::*downloader_succeeded) () = &Downloader::succeeded;
    void (Downloader::*downloader_failed) (const QString &) = &Downloader::failed;
    void (CheckAndDownload::*check_and_download_downloaderProgressing) (qint64, qint64) = &CheckAndDownload::downloaderProgressing;
    void (CheckAndDownload::*check_and_download_downloaderSucceeded) () = &CheckAndDownload::downloaderSucceeded;
    void (CheckAndDownload::*check_and_download_downloaderFailed) (const QString &) = &CheckAndDownload::downloaderFailed;
    connect(&(this->updateDownloader), downloader_progressing, this, check_and_download_downloaderProgressing);
    connect(&(this->updateDownloader), downloader_succeeded, this, check_and_download_downloaderSucceeded);
    connect(&(this->updateDownloader), downloader_failed, this, check_and_download_downloaderFailed);
}

void CheckAndDownload::disconnectDownloader(){
    void (Downloader::*downloader_progressing) (qint64, qint64) = &Downloader::progressing;
    void (Downloader::*downloader_succeeded) () = &Downloader::succeeded;
    void (Downloader::*downloader_failed) (const QString &) = &Downloader::failed;
    void (CheckAndDownload::*check_and_download_downloaderProgressing) (qint64, qint64) = &CheckAndDownload::downloaderProgressing;
    void (CheckAndDownload::*check_and_download_downloaderSucceeded) () = &CheckAndDownload::downloaderSucceeded;
    void (CheckAndDownload::*check_and_download_downloaderFailed) (const QString &) = &CheckAndDownload::downloaderFailed;
    disconnect(&(this->updateDownloader), downloader_progressing, this, check_and_download_downloaderProgressing);
    disconnect(&(this->updateDownloader), downloader_succeeded, this, check_and_download_downloaderSucceeded);
    disconnect(&(this->updateDownloader), downloader_failed, this, check_and_download_downloaderFailed);
}

void CheckAndDownload::connectUnzipper(){
    void (UnZipper::*unzipper_succeeded) () = &UnZipper::succeeded;
    void (UnZipper::*unzipper_failed) (const QString &) = &UnZipper::failed;
    void (CheckAndDownload::*check_and_download_unzipperSucceeded) () = &CheckAndDownload::unzipperSucceeded;
    void (CheckAndDownload::*check_and_download_unzipperFailed) (const QString &) = &CheckAndDownload::unzipperFailed;
    connect(&(this->updateUnzipper), unzipper_succeeded, this, check_and_download_unzipperSucceeded);
    connect(&(this->updateUnzipper), unzipper_failed, this, check_and_download_unzipperFailed);
}

void CheckAndDownload::disconnectUnzipper(){
    void (UnZipper::*unzipper_succeeded) () = &UnZipper::succeeded;
    void (UnZipper::*unzipper_failed) (const QString &) = &UnZipper::failed;
    void (CheckAndDownload::*check_and_download_unzipperSucceeded) () = &CheckAndDownload::unzipperSucceeded;
    void (CheckAndDownload::*check_and_download_unzipperFailed) (const QString &) = &CheckAndDownload::unzipperFailed;
    disconnect(&(this->updateUnzipper), unzipper_succeeded, this, check_and_download_unzipperSucceeded);
    disconnect(&(this->updateUnzipper), unzipper_failed, this, check_and_download_unzipperFailed);
}
