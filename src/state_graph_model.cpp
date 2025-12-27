#include "state_graph_model.h"
#include <QVariantMap>
#include <QVariantList>
#include <algorithm>
#include <cmath>

namespace tla_visualiser {

class StateGraphModel::Impl {
public:
    std::vector<TLCRunner::State> states;
    std::vector<TLCRunner::Transition> transitions;
    std::vector<std::pair<double, double>> positions;
    double layout_radius = 200.0;  // Configurable radius

    void calculateLayout() {
        // Simple circular layout with configurable radius
        int n = states.size();
        if (n == 0) return;

        positions.clear();
        
        // Adjust radius based on number of nodes for better spacing
        double radius = layout_radius;
        if (n > 10) {
            radius = layout_radius * (1.0 + std::log(n / 10.0));
        }
        
        double angle_step = 2.0 * M_PI / n;

        for (int i = 0; i < n; ++i) {
            double angle = i * angle_step;
            double x = radius * std::cos(angle);
            double y = radius * std::sin(angle);
            positions.push_back({x, y});
        }
    }
};

StateGraphModel::StateGraphModel(QObject* parent)
    : QAbstractListModel(parent), pImpl(std::make_unique<Impl>()) {}

StateGraphModel::~StateGraphModel() = default;

int StateGraphModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return pImpl->states.size();
}

QVariant StateGraphModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= static_cast<int>(pImpl->states.size())) {
        return QVariant();
    }

    const auto& state = pImpl->states[index.row()];
    const auto& pos = pImpl->positions[index.row()];

    switch (role) {
    case StateIdRole:
        return state.id;
    case StateDescriptionRole:
        return QString::fromStdString(state.description);
    case StateVariablesRole: {
        QVariantList vars;
        for (const auto& [key, value] : state.variables) {
            QVariantMap var;
            var["name"] = QString::fromStdString(key);
            var["value"] = QString::fromStdString(value);
            vars.append(var);
        }
        return vars;
    }
    case StateXRole:
        return pos.first;
    case StateYRole:
        return pos.second;
    }

    return QVariant();
}

QHash<int, QByteArray> StateGraphModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[StateIdRole] = "stateId";
    roles[StateDescriptionRole] = "description";
    roles[StateVariablesRole] = "variables";
    roles[StateXRole] = "x";
    roles[StateYRole] = "y";
    return roles;
}

void StateGraphModel::loadFromResults(const TLCRunner::RunResults& results) {
    beginResetModel();
    pImpl->states = results.states;
    pImpl->transitions = results.transitions;
    pImpl->calculateLayout();
    endResetModel();
    emit graphUpdated();
}

void StateGraphModel::clear() {
    beginResetModel();
    pImpl->states.clear();
    pImpl->transitions.clear();
    pImpl->positions.clear();
    endResetModel();
}

QVariantList StateGraphModel::getTransitions() const {
    QVariantList result;
    for (const auto& trans : pImpl->transitions) {
        QVariantMap t;
        t["from"] = trans.from_state;
        t["to"] = trans.to_state;
        t["action"] = QString::fromStdString(trans.action);
        result.append(t);
    }
    return result;
}

QVariantMap StateGraphModel::getStateDetails(int stateId) const {
    auto it = std::find_if(pImpl->states.begin(), pImpl->states.end(),
                          [stateId](const TLCRunner::State& s) { return s.id == stateId; });
    
    QVariantMap result;
    if (it != pImpl->states.end()) {
        result["id"] = it->id;
        result["description"] = QString::fromStdString(it->description);
        
        QVariantList vars;
        for (const auto& [key, value] : it->variables) {
            QVariantMap var;
            var["name"] = QString::fromStdString(key);
            var["value"] = QString::fromStdString(value);
            vars.append(var);
        }
        result["variables"] = vars;
    }
    
    return result;
}

int StateGraphModel::nodeCount() const {
    return pImpl->states.size();
}

int StateGraphModel::edgeCount() const {
    return pImpl->transitions.size();
}

} // namespace tla_visualiser
