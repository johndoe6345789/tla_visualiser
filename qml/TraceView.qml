import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    property var model

    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal

        // Trace steps list
        GroupBox {
            SplitView.minimumWidth: 300
            SplitView.preferredWidth: 400
            title: "Trace Steps (" + (model ? model.stepCount : 0) + ")"

            ColumnLayout {
                anchors.fill: parent
                spacing: 10

                ListView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    model: root.model

                    delegate: ItemDelegate {
                        width: ListView.view.width
                        highlighted: ListView.isCurrentItem

                        contentItem: ColumnLayout {
                            spacing: 5

                            Label {
                                text: "Step " + stepNumber
                                font.bold: true
                            }

                            Label {
                                text: "Action: " + action
                                color: "#0066cc"
                            }

                            Label {
                                text: "State ID: " + stateId
                                font.pointSize: 9
                                color: "#666666"
                            }
                        }

                        onClicked: {
                            ListView.view.currentIndex = index
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true

                    Button {
                        text: "◀ Previous"
                        enabled: model && model.currentStep > 0
                        onClicked: {
                            if (model) {
                                model.currentStep = model.currentStep - 1
                            }
                        }
                    }

                    Label {
                        Layout.fillWidth: true
                        text: model ? ("Step " + model.currentStep + " of " + model.stepCount) : ""
                        horizontalAlignment: Text.AlignHCenter
                    }

                    Button {
                        text: "Next ▶"
                        enabled: model && model.currentStep < model.stepCount - 1
                        onClicked: {
                            if (model) {
                                model.currentStep = model.currentStep + 1
                            }
                        }
                    }
                }
            }
        }

        // Step details
        GroupBox {
            SplitView.fillWidth: true
            title: "Step Details"

            ColumnLayout {
                anchors.fill: parent
                spacing: 10

                Label {
                    text: "State Variables"
                    font.bold: true
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true

                    TextArea {
                        id: variablesText
                        readOnly: true
                        font.family: "Monospace"
                        placeholderText: "Select a step to view variable values"
                        wrapMode: TextEdit.NoWrap
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: "#cccccc"
                }

                Label {
                    text: "Export Options"
                    font.bold: true
                }

                RowLayout {
                    Layout.fillWidth: true

                    Button {
                        text: "Export as JSON"
                        Layout.fillWidth: true
                        enabled: model && model.stepCount > 0
                        onClicked: {
                            if (model) {
                                var json = model.exportToJson()
                                // Save to file
                                console.log("JSON:", json)
                            }
                        }
                    }

                    Button {
                        text: "Export as Markdown"
                        Layout.fillWidth: true
                        enabled: model && model.stepCount > 0
                        onClicked: {
                            if (model) {
                                var md = model.exportToMarkdown()
                                // Save to file
                                console.log("Markdown:", md)
                            }
                        }
                    }
                }
            }
        }
    }

    Connections {
        target: model
        function onCurrentStepChanged() {
            if (model) {
                var details = model.getStepDetails(model.currentStep)
                var text = ""
                if (details.variables) {
                    for (var i = 0; i < details.variables.length; i++) {
                        var v = details.variables[i]
                        text += v.name + " = " + v.value + "\n"
                    }
                }
                variablesText.text = text
            }
        }
    }
}
