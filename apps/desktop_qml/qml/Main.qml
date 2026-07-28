import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import CppProject.Desktop

Window {
    id: root

    required property string startupMessage
    required property bool smokeTest
    readonly property bool isMacOs: Qt.platform.os === "osx"

    width: 960
    height: 640
    minimumWidth: 640
    minimumHeight: 420
    visible: false
    color: "#f5f6f8"
    title: qsTr("CppProject QML")

    WindowAgent {
        id: windowAgent
    }

    Component.onCompleted: {
        if (smokeTest)
            return

        windowAgent.setup(root)
        windowAgent.setTitleBar(titleBar)
        if (!root.isMacOs) {
            windowAgent.setSystemButton(WindowAgent.Minimize, minimizeButton)
            windowAgent.setSystemButton(WindowAgent.Maximize, maximizeButton)
            windowAgent.setSystemButton(WindowAgent.Close, closeButton)
        }
        root.visible = true
    }

    Rectangle {
        id: titleBar

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 42
        color: root.active ? "#eef0f4" : "#f5f6f8"

        Label {
            anchors.left: parent.left
            anchors.leftMargin: root.isMacOs ? 78 : 16
            anchors.verticalCenter: parent.verticalCenter
            text: root.title
            color: "#202124"
            font.bold: true
        }

        Row {
            visible: !root.isMacOs
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: parent.right

            ToolButton {
                id: minimizeButton
                width: 48
                height: parent.height
                text: "—"
                onClicked: root.showMinimized()
            }

            ToolButton {
                id: maximizeButton
                width: 48
                height: parent.height
                text: root.visibility === Window.Maximized ? "❐" : "□"
                onClicked: {
                    if (root.visibility === Window.Maximized)
                        root.showNormal()
                    else
                        root.showMaximized()
                }
            }

            ToolButton {
                id: closeButton
                width: 48
                height: parent.height
                text: "×"
                onClicked: root.close()

                background: Rectangle {
                    color: closeButton.hovered ? "#c42b1c" : "transparent"
                }

                contentItem: Text {
                    text: closeButton.text
                    color: closeButton.hovered ? "white" : "#202124"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.topMargin: titleBar.height
        anchors.margins: 32
        spacing: 16

        Item {
            Layout.fillHeight: true
        }

        Label {
            Layout.alignment: Qt.AlignHCenter
            text: qsTr("QML frontend is ready")
            color: "#202124"
            font.pixelSize: 30
            font.bold: true
        }

        Label {
            Layout.alignment: Qt.AlignHCenter
            text: root.startupMessage
            color: "#5f6368"
            font.pixelSize: 15
        }

        Label {
            Layout.alignment: Qt.AlignHCenter
            text: qsTr("Qt Quick · QWindowKit · shared app_core")
            color: "#7a7f87"
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
