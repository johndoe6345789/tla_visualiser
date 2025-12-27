#ifndef TLC_RUNNER_H
#define TLC_RUNNER_H

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace tla_visualiser {

/**
 * @brief Manages TLC model checker execution and result parsing
 * 
 * Handles running the TLC model checker as a Java process,
 * parsing its output, and providing structured access to results.
 */
class TLCRunner {
public:
    enum class Status {
        NotStarted,
        Running,
        Completed,
        Failed,
        Cancelled
    };

    struct State {
        int id;
        std::string description;
        std::vector<std::pair<std::string, std::string>> variables;
    };

    struct Transition {
        int from_state;
        int to_state;
        std::string action;
    };

    struct Invariant {
        std::string name;
        bool passed;
        std::string error_message;
        int error_state_id;
    };

    struct CounterExample {
        std::vector<int> state_sequence;
        std::string description;
    };

    struct RunResults {
        Status status;
        std::vector<State> states;
        std::vector<Transition> transitions;
        std::vector<Invariant> invariants;
        std::vector<CounterExample> counterexamples;
        int states_generated;
        int distinct_states;
        double execution_time_seconds;
        std::string error_message;
    };

    TLCRunner();
    ~TLCRunner();

    /**
     * @brief Start TLC model checking
     * @param spec_file Path to the TLA+ specification file
     * @param config_file Path to the TLC configuration file (optional)
     * @return true if started successfully
     */
    bool startModelCheck(const std::string& spec_file, 
                         const std::string& config_file = "");

    /**
     * @brief Cancel a running model check
     */
    void cancel();

    /**
     * @brief Get current status
     */
    Status getStatus() const;

    /**
     * @brief Get model checking results
     * @return RunResults structure with all results
     */
    RunResults getResults() const;

    /**
     * @brief Set callback for status updates
     */
    void setStatusCallback(std::function<void(Status)> callback);

    /**
     * @brief Set callback for progress updates
     */
    void setProgressCallback(std::function<void(int, const std::string&)> callback);

    /**
     * @brief Save run results to file
     * @param filename Path to save results
     * @return true if saved successfully
     */
    bool saveResults(const std::string& filename) const;

    /**
     * @brief Load run results from file
     * @param filename Path to load results from
     * @return true if loaded successfully
     */
    bool loadResults(const std::string& filename);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace tla_visualiser

#endif // TLC_RUNNER_H
