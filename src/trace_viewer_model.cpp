#include "trace_viewer_model.h"
#include <QVariantMap>
#include <QVariantList>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <sstream>

namespace tla_visualiser {

class TraceViewerModel::Impl {
public:
    struct TraceStep {
        int step_number;
        int state_id;
        std::string state_description;
        std::string action;
        std::vector<std::pair<std::string, std::string>> variables;
    };

    std::vector<TraceStep> steps;
    int current_step;

    Impl() : current_step(0) {}
};

TraceViewerModel::TraceViewerModel(QObject* parent)
    : QAbstractListModel(parent), pImpl(std::make_unique<Impl>()) {}

TraceViewerModel::~TraceViewerModel() = default;

int TraceViewerModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return pImpl->steps.size();
}

QVariant TraceViewerModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= static_cast<int>(pImpl->steps.size())) {
        return QVariant();
    }

    const auto& step = pImpl->steps[index.row()];

    switch (role) {
    case StepNumberRole:
        return step.step_number;
    case StateIdRole:
        return step.state_id;
    case StateDescriptionRole:
        return QString::fromStdString(step.state_description);
    case ActionRole:
        return QString::fromStdString(step.action);
    case VariablesRole: {
        QVariantList vars;
        for (const auto& [key, value] : step.variables) {
            QVariantMap var;
            var["name"] = QString::fromStdString(key);
            var["value"] = QString::fromStdString(value);
            vars.append(var);
        }
        return vars;
    }
    }

    return QVariant();
}

QHash<int, QByteArray> TraceViewerModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[StepNumberRole] = "stepNumber";
    roles[StateIdRole] = "stateId";
    roles[StateDescriptionRole] = "description";
    roles[ActionRole] = "action";
    roles[VariablesRole] = "variables";
    return roles;
}

void TraceViewerModel::loadTrace(const TLCRunner::CounterExample& trace,
                                  const TLCRunner::RunResults& results) {
    beginResetModel();
    pImpl->steps.clear();

    int step_num = 0;
    for (int state_id : trace.state_sequence) {
        // Find state in results
        auto it = std::find_if(results.states.begin(), results.states.end(),
                              [state_id](const TLCRunner::State& s) { return s.id == state_id; });
        
        if (it != results.states.end()) {
            Impl::TraceStep step;
            step.step_number = step_num++;
            step.state_id = state_id;
            step.state_description = it->description;
            step.variables = it->variables;
            
            // Find action (transition to this state)
            if (step_num > 1) {
                auto trans_it = std::find_if(results.transitions.begin(), results.transitions.end(),
                    [state_id](const TLCRunner::Transition& t) { return t.to_state == state_id; });
                if (trans_it != results.transitions.end()) {
                    step.action = trans_it->action;
                }
            } else {
                step.action = "Initial";
            }
            
            pImpl->steps.push_back(step);
        }
    }

    endResetModel();
    emit traceUpdated();
}

void TraceViewerModel::clear() {
    beginResetModel();
    pImpl->steps.clear();
    pImpl->current_step = 0;
    endResetModel();
}

QVariantMap TraceViewerModel::getStepDetails(int step) const {
    QVariantMap result;
    if (step >= 0 && step < static_cast<int>(pImpl->steps.size())) {
        const auto& s = pImpl->steps[step];
        result["stepNumber"] = s.step_number;
        result["stateId"] = s.state_id;
        result["description"] = QString::fromStdString(s.state_description);
        result["action"] = QString::fromStdString(s.action);
        
        QVariantList vars;
        for (const auto& [key, value] : s.variables) {
            QVariantMap var;
            var["name"] = QString::fromStdString(key);
            var["value"] = QString::fromStdString(value);
            vars.append(var);
        }
        result["variables"] = vars;
    }
    return result;
}

QString TraceViewerModel::exportToMarkdown() const {
    std::stringstream ss;
    ss << "# Trace\n\n";
    
    for (const auto& step : pImpl->steps) {
        ss << "## Step " << step.step_number << "\n\n";
        ss << "**State ID:** " << step.state_id << "\n\n";
        ss << "**Action:** " << step.action << "\n\n";
        ss << "**Variables:**\n\n";
        for (const auto& [key, value] : step.variables) {
            ss << "- `" << key << "` = " << value << "\n";
        }
        ss << "\n";
    }
    
    return QString::fromStdString(ss.str());
}

QString TraceViewerModel::exportToJson() const {
    QJsonArray stepsArray;
    
    for (const auto& step : pImpl->steps) {
        QJsonObject stepObj;
        stepObj["stepNumber"] = step.step_number;
        stepObj["stateId"] = step.state_id;
        stepObj["action"] = QString::fromStdString(step.action);
        
        QJsonArray varsArray;
        for (const auto& [key, value] : step.variables) {
            QJsonObject varObj;
            varObj["name"] = QString::fromStdString(key);
            varObj["value"] = QString::fromStdString(value);
            varsArray.append(varObj);
        }
        stepObj["variables"] = varsArray;
        
        stepsArray.append(stepObj);
    }
    
    QJsonObject root;
    root["trace"] = stepsArray;
    
    QJsonDocument doc(root);
    return QString::fromUtf8(doc.toJson(QJsonDocument::Indented));
}

int TraceViewerModel::stepCount() const {
    return pImpl->steps.size();
}

int TraceViewerModel::currentStep() const {
    return pImpl->current_step;
}

void TraceViewerModel::setCurrentStep(int step) {
    if (step != pImpl->current_step && step >= 0 && step < static_cast<int>(pImpl->steps.size())) {
        pImpl->current_step = step;
        emit currentStepChanged();
    }
}

} // namespace tla_visualiser
