import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12
import Qt.labs.settings 1.0
import MaterialIcons 1.0
import CouchDB 1.0
import "qml"

ApplicationWindow {
    id: window
    width: 960
    height: 540
    visible: true

    Material.accent: "#c83f32"
    Material.primary: "#c83f32"
    Material.foreground: "#ffffff"
    Material.theme: Material.Dark

    Settings {
        id: settings
        property alias hostname: loginDialog.hostname
        property alias port: loginDialog.port
        property alias username: loginDialog.username
        property alias password: loginDialog.password
    }

    CouchClient {
        id: client
        url: "http://%1:%2@%3:%4".arg(settings.username)
                                 .arg(settings.password)
                                 .arg(settings.hostname || "localhost")
                                 .arg(settings.port)
        onDatabaseCreated: listDatabases()
        onDatabaseDeleted: listDatabases()
        onDatabasesListed: databaseListPage.model = databases
        onErrorOccurred: messageDialog.createObject(window, {title: error.error, icon: MaterialIcons.error, text: error.reason}).open()
    }

    CouchDatabase {
        id: database
        client: client
        onDocumentCreated: listFullDocuments()
        onDocumentDeleted: listFullDocuments()
        onDocumentsListed: {
            if (stackView.currentItem instanceof DocumentListPage)
                stackView.currentItem.model = documents
            else
                stackView.push(documentListPage, {model: documents})
        }
    }

    StackView {
        id: stackView
        anchors.fill: parent

        initialItem: DatabaseListPage {
            id: databaseListPage
            onAdd: inputDialog.createObject(window, {title: qsTr("Add Database"), label: qsTr("Name"), callback: client.createDatabase}).open()
            onRemove: client.deleteDatabase(db)
            onSelected: {
                database.name = db
                database.queryDocuments(Couch.query(0, 0, Qt.AscendingOrder, true))
            }
        }

        pushEnter: Transition {
            NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; duration: 225; easing.type: Easing.OutCubic }
            NumberAnimation { property: "y"; from: stackView.height / 2; to: 0; duration: 225; easing.type: Easing.OutCubic }
        }
        pushExit: Transition {
            NumberAnimation { property: "opacity"; from: 1.0; to: 0.0; duration: 125; easing.type: Easing.InCubic }
            NumberAnimation { property: "y"; from: 0; to: stackView.height / 2; duration: 125; easing.type: Easing.InCubic }
        }
        popEnter: Transition {
            NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; duration: 225; easing.type: Easing.OutCubic }
            NumberAnimation { property: "y"; from: stackView.height / 2; to: 0; duration: 225; easing.type: Easing.OutCubic }
        }
        popExit: Transition {
            NumberAnimation { property: "opacity"; from: 1.0; to: 0.0; duration: 125; easing.type: Easing.InCubic }
            NumberAnimation { property: "y"; from: 0; to: stackView.height / 2; duration: 125; easing.type: Easing.InCubic }
        }
    }

    BusyIndicator {
        running: client.busy
        anchors.centerIn: stackView
    }

    Component {
        id: documentListPage
        DocumentListPage {
            onAdd: documentDialog.createObject(window, {callback: database.createDocument}).open()
            onBack: stackView.pop()
            onRemove: database.deleteDocument(doc)
            onSelected: stackView.push(documentPage, {document: doc})
        }
    }

    Component {
        id: documentPage
        DocumentPage {
            onBack: stackView.pop()
        }
    }

    LoginDialog {
        id: loginDialog
        modal: true
        focus: true
        visible: true
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        parent: window.overlay
        onAccepted: client.listDatabases()
    }

    Component {
        id: messageDialog
        MessageDialog {
            modal: true
            focus: true
            margins: 96
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            parent: window.overlay
            onClosed: destroy()
        }
    }

    Component {
        id: inputDialog
        InputDialog {
            modal: true
            focus: true
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            width: parent.width / 2
            height: parent.height / 2
            parent: window.overlay
            onClosed: destroy()
        }
    }

    Component {
        id: documentDialog
        DocumentDialog {
            modal: true
            focus: true
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            width: parent.width - 96
            height: parent.height - 96
            parent: window.overlay
            onClosed: destroy()
        }
    }
}
