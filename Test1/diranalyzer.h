#ifndef DIRANALYZER_H
#define DIRANALYZER_H

#include <QList>
#include <QMap>
#include <QString>
#include <QDir>

class DirAnalyzer
{
public:
    DirAnalyzer();
    QList<QPair<QString, int>> getMostCommon(const int count, const QMap<QString, int> &sourceValues) const;
    QMap<QString, int> getCountOfTheSameNames(const QString &startDir);
private:
    bool isValidDir(const QString &dirPath) const;
    void getCountOfTheSameNamesInternal(const QString &startDir);
    QList<QPair<QString, int>> toList(const QMap<QString, int> &sourceValues) const;
    QMap<QString, int> _result;
};

#endif // DIRANALYZER_H
