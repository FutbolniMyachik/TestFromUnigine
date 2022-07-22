#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>

class QHBoxLayout;
class DirAnalyzer;

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = nullptr);

signals:
    void currentDirChanged(const QString &currentDir);
private slots:
    void setCurrentDirFromDialog();
    void setCurrentDir(const QString &dirPath);
    void findSameFilesCount();
private:
    QHBoxLayout *makeControlLayout() const;
    QString _currentChoosedDir;
    DirAnalyzer *_dirAnalyzer{nullptr};
    int countOfViewElemets = 10;
};

#endif // MAINWIDGET_H
