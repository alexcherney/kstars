// Copyright (C) 2016 Artem Fedoskin <afedoskin3@gmail.com>
/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.2
import "../modules"
import "../constants" 1.0

KSPage {
    id: indiPage
    objectName: "indiControlPanel"
    title: "INDI Control Panel"

    property bool connected: ClientManagerLite.connected
    property alias webMStatusText: webMStatusLabel.text
    property alias webMStatusTextVisible: webMStatusLabel.visible
    property alias webMActiveProfileText: webMActiveProfileLabel.text
    property alias webMActiveProfileLayoutVisible: webMActiveProfileLayout.visible
    property alias webMBrowserButtonVisible: webMBrowserButton.visible
    property alias webMProfileListVisible: webMProfileList.visible

    Component.onCompleted: {
        // Debug purposes
        ClientManagerLite.setHost("localhost", 7624)
    }

    onConnectedChanged: {
        if (!indiPage.connected) {
            for (var i = 0; i < devicesModel.count; ++i) {
                devicesModel.get(i).panel.destroy()
                stackView.pop(indiPage)
            }
            devicesModel.clear()
            notification.showNotification("Disconnected from the server")
        }
    }

    ColumnLayout {
        anchors.fill: parent
        id: cPanelColumn
        spacing: 5 * num.dp

        ColumnLayout {
            visible: !indiPage.connected
            anchors {
                left: parent.left
                right: parent.right
            }

            KSLabel {
                text: xi18n("IP Address")
            }

            RowLayout {
                anchors {
                    left: parent.left
                    right: parent.right
                }

                KSTextField {
                    id: ipHost
                    placeholderText: xi18n("xxx.xxx.xxx.xxx")
                    Layout.alignment: Qt.AlignHCenter
                    Layout.maximumWidth: parent.width*0.8
                    Layout.fillWidth: true
                    text: ClientManagerLite.lastUsedServer
                }
            }

            KSLabel {
                text: xi18n("Web Manager Port")
            }

            RowLayout {
                anchors {
                    left: parent.left
                    right: parent.right
                }

                KSTextField {
                    id: portWebManager
                    placeholderText: xi18n("xxxx")
                    Layout.alignment: Qt.AlignHCenter
                    Layout.maximumWidth: parent.width*0.2
                    Layout.fillWidth: true
                    text: ClientManagerLite.lastUsedWebManagerPort
                }

                Button {
                    id: webMConnectButton
                    text: xi18n("Get Status")

                    onClicked: {
                        ClientManagerLite.getWebManagerProfiles(ipHost.text, parseInt(portWebManager.text));
                        Qt.inputMethod.hide()
                    }
                }
            }

            KSLabel {
                id: webMStatusLabel
                text: xi18n("Web Manager Status:")
                visible: false
            }

            RowLayout {
                id: webMActiveProfileLayout
                visible: false

                KSLabel {
                    id: webMActiveProfileLabel
                    text: xi18n("Active Profile:")
                }

                Button {
                    id: webMStopButton
                    text: xi18n("Stop")

                    onClicked: {
                        ClientManagerLite.stopWebManagerProfile();
                    }
                }
            }

            ListView {
                id: webMProfileList
                model: webMProfileModel
                highlightFollowsCurrentItem: false
                width: parent.width
                height: childrenRect.height
                visible: false

                delegate: RowLayout {
                    spacing: 60
                    height: webMConnectButton.height

                    KSLabel {
                        height: webMConnectButton.height
                        text: xi18n("Profile:")+" "+modelData
                    }

                    Button {
                        height: webMConnectButton.height
                        text: xi18n("Start")

                        onClicked: {
                            ClientManagerLite.startWebManagerProfile(modelData);
                        }
                    }
                }
            } // ListView

            Button {
                id: webMBrowserButton
                text: xi18n("Manage Profiles")
                visible: false

                onClicked: {
                    Qt.openUrlExternally("http://"+ipHost.text+":"+portWebManager.text)
                }
            }

            KSLabel {
                text: xi18n("Server Port")
            }

            RowLayout {
                anchors {
                    left: parent.left
                    right: parent.right
                }

                KSTextField {
                    id: portHost
                    placeholderText: xi18n("INDI Server Port")
                    Layout.alignment: Qt.AlignHCenter
                    Layout.maximumWidth: parent.width*0.2
                    Layout.fillWidth: true
                    text: ClientManagerLite.lastUsedPort
                }

                Button {
                    text: indiPage.connected ? xi18n("Disconnect") : xi18n("Connect")

                    onClicked: {
                        if (!indiPage.connected) {
                            if(ClientManagerLite.setHost(ipHost.text, parseInt(portHost.text))) {
                                notification.showNotification(xi18n("Successfully connected to the server"))
                            } else {
                                notification.showNotification(xi18n("Couldn't connect to the server"))
                            }
                        } else {

                            ClientManagerLite.disconnectHost()
                        }
                        Qt.inputMethod.hide()
                    }
                }

            }
        }

        KSLabel {
            id: connectedTo
            visible: indiPage.connected
            text: xi18n("Connected to ") + ClientManagerLite.connectedHost
        }


        ColumnLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true
            visible: indiPage.connected

            Rectangle {
                Layout.fillWidth: true
                height: 1 * num.dp
                color: "gray"
            }

            KSLabel {
                id: devicesLabel
                text: xi18n("Available Devices")
            }

            ListModel {
                id: devicesModel
            }

            Connections {
                target: ClientManagerLite
                onNewINDIDevice: {
                    var component = Qt.createComponent(Qt.resolvedUrl("./DevicePanel.qml"));
                    var devicePanel = component.createObject(window);
                    devicePanel.deviceName = deviceName
                    devicesModel.append({ name: deviceName, panel: devicePanel })
                }
                onRemoveINDIDevice: {
                    for (i = 0; i < devicesModel.count; ++i) {
                        if(devicesModel.get(i).name == deviceName) {
                            devicesModel.panel.destroy()
                            devicesModel.remove(i)
                        }
                    }
                }
                onNewINDIMessage: {
                    notification.showNotification(message)
                }
            }
        }

        KSListView {
            id: devicesPage
            Layout.fillHeight: true
            Layout.fillWidth: true

            model: devicesModel
            textRole: "name"

            onClicked: {
                stackView.push(devicesModel.get(currentIndex).panel)
            }
        }
    }
}
