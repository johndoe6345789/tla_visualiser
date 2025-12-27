#include <QtTest/QtTest>
#include <QDir>
#include <QFile>
#include "tlc_runner.h"

class TestTLCRunner : public QObject
{
    Q_OBJECT

private slots:
    void testInitialStatus();
    void testResultsSaving();
};

void TestTLCRunner::testInitialStatus()
{
    tla_visualiser::TLCRunner runner;
    
    QCOMPARE(runner.getStatus(), tla_visualiser::TLCRunner::Status::NotStarted);
    
    auto results = runner.getResults();
    QCOMPARE(results.status, tla_visualiser::TLCRunner::Status::NotStarted);
    QCOMPARE(results.states_generated, 0);
    QCOMPARE(results.distinct_states, 0);
}

void TestTLCRunner::testResultsSaving()
{
    tla_visualiser::TLCRunner runner;
    
    // Create a temporary file for testing
    QString tempFile = QDir::tempPath() + "/test_results.txt";
    
    bool saved = runner.saveResults(tempFile.toStdString());
    QVERIFY(saved);
    
    // Verify file exists
    QVERIFY(QFile::exists(tempFile));
    
    // Clean up
    QFile::remove(tempFile);
}

QTEST_MAIN(TestTLCRunner)
#include "test_tlc_runner.moc"
