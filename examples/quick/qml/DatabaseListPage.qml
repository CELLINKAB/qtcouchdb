import QtQuick 2.12
import QtQuick.Controls 2.12
import MaterialIcons 1.0

Page {
    id: page

    title: qsTr("Databases")

    property alias model: listView.model

    signal add()
    signal back()
    signal remove(string db)
    signal selected(string db)

    header: TitleBar {
        title: page.title
        backButton.visible: false
        toolButton.text: MaterialIcons.add
        toolButton.onClicked: page.add()
    }

    ScrollView {
        anchors.fill: parent
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

        ListView {
            id: listView
            header: Label {
                text: qsTr("Name")
                font.bold: true
                padding: 16
            }
            delegate: ItemDelegate {
                id: delegate
                text: modelData
                rightPadding: 48
                width: parent.width
                onClicked: page.selected(modelData)

                ToolButton {
                    text: MaterialIcons.close
                    font.family: MaterialIcons.fontFamily
                    visible: hovered || delegate.hovered
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked: page.remove(modelData)
                }
            }
        }
    }
}
