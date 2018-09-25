#include <QCoreApplication>

#include <QtTest>

#include "TestObject.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    TestObject test1;
    QTest::qExec(&test1, argc, argv);

    return 0;
}
