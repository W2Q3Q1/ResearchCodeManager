#include "downloader.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QByteArray>
#include <QCryptographicHash>

Downloader::Downloader(){
    this->reply = nullptr;
}

void Downloader::startDownload_Async(const QUrl &url, const QString &file_path, const QString &sha512, const QNetworkProxy &proxy){
    assert(this->reply == nullptr);
    assert(!this->tmpFile.isOpen());
    assert(this->tmpSHA512.isNull());
    this->manager.setProxy(proxy);
    this->tmpSHA512 = sha512;
    if(QFileInfo(file_path).dir().exists()){
        QDir().remove(QFileInfo(file_path).dir().absolutePath());
    }
    QDir().mkdir(QFileInfo(file_path).dir().absolutePath());
    this->tmpFile.setFileName(file_path);
    this->tmpFile.open(QIODeviceBase::Truncate | QIODeviceBase::ReadWrite);
    this->request.setUrl(url);
    this->reply = this->manager.get(this->request);
    void (QNetworkReply::*network_reply_progressing) (qint64, qint64) = &QNetworkReply::downloadProgress;
    void (QNetworkReply::*network_reply_finished) () = &QNetworkReply::finished;
    void (QNetworkReply::*network_reply_readyRead) () = &QNetworkReply::readyRead;
    void (Downloader::*downloader_downloadProgressing) (qint64, qint64) = &Downloader::downloadProgressing;
    void (Downloader::*downloader_downloadFinished) () = &Downloader::downloadFinished;
    void (Downloader::*downloader_downloadReadyRead) () = &Downloader::downloadReadyRead;
    connect(this->reply, network_reply_progressing, this, downloader_downloadProgressing);
    connect(this->reply, network_reply_finished, this, downloader_downloadFinished);
    connect(this->reply, network_reply_readyRead, this, downloader_downloadReadyRead);
}

void Downloader::clear(){
    if(this->reply != nullptr){
        this->reply->deleteLater();
        this->reply = nullptr;
    }
    if(this->tmpFile.isOpen()){
        this->tmpFile.close();
    }
    this->tmpSHA512.clear();
    this->manager.setProxy(QNetworkProxy());
}

void Downloader::downloadProgressing(qint64 now, qint64 max){
    qDebug() << "Progress: " << now << " / " << max << "\n";
    emit Downloader::progressing(now, max);
}

void Downloader::downloadFinished(){
    assert(this->tmpFile.isOpen());
    if(this->reply->error() != QNetworkReply::NoError){
        QString tmp_error_str = this->reply->errorString();
        this->clear();
        qDebug () << "ERROR: " << tmp_error_str << "\n";
        emit Downloader::failed(tmp_error_str);
        return;
    }
    if(this->tmpSHA512.isNull()){
        qDebug() << "Download OK\n";
        this->clear();
        emit Downloader::succeeded();
        return;
    }
    //计算sha512
    this->tmpFile.seek(0);
    QByteArray array = this->tmpFile.readAll();
    QString file_sha512 = QCryptographicHash::hash(array, QCryptographicHash::Sha512).toHex().toUpper();
    if(file_sha512 == this->tmpSHA512){
        this->clear();
        qDebug() << "Download OK\n";
        emit Downloader::succeeded();
    }else{
        this->clear();
        qDebug() << "SHA512 Failed\n";
        emit Downloader::failed("SHA512 Failed");
    }
    return;
}

void Downloader::downloadReadyRead(){
    assert(this->tmpFile.isOpen());
    assert(this->reply != nullptr);
    this->tmpFile.write(this->reply->readAll());
}
