#include "mainwidget.h"

#include "diranalyzer.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

#include <QFileDialog>

#include <QTableWidget>

MainWidget::MainWidget(QWidget *parent)
    : QWidget{parent}
{
    _dirAnalyzer = new DirAnalyzer;

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(makeControlLayout());
    _tableWidget = makeTableWidget();
    mainLayout->addWidget(_tableWidget);
    setLayout(mainLayout);
    setCurrentDir(QDir::currentPath());
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
    updateTableWidget(result);
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

QTableWidget *MainWidget::makeTableWidget() const
{
    QTableWidget *tableWidget = new QTableWidget;
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderLabels({tr("Имя"), tr("Количество повторений")});
    return tableWidget;
}

void MainWidget::updateTableWidget(const QList<QPair<QString, int> > &dataItems)
{
    const int rowCount = dataItems.size();
    fitTableForRowCount(rowCount);
    const int rowCountAlreadyExists = _tableWidget->rowCount();
    int currentRow = 0;
    for (; currentRow < rowCountAlreadyExists; ++currentRow) {
        const QPair<QString, int> &dataItem = dataItems[currentRow];
        _tableWidget->itemAt(currentRow, 0)->setText(dataItem.first);
        _tableWidget->itemAt(currentRow, 0)->setText(QString::number(dataItem.second));
    }
    for (; currentRow < rowCount; ++currentRow) {
        const QPair<QString, int> &dataItem = dataItems[currentRow];
        _tableWidget->insertRow(currentRow);
        _tableWidget->setItem(currentRow, 0, new QTableWidgetItem(dataItem.first));
        _tableWidget->setItem(currentRow, 1, new QTableWidgetItem(QString::number(dataItem.second)));
    }
}

void MainWidget::fitTableForRowCount(const int rowCount)
{
    int  rowCountAlreadyExists = _tableWidget->rowCount();
    if (rowCountAlreadyExists <= rowCount)
        return;
    for (; rowCountAlreadyExists > rowCount; --rowCountAlreadyExists) {
        _tableWidget->removeRow(rowCountAlreadyExists);
    }
}
