import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import MaterialIcons 1.0

Page {
    id: page

    title: qsTr("Document")

    property var document

    signal back()

    header: TitleBar {
        title: page.title
        toolButton.visible: false
        backButton.onClicked: page.back()
    }

    ScrollView {
        anchors.fill: parent

        TextArea {
            text: document.content
            readOnly: true
            selectByMouse: true
            font.family: "Monospace"
            padding: 16
            background: null
        }
    }
}
