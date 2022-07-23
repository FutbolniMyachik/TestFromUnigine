#include "mainwidget.h"

#include "diranalyzer.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QTableWidget>
#include <QHeaderView>
#include <QProgressDialog>
#include <QComboBox>

#include <QFuture>
#include <QtConcurrent/QtConcurrent>

MainWidget::MainWidget(QWidget *parent)
    : QWidget{parent}
{
    _dirAnalyzer = new DirAnalyzer(this);
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
    if (!QDir(dirPath).exists() || (_currentChoosedDir == dirPath))
        return;
    _currentChoosedDir = dirPath;
    _settings->setValue("currentDir", _currentChoosedDir);
    emit currentDirChanged(_currentChoosedDir);
}

void MainWidget::findSameFilesCount()
{

    QProgressDialog progressDialog(this);
    configureProgressDialog(&progressDialog);
    progressDialog.show();

    QFutureWatcher<QMap<QString, int>> watcher;
    QFuture<QMap<QString, int>> future = QtConcurrent::run(std::bind(&DirAnalyzer::getCountOfTheSameNames, _dirAnalyzer, _currentChoosedDir));
    watcher.setFuture(future);
    QEventLoop eventLoop;
    connect(&watcher, &QFutureWatcher<QMap<QString, int>>::finished, &eventLoop, &QEventLoop::quit, Qt::QueuedConnection);
    eventLoop.exec();

    const QMap<QString, int> countOfTheSameNames = future.result();
    progressDialog.setLabelText(tr("Поиск максимальных значений"));
    const QList<QPair<QString, int>>  result = _dirAnalyzer->getMostCommon(countOfViewElemets, countOfTheSameNames);
    updateTableWidget(result);
}

int MainWidget::maxThreadCount() const
{
    return QThread::idealThreadCount() - 1; // Один поток для GUI
}

void MainWidget::makeGui()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(makeControlLayout());
    _tableWidget = makeTableWidget();
    mainLayout->addWidget(_tableWidget);
    setLayout(mainLayout);
    setMinimumSize(400, 400);
}

QHBoxLayout *MainWidget::makeControlLayout() const
{
    QHBoxLayout *layout = new QHBoxLayout();
    QLabel *currentChoosedDirLabel = new QLabel;
    connect(this, &MainWidget::currentDirChanged, currentChoosedDirLabel, &QLabel::setText);
    layout->addWidget(currentChoosedDirLabel);
    const auto makeButton = [this, layout](const QString &title, const auto &slot) {
       QPushButton *button = new QPushButton(title);
       button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
       connect(button, &QPushButton::clicked, this, slot);
       layout->addWidget(button);
    };
    makeButton(tr("Выбрать директорию"), &MainWidget::setCurrentDirFromDialog);
    makeButton(tr("Поиск совпадений"), &MainWidget::findSameFilesCount);
    layout->addLayout(makeChooseThreadCountLayout());
    return layout;
}

QVBoxLayout *MainWidget::makeChooseThreadCountLayout() const
{
    QVBoxLayout *layout = new QVBoxLayout;
    QComboBox *numberOfThreadComboBox = new QComboBox;
    for (int i = 1, itemCount = maxThreadCount(); i <= itemCount; ++i) {
        numberOfThreadComboBox->addItem(QString::number(i));
    }
    connect(numberOfThreadComboBox, qOverload<int>(&QComboBox::currentIndexChanged),
            _dirAnalyzer, [this](const int currentIndex) {
        _dirAnalyzer->setThreadCount(currentIndex + 1);
    });
    connect(_dirAnalyzer, &DirAnalyzer::threadCountChanged, numberOfThreadComboBox, [numberOfThreadComboBox](const int threadCount) {
        numberOfThreadComboBox->setCurrentIndex(threadCount - 1);
    });
    numberOfThreadComboBox->setCurrentIndex(_dirAnalyzer->threadCount() - 1);
    layout->addWidget(new QLabel(tr("Число потоков")));
    layout->addWidget(numberOfThreadComboBox);
    return layout;
}

QTableWidget *MainWidget::makeTableWidget() const
{
    QTableWidget *tableWidget = new QTableWidget;
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderLabels({tr("Имя"), tr("Количество повторений")});
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    return tableWidget;
}

void MainWidget::configureProgressDialog(QProgressDialog *progressDialog)
{
    progressDialog->setLabelText(tr("Расчет"));
    progressDialog->setWindowTitle(tr("Расчет"));
    progressDialog->setMaximum(0);
    connect(progressDialog, &QProgressDialog::canceled, _dirAnalyzer, &DirAnalyzer::interrupt);
    connect(progressDialog, &QProgressDialog::canceled, progressDialog, [progressDialog]() {
        progressDialog->setLabelText(tr("Остановка"));
    });
}

void MainWidget::updateTableWidget(const QList<QPair<QString, int> > &dataItems)
{
    _tableWidget->clearContents();
    const int rowCount = dataItems.size();
    _tableWidget->setRowCount(rowCount);
    for (int currentRow = 0; currentRow < rowCount; ++currentRow) {
        const QPair<QString, int> &dataItem = dataItems[currentRow];
        _tableWidget->setItem(currentRow, 0, new QTableWidgetItem(dataItem.first));
        _tableWidget->setItem(currentRow, 1, new QTableWidgetItem(QString::number(dataItem.second)));
    }
}
