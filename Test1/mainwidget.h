#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>

class QHBoxLayout;
class DirAnalyzer;
class QTableWidget;

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
    QTableWidget *makeTableWidget() const;
    void updateTableWidget(const QList<QPair<QString, int>> &dataItems);

    QString _currentChoosedDir;
    DirAnalyzer *_dirAnalyzer{nullptr};
    QTableWidget *_tableWidget{nullptr};
    int countOfViewElemets = 10;
};

#endif // MAINWIDGET_H
