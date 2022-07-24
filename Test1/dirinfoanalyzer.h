#ifndef DIRINFOANALYZER_H
#define DIRINFOANALYZER_H

#include <QMap>
#include <QString>
#include <QList>

class DirInfoAnalyzer
{
public:
    DirInfoAnalyzer();
    explicit DirInfoAnalyzer(const QMap<QString, int> &data);

    ///
    /// \brief getMostCommonNamesList Получить список самых распространенных названий
    /// \param count Число элементов в выходном списке
    /// \note Если размер данных анализатора меньше запрошенного размера выходного списка,
    ///  то размер выходного списка будет равен размеру данных анализатора
    /// \return Список самых распространенных названий
    ///
    QList<QPair<QString, int>> getMostCommonNamesList(const int count) const;
private:
    ///
    /// \brief toList Преобразование в спиcок
    /// \param sourceValues Исходные данные
    /// \return Исходные данные преоборазованные в список
    ///
    QList<QPair<QString, int>> toList(const QMap<QString, int> &sourceValues) const;

    QList<QPair<QString, int>> _data; ///< Данные
};

#endif // DIRINFOANALYZER_H
