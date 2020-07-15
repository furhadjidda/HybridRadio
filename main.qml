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
        gradient: Gradient {
            GradientStop {
                position: 0
                color: "#0e6082"
            }

            GradientStop {
                position: 1
                color: "#000000"
            }
        }
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
            fillMode: Image.Stretch
            z: 1
            antialiasing: true
            clip: true
            source: "default-art.jpg"
        }

        Text {
            id: text1
            objectName: "StationNameObj"
            x: 13
            y: 65
            width: 437
            height: 85
            color: "#dbdde8"
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
            font.pixelSize: 17
            wheelEnabled: true
            model: ListModel {
                  id: cbItems
                  ListElement { text: ""}
                  ListElement { text: "FM(USA)-73978"}
                  ListElement { text: "FM(DEUTSCHLAND)" } // new
                  ListElement { text: "DAB(DEUTSCHLAND)" } // new
                  ListElement { text: "FM(RADIO1 UK)" } // new
                  ListElement { text: "FM(BBC-2 UK)" } // new
                  ListElement { text: "FM(BBC-4 UK)" } // new
                  ListElement { text: "FM(UK)"}
                  ListElement { text: "DAB(NORWAY)"}
                  ListElement { text: "DAB(NORWAY)2"}
                  ListElement { text: "DAB(UK)"}
                  ListElement { text: "DAB(AUS)"}
                  ListElement { text: "FM(SPAIN)"}
                  ListElement { text: "FM(NETHERLANDS)"}
                  ListElement { text: "FM(IRELAND)"}
                  ListElement { text: "FM(AUSTRIA)"}
              }
            signal sendSelectionChanged(string data)
            x: 456
            y: 27
            width: 208
            height: 31
            font.wordSpacing: 0.1
            spacing: 1
            onCurrentIndexChanged:
            {
                sendSelectionChanged(cbItems.get(currentIndex).text)
            }
        }

        Text {
            id: bitrate
            objectName: "BitRateObject"
            x: 605
            y: 361
            width: 91
            height: 14
            color: "#d5d7e6"
            text: "bitrate"
            font.bold: true
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 15
        }

        Text {
            id: description
            objectName: "DescriptionObject"
            x: 13
            y: 223
            width: 437
            height: 31
            color: "#edeef0"
            text: "Description"
            lineHeight: 1
            wrapMode: Text.WordWrap
            styleColor: "#dd0b0b"
            font.family: "Tahoma"
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 20
        }

        Text {
            id: songName
            objectName: "SongObject"
            x: 13
            y: 356
            width: 437
            height: 41
            color: "#dce0e8"
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
            x: 13
            y: 564
            width: 191
            height: 19
            color: "#e3e6e9"
            text: qsTr("Media status")
            visible: false
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            font.pixelSize: 15
            onTextChanged: {
                if( text === "Playing" )
                {
                    animatedImage.visible = true
                }
                else
                {
                    animatedImage.visible = false
                }
            }
        }

        Popup {
            id: popup

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
                    source: "Qml-Popup-SoundUnavailable.png"
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

        Popup {
            id: popupProgramView

            x: Math.round((parent.width - width) / 2)
            y: Math.round((parent.height - height) / 2)
            width: 800
            height: 175

            Rectangle{
                width: 750
                height: 150
                anchors.fill: parent
                ListView {
                    id: programView2
                    x: 0
                    y: 0
                    width: 700
                    height: 200
                    layer.enabled: false
                    highlightRangeMode: ListView.ApplyRange
                    snapMode: ListView.SnapToItem
                    cacheBuffer: 20
                    keyNavigationWraps: true
                    boundsBehavior: Flickable.StopAtBounds
                    orientation: ListView.Horizontal
                    flickableDirection: Flickable.HorizontalFlick
                    pixelAligned: false
                    model: programInfoModel
                    delegate: programmeInformationDelegate
                }
            }

            Image{
                width: 25
                height: 25
                x: 750
                y: 5
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        popupProgramView.close()
                    }
                }
                source: "Qml-CloseImage.png"
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
                        //color: index % 2 == 0 ? "#0e6082" : "#0e6082"
                        color: "#0e6082"
                        border.color: "black"
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
                            color: "snow"
                            font.pixelSize: 14
                            width: 220
                            wrapMode: Text.WordWrap
                            textFormat: Text.RichText
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
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
                name: "Empty"
                genre: "Empty"
                image: "default-art.jpg"
            }

        }

        ListView {
            id: listView
            objectName: "ServiceListTableObject"
            signal selectIndex( int index)
            x: 710
            y: 58
            width: 282
            height: 480
            keyNavigationWraps: true
            pixelAligned: false
            highlight: Rectangle { color: "lightsteelblue"}
            focus: true
            delegate: programmeDelegate
            model: programListModel
        }

        // This is for EPG
        Component {
            id: programmeInformationDelegate
            Item {
                width: 250; height: 150
                Column {
                    Rectangle {
                        width: 250; height: 150
                        color: index % 2 == 0 ? "Dark Gray" : "Light Gray"
                        Text {
                            id: programText
                            text: name
                            font.family: "Tahoma"
                            x: 5
                            y: 4
                            color: "Black"
                            font.pixelSize: 13
                            width: 200
                            wrapMode: Text.WordWrap
                            textFormat: Text.RichText
                        }
                        Image {
                            id: image2
                            x: 5
                            y: 21
                            width: 90
                            height: 90
                            opacity: 1
                            source: image
                        }
                        Text {
                            id: duration
                            text: time
                            font.family: "Tahoma"
                            x: 110
                            y: 60
                            color: "Black"
                            font.pixelSize: 13
                            width: 200
                            wrapMode: Text.WordWrap
                            textFormat: Text.RichText
                        }
                    }
                }
            }
        }

        ListModel {
            id: programInfoModel
            objectName: "programInfoModel"
            ListElement {
                name: "Station"
                time: "Station"
                image: ""
            }

        }

        Image {
            id: epgImage
            objectName: "epgImageVisibility"
            x: 456
            y: 361
            width: 49
            height: 43
            visible: false
            fillMode: Image.PreserveAspectFit
            source: "Qml-Epg-Image.jpg"
            MouseArea{
                anchors.fill: parent
                onClicked: popupProgramView.open()
            }
        }

        Text {
            id: element
            x: 616
            y: 387
            width: 88
            height: 17
            color: "#cfd2e3"
            text: qsTr("Count: " + programListModel.count )
            font.bold: true
            font.pixelSize: 14
        }

        Image {
            id: httpProtocolImage
            x: 526
            y: 361
            width: 73
            height: 27
            visible: false
            source: "Qml-HttpLogo.png"
            fillMode: Image.Stretch
        }

        Image {
            id: stompProtocolImage
            x: 526
            y: 394
            width: 73
            height: 26
            visible: false
            source: "Qml-StompImage.png"
            fillMode: Image.PreserveAspectFit
        }


        Popup {
            id: additionalInfoPopup

            x: Math.round((parent.width - width) / 2)
            y: Math.round((parent.height - height) / 2)
            width: 700
            height: 175

            Rectangle{
                width: 700
                height: 175
                anchors.fill: parent
                Text {
                    id: moreInfoId
                    anchors.fill: parent
                    wrapMode: Text.WordWrap
                    text: qsTr("text")
                }

                // Image for closing
                Image{
                    width: 25
                    height: 25
                    x: 660
                    y: 1
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            additionalInfoPopup.close()
                        }
                    }
                    source: "Qml-CloseImage.png"
                }
            }

            closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent | Popup.CloseOnReleaseOutside | Popup.CloseOnReleaseOutsideParent
        }

        Image {
            id: additionalInfoImage
            x: 461
            y: 71
            width: 30
            height: 32
            source: "Qml-MoreInfo.png"
            fillMode: Image.PreserveAspectFit
            MouseArea{
                anchors.fill: parent
                onClicked: additionalInfoPopup.open()
            }
        }

        Image {
            id: playIconsImage
            x: 22
            y: 462
            width: 366
            height: 103
            source: "playIcons_Mod_negate.png"
            fillMode: Image.PreserveAspectFit
            MouseArea {
                id: stopMouseArea
                objectName: "Stop"
                x: 290
                y: 4
                width: 84
                height: 95
                signal sendStop()
                onClicked: sendStop()
            }

            MouseArea {
                id: playMouseArea
                objectName: "Play"
                x: 81
                y: 3
                width: 84
                height: 97
                z: 12
                signal sendPlay()
                onClicked: sendPlay()
            }



            MouseArea {
                id: mouseAreaNext
                objectName: "Next"
                x: 181
                y: 1
                width: 87
                height: 94
                signal sendNext()
                onClicked: sendNext()
            }



            MouseArea {
                id: mouseAreaPrev
                objectName: "Previous"
                x: 0
                y: 4
                width: 82
                height: 96
                signal sendPrevious()
                onClicked: sendPrevious()
            }

        }

        AnimatedImage {
            id: animatedImage
            x: 517
            y: 65
            width: 119
            height: 35
            visible: false
            fillMode: Image.PreserveAspectCrop
            source: "qml-animated-playing.gif"
        }

        function displayPopUp()
        {
            popup.open()
        }
        function hidePopUp()
        {
            popup.close()
        }

        function displayEpgImage()
        {
            epgImage.visible = true
        }

        function displayHttpProtocolStatus( status )
        {
            httpProtocolImage.visible = status
        }

        function displayStompProtocolStatus( status )
        {
            stompProtocolImage.visible = status
        }

        function hideEpgImage()
        {
            epgImage.visible = false
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

        function clearProgramElementsElement()
        {
            programInfoModel.clear()
            return 0;
        }

        function addProgramElement(programName,image,timeAndDate)
        {
            programInfoModel.append({"name":programName,"image":image,"time":timeAndDate})
            return 0;
        }

        function addBearerInformation( bearerInfoArg )
        {
            moreInfoId.text = bearerInfoArg;
        }
    }
}

/*##^##
Designer {
    D{i:7;anchors_height:31;anchors_width:208;anchors_x:456;anchors_y:27}
}
##^##*/
