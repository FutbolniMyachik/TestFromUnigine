#include "dirinfocollector.h"

#include <QThreadPool>

DirInfoCollector::DirInfoCollector(QObject *parent) : QObject(parent)
{
    _threads = new QThreadPool(this);
    _threads->setMaxThreadCount(1);
}

QList<QPair<QString, int>> DirInfoCollector::getMostCommon(const int count, const QMap<QString, int> &sourceValues) const
{
    QList<QPair<QString, int> > listOfResult = toList(sourceValues);
    std::sort(listOfResult.begin(), listOfResult.end(), [](const QPair<QString, int> &value1, const QPair<QString, int> &value2) {
        return value1.second > value2.second;
    });
    int resultSize = std::min(count, sourceValues.size());
    QList<QPair<QString, int> > listOfMostCommon;
    listOfMostCommon.reserve(resultSize);
    auto currentElement = listOfResult.begin();
    for (int i = 0; i < resultSize; ++i) {
        listOfMostCommon.append({currentElement->first, currentElement->second});
        ++currentElement;
    }
    return listOfMostCommon;
}

QMap<QString, int> DirInfoCollector::collectDitInfo(const QString &dirPath)
{
    _interrupt = false;
    for (const QFileInfo &file : QDir(dirPath).entryInfoList()) {
        handleDirInSeparateThread(file.filePath());
    }
    _threads->waitForDone();
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
    _threads->start(std::bind(&DirInfoCollector::collectDitInfoInternal, this, dirPath));
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

void DirInfoCollector::collectDitInfoInternal(const QString &dirPath)
{
    if (!isValidElement(dirPath))
        return;
    incrementFileNameRepeatCount(QFileInfo(dirPath).fileName());
    for (const QFileInfo &file : QDir(dirPath).entryInfoList()) {
        handleDirInSeparateThread(file.filePath());
    }
}

QList<QPair<QString, int>> DirInfoCollector::toList(const QMap<QString, int> &sourceValues) const
{
    QList<QPair<QString, int>> result;
    result.reserve(sourceValues.size());
    int i = 0;
    for (const QString &name : sourceValues.keys()) {
        result.append({name, sourceValues.value(name)});
        ++i;
    }
    return result;
}
