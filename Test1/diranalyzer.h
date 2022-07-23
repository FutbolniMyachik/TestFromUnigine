#ifndef DIRANALYZER_H
#define DIRANALYZER_H

#include <QList>
#include <QMap>
#include <QString>
#include <QDir>

class QThreadPool;

class DirAnalyzer : public QObject
{
    Q_OBJECT
public:
    explicit DirAnalyzer(QObject *parent = nullptr);
    QList<QPair<QString, int>> getMostCommon(const int count, const QMap<QString, int> &sourceValues) const;
    QMap<QString, int> getCountOfTheSameNames(const QString &startDir);
    void setThreadCount(const int threadCount);
    int threadCount() const;
signals:
    void threadCountChanged(int threadCount);
public slots:
    void interrupt();
private:
    void clear();
    void incrementFileRepeatCount(const QString &fileName);
    void checkDirAsync(const QString &dirPath);
    bool isValidElement(const QString &dirPath) const;
    void getCountOfTheSameNamesInternal(const QString &startDir);
    QList<QPair<QString, int>> toList(const QMap<QString, int> &sourceValues) const;
    QMap<QString, int> _result;
    QThreadPool *_threads{nullptr};
    bool _interrupt{false};
};

#endif // DIRANALYZER_H
