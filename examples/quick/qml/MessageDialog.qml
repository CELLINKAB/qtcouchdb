import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import MaterialIcons 1.0

Dialog {
    id: dialog

    property alias icon: iconLabel.text
    property alias text: textLabel.text

    standardButtons: Dialog.Close

    header: TitleBar {
        title: dialog.title
        leftPadding: dialog.padding
        backButton.visible: false
        toolButton.text: MaterialIcons.close
        toolButton.onClicked: dialog.reject()
    }

    contentItem: RowLayout {
        focus: true
        spacing: 24
        anchors.fill: parent

        Label {
            id: iconLabel
            font.pixelSize: 48
            font.family: MaterialIcons.fontFamily
        }

        Label {
            id: textLabel
            wrapMode: Label.Wrap
            Layout.fillWidth: true
        }

        Keys.onShortcutOverride: {
            if (event.key === Qt.Key_Enter || event.key === Qt.Key_Return || event.key === Qt.Key_Space)
                dialog.accept()
        }
    }
}
