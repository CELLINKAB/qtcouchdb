import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import MaterialIcons 1.0

ToolBar {
    id: titleBar

    property alias title: label.text
    property alias backButton: backButton
    property alias toolButton: toolButton

    leftPadding: backButton.visible ? 0 : 16

    Material.elevation: 0
    Material.foreground: "#ffffff"

    RowLayout {
        anchors.fill: parent

        ToolButton {
            id: backButton
            text: MaterialIcons.arrow_back
            font.family: MaterialIcons.fontFamily
        }

        Label {
            id: label
            elide: Label.ElideRight
            Layout.fillWidth: true
        }

        ToolButton {
            id: toolButton
            font.family: MaterialIcons.fontFamily
        }
    }
}
