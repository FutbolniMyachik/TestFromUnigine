#include "dirinfocollector.h"

#include <QThreadPool>

DirInfoCollector::DirInfoCollector(QObject *parent) : QObject(parent)
{
    _threads = new QThreadPool(this);
    _threads->setMaxThreadCount(1);
}

QMap<QString, int> DirInfoCollector::collectDirInfo(const QString &dirPath)
{
    _interrupt = false;
    const QFileInfoList filesAndDirsNames = QDir(dirPath).entryInfoList();
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
    return fileInfo.exists() && !fileInfo.fileName().startsWith('.');
}

void DirInfoCollector::collectDirInfoInternal(const QString &dirPath)
{
    if (!isValidElement(dirPath))
        return;
    incrementFileNameRepeatCount(QFileInfo(dirPath).fileName());
    for (const QFileInfo &file : QDir(dirPath).entryInfoList()) {
        handleDirInSeparateThread(file.filePath());
    }
}
