#include "dirinfotest.h"

#include <QProcess>
#include <QTest>

#include "dirinfoanalyzer.h"
#include "dirinfocollector.h"

void DirInfoTest::setDirectory(const QString &dirPath)
{
    _directory = dirPath;
}

void DirInfoTest::compareDirInfoResultAndBashResult()
{
    DirInfoCollector collector;
    DirInfoAnalyzer analyzer(collector.collectDirInfo(_directory));
    const auto result = analyzer.getMostCommonNamesList(10);
    for (const auto &item : result) {
        QCOMPARE(item.second, findCountFromBash(item.first));
    }
}

int DirInfoTest::findCountFromBash(const QString &dirName)
{
    QProcess bashProcess;
    bashProcess.setWorkingDirectory(_directory);
    bashProcess.start("find", {"-name", dirName});
    bashProcess.waitForFinished();
    return QString(bashProcess.readAllStandardOutput()).split('\n').size() - 1; // В последней строке тоже есть '\n'
}
