import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import MaterialIcons 1.0

Dialog {
    id: dialog

    property alias hostname: hostnameField.text
    property alias port: portField.value
    property alias username: usernameField.text
    property alias password: passwordField.text

    standardButtons: Dialog.Ok

    header: TitleBar {
        title: qsTr("CouchDB")
        leftPadding: dialog.padding
        backButton.visible: false
        toolButton.text: MaterialIcons.close
        toolButton.onClicked: dialog.reject()
    }

    RowLayout {
        spacing: 48
        anchors.fill: parent

        Image {
            source: "qrc:/images/couch.png"
        }

        GridLayout {
            columns: 2
            columnSpacing: 24

            Label { text: qsTr("Host") }
            TextField {
                id: hostnameField
                focus: true
                placeholderText: qsTr("localhost")
                Layout.fillWidth: true
                onAccepted: dialog.accept()
            }

            Label { text: qsTr("Port") }
            SpinBox {
                id: portField
                value: 5984
                to: 65535
                editable: true
                leftPadding: 0
                rightPadding: 0
                down.indicator.anchors.right: portField.right
                up.indicator.anchors.right: down.indicator.left
                Layout.fillWidth: true
                Component.onCompleted: contentItem.horizontalAlignment = Qt.AlignLeft
                Keys.onEnterPressed: dialog.accept()
                Keys.onReturnPressed: dialog.accept()
            }

            Label { text: qsTr("Username") }
            TextField {
                id: usernameField
                Layout.fillWidth: true
                onAccepted: dialog.accept()
            }

            Label { text: qsTr("Password") }
            TextField {
                id: passwordField
                echoMode: TextField.Password
                Layout.fillWidth: true
                onAccepted: dialog.accept()
            }
        }
    }
}
