#include "dirinfoanalyzer.h"

DirInfoAnalyzer::DirInfoAnalyzer()
{

}

DirInfoAnalyzer::DirInfoAnalyzer(const QMap<QString, int> &data)
    : _data(toList(data))
{
    std::sort(_data.begin(), _data.end(),
              [](const QPair<QString, int> &value1, const QPair<QString, int> &value2) {
        return value1.second > value2.second;
    });
}

QList<QPair<QString, int>> DirInfoAnalyzer::getMostCommonNamesList(const int count) const
{
    const int resultSize = std::min(count, _data.size());
    QList<QPair<QString, int> > listOfMostCommonNames;
    listOfMostCommonNames.reserve(resultSize);
    auto currentElement = _data.begin();
    for (int i = 0; i < resultSize; ++i) {
        listOfMostCommonNames.append({currentElement->first, currentElement->second});
        ++currentElement;
    }
    return listOfMostCommonNames;
}

QList<QPair<QString, int> > DirInfoAnalyzer::toList(const QMap<QString, int> &sourceValues) const
{
    QList<QPair<QString, int>> result;
    result.reserve(sourceValues.size());
    for (const QString &name : sourceValues.keys()) {
        result.append({name, sourceValues.value(name)});
    }
    return result;
}
