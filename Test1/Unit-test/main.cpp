#include "dirinfotest.h"

#include <QTest>

int main(int argc, char *argv[])
{
    DirInfoTest test;
    test.setDirectory(""); // Указать директорию для текста
    QTest::qExec(&test, argc, argv);
    return 0;
}
