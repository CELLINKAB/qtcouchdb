import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import MaterialIcons 1.0

Dialog {
    id: dialog

    property alias label: label.text
    property alias text: field.text
    property var callback

    standardButtons: Dialog.Ok

    onAccepted: callback(text)

    header: TitleBar {
        title: dialog.title
        leftPadding: dialog.padding
        backButton.visible: false
        toolButton.text: MaterialIcons.close
        toolButton.onClicked: dialog.reject()
    }

    footer.enabled: !!text

    RowLayout {
        spacing: 24
        anchors.fill: parent

        Label {
            id: label
        }

        TextField {
            id: field
            focus: true
            Layout.fillWidth: true
            onAccepted: dialog.accept()
        }
    }
}
