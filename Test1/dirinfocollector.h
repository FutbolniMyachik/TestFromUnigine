#ifndef DIRINFOCOLLECTOR_H
#define DIRINFOCOLLECTOR_H

#include <QList>
#include <QMap>
#include <QString>
#include <QDir>

#include <QMutex>

class QThreadPool;

///
/// \brief Класс для сбора информации о директориях
///
class DirInfoCollector : public QObject
{
    Q_OBJECT
public:
    explicit DirInfoCollector(QObject *parent = nullptr);
    QList<QPair<QString, int>> getMostCommon(const int count, const QMap<QString, int> &sourceValues) const;
    ///
    /// \brief collectDitInfo Сбрать информацию о директории
    /// \param dirPath Путь до директории
    /// \note Отличается от collectDitInfoInternal наличием дополнительной работы с потоками,
    ///  наличием обработки результата, отсутствием обработки директории по пути dirPath
    /// \return Информация об директориях
    ///
    QMap<QString, int> collectDitInfo(const QString &dirPath);
    ///
    /// \brief setMaxThreadCount Установить максимальное число потоков
    /// \param threadCount Число потоков
    /// \note Число потоков должно быть больше 0, иначе значение потоков не будет установлено
    ///
    void setMaxThreadCount(const int threadCount);
    ///
    /// \brief maxThreadCount Получить максимальное число потоков
    /// \return Число потоков
    ///
    int maxThreadCount() const;
signals:
    ///
    /// \brief Сигнал, уведомляющий об изменении максимального числа потоков
    /// \param threadCount Текущее максимальное число потоков
    ///
    void maxThreadCountChanged(int threadCount);
public slots:
    ///
    /// \brief interrupt Прервать сбор информации
    /// \note Прерывание происходит не мгновенно, потому что прерывание только останавливает
    /// прохождение по дереву директорий. Директории, которые попали в поток, но еще не до
    /// конца обработаны будут обработаны до конца.
    void interrupt();
private:
    ///
    /// \brief incrementFileNameRepeatCount Инкрементировать число повторений названий файла
    /// \param fileName Название инкрементируемого файла
    /// \note Функция потокобезопасна
    ///
    void incrementFileNameRepeatCount(const QString &fileName);
    ///
    /// \brief handleDirInSeparateThread Обработать директорию в отдельном потоке
    /// \param dirPath Путь до директории
    ///
    void handleDirInSeparateThread(const QString &dirPath);
    ///
    /// \brief isValidElement Проверить на валидность путь до директории
    /// \param dirPath Путь до директории
    /// \note Невалидными считаются несуществующие и скрытые директории
    /// \return Валидность директории
    ///
    bool isValidElement(const QString &dirPath) const;
    ///
    /// \brief collectDitInfoInternal Собрать информацию о директории
    /// \param dirPath Путь до директории
    /// \note Отличается от collectDitInfo отсутствием дополнительной работы с потоками,
    ///  отсутствием обработки результата, наличием обработки директории по пути dirPath
    ///
    void collectDitInfoInternal(const QString &dirPath);
    QList<QPair<QString, int>> toList(const QMap<QString, int> &sourceValues) const;
    QMap<QString, int> _dirInfoMap; ///< Дерево хранящее информацию о директориях
    QThreadPool *_threads{nullptr}; ///< Потоки
    bool _interrupt{false}; ///< Флаг прерывания сбора информации
    QMutex _mutex; ///< Мютекс для потокобезопасной записи в _dirInfoMap
};

#endif // DIRINFOCOLLECTOR_H
