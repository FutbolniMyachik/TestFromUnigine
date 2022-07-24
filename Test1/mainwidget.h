#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>

class QHBoxLayout;
class QVBoxLayout;
class DirInfoCollector;
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
    int maxThreadCount() const;
    void makeGui();
    QHBoxLayout *makeControlLayout() const;
    QVBoxLayout *makeChooseThreadCountLayout() const;
    QTableWidget *makeTableWidget() const;
    void configureProgressDialog(QProgressDialog *progressDialog);
    void updateTableWidget(const QList<QPair<QString, int>> &dataItems);

    QString _currentChoosedDir;
    DirInfoCollector *_dirAnalyzer{nullptr};
    QTableWidget *_tableWidget{nullptr};
    QSettings *_settings{nullptr};
    int countOfViewElemets = 10;
};

#endif // MAINWIDGET_H
