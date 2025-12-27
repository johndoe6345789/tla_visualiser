#ifndef GITHUB_IMPORTER_H
#define GITHUB_IMPORTER_H

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace tla_visualiser {

/**
 * @brief Handles importing TLA+ specifications from GitHub URLs
 * 
 * Supports:
 * - File URLs: https://github.com/owner/repo/blob/branch/file.tla
 * - Raw URLs: https://raw.githubusercontent.com/owner/repo/branch/file.tla
 * - Repo URLs: https://github.com/owner/repo
 */
class GitHubImporter {
public:
    struct UrlInfo {
        std::string owner;
        std::string repo;
        std::string branch;
        std::string file_path;
        std::string commit_sha;
        bool is_file_url;
        bool is_raw_url;
    };

    struct FileInfo {
        std::string path;
        std::string content;
        std::string sha;
    };

    GitHubImporter();
    ~GitHubImporter();

    /**
     * @brief Parse a GitHub URL into its components
     * @param url The GitHub URL to parse
     * @return UrlInfo structure with parsed components
     */
    UrlInfo parseUrl(const std::string& url);

    /**
     * @brief Fetch a single file from GitHub
     * @param url_info Parsed URL information
     * @return File content as string
     */
    std::string fetchFile(const UrlInfo& url_info);

    /**
     * @brief Fetch all TLA+ files from a repository
     * @param url_info Parsed URL information
     * @return Vector of FileInfo structures
     */
    std::vector<FileInfo> fetchRepository(const UrlInfo& url_info);

    /**
     * @brief Save fetched content to local cache
     * @param url_info Source URL information
     * @param content File content to cache
     */
    void cacheContent(const UrlInfo& url_info, const std::string& content);

    /**
     * @brief Load content from local cache
     * @param url_info URL information for cache lookup
     * @return Cached content or empty string if not found
     */
    std::string loadFromCache(const UrlInfo& url_info);

    /**
     * @brief Set callback for progress updates
     */
    void setProgressCallback(std::function<void(int)> callback);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace tla_visualiser

#endif // GITHUB_IMPORTER_H
