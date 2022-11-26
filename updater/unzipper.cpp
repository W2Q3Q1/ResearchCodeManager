#include "unzipper.h"
#include <QCoreApplication>
#include <QDir>
#include <QDebug>

UnZipper::UnZipper(){
    this->process.setProgram(QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("7z/7za.exe"));
}

void UnZipper::startUnzipping_Async(const QString &zip_path){
    assert(!this->process.isOpen());
    assert(this->processOutput.isNull());
    this->process.setArguments(
        {
            "x",
            "-o"+QFileInfo(zip_path).dir().absolutePath(),
            zip_path,
            "-y"
        }
    );
    void (QProcess::*process_finished) (int, QProcess::ExitStatus) = &QProcess::finished;
    void (QProcess::*process_readyRead) () = &QProcess::readyRead;
    void (UnZipper::*unzipper_unzipFinished) (int, QProcess::ExitStatus) = &UnZipper::unzipFinished;
    void (UnZipper::*unzipper_unzipReadyRead) () = &UnZipper::unzipReadyRead;
    connect(&(this->process), process_finished, this, unzipper_unzipFinished);
    connect(&(this->process), process_readyRead, this, unzipper_unzipReadyRead);
    this->process.start();
}

void UnZipper::clear(){
    if(this->process.isOpen()){
        this->process.close();
    }
    this->processOutput.clear();
}

void UnZipper::unzipFinished(int exitCode, QProcess::ExitStatus exitStatus){
    qDebug() << "UpZip Output: " << this->processOutput << "\n";
    if(exitStatus == QProcess::ExitStatus::NormalExit){
        this->clear();
        qDebug() << "UnZipped Succeeded\n";
        emit UnZipper::succeeded();
    }else{
        QString tmp_error_str = this->process.errorString();
        this->clear();
        qDebug() << "UnZipped Failed\n";
        emit UnZipper::failed(tmp_error_str);
    }
    return;
}

void UnZipper::unzipReadyRead(){
    this->processOutput += this->process.readAll();
}
