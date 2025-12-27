#ifndef TRACE_VIEWER_MODEL_H
#define TRACE_VIEWER_MODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QString>
#include <vector>
#include "tlc_runner.h"

namespace tla_visualiser {

/**
 * @brief Qt model for displaying trace/counterexample steps
 * 
 * Provides data for QML trace viewer with step-by-step inspection.
 */
class TraceViewerModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int stepCount READ stepCount NOTIFY dataChanged)
    Q_PROPERTY(int currentStep READ currentStep WRITE setCurrentStep NOTIFY currentStepChanged)

public:
    enum Roles {
        StepNumberRole = Qt::UserRole + 1,
        StateIdRole,
        StateDescriptionRole,
        ActionRole,
        VariablesRole
    };

    explicit TraceViewerModel(QObject* parent = nullptr);
    ~TraceViewerModel() override;

    // QAbstractListModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Custom methods
    Q_INVOKABLE void loadTrace(const TLCRunner::CounterExample& trace,
                               const TLCRunner::RunResults& results);
    Q_INVOKABLE void clear();
    Q_INVOKABLE QVariantMap getStepDetails(int step) const;
    Q_INVOKABLE QString exportToMarkdown() const;
    Q_INVOKABLE QString exportToJson() const;

    int stepCount() const;
    int currentStep() const;
    void setCurrentStep(int step);

signals:
    void currentStepChanged();
    void traceUpdated();

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace tla_visualiser

#endif // TRACE_VIEWER_MODEL_H
