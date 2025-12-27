import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import TLAVisualiser 1.0

ApplicationWindow {
    id: root
    width: 1280
    height: 800
    visible: true
    title: "TLA+ Visualiser"

    menuBar: MenuBar {
        Menu {
            title: "&File"
            MenuItem {
                text: "&Import from GitHub..."
                onTriggered: importDialog.open()
            }
            MenuItem {
                text: "&Open Cached Spec..."
                onTriggered: {
                    // Open file dialog
                }
            }
            MenuSeparator {}
            MenuItem {
                text: "&Export Graph..."
                enabled: tabBar.currentIndex === 1
                onTriggered: {
                    // Export graph
                }
            }
            MenuItem {
                text: "Export &Trace..."
                enabled: tabBar.currentIndex === 2
                onTriggered: {
                    // Export trace
                }
            }
            MenuSeparator {}
            MenuItem {
                text: "&Quit"
                onTriggered: Qt.quit()
            }
        }
        Menu {
            title: "&Model"
            MenuItem {
                text: "&Run Model Checker"
                enabled: importView.hasSpec
                onTriggered: {
                    // Run TLC
                }
            }
            MenuItem {
                text: "&Stop"
                enabled: false
                onTriggered: {
                    // Stop TLC
                }
            }
        }
        Menu {
            title: "&Help"
            MenuItem {
                text: "&About"
                onTriggered: aboutDialog.open()
            }
        }
    }

    header: TabBar {
        id: tabBar
        TabButton {
            text: "Import"
        }
        TabButton {
            text: "Graph View"
            enabled: stateGraphModel.nodeCount > 0
        }
        TabButton {
            text: "Trace View"
            enabled: traceViewerModel.stepCount > 0
        }
        TabButton {
            text: "Invariants"
            enabled: false
        }
    }

    StackLayout {
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        ImportView {
            id: importView
        }

        GraphView {
            id: graphView
            model: stateGraphModel
        }

        TraceView {
            id: traceView
            model: traceViewerModel
        }

        InvariantView {
            id: invariantView
        }
    }

    StateGraphModel {
        id: stateGraphModel
    }

    TraceViewerModel {
        id: traceViewerModel
    }

    Dialog {
        id: importDialog
        title: "Import from GitHub"
        modal: true
        standardButtons: Dialog.Ok | Dialog.Cancel
        
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        width: 600

        ColumnLayout {
            anchors.fill: parent
            spacing: 10

            Label {
                text: "Enter GitHub URL:"
                font.bold: true
            }

            TextField {
                id: urlField
                Layout.fillWidth: true
                placeholderText: "https://github.com/owner/repo/blob/main/spec.tla"
            }

            Label {
                text: "Supported formats:"
                font.pointSize: 9
            }

            Label {
                text: "• File URL: https://github.com/owner/repo/blob/branch/file.tla\n" +
                      "• Raw URL: https://raw.githubusercontent.com/owner/repo/branch/file.tla\n" +
                      "• Repo URL: https://github.com/owner/repo"
                font.pointSize: 9
                color: "#666666"
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
            }
        }

        onAccepted: {
            if (urlField.text) {
                importView.importFromUrl(urlField.text)
                urlField.text = ""
            }
        }
    }

    Dialog {
        id: aboutDialog
        title: "About TLA+ Visualiser"
        modal: true
        standardButtons: Dialog.Ok
        
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        width: 400

        ColumnLayout {
            anchors.fill: parent
            spacing: 10

            Label {
                text: "TLA+ Visualiser"
                font.bold: true
                font.pointSize: 16
            }

            Label {
                text: "Version 1.0.0"
            }

            Label {
                text: "A cross-platform desktop tool for visualizing TLA+ specifications."
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }

            Label {
                text: "\nFeatures:"
                font.bold: true
            }

            Label {
                text: "• Import specs from GitHub URLs\n" +
                      "• Interactive state/transition graphs\n" +
                      "• Trace/counterexample viewer\n" +
                      "• Invariant dashboard\n" +
                      "• Export visuals and traces"
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
            }
        }
    }
}
