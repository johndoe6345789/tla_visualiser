#include "tlc_runner.h"
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
#include <array>
#include <memory>
#include <cstdio>
#include <iostream>

namespace tla_visualiser {

class TLCRunner::Impl {
public:
    Status status;
    RunResults results;
    std::function<void(Status)> status_callback;
    std::function<void(int, const std::string&)> progress_callback;
    std::thread runner_thread;
    bool should_cancel;

    Impl() : status(Status::NotStarted), should_cancel(false) {
        results.status = Status::NotStarted;
        results.states_generated = 0;
        results.distinct_states = 0;
        results.execution_time_seconds = 0.0;
    }

    ~Impl() {
        should_cancel = true;
        if (runner_thread.joinable()) {
            runner_thread.join();
        }
    }

    std::string executeCommand(const std::string& cmd) {
        std::array<char, 128> buffer;
        std::string result;
        
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
        if (!pipe) {
            return "";
        }
        
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }
        
        return result;
    }

    void parseResults(const std::string& output) {
        // Parse TLC output (simplified - real implementation would be more robust)
        std::istringstream stream(output);
        std::string line;
        
        while (std::getline(stream, line)) {
            // Look for state count
            if (line.find("states generated") != std::string::npos) {
                // Extract number
                size_t pos = line.find_first_of("0123456789");
                if (pos != std::string::npos) {
                    results.states_generated = std::stoi(line.substr(pos));
                }
            }
            
            // Look for distinct states
            if (line.find("distinct states") != std::string::npos) {
                size_t pos = line.find_first_of("0123456789");
                if (pos != std::string::npos) {
                    results.distinct_states = std::stoi(line.substr(pos));
                }
            }
            
            // Look for errors
            if (line.find("Error:") != std::string::npos) {
                results.error_message += line + "\n";
            }
        }
    }
};

TLCRunner::TLCRunner() : pImpl(std::make_unique<Impl>()) {}

TLCRunner::~TLCRunner() = default;

bool TLCRunner::startModelCheck(const std::string& spec_file, const std::string& config_file) {
    if (pImpl->status == Status::Running) {
        return false;
    }

    pImpl->status = Status::Running;
    pImpl->results = RunResults{};
    pImpl->results.status = Status::Running;
    pImpl->should_cancel = false;

    if (pImpl->status_callback) {
        pImpl->status_callback(Status::Running);
    }

    // Start TLC in a separate thread
    pImpl->runner_thread = std::thread([this, spec_file, config_file]() {
        auto start_time = std::chrono::steady_clock::now();

        // Build TLC command
        std::string cmd = "java -jar tla2tools.jar -tool " + spec_file;
        if (!config_file.empty()) {
            cmd += " -config " + config_file;
        }

        // Execute TLC
        std::string output = pImpl->executeCommand(cmd);
        
        auto end_time = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = end_time - start_time;
        
        pImpl->results.execution_time_seconds = elapsed.count();
        
        // Parse output
        pImpl->parseResults(output);

        // Update status
        if (pImpl->should_cancel) {
            pImpl->status = Status::Cancelled;
            pImpl->results.status = Status::Cancelled;
        } else if (!pImpl->results.error_message.empty()) {
            pImpl->status = Status::Failed;
            pImpl->results.status = Status::Failed;
        } else {
            pImpl->status = Status::Completed;
            pImpl->results.status = Status::Completed;
        }

        if (pImpl->status_callback) {
            pImpl->status_callback(pImpl->status);
        }
    });

    return true;
}

void TLCRunner::cancel() {
    pImpl->should_cancel = true;
}

TLCRunner::Status TLCRunner::getStatus() const {
    return pImpl->status;
}

TLCRunner::RunResults TLCRunner::getResults() const {
    return pImpl->results;
}

void TLCRunner::setStatusCallback(std::function<void(Status)> callback) {
    pImpl->status_callback = callback;
}

void TLCRunner::setProgressCallback(std::function<void(int, const std::string&)> callback) {
    pImpl->progress_callback = callback;
}

bool TLCRunner::saveResults(const std::string& filename) const {
    std::ofstream out(filename);
    if (!out) return false;

    // Save as simple text format (could be JSON in real implementation)
    out << "Status: " << static_cast<int>(pImpl->results.status) << "\n";
    out << "States: " << pImpl->results.states_generated << "\n";
    out << "Distinct: " << pImpl->results.distinct_states << "\n";
    out << "Time: " << pImpl->results.execution_time_seconds << "\n";
    if (!pImpl->results.error_message.empty()) {
        out << "Error: " << pImpl->results.error_message << "\n";
    }

    return true;
}

bool TLCRunner::loadResults(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) return false;

    // Load from simple text format
    std::string line;
    while (std::getline(in, line)) {
        if (line.find("States:") == 0) {
            pImpl->results.states_generated = std::stoi(line.substr(8));
        } else if (line.find("Distinct:") == 0) {
            pImpl->results.distinct_states = std::stoi(line.substr(10));
        } else if (line.find("Time:") == 0) {
            pImpl->results.execution_time_seconds = std::stod(line.substr(6));
        }
    }

    pImpl->status = Status::Completed;
    return true;
}

} // namespace tla_visualiser
