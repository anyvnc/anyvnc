import QtQuick 2.6
import QtQuick.Window 2.1
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.0
import AnyVnc.Core 1.0
import AnyVnc.QtQuick 1.0

Window {
	id: root
	visible: true
	width: 640
	height: 1024

	Material.theme: Material.Light
	Material.accent: Material.Indigo
	Material.primary: Material.Indigo

	title: qsTr("AnyVNC")

	Component {
		id: vncClient
		ColumnLayout {
			Button {
				onClicked: stackView.pop()
				text: "Close"
			}

			VncViewItem {
				id: vncView
				host: clientHost.text
				password: clientPassword.text
				Layout.fillWidth: true
				Layout.fillHeight: true
			}
		}
	}

	VncServer {
		id: vncServer
		port: parseInt(serverPort.text)
		onPortChanged: console.log(port)
		password: serverPassword.text
	}

	StackView {
		id: stackView
		anchors.fill: parent

		initialItem: Page {
			id: mainPage

			Flickable {
				anchors.fill: parent
				contentWidth: width
				contentHeight: mainFlickableContent.implicitHeight
				ScrollIndicator.vertical: ScrollIndicator { }
				boundsBehavior: Flickable.StopAtBounds
				flickableDirection: Flickable.VerticalFlick

				Control {
					id: mainFlickableContent
					width: mainPage.availableWidth
					implicitHeight: mainGroupBoxesLayout.implicitHeight
					padding: 20
					GridLayout {
						id: mainGroupBoxesLayout
						anchors.horizontalCenter: parent.horizontalCenter
						width: parent.availableWidth
						columns: width > 400 ? 2 : 1
						GroupBox {
							Layout.margins: 20
							Layout.fillWidth: true
							title: "Access remote host"
							ColumnLayout {
								anchors.fill: parent
								GridLayout {
									Layout.fillWidth: true
									columns: 2
									Label { text: "Host:" }
									TextField { id: clientHost; Layout.fillWidth: true }
									Label { text: "Password:" }
									TextField { id: clientPassword; Layout.fillWidth: true }
								}
								Button {
									Layout.margins: 10
									text: "Connect"
									onClicked: stackView.push(vncClient)
								}
							}
						}

						GroupBox {
							Layout.margins: 20
							Layout.fillWidth: true
							title: "Allow remote control"
							ColumnLayout {
								anchors.fill: parent
								GridLayout {
									Layout.fillWidth: true
									columns: 2
									Label { text: "Password:" }
									TextField { id: serverPassword; Layout.fillWidth: true }
									Label { text: "Port:" }
									TextField { id: serverPort; text: "5900"; Layout.fillWidth: true }
								}
								Button {
									Layout.margins: 10
									text: vncServer.running ? "Stop" : "Start"
									onClicked: vncServer.running = !vncServer.running;
								}
							}
						}
					}
				}
			}
		}
	}
}
