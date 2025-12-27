import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    property bool hasSpec: specContent.text.length > 0

    function importFromUrl(url) {
        statusLabel.text = "Importing from: " + url
        statusLabel.color = "#0066cc"
        // In real implementation, this would call C++ backend
        // For now, show placeholder
        specContent.text = "// Spec content will be loaded here\n// URL: " + url
        hasSpec = true
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        Label {
            text: "Import TLA+ Specification"
            font.bold: true
            font.pointSize: 16
        }

        GroupBox {
            title: "GitHub Import"
            Layout.fillWidth: true

            ColumnLayout {
                anchors.fill: parent
                spacing: 10

                Label {
                    text: "Paste a GitHub URL to import a TLA+ specification:"
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 10

                    TextField {
                        id: urlInput
                        Layout.fillWidth: true
                        placeholderText: "https://github.com/owner/repo/blob/main/spec.tla"
                    }

                    Button {
                        text: "Import"
                        enabled: urlInput.text.length > 0
                        onClicked: {
                            root.importFromUrl(urlInput.text)
                            urlInput.text = ""
                        }
                    }
                }

                Label {
                    id: statusLabel
                    text: "Ready to import"
                    color: "#666666"
                }
            }
        }

        GroupBox {
            title: "Specification Preview"
            Layout.fillWidth: true
            Layout.fillHeight: true

            ScrollView {
                anchors.fill: parent
                clip: true

                TextArea {
                    id: specContent
                    readOnly: true
                    font.family: "Monospace"
                    placeholderText: "No specification loaded.\n\nUse the Import button above to load a spec from GitHub."
                    wrapMode: TextEdit.NoWrap
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            Label {
                text: "Configuration File:"
            }

            TextField {
                id: configInput
                Layout.fillWidth: true
                placeholderText: "Optional: path to .cfg file"
                enabled: hasSpec
            }

            Button {
                text: "Run Model Checker"
                enabled: hasSpec
                highlighted: true
                onClicked: {
                    statusLabel.text = "Running TLC model checker..."
                    statusLabel.color = "#0066cc"
                    // In real implementation, trigger TLC run
                }
            }
        }
    }
}
