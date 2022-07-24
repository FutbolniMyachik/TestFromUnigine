#ifndef DIRINFOTEST_H
#define DIRINFOTEST_H

#include <QObject>

class DirInfoTest : public QObject
{
    Q_OBJECT
public:
    void setDirectory(const QString &dirPath);

private slots:
    void compareDirInfoResultAndBashResult();
private:
    int findCountFromBash(const QString &dirName);
    QString _directory;
};

#endif // DIRINFOTEST_H
