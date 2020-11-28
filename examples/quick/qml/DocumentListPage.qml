import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import MaterialIcons 1.0

Page {
    id: page

    title: qsTr("Documents")

    property alias model: listView.model

    signal add()
    signal back()
    signal remove(var doc)
    signal selected(var doc)

    header: TitleBar {
        title: page.title
        backButton.onClicked: page.back()
        toolButton.text: MaterialIcons.add
        toolButton.onClicked: page.add()
    }

    ScrollView {
        anchors.fill: parent
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

        ListView {
            id: listView
            header: RowLayout {
                Label {
                    text: qsTr("ID")
                    font.bold: true
                    padding: 16
                    Layout.preferredWidth: page.width / 3 * 2
                }
                Label {
                    text: qsTr("Revision")
                    padding: 16
                    font.bold: true
                    Layout.preferredWidth: page.width / 3
                }
            }
            delegate: ItemDelegate {
                id: delegate
                contentItem: RowLayout {
                    Label {
                        text: modelData.id
                        Layout.preferredWidth: page.width / 3 * 2
                    }
                    Label {
                        text: modelData.revision
                        Layout.preferredWidth: page.width / 3
                    }
                }
                width: parent ? parent.width : 0
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
