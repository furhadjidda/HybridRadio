import QtQuick 2.3
import QtQuick.Window 2.2
import QtQuick.Layouts 1.3
import QtQuick.Extras 1.4
import Qt.labs.calendar 1.0
import QtQuick.Controls 2.0
import Qt.labs.qmlmodels 1.0


Window {
    id: root
    width: 1024
    height: 600
    visible: true
    visibility: Window.FullScreen

    Rectangle {
        id: rectangle1
        x: 0
        y: 0
        width: 1024
        height: 600
        color: "#ffffff"
        rotation: 0
        z: 12
        clip: true
        border.color: "#090909"
        objectName: "RectBox"

        Image {
            id: image1
            objectName: "artWork"
            x: 456
            y: 109
            width: 240
            height: 240
            z: 1

            antialiasing: true
            clip: true
            source: "images.jpeg"
        }

        Text {
            id: text1
            objectName: "StationNameObj"
            x: 41
            y: 65
            width: 376
            height: 85
            color: "#061a7e"
            text: "StationName"
            font.family: "Tahoma"
            wrapMode: Text.WordWrap
            style: Text.Raised
            font.bold: true
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            textFormat: Text.PlainText
            font.pixelSize: 30
        }

        ComboBox {
            id: comboBox1
            currentIndex: 0
            objectName: "selection"
            x: 531
            y: 27
            model: ListModel {
                  id: cbItems
                  ListElement { text: ""}
                  ListElement { text: "UK"}
                  ListElement { text: "UK-DAB"}
                  ListElement { text: "DE" }
              }
            signal sendSelectionChanged(string data)
             onCurrentIndexChanged:
             {
                 sendSelectionChanged(cbItems.get(currentIndex).text)
             }
        }

        Image {
            id: image2
            x: 36
            y: 477
            width: 404
            height: 73
            clip: false
            opacity: 1
            source: "PlayStrip.PNG"

            MouseArea {
                id: stopMouseArea
                objectName: "Stop"
                x: 254
                y: 0
                width: 63
                height: 73
                signal sendStop()
                onClicked: sendStop()
            }

            MouseArea {
                id: playMouseArea
                objectName: "Play"
                x: 0
                y: 0
                width: 70
                height: 65
                z: 12
                signal sendPlay()
                onClicked: sendPlay()
            }



            MouseArea {
                id: mouseAreaNext
                objectName: "Next"
                x: 323
                y: 2
                width: 73
                height: 63
                signal sendNext()
                onClicked: sendNext()
            }



            MouseArea {
                id: mouseAreaPrev
                objectName: "Previous"
                x: 173
                y: 2
                width: 69
                height: 71
                signal sendPrevious()
                onClicked: sendPrevious()
            }

        }

        Text {
            id: genre
            objectName: "GenreObject"
            x: 36
            y: 348
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
            x: 581
            y: 361
            width: 115
            height: 14
            text: "bitrate"
            font.bold: true
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 15
        }

        Text {
            id: description
            objectName: "DescriptionObject"
            x: 41
            y: 240
            width: 376
            height: 31
            color: "#6581f5"
            text: "Description"
            wrapMode: Text.WordWrap
            styleColor: "#dd0b0b"
            font.family: "Tahoma"
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 20
        }

        Text {
            id: songName
            objectName: "SongObject"
            x: 41
            y: 387
            width: 376
            height: 41
            color: "#054bba"
            text: qsTr("SongName")
            font.family: "Tahoma"
            wrapMode: Text.WordWrap
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 17
        }

        ListModel {
            id: libraryModel
            objectName: "listModelObject"
        }

        Text {
            id: mediastatus
            objectName: "MediaStatus"
            x: 510
            y: 84
            width: 161
            height: 19
            text: qsTr("Media status")
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            font.pixelSize: 15
        }

        Text {
            id: additionalInfo
            objectName: "additionalInfo"
            x: 465
            y: 394
            width: 231
            height: 61
            text: qsTr("Additional Info")
            wrapMode: Text.WordWrap
            font.pixelSize: 15
        }

        Popup {
            id: popup

            parent: Overlay.overlay

            x: Math.round((parent.width - 300 - width) / 2)
            y: Math.round((parent.height - height) / 2)
            width: 350
            height: 150

            Rectangle{
                width: 350
                height: 150
                anchors.fill: parent
                color: "light blue"
                Image {
                    id: soundWave
                    anchors.fill: parent
                    source: "b122_3-512.png"
                }
                Text {
                    id: popupText
                    anchors.fill: parent
                    color: "black"
                    wrapMode: Text.WordWrap
                    font.pixelSize: 20
                    horizontalAlignment: Text.AlignHCenter
                    text: qsTr("No Audio Stream Available")
                }
            }

            closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent | Popup.CloseOnReleaseOutside | Popup.CloseOnReleaseOutsideParent
        }

        Component {
            id: programmeDelegate
            Item {
                width: 282; height: 60
                Row {
                    Rectangle {
                        width: 282; height: 60
                        color: index % 2 == 0 ? "Light Blue" : "Light Pink"
                        Image {
                            id: image2
                            x: 5
                            y: 5
                            width: 50
                            height: 50
                            opacity: 1
                            source: image
                        }
                        Text {
                            id: itemText
                            text: name
                            font.family: "Tahoma"
                            x: 60
                            y: 5
                            color: "Black"
                            font.pixelSize: 14
                            width: 220
                            wrapMode: Text.WordWrap
                            textFormat: Text.RichText
                            MouseArea {
                                width: 282; height: 60
                                onClicked: {
                                    listView.currentIndex = index
                                    listView.selectIndex(index)
                                }
                            }
                        }
                    }
                }
            }
        }

        ListModel {
            id: programListModel
            objectName: "programListModel"
            ListElement {
                name: "Station"
                genre: "Station"
                image: ""
            }

        }

        ListView {
            id: listView
            objectName: "ServiceListTableObject"
            signal selectIndex( int index)
            x: 720
            y: 84
            width: 282
            height: 432
            keyNavigationWraps: true
            pixelAligned: false
            highlight: Rectangle { color: "lightsteelblue"}
            focus: true
            delegate: programmeDelegate
            model: programListModel
        }

        function displayPopUp()
        {
            popup.open()
        }
        function hidePopUp()
        {
            popup.close()
        }

        function clearListElement()
        {
            programListModel.clear()
            return 0;
        }

        function addListElement(serviceName,genre,imagePath)
        {
            programListModel.append({"name":serviceName,"genre":genre,"image":imagePath})
            return 0;
        }
    }
}
