import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Ripose.Memento

Page {
    id: root

    property int preferredWidth: 600
    property int groupSpacing: 10

    Dialog {
        id: messageDialog
        anchors.centerIn: parent
        modal: true
        standardButtons: Dialog.Ok

        Label {
            id: messageDialogLabel
        }
    }

    footer: ColumnLayout {
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            visible: Features.isUnix && !scrollView.atBottom
            color: MementoPalette.border
            height: 1
        }

        DialogButtonBox {
            Layout.fillWidth: true
            standardButtons: DialogButtonBox.Apply | DialogButtonBox.Reset

            onApplied: IssenConfig.write()
            onClicked: function(button) {
                if (button === standardButton(DialogButtonBox.Reset))
                {
                    IssenConfig.load();
                }
            }
        }
    }

    ScrollView {
        id: scrollView
        anchors.fill: parent
        contentWidth: scrollView.contentWidth
        leftPadding: root.groupSpacing
        rightPadding: root.groupSpacing
        clip: true

        readonly property bool atBottom:
            (ScrollBar.vertical.position + ScrollBar.vertical.size) >= 0.99

        ColumnLayout {
            width: parent.width
            spacing: root.groupSpacing

            SettingsBox {
                Layout.preferredWidth: root.preferredWidth
                Layout.topMargin: root.groupSpacing
                Layout.alignment: Qt.AlignHCenter
                title: qsTr("General")

                ColumnLayout {
                    anchors.fill: parent
                    spacing: root.groupSpacing

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Enable ISSEN integration")
                        }
                        Switch {
                            Layout.alignment: Qt.AlignRight
                            checked: IssenConfig.enabled
                            onClicked: IssenConfig.enabled = checked
                        }
                    }

                    Label {
                        Layout.fillWidth: true
                        wrapMode: Text.Wrap
                        text: qsTr(
                            "When enabled, the add button in the dictionary " +
                            "popup saves words to ISSEN instead of Anki."
                        )
                    }
                }
            }

            SettingsBox {
                Layout.preferredWidth: root.preferredWidth
                Layout.alignment: Qt.AlignHCenter
                title: qsTr("Account")

                ColumnLayout {
                    anchors.fill: parent
                    spacing: root.groupSpacing

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Server")
                        }
                        TextField {
                            Layout.alignment: Qt.AlignRight
                            Layout.preferredWidth: 250
                            placeholderText: qsTr("Server")
                            validator: RegularExpressionValidator {
                                regularExpression: /^https?:\/\/.+/
                            }
                            text: IssenConfig.server
                            onEditingFinished: IssenConfig.server = text
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Username")
                        }
                        TextField {
                            Layout.alignment: Qt.AlignRight
                            Layout.preferredWidth: 250
                            placeholderText: qsTr("Username")
                            text: IssenConfig.username
                            onEditingFinished: IssenConfig.username = text
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Password")
                        }
                        TextField {
                            Layout.alignment: Qt.AlignRight
                            Layout.preferredWidth: 250
                            placeholderText: qsTr("Password")
                            echoMode: TextInput.Password
                            text: IssenConfig.password
                            onEditingFinished: IssenConfig.password = text
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Language")
                        }
                        TextField {
                            Layout.alignment: Qt.AlignRight
                            Layout.preferredWidth: 250
                            placeholderText: qsTr("japanese")
                            text: IssenConfig.language
                            onEditingFinished: IssenConfig.language = text
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                    }

                    Button {
                        id: loginButton
                        Layout.alignment: Qt.AlignRight
                        text: qsTr("Test Login")
                        onClicked: {
                            loginButton.enabled = false;
                            IssenClient.login().then(function(result) {
                                loginButton.enabled = true;
                                if (result.success)
                                {
                                    messageDialog.title = qsTr("ISSEN Login");
                                    messageDialogLabel.text =
                                        qsTr("Login successful.");
                                }
                                else
                                {
                                    messageDialog.title =
                                        qsTr("ISSEN Login Error");
                                    messageDialogLabel.text = result.error;
                                }
                                messageDialog.open();
                            });
                        }
                    }
                }
            }
        }
    }
}
