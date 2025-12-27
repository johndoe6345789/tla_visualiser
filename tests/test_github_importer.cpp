#include <QtTest/QtTest>
#include "github_importer.h"

class TestGitHubImporter : public QObject
{
    Q_OBJECT

private slots:
    void testParseFileUrl();
    void testParseRawUrl();
    void testParseRepoUrl();
    void testInvalidUrl();
};

void TestGitHubImporter::testParseFileUrl()
{
    tla_visualiser::GitHubImporter importer;
    
    std::string url = "https://github.com/owner/repo/blob/main/spec.tla";
    auto info = importer.parseUrl(url);
    
    QCOMPARE(info.owner, std::string("owner"));
    QCOMPARE(info.repo, std::string("repo"));
    QCOMPARE(info.branch, std::string("main"));
    QCOMPARE(info.file_path, std::string("spec.tla"));
    QVERIFY(info.is_file_url);
    QVERIFY(!info.is_raw_url);
}

void TestGitHubImporter::testParseRawUrl()
{
    tla_visualiser::GitHubImporter importer;
    
    std::string url = "https://raw.githubusercontent.com/owner/repo/main/spec.tla";
    auto info = importer.parseUrl(url);
    
    QCOMPARE(info.owner, std::string("owner"));
    QCOMPARE(info.repo, std::string("repo"));
    QCOMPARE(info.branch, std::string("main"));
    QCOMPARE(info.file_path, std::string("spec.tla"));
    QVERIFY(info.is_file_url);
    QVERIFY(info.is_raw_url);
}

void TestGitHubImporter::testParseRepoUrl()
{
    tla_visualiser::GitHubImporter importer;
    
    std::string url = "https://github.com/owner/repo";
    auto info = importer.parseUrl(url);
    
    QCOMPARE(info.owner, std::string("owner"));
    QCOMPARE(info.repo, std::string("repo"));
    QCOMPARE(info.branch, std::string("main")); // Default
    QVERIFY(!info.is_file_url);
}

void TestGitHubImporter::testInvalidUrl()
{
    tla_visualiser::GitHubImporter importer;
    
    std::string url = "https://example.com/invalid";
    auto info = importer.parseUrl(url);
    
    QVERIFY(info.owner.empty());
    QVERIFY(info.repo.empty());
}

QTEST_MAIN(TestGitHubImporter)
#include "test_github_importer.moc"
