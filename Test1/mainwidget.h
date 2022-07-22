#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>

class QHBoxLayout;

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
private:
    QHBoxLayout *makeChooseDirLayout() const;
    QString _currentChoosedDir;
};

#endif // MAINWIDGET_H
