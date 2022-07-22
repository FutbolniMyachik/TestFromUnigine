#include "mainwidget.h"

#include "diranalyzer.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

#include <QFileDialog>

#include <QDebug>

MainWidget::MainWidget(QWidget *parent)
    : QWidget{parent}
{
    _dirAnalyzer = new DirAnalyzer;

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(makeControlLayout());
    setLayout(mainLayout);
}

void MainWidget::setCurrentDirFromDialog()
{
    setCurrentDir(QFileDialog::getExistingDirectory(this, tr("Выбор директории для поиска"), _currentChoosedDir));
}

void MainWidget::setCurrentDir(const QString &dirPath)
{
    if (!QDir(dirPath).exists())
        return;
    _currentChoosedDir = dirPath;
    emit currentDirChanged(_currentChoosedDir);
}

void MainWidget::findSameFilesCount()
{
    const QMap<QString, int> countOfTheSameNames = _dirAnalyzer->getCountOfTheSameNames(_currentChoosedDir);
    QList<QPair<QString, int>>  result = _dirAnalyzer->getMostCommon(countOfViewElemets, countOfTheSameNames);
    qInfo() << result;
}

QHBoxLayout *MainWidget::makeControlLayout() const
{
    QHBoxLayout *layout = new QHBoxLayout();
    QLabel *currentChoosedDirLabel = new QLabel;
    connect(this, &MainWidget::currentDirChanged, currentChoosedDirLabel, &QLabel::setText);
    layout->addWidget(currentChoosedDirLabel);
    QPushButton *chooseDirButton = new QPushButton(tr("Выбрать директорию"));
    connect(chooseDirButton, &QPushButton::clicked, this, &MainWidget::setCurrentDirFromDialog);
    layout->addWidget(chooseDirButton);
    QPushButton *searchButton = new QPushButton(tr("Поиск совпадений"));
    connect(searchButton, &QPushButton::clicked, this, &MainWidget::findSameFilesCount);
    layout->addWidget(searchButton);
    return layout;
}
