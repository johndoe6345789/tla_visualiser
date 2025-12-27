#ifndef STATE_GRAPH_MODEL_H
#define STATE_GRAPH_MODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QString>
#include <vector>
#include "tlc_runner.h"

namespace tla_visualiser {

/**
 * @brief Qt model for displaying state/transition graph
 * 
 * Provides data for QML visualization of the state space graph.
 */
class StateGraphModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int nodeCount READ nodeCount NOTIFY dataChanged)
    Q_PROPERTY(int edgeCount READ edgeCount NOTIFY dataChanged)

public:
    enum Roles {
        StateIdRole = Qt::UserRole + 1,
        StateDescriptionRole,
        StateVariablesRole,
        StateXRole,
        StateYRole
    };

    explicit StateGraphModel(QObject* parent = nullptr);
    ~StateGraphModel() override;

    // QAbstractListModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Custom methods
    Q_INVOKABLE void loadFromResults(const TLCRunner::RunResults& results);
    Q_INVOKABLE void clear();
    Q_INVOKABLE QVariantList getTransitions() const;
    Q_INVOKABLE QVariantMap getStateDetails(int stateId) const;

    int nodeCount() const;
    int edgeCount() const;

signals:
    void graphUpdated();

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace tla_visualiser

#endif // STATE_GRAPH_MODEL_H
