TEMPLATE = app
QT += couchdb quickcontrols2
CONFIG += c++17

SOURCES += \
    quick.cpp

RESOURCES += \
    quick.qml \
    fonts/MaterialIcons-Regular.ttf \
    images/couch.png \
    qml/DatabaseListPage.qml \
    qml/DocumentDialog.qml \
    qml/DocumentListPage.qml \
    qml/DocumentPage.qml \
    qml/InputDialog.qml \
    qml/LoginDialog.qml \
    qml/MessageDialog.qml \
    qml/TitleBar.qml

target.path = $$[QT_INSTALL_EXAMPLES]/quick
INSTALLS += target
