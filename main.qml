import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Window {
    width: 800
    height: 460
    visible: true

    MouseArea {
        anchors.leftMargin: 278
        anchors.topMargin: 0
        anchors.rightMargin: 116
        anchors.bottomMargin: 400
        anchors.fill: parent
    }

    Rectangle {
        id: rectangle1
        width: 800
        height: 460
        color: "#ffffff"
        rotation: 0
        z: 12
        clip: true
        border.color: "#090909"

        Image {
            id: image1
            objectName: "artWork"
            x: 530
            y: 65
            width: 239
            height: 244
            z: 1
            antialiasing: true
            clip: true
            source: "http://www.broadcastradio.com/media/52043/logosquare-400.png"
        }

        Text {
            id: text1
            objectName: "StationNameObj"
            x: 102
            y: 65
            width: 376
            height: 85
            color: "#061a7e"
            text: "StationName"
            style: Text.Raised
            font.bold: true
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            textFormat: Text.PlainText
            font.pixelSize: 50
        }

        ComboBox {
            id: comboBox1
            objectName: "selection"
            x: 599
            y: 27
            width: 145
            height: 20
            clip: false
            opacity: 0.8
            activeFocusOnPress: true
            model: [ "FM Demo - 1", "FM Demo - 2 ", "DAB Demo - 1", "DAB Demo - 2" ]
            signal sendSelectionChanged(string data)
            onCurrentIndexChanged:
            {
                sendSelectionChanged(comboBox1.currentText)
            }
        }

        Image {
            id: image2
            x: 182
            y: 348
            width: 296
            height: 58
            source: "PlayStrip.PNG"

            MouseArea {
                id: stopMouseArea
                objectName: "Stop"
                x: 179
                y: 0
                width: 56
                height: 58
                signal sendStop()
                onClicked: sendStop()
            }

            MouseArea {
                id: playMouseArea
                objectName: "Play"
                x: 0
                y: 0
                width: 56
                height: 58
                z: 12
                signal sendPlay()
                onClicked: sendPlay()
            }
        }

        Text {
            id: genre
            objectName: "GenreObject"
            x: 102
            y: 298
            width: 376
            height: 22
            text: ""
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 12
        }

        Text {
            id: bitrate
            objectName: "BitRateObject"
            x: 515
            y: 321
            width: 115
            height: 14
            text: ""
            font.bold: true
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 12
        }

        Text {
            id: description
            objectName: "DescriptionObject"
            x: 102
            y: 156
            width: 376
            height: 31
            color: "#6581f5"
            text: ""
            styleColor: "#dd0b0b"
            font.family: "Tahoma"
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 30
        }

        Text {
            id: songName
            x: 102
            y: 200
            width: 376
            height: 41
            text: qsTr("")
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 12
        }
    }
}
