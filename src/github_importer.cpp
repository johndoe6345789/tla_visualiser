#include "github_importer.h"
#include <curl/curl.h>
#include <regex>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>

namespace tla_visualiser {

// Callback for CURL to write data
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

class GitHubImporter::Impl {
public:
    CURL* curl;
    std::function<void(int)> progress_callback;
    std::string cache_dir;

    Impl() {
        curl = curl_easy_init();
        // Create cache directory in user's home
        cache_dir = std::filesystem::temp_directory_path() / "tla_visualiser_cache";
        std::filesystem::create_directories(cache_dir);
    }

    ~Impl() {
        if (curl) {
            curl_easy_cleanup(curl);
        }
    }

    std::string performRequest(const std::string& url) {
        std::string response;
        if (!curl) return response;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "tla_visualiser/1.0");

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
            return "";
        }

        return response;
    }
};

GitHubImporter::GitHubImporter() : pImpl(std::make_unique<Impl>()) {}

GitHubImporter::~GitHubImporter() = default;

GitHubImporter::UrlInfo GitHubImporter::parseUrl(const std::string& url) {
    UrlInfo info{};
    
    // Pattern for file URL: https://github.com/owner/repo/blob/branch/path/file.tla
    std::regex file_pattern(R"(github\.com/([^/]+)/([^/]+)/blob/([^/]+)/(.+))");
    
    // Pattern for raw URL: https://raw.githubusercontent.com/owner/repo/branch/path/file.tla
    std::regex raw_pattern(R"(raw\.githubusercontent\.com/([^/]+)/([^/]+)/([^/]+)/(.+))");
    
    // Pattern for repo URL: https://github.com/owner/repo
    std::regex repo_pattern(R"(github\.com/([^/]+)/([^/]+)/?$)");
    
    std::smatch match;
    
    if (std::regex_search(url, match, file_pattern)) {
        info.owner = match[1];
        info.repo = match[2];
        info.branch = match[3];
        info.file_path = match[4];
        info.is_file_url = true;
        info.is_raw_url = false;
    } else if (std::regex_search(url, match, raw_pattern)) {
        info.owner = match[1];
        info.repo = match[2];
        info.branch = match[3];
        info.file_path = match[4];
        info.is_file_url = true;
        info.is_raw_url = true;
    } else if (std::regex_search(url, match, repo_pattern)) {
        info.owner = match[1];
        info.repo = match[2];
        info.branch = "main"; // Default branch
        info.is_file_url = false;
        info.is_raw_url = false;
    }
    
    return info;
}

std::string GitHubImporter::fetchFile(const UrlInfo& url_info) {
    // Check cache first
    std::string cached = loadFromCache(url_info);
    if (!cached.empty()) {
        return cached;
    }

    // Build raw URL
    std::string raw_url = "https://raw.githubusercontent.com/" +
                         url_info.owner + "/" +
                         url_info.repo + "/" +
                         url_info.branch + "/" +
                         url_info.file_path;

    std::string content = pImpl->performRequest(raw_url);
    
    if (!content.empty()) {
        cacheContent(url_info, content);
    }
    
    return content;
}

std::vector<GitHubImporter::FileInfo> GitHubImporter::fetchRepository(const UrlInfo& url_info) {
    std::vector<FileInfo> files;
    
    // Use GitHub API to list repository contents
    std::string api_url = "https://api.github.com/repos/" +
                         url_info.owner + "/" +
                         url_info.repo + "/git/trees/" +
                         url_info.branch + "?recursive=1";
    
    std::string response = pImpl->performRequest(api_url);
    
    // Parse response and find .tla files (simplified - real implementation would use JSON parser)
    // For now, return empty vector as this requires JSON parsing library
    
    return files;
}

void GitHubImporter::cacheContent(const UrlInfo& url_info, const std::string& content) {
    std::string cache_file = pImpl->cache_dir + "/" +
                            url_info.owner + "_" +
                            url_info.repo + "_" +
                            url_info.branch + "_" +
                            url_info.file_path;
    
    // Replace path separators with underscores
    std::replace(cache_file.begin(), cache_file.end(), '/', '_');
    
    std::ofstream out(cache_file);
    if (out) {
        out << content;
    }
}

std::string GitHubImporter::loadFromCache(const UrlInfo& url_info) {
    std::string cache_file = pImpl->cache_dir + "/" +
                            url_info.owner + "_" +
                            url_info.repo + "_" +
                            url_info.branch + "_" +
                            url_info.file_path;
    
    std::replace(cache_file.begin(), cache_file.end(), '/', '_');
    
    std::ifstream in(cache_file);
    if (!in) return "";
    
    std::stringstream buffer;
    buffer << in.rdbuf();
    return buffer.str();
}

void GitHubImporter::setProgressCallback(std::function<void(int)> callback) {
    pImpl->progress_callback = callback;
}

} // namespace tla_visualiser
