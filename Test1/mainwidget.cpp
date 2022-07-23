#include "mainwidget.h"

#include "diranalyzer.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QTableWidget>
#include <QProgressDialog>

#include <QFuture>
#include <QtConcurrent/QtConcurrent>

MainWidget::MainWidget(QWidget *parent)
    : QWidget{parent}
{
    _dirAnalyzer = new DirAnalyzer;
    _dirAnalyzer->setThreadCount(QThread::idealThreadCount() - 1); // Один поток для GUI
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
    QProgressDialog progressDialog(this);
    progressDialog.setLabelText(tr("Расчет"));
    progressDialog.setMaximum(0);
    progressDialog.show();
    connect(&progressDialog, &QProgressDialog::canceled, _dirAnalyzer, &DirAnalyzer::interrupt);

    QFutureWatcher<QMap<QString, int>> watcher;
    QFuture<QMap<QString, int>> future = QtConcurrent::run(std::bind(&DirAnalyzer::getCountOfTheSameNames, _dirAnalyzer, _currentChoosedDir));
    watcher.setFuture(future);
    QEventLoop eventLoop;
    connect(&watcher, &QFutureWatcher<QMap<QString, int>>::finished, &eventLoop, &QEventLoop::quit, Qt::QueuedConnection);
    eventLoop.exec();

    const QMap<QString, int> countOfTheSameNames = future.result()/*_dirAnalyzer->getCountOfTheSameNames(_currentChoosedDir)*/;
    const QList<QPair<QString, int>>  result = _dirAnalyzer->getMostCommon(countOfViewElemets, countOfTheSameNames);
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
    _tableWidget->clearContents();
    const int rowCount = dataItems.size();
    _tableWidget->setRowCount(dataItems.size());
    for (int currentRow = 0; currentRow < rowCount; ++currentRow) {
        const QPair<QString, int> &dataItem = dataItems[currentRow];
        _tableWidget->setItem(currentRow, 0, new QTableWidgetItem(dataItem.first));
        _tableWidget->setItem(currentRow, 1, new QTableWidgetItem(QString::number(dataItem.second)));
    }
}
