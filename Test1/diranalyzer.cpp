#include "diranalyzer.h"

#include <QDebug>

DirAnalyzer::DirAnalyzer()
{

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
    ++_result[QFileInfo(startDir).fileName()];
    getCountOfTheSameNamesInternal(startDir);
    return _result;
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
        ++_result[file.fileName()];
        getCountOfTheSameNamesInternal(file.filePath());
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
