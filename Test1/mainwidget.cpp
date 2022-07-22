#include "mainwidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

#include <QFileDialog>

MainWidget::MainWidget(QWidget *parent)
    : QWidget{parent}
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(makeChooseDirLayout());
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

QHBoxLayout *MainWidget::makeChooseDirLayout() const
{
    QPushButton *chooseDirButton = new QPushButton(tr("Выбрать директорию"));
    connect(chooseDirButton, &QPushButton::clicked, this, &MainWidget::setCurrentDirFromDialog);
    QLabel *currentChoosedDirLabel = new QLabel;
    connect(this, &MainWidget::currentDirChanged, currentChoosedDirLabel, &QLabel::setText);
    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(currentChoosedDirLabel);
    layout->addWidget(chooseDirButton);
    return layout;
}
