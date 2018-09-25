#ifndef TESTOBJECT
#define TESTOBJECT

#include <QtTest>
#include <QObject>

class TestObject : public QObject
{
Q_OBJECT
public:
    TestObject() : m_receivedResponse(false) {}

    void waitForResponse(int timeout = 5000)
    {
        int i = 0;
        while (!m_receivedResponse && i < timeout) {
            QTest::qWait(250);
            i += 250;
        }

        if (!m_receivedResponse)
          QFAIL("Failed to receive a response!");
    }

    bool receivedResponse() const
    { return m_receivedResponse; }
    void setReceivedResponse(bool receivedResponse)
    { m_receivedResponse = receivedResponse; }

private:
    bool m_receivedResponse;
};

#endif
