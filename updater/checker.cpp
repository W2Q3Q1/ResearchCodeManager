#include "checker.h"
#include <QUrl>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QEventLoop>

Checker::Checker(int major, int minor, int fix):
    major(major),
    minor(minor),
    fix(fix)
{
    this->request.setUrl(QUrl("https://raw.githubusercontent.com/W2Q3Q1/ResearchCodeManager/master/version_list.json"));
    this->reply = nullptr;
    this->checkingStatus = -1;
}

void Checker::startChecking_Async(int status, const QNetworkProxy &proxy){
    assert(this->reply == nullptr);
    assert(this->tmpJson.isNull());
    assert(this->checkingStatus == -1);
    this->manager.setProxy(proxy);
    this->checkingStatus = status;
    this->reply = this->manager.get(this->request);
    void (QNetworkReply::*network_reply_progressing) (qint64, qint64) = &QNetworkReply::downloadProgress;
    void (QNetworkReply::*network_reply_finished) () = &QNetworkReply::finished;
    void (QNetworkReply::*network_reply_readyRead) () = &QNetworkReply::readyRead;
    void (Checker::*checker_checkProgressing) (qint64, qint64) = &Checker::checkProgressing;
    void (Checker::*checker_checkFinished) () = &Checker::checkFinished;
    void (Checker::*checker_checkReadyRead) () = &Checker::checkReadyRead;
    connect(this->reply, network_reply_progressing, this, checker_checkProgressing);
    connect(this->reply, network_reply_finished, this, checker_checkFinished);
    connect(this->reply, network_reply_readyRead, this, checker_checkReadyRead);
}

bool Checker::updatable(int mf_major, int mf_minor, int mf_fix){
    if(mf_major < this->major){
        return true;
    }
    if(mf_major == this->major && mf_minor < this->minor){
        return true;
    }
    if(mf_major == this->major && mf_minor == this->minor && mf_fix <= this->fix){
        //最小升级版本相等也可以升级
        return true;
    }
    return false;
}

bool Checker::higherThanNow(int new_major, int new_minor, int new_fix){
    if(new_major > this->major){
        return true;
    }
    if(new_major == this->major && new_minor > this->minor){
        return true;
    }
    if(new_major == this->major && new_minor == this->minor && new_fix > this->fix){
        //必须严格大于
        return true;
    }
    return false;
}

bool Checker::statusMatch(const QString &status_str){
    int status = Checker::Status::ALPHA | Checker::Status::BETA | Checker::Status::RELEASE | Checker::Status::LTS;
    if(status_str.toLower() == "alpha"){
        status = 1;
    }
    if(status_str.toLower() == "beta"){
        status = 2;
    }
    if(status_str.toLower() == "release"){
        status = 4;
    }
    if(status_str.toLower() == "lts"){
        status = 8;
    }
    return (status & this->checkingStatus) != 0;
}

void Checker::clear(){
    if(this->reply != nullptr){
        this->reply->deleteLater();
        this->reply = nullptr;
    }
    this->tmpJson.clear();
    this->checkingStatus = -1;
    this->manager.setProxy(QNetworkProxy());
}

void Checker::checkProgressing(qint64 now, qint64 max){
    emit Checker::progressing(now, max);
}

void Checker::checkFinished(){
    if(this->reply->error() != QNetworkReply::NoError){
        QString tmp_error_str = this->reply->errorString();
        this->clear();
        qDebug() << "ERROR: " << tmp_error_str << "\n";
        emit Checker::failed(tmp_error_str);
        return;
    }
    QJsonParseError json_parser_error;
    QJsonDocument version_list_json_doc = QJsonDocument::fromJson(this->tmpJson, &json_parser_error);
    if(json_parser_error.error != QJsonParseError::NoError){
        this->clear();
        qDebug() << "ERROR: " << json_parser_error.errorString() << "\n";
        emit Checker::failed(json_parser_error.errorString());
        return;
    }
    assert(version_list_json_doc.isArray());
    QUrl url;
    QString sha512;
    int new_major = -1, new_minor = -1, new_fix = -1;
    Checker::Status new_status = Checker::Status::ALPHA;
    QJsonArray version_block_array = version_list_json_doc.array();
    for(auto it_version_block = version_block_array.cbegin(); it_version_block != version_block_array.cend(); it_version_block++){
        assert(it_version_block->isObject());
        QJsonObject version_block = it_version_block->toObject();
        if(!this->statusMatch(version_block["Status"].toString())){
            continue;
        }
        assert(version_block["MF_Version"].isArray());
        assert(version_block["Version"].isArray());
        QJsonArray mf_version_array = version_block["MF_Version"].toArray();
        QJsonArray new_version_array = version_block["Version"].toArray();
        if(this->updatable(mf_version_array[0].toInt(), mf_version_array[1].toInt(), mf_version_array[2].toInt())){
            if(!this->higherThanNow(new_version_array[0].toInt(), new_version_array[1].toInt(), new_version_array[2].toInt())){
                continue;
            }
            url = QUrl(version_block["URL"].toString());
            sha512 = version_block["SHA512"].toString();
            new_major = new_version_array[0].toInt();
            new_minor = new_version_array[1].toInt();
            new_fix = new_version_array[2].toInt();
            QString new_status_str = version_block["Status"].toString().toLower();
            if(new_status_str == "alpha"){
                new_status = Checker::Status::ALPHA;
            }else if(new_status_str == "beta"){
                new_status = Checker::Status::BETA;
            }else if(new_status_str == "release"){
                new_status = Checker::Status::RELEASE;
            }else if(new_status_str == "lts"){
                new_status = Checker::Status::LTS;
            }else{
                assert(false);
            }
        }else{
            break;
        }
    }
    qDebug() << "URL: " << url << "\nSHA512: " << sha512 << "\n";
    this->clear();
    if(sha512.isNull()){
        emit Checker::failed("Nothing Update");
    }else{
        emit Checker::succeeded(new_major, new_minor, new_fix, new_status, url, sha512);
    }
    return;
}

void Checker::checkReadyRead(){
    assert(this->reply != nullptr);
    this->tmpJson += this->reply->readAll();
}
