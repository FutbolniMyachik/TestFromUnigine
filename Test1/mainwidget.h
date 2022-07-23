#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>

class QHBoxLayout;
class DirAnalyzer;
class QTableWidget;
class QProgressDialog;
class QSettings;

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
    void makeGui();
    QHBoxLayout *makeControlLayout() const;
    QTableWidget *makeTableWidget() const;
    QProgressDialog *makeProgressDialog();
    void updateTableWidget(const QList<QPair<QString, int>> &dataItems);

    QString _currentChoosedDir;
    DirAnalyzer *_dirAnalyzer{nullptr};
    QTableWidget *_tableWidget{nullptr};
    QSettings *_settings{nullptr};
    int countOfViewElemets = 10;
};

#endif // MAINWIDGET_H
