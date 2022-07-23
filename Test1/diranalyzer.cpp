#include "diranalyzer.h"

#include <QThreadPool>
#include <QMutex>

#include <QElapsedTimer>
#include <QDebug>

DirAnalyzer::DirAnalyzer(QObject *parent) : QObject(parent)
{
    _threads = new QThreadPool(this);
}

QList<QPair<QString, int>> DirAnalyzer::getMostCommon(const int count, const QMap<QString, int> &sourceValues) const
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

QMap<QString, int> DirAnalyzer::getCountOfTheSameNames(const QString &startDir)
{
    QElapsedTimer timer;
    timer.start();
    incrementFileRepeatCount(QFileInfo(startDir).fileName());
    checkDirAsync(startDir);
    //getCountOfTheSameNamesInternal(startDir);
    _threads->waitForDone();
    QMap<QString, int> result = std::move(_result);
    clear();
    qInfo() << timer.elapsed();
    return result;
}

void DirAnalyzer::setThreadCount(const int threadCount)
{
    _threads->setMaxThreadCount(threadCount);
}

void DirAnalyzer::clear()
{
    _result.clear();
    _interrupt = false;
}

void DirAnalyzer::incrementFileRepeatCount(const QString &fileName)
{
    QMutex mutex;
    mutex.lock();
    ++_result[fileName];
    mutex.unlock();
}

void DirAnalyzer::checkDirAsync(const QString &dirPath)
{
    if (_interrupt)
        return;
    _threads->start(std::bind(&DirAnalyzer::getCountOfTheSameNamesInternal, this, dirPath));
}

void DirAnalyzer::interrupt()
{
    _interrupt = true;
}

bool DirAnalyzer::isValidDir(const QString &dirPath) const
{
    const QDir dir(dirPath);
    return dir.exists() && !dir.dirName().startsWith('.');
}

void DirAnalyzer::getCountOfTheSameNamesInternal(const QString &startDir)
{

    if (!isValidDir(startDir))
        return;
    for (const QFileInfo &file : QDir(startDir).entryInfoList()) {
        incrementFileRepeatCount(file.fileName());
        //getCountOfTheSameNamesInternal(file.filePath());
        checkDirAsync(file.filePath());
    }
}

QList<QPair<QString, int>> DirAnalyzer::toList(const QMap<QString, int> &sourceValues) const
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
