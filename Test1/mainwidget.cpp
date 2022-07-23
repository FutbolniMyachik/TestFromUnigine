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
    _dirAnalyzer = new DirAnalyzer(this);
    _dirAnalyzer->setThreadCount(QThread::idealThreadCount() - 1); // Один поток для GUI
    _settings = new QSettings("settings.ini", QSettings::IniFormat, this);

    makeGui();
    setCurrentDir(_settings->value("currentDir", QDir::currentPath()).toString());
}

void MainWidget::setCurrentDirFromDialog()
{
    const QString directoryPath = QFileDialog::getExistingDirectory(this, tr("Выбор директории для поиска"), _currentChoosedDir);
    if (directoryPath.isEmpty())
        return;
    setCurrentDir(directoryPath);
}

void MainWidget::setCurrentDir(const QString &dirPath)
{
    if (!QDir(dirPath).exists())
        return;
    _currentChoosedDir = dirPath;
    _settings->setValue("currentDir", _currentChoosedDir);
    emit currentDirChanged(_currentChoosedDir);
}

void MainWidget::findSameFilesCount()
{

    QPointer<QProgressDialog> progressDialog = makeProgressDialog();
    progressDialog->show();

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

void MainWidget::makeGui()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(makeControlLayout());
    _tableWidget = makeTableWidget();
    mainLayout->addWidget(_tableWidget);
    setLayout(mainLayout);
}

QHBoxLayout *MainWidget::makeControlLayout() const
{
    QHBoxLayout *layout = new QHBoxLayout();
    QLabel *currentChoosedDirLabel = new QLabel;
    connect(this, &MainWidget::currentDirChanged, currentChoosedDirLabel, &QLabel::setText);
    layout->addWidget(currentChoosedDirLabel);
    const auto makeButton = [this, layout](const QString &title, const auto &slot) {
       QPushButton *button = new QPushButton(title);
       connect(button, &QPushButton::clicked, this, slot);
       layout->addWidget(button);
    };
    makeButton(tr("Выбрать директорию"), &MainWidget::setCurrentDirFromDialog);
    makeButton(tr("Поиск совпадений"), &MainWidget::findSameFilesCount);

    return layout;
}

QTableWidget *MainWidget::makeTableWidget() const
{
    QTableWidget *tableWidget = new QTableWidget;
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderLabels({tr("Имя"), tr("Количество повторений")});
    return tableWidget;
}

QProgressDialog *MainWidget::makeProgressDialog()
{
    QProgressDialog *progressDialog = new QProgressDialog(this);
    progressDialog->setLabelText(tr("Расчет"));
    progressDialog->setMaximum(0);
    connect(progressDialog, &QProgressDialog::canceled, _dirAnalyzer, &DirAnalyzer::interrupt);
    return progressDialog;
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
