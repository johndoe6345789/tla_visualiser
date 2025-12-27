import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        Label {
            text: "Invariants & Properties"
            font.bold: true
            font.pointSize: 16
        }

        GroupBox {
            title: "Invariant Status"
            Layout.fillWidth: true
            Layout.fillHeight: true

            ScrollView {
                anchors.fill: parent
                clip: true

                ColumnLayout {
                    width: parent.width - 20
                    spacing: 10

                    Label {
                        text: "No invariants to display"
                        color: "#666666"
                        Layout.fillWidth: true
                    }

                    // Example invariant items (would be populated from model)
                    Repeater {
                        model: 0 // Would be connected to invariants model

                        delegate: Rectangle {
                            Layout.fillWidth: true
                            height: 80
                            border.color: "#cccccc"
                            border.width: 1
                            radius: 5

                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 10
                                spacing: 10

                                Rectangle {
                                    width: 50
                                    height: 50
                                    radius: 25
                                    color: true ? "#4CAF50" : "#f44336"

                                    Label {
                                        anchors.centerIn: parent
                                        text: true ? "✓" : "✗"
                                        color: "white"
                                        font.pointSize: 20
                                    }
                                }

                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 5

                                    Label {
                                        text: "Invariant Name"
                                        font.bold: true
                                    }

                                    Label {
                                        text: "Status: Passed"
                                        color: "#666666"
                                    }
                                }

                                Button {
                                    text: "Details"
                                    onClicked: {
                                        // Show invariant details
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        GroupBox {
            title: "Summary"
            Layout.fillWidth: true

            GridLayout {
                anchors.fill: parent
                columns: 2
                rowSpacing: 5
                columnSpacing: 10

                Label { text: "Total Invariants:" }
                Label { text: "0" }

                Label { text: "Passed:" }
                Label { text: "0"; color: "#4CAF50" }

                Label { text: "Failed:" }
                Label { text: "0"; color: "#f44336" }
            }
        }
    }
}
