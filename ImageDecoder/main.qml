import QtQuick 2.12
import QtQuick.Controls 2.4
import QtQuick.Window 2.11
import QtQuick.Dialogs 1.3

Window {
    visible: true
    width: 1024
    height: 480
    title: qsTr("Image Decoder")
    color: '#222222'

    TableView {
        id: tableView

        columnWidthProvider: function (column) { return 256; }
        rowHeightProvider: function (column) { return 60; }
        anchors.fill: parent
        topMargin: columnsHeader.implicitHeight
        model: table_model
        ScrollBar.horizontal: ScrollBar{}
        ScrollBar.vertical: ScrollBar{}
        clip: true
        delegate: Rectangle {
            Text {
                id: text
                text: display
                anchors.fill: parent
                anchors.margins: 10
                color: 'black'
                font.pixelSize: 15
                verticalAlignment: Text.AlignVCenter
            }
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onEntered: text.color = 'blue'
                onExited: text.color = 'black'
                onClicked: table_model.onClick(row)
                onPressed: text.color = 'red'
                onReleased: text.color = 'blue'
            }
        }
        Rectangle {
            z: 3
            color: "#222222"
            y: tableView.contentY
            x: tableView.contentX
            width: tableView.leftMargin
            height: tableView.topMargin
        }

        Row {
            id: columnsHeader
            y: tableView.contentY
            z: 2
            Repeater {
                model: tableView.columns > 0 ? tableView.columns : 1
                Label {
                    width: tableView.columnWidthProvider(modelData)
                    height: 35
                    text: table_model.headerData(modelData, Qt.Horizontal)
                    color: '#4488ff'
                    font.pixelSize: 15
                    padding: 10
                    verticalAlignment: Text.AlignVCenter

                    background: Rectangle { color: "#224488" }

                }
            }
        }

        ScrollIndicator.horizontal: ScrollIndicator { }
        ScrollIndicator.vertical: ScrollIndicator { }
    }

    MessageDialog {
        id: messageDialog
        title: "Error"
        text: "File is corrupted or not supported."
        onAccepted: {
            console.log("File was not processed")
        }
    }

    Connections {
        target: table_model
        onShowMessageBox: messageDialog.visible = true;
    }
}