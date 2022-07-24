#include "dirinfocollector.h"

#include <QThreadPool>
#include <QDebug>

DirInfoCollector::DirInfoCollector(QObject *parent) : QObject(parent)
{
    _threads = new QThreadPool(this);
    _threads->setMaxThreadCount(1);
}

QMap<QString, int> DirInfoCollector::collectDirInfo(const QString &dirPath)
{
    _interrupt = false;
    const QFileInfoList filesAndDirsNames = dirsAndFilesAtDir(dirPath);
    for (const QFileInfo &file : filesAndDirsNames) {
        handleDirInSeparateThread(file.filePath());
    }
    _threads->waitForDone();
    if (_interrupt) {
        _dirInfoMap.clear();
        return {};
    }
    QMap<QString, int> result;
    std::swap(result, _dirInfoMap);
    return result;
}

void DirInfoCollector::setMaxThreadCount(const int threadCount)
{
    if ((threadCount == this->maxThreadCount()) || (threadCount < 1))
        return;
    _threads->setMaxThreadCount(threadCount);
    emit maxThreadCountChanged(threadCount);
}

int DirInfoCollector::maxThreadCount() const
{
    return _threads->maxThreadCount();
}

void DirInfoCollector::incrementFileNameRepeatCount(const QString &fileName)
{
    _mutex.lock();
    ++_dirInfoMap[fileName];
    _mutex.unlock();
}

void DirInfoCollector::handleDirInSeparateThread(const QString &dirPath)
{
    if (_interrupt)
        return;
    _threads->start(std::bind(&DirInfoCollector::collectDirInfoInternal, this, dirPath));
}

void DirInfoCollector::interrupt()
{
    _interrupt = true;
}

bool DirInfoCollector::isValidElement(const QString &dirPath) const
{
    const QFileInfo fileInfo(dirPath);
    const QString fileName = fileInfo.fileName();
    return fileInfo.exists() && fileName != "." && fileName != "..";
}

void DirInfoCollector::collectDirInfoInternal(const QString &dirPath)
{
    incrementFileNameRepeatCount(QFileInfo(dirPath).fileName());
    const QFileInfoList filesAndDirsNames = dirsAndFilesAtDir(dirPath);
    for (const QFileInfo &file : filesAndDirsNames) {
        handleDirInSeparateThread(file.filePath());
    }
}

QFileInfoList DirInfoCollector::dirsAndFilesAtDir(const QString &dirPath) const
{
    return QDir(dirPath).entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
}
