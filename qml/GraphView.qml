import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    property var model

    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal

        // Graph visualization area
        Rectangle {
            SplitView.fillWidth: true
            SplitView.minimumWidth: 400
            color: "#f5f5f5"
            border.color: "#cccccc"
            border.width: 1

            Item {
                anchors.fill: parent
                anchors.margins: 10

                Label {
                    anchors.centerIn: parent
                    text: "State Graph Visualization\n\n" +
                          "Nodes: " + (model ? model.nodeCount : 0) + "\n" +
                          "Edges: " + (model ? model.edgeCount : 0) + "\n\n" +
                          "Interactive graph view will be rendered here"
                    horizontalAlignment: Text.AlignHCenter
                    color: "#666666"
                }

                // Simplified graph representation (for demo)
                Canvas {
                    id: canvas
                    anchors.fill: parent
                    visible: model && model.nodeCount > 0

                    onPaint: {
                        if (!model || model.nodeCount === 0) return

                        var ctx = getContext("2d")
                        ctx.clearRect(0, 0, width, height)

                        // Draw simple circular layout
                        var centerX = width / 2
                        var centerY = height / 2
                        var radius = Math.min(width, height) / 3
                        var nodeCount = model.nodeCount

                        // Draw edges (simplified)
                        ctx.strokeStyle = "#999999"
                        ctx.lineWidth = 2
                        // Edge drawing would use model.getTransitions()

                        // Draw nodes (limit to 100 for performance)
                        var maxNodesToShow = Math.min(nodeCount, 100)
                        for (var i = 0; i < maxNodesToShow; i++) {
                            var angle = (i / nodeCount) * 2 * Math.PI
                            var x = centerX + radius * Math.cos(angle)
                            var y = centerY + radius * Math.sin(angle)

                            ctx.fillStyle = "#4CAF50"
                            ctx.beginPath()
                            ctx.arc(x, y, 20, 0, 2 * Math.PI)
                            ctx.fill()

                            ctx.fillStyle = "#ffffff"
                            ctx.font = "12px sans-serif"
                            ctx.textAlign = "center"
                            ctx.textBaseline = "middle"
                            ctx.fillText("S" + i, x, y)
                        }
                        
                        // Show indicator if there are more nodes
                        if (nodeCount > maxNodesToShow) {
                            ctx.fillStyle = "#666666"
                            ctx.font = "14px sans-serif"
                            ctx.textAlign = "center"
                            ctx.fillText("... and " + (nodeCount - maxNodesToShow) + " more states", centerX, height - 20)
                        }
                    }
                }
            }
        }

        // Details panel
        GroupBox {
            SplitView.minimumWidth: 300
            SplitView.preferredWidth: 350
            title: "Graph Details"

            ColumnLayout {
                anchors.fill: parent
                spacing: 10

                Label {
                    text: "Statistics"
                    font.bold: true
                }

                GridLayout {
                    columns: 2
                    rowSpacing: 5
                    columnSpacing: 10

                    Label { text: "States:" }
                    Label { text: model ? model.nodeCount : "0" }

                    Label { text: "Transitions:" }
                    Label { text: model ? model.edgeCount : "0" }
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: "#cccccc"
                }

                Label {
                    text: "Selected Node"
                    font.bold: true
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true

                    TextArea {
                        readOnly: true
                        placeholderText: "Click on a node to see details"
                        wrapMode: TextEdit.WordWrap
                    }
                }

                RowLayout {
                    Layout.fillWidth: true

                    Button {
                        text: "Export as SVG"
                        Layout.fillWidth: true
                        onClicked: {
                            // Export graph
                        }
                    }

                    Button {
                        text: "Export as PNG"
                        Layout.fillWidth: true
                        onClicked: {
                            // Export graph
                        }
                    }
                }
            }
        }
    }
}
