#include "mainwidget.h"

#include "dirinfocollector.h"
#include "dirinfoanalyzer.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QTableWidget>
#include <QHeaderView>
#include <QProgressDialog>
#include <QComboBox>
#include <QMessageBox>

#include <QFuture>
#include <QtConcurrent/QtConcurrent>

MainWidget::MainWidget(QWidget *parent)
    : QWidget{parent}
{
    _dirInfoCollector = new DirInfoCollector(this);
    _settings = new QSettings("settings.ini", QSettings::IniFormat, this);

    makeGui();
    setCurrentDir(_settings->value("currentDir", QDir::currentPath()).toString());
}

void MainWidget::setCurrentDirFromDialog()
{
    const QString directoryPath = QFileDialog::getExistingDirectory(this,
                                                                    tr("Выбор директории для поиска"),
                                                                    _currentChoosedDir);
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

void MainWidget::findMostCommonFileAndDirNames()
{
    _interrupted = false;
    QProgressDialog progressDialog(this);
    configureProgressDialog(&progressDialog);
    progressDialog.show();

    QElapsedTimer timer;
    timer.start();
    const QString handleDir = _currentChoosedDir;
    const QMap<QString, int> dirInfo = collectDirInfoInSeparateThread();
    progressDialog.setLabelText(tr("Поиск максимальных значений"));
    updateTableWidget(DirInfoAnalyzer(dirInfo).getMostCommonNamesList(_countOfTableViewElemets));
    progressDialog.hide();
    emit handledDir(_interrupted ? handleDir : "");
    QMessageBox::information(this, tr("Время расчета"),
                             QTime(0, 0, 0).addMSecs(timer.elapsed()).toString("hh:mm:ss.zzz"));
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
    layout->addLayout(makeDirLabelsLayout());

    const auto addButton = [this, layout](const QString &title, const auto &slot) {
       QPushButton *button = new QPushButton(title);
       button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
       connect(button, &QPushButton::clicked, this, slot);
       layout->addWidget(button);
    };
    addButton(tr("Выбрать директорию"), &MainWidget::setCurrentDirFromDialog);
    addButton(tr("Поиск совпадений"), &MainWidget::findMostCommonFileAndDirNames);
    layout->addLayout(makeChooseThreadCountLayout());
    return layout;
}

QVBoxLayout *MainWidget::makeDirLabelsLayout() const
{
    QVBoxLayout *layout = new QVBoxLayout;
    const auto addLabel = [this, layout](const QString &title, const auto &updateSignal) {
        QLabel *label = new QLabel;
        layout->addWidget(new QLabel(title));
        label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        connect(this, updateSignal, label, &QLabel::setText);
        layout->addWidget(label);
    };
    addLabel(tr("Текущая выбранная директория"), &MainWidget::currentDirChanged);
    addLabel(tr("Последняя отображенная директория"), &MainWidget::handledDir);
    return layout;
}

QVBoxLayout *MainWidget::makeChooseThreadCountLayout() const
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(new QLabel(tr("Число потоков")));

    QComboBox *numberOfThreadComboBox = new QComboBox;
    for (int i = 1, itemCount = maxThreadCount(); i <= itemCount; ++i) {
        numberOfThreadComboBox->addItem(QString::number(i));
    }
    connect(numberOfThreadComboBox, qOverload<int>(&QComboBox::currentIndexChanged),
            _dirInfoCollector, [this](const int currentIndex) {
        _dirInfoCollector->setMaxThreadCount(currentIndex + 1);
    });
    connect(_dirInfoCollector, &DirInfoCollector::maxThreadCountChanged, numberOfThreadComboBox, [numberOfThreadComboBox](const int threadCount) {
        numberOfThreadComboBox->setCurrentIndex(threadCount - 1);
    });
    numberOfThreadComboBox->setCurrentIndex(_dirInfoCollector->maxThreadCount() - 1);
    layout->addWidget(numberOfThreadComboBox);
    return layout;
}

QTableWidget *MainWidget::makeTableWidget() const
{
    QTableWidget *tableWidget = new QTableWidget;
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderLabels({tr("Имя"), tr("Количество повторений")});
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    return tableWidget;
}

void MainWidget::configureProgressDialog(QProgressDialog *progressDialog)
{
    progressDialog->setLabelText(tr("Поиск совпадений"));
    progressDialog->setWindowTitle(tr("Поиск совпадений"));
    progressDialog->setMaximum(0); // Количество шагов не определяется, поэтому бесконечный прогресс бар
    connect(progressDialog, &QProgressDialog::canceled, _dirInfoCollector, &DirInfoCollector::interrupt);
    connect(progressDialog, &QProgressDialog::canceled, progressDialog, [progressDialog, this]() {
        progressDialog->setLabelText(tr("Остановка"));
        _interrupted = true;
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

QMap<QString, int> MainWidget::collectDirInfoInSeparateThread()
{
    QFutureWatcher<QMap<QString, int>> watcher;
    QFuture<QMap<QString, int>> future = QtConcurrent::run(std::bind(&DirInfoCollector::collectDirInfo,
                                                                     _dirInfoCollector, _currentChoosedDir));
    watcher.setFuture(future);
    QEventLoop eventLoop;
    connect(&watcher, &QFutureWatcher<QMap<QString, int>>::finished, &eventLoop, &QEventLoop::quit, Qt::QueuedConnection);
    eventLoop.exec();
    return future.result();
}
