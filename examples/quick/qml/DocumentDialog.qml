import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import MaterialIcons 1.0
import CouchDB 1.0

Dialog {
    id: dialog

    property alias id: idField.text
    property alias revision: revisionField.text
    property alias content: contentArea.text
    property var callback

    title: qsTr("Add Document")
    standardButtons: Dialog.Ok

    onAccepted: callback(Couch.document(id, revision, content))

    header: TitleBar {
        title: dialog.title
        leftPadding: dialog.padding
        backButton.visible: false
        toolButton.text: MaterialIcons.close
        toolButton.onClicked: dialog.reject()
    }

    footer.enabled: !!content

    GridLayout {
        columns: 2
        columnSpacing: 24
        anchors.fill: parent

        Label { text: qsTr("ID") }
        TextField {
            id: idField
            Layout.fillWidth: true
            onAccepted: dialog.accept()
        }

        Label { text: qsTr("Revision") }
        TextField {
            id: revisionField
            Layout.fillWidth: true
            onAccepted: dialog.accept()
        }

        Label { text: qsTr("Content"); Layout.alignment: Qt.AlignTop }
        ScrollView {
            focus: true
            TextArea {
                id: contentArea
                focus: true
            }
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
