#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>

class QHBoxLayout;
class QVBoxLayout;
class DirInfoCollector;
class QTableWidget;
class QProgressDialog;
class QSettings;

///
/// \brief The Класс главного виджета
///
class MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = nullptr);

signals:
    ///
    /// \brief currentDirChanged Сигнал, уведомляющий об изменении текущйе директории
    /// \param currentDir Текущая директория
    ///
    void currentDirChanged(const QString &currentDir);
    ///
    /// \brief handledDir Сигнал, увдомляющий об успешной обработке директории
    /// \param dirPath Путь до директории
    ///
    void handledDir(const QString &dirPath);
private slots:
    ///
    /// \brief setCurrentDirFromDialog Установить текущую директорию из диалога с пользователем
    ///
    void setCurrentDirFromDialog();
    ///
    /// \brief setCurrentDir Установить текущую директорию
    /// \param dirPath Путь до директории
    ///
    void setCurrentDir(const QString &dirPath);
    ///
    /// \brief findMostCommonFileNames Найти самые распространенные имена файлов и директорий
    ///
    void findMostCommonFileAndDirNames();
private:
    ///
    /// \brief maxThreadCount Получить максимальное число потоков
    /// \return Максимальное число потоков
    ///
    int maxThreadCount() const;
    ///
    /// \brief makeGui Создать GUI
    ///
    void makeGui();
    ///
    /// \brief makeControlLayout Создать панель для управления
    /// \return Панелья для управления
    ///
    QHBoxLayout *makeControlLayout() const;
    ///
    /// \brief makeDirLabelsLayout Создать панель для отображения информации об директориях
    /// \return Панелья для отображения информации об директориях
    ///
    QVBoxLayout *makeDirLabelsLayout() const;
    ///
    /// \brief makeChooseThreadCountLayout Создать панель для управлением числом потоков
    /// \return Панель для управления числом потоков
    ///
    QVBoxLayout *makeChooseThreadCountLayout() const;
    ///
    /// \brief makeTableWidget Создать виджет для отображения информации о файлах в виде таблицы
    /// \return Виджет для отображения информации о файлах в виде таблицы
    ///
    QTableWidget *makeTableWidget() const;
    ///
    /// \brief configureProgressDialog Сконфигурировать диалог с прогрессом сбора информации
    /// \param progressDialog Указатель на конфигурируемый диалог
    ///
    void configureProgressDialog(QProgressDialog *progressDialog);
    ///
    /// \brief updateTableWidget Обновить значения в таблице
    /// \param dataItems Данные
    ///
    void updateTableWidget(const QList<QPair<QString, int>> &dataItems);
    ///
    /// \brief collectDirInfoInSepareteThread Собрать информацию об директориях в отдельном потоке
    /// \return Информация об директориях
    ///
    QMap<QString, int> collectDirInfoInSeparateThread();

    QString _currentChoosedDir; ///< Текущая директория, по которой будет собираться информация
    DirInfoCollector *_dirInfoCollector{nullptr}; ///< Сборщик информации
    QTableWidget *_tableWidget{nullptr}; ///< Таблица с информацией о файлах
    QSettings *_settings{nullptr}; ///< Настройки
    bool _interrupted; ///< Флаг, был ли прерван расчет
    static constexpr int _countOfTableViewElemets{10}; ///< Число отображаемых строк в таблице
};

#endif // MAINWIDGET_H
