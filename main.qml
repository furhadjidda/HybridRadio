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
        objectName: "RectBox"
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

        signal sendSelectionChanged(string data)

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
            x: 254
            y: 18
            width: 151
            height: 31
            visible: false
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
            x: 456
            y: 383
            width: 91
            height: 14
            color: "#d5d7e6"
            text: "Bitrate"
            font.bold: true
            verticalAlignment: Text.AlignTop
            horizontalAlignment: Text.AlignLeft
            font.pixelSize: 15
        }

        Text {
            id: description
            objectName: "DescriptionObject"
            x: 14
            y: 214
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
            x: 14
            y: 340
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
            x: 456
            y: 434
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


            Image{
                width: 25
                height: 25
                x: 300
                y: 5
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        popup.close()
                    }
                }
                source: "Qml-CloseImage.png"
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
                        //color: index % 2 == 0 ? "Dark Gray" : "Light Gray"
                        color: "#0e6082"
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
            x: 643
            y: 61
            width: 45
            height: 45
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
            x: 456
            y: 356
            width: 88
            height: 17
            color: "#cfd2e3"
            text: qsTr("Count: " + programListModel.count )
            font.bold: true
            font.pixelSize: 15
        }

        Image {
            id: httpProtocolImage
            x: 626
            y: 356
            width: 70
            height: 25
            visible: false
            source: "Qml-HttpLogo.png"
            fillMode: Image.Stretch
        }

        Image {
            id: stompProtocolImage
            x: 626
            y: 395
            width: 70
            height: 25
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
            x: 49
            y: 403
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
                width: 88
                height: 92
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
                width: 81
                height: 91
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

        Text {
            id: clockText
            x: 13
            y: 8
            width: 199
            height: 17
            color: "#f7f7f7"
            text: qsTr("")
            font.wordSpacing: 0.6
            font.bold: true
            font.letterSpacing: 0.6
            style: Text.Raised
            font.weight: Font.DemiBold
            font.capitalization: Font.AllUppercase
            fontSizeMode: Text.HorizontalFit
            renderType: Text.NativeRendering
            textFormat: Text.RichText
            font.pixelSize: 14
        }

        Timer {
            interval: 500; running: true; repeat: true
            onTriggered: {
                var date = new Date;
                clockText.text = date.toString()
            }
        }

        Image {
            id: presetid1
            objectName: "presetid1"
            x: 23
            y: 501
            width: 90
            height: 90
            source: "Qml-PresetAdd.png"
            fillMode: Image.PreserveAspectFit
            MouseArea{
                id: presetid1_MouseArea
                objectName: "presetid1_MouseArea"
                anchors.fill: parent
                signal sendPresetSave(string index)
                signal sendPresetRecall(string index)
                onPressAndHold: {
                    sendPresetSave("1")
                }
                onClicked: {
                    sendPresetRecall("1")
                }
            }

        }

        Image {
            id: presetid2
            objectName: "presetid2"
            x: 135
            y: 501
            width: 90
            height: 90
            source: "Qml-PresetAdd.png"
            fillMode: Image.PreserveAspectFit
            MouseArea{
                id: presetid2_MouseArea
                objectName: "presetid2_MouseArea"
                anchors.fill: parent
                signal sendPresetSave(string index)
                signal sendPresetRecall(string index)
                width: 90
                onPressAndHold: {
                    sendPresetSave("2")
                }
                onClicked: {
                    sendPresetRecall("2")
                }
            }
        }

        Image {
            id: presetid3
            objectName: "presetid3"
            x: 244
            y: 501
            width: 90
            height: 90
            source: "Qml-PresetAdd.png"
            fillMode: Image.PreserveAspectFit
            MouseArea{
                id: presetid3_MouseArea
                objectName: "presetid3_MouseArea"
                anchors.fill: parent
                signal sendPresetSave(string index)
                signal sendPresetRecall(string index)
                onPressAndHold: {
                    sendPresetSave("3")
                }
                onClicked: {
                    sendPresetRecall("3")
                }
            }
        }


        Image {
            id: presetid4
            objectName: "presetid4"
            x: 360
            y: 501
            width: 90
            height: 90
            source: "Qml-PresetAdd.png"
            fillMode: Image.PreserveAspectFit
            MouseArea{
                id: presetid4_MouseArea
                objectName: "presetid4_MouseArea"
                anchors.fill: parent
                signal sendPresetSave(string index)
                signal sendPresetRecall(string index)
                onPressAndHold: {
                    sendPresetSave("4")
                }
                onClicked: {
                    sendPresetRecall("4")
                }
            }
        }


        Image {
            id: presetid5
            objectName: "presetid5"
            x: 470
            y: 501
            width: 90
            height: 90
            source: "Qml-PresetAdd.png"
            fillMode: Image.PreserveAspectFit
            MouseArea{
                id: presetid5_MouseArea
                objectName: "presetid5_MouseArea"
                anchors.fill: parent
                signal sendPresetSave(string index)
                signal sendPresetRecall(string index)
                onPressAndHold: {
                    sendPresetSave("5")
                }
                onClicked: {
                    sendPresetRecall("5")
                }
            }
        }


        Image {
            id: presetid6
            objectName: "presetid6"
            x: 580
            y: 501
            width: 90
            height: 90
            source: "Qml-PresetAdd.png"
            fillMode: Image.PreserveAspectFit
            MouseArea{
                id: presetid6_MouseArea
                objectName: "presetid6_MouseArea"
                anchors.fill: parent
                signal sendPresetSave(string index)
                signal sendPresetRecall(string index)
                onPressAndHold: {
                    sendPresetSave("6")
                }
                onClicked: {
                    sendPresetRecall("6")
                }
            }
        }

        Image {
            id: countrySelection
            x: 502
            y: 12
            width: 149
            height: 43
            fillMode: Image.Stretch
            source: "Qml-WorldMapIcon.png"
            MouseArea{
                anchors.fill: parent
                onClicked: {
                    popupSelection.open()
                }
            }
        }

        Popup {
            id: popupSelection
            objectName: "popUpSelection"
            x: Math.round((parent.width - width) / 2)
            y: Math.round((parent.height - height) / 2)
            width: 800
            height: 175
            Rectangle{
                width: 750
                height: 150
                anchors.fill: parent
                ListView {
                    id: selectionListView
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
                    model: selectionInfoModel
                    delegate: selectionComponentDelegate
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
                        popupSelection.close()
                    }
                }
                source: "Qml-CloseImage.png"
            }
        }

        ListModel {
            id: selectionInfoModel
            objectName: "selectionInfoModel"
            ListElement {
                name: "Australia"
                image: "countries_flags/Australia.jpg"
                selection: "DAB(AUS)"
            }
            ListElement {
                name: "USA"
                image: "countries_flags/Usa.png"
                selection: "FM(USA)-73978"
            }
            ListElement {
                name: "United Kingdom - Cluster 1"
                image: "countries_flags/Uk.png"
                selection: "FM(UK)"
            }
            ListElement {
                name: "United Kingdom - Cluster 2"
                image: "countries_flags/Uk.png"
                selection: "FM(RADIO1 UK)"
            }
            ListElement {
                name: "United Kingdom - Cluster 3"
                image: "countries_flags/Uk.png"
                selection: "FM(BBC-2 UK)"
            }
            ListElement {
                name: "United Kingdom - Cluster 4"
                image: "countries_flags/Uk.png"
                selection: "FM(BBC-4 UK)"
            }
            ListElement {
                name: "Norway - Cluster 1"
                image: "countries_flags/Norway.png"
                selection: "DAB(NORWAY)"
            }
            ListElement {
                name: "Norway - Cluster 2"
                image: "countries_flags/Norway.png"
                selection: "DAB(NORWAY)2"
            }
            ListElement {
                name: "Netherlands"
                image: "countries_flags/Uk.png"
                selection: "FM(NETHERLANDS)"
            }
            ListElement {
                name: "Ireland"
                image: "countries_flags/Ireland.jpg"
                selection: "FM(IRELAND)"
            }
            ListElement {
                name: "Austria"
                image: "countries_flags/Austria.png"
                selection: "FM(AUSTRIA)"
            }
            ListElement {
                name: "Germany - Cluster 1(DAB)"
                image: "countries_flags/Germany.png"
                selection: "DAB(DEUTSCHLAND)"
            }
            ListElement {
                name: "Germany - Cluster 2(FM)"
                image: "countries_flags/Germany.png"
                selection: "FM(DEUTSCHLAND)"
            }
            ListElement {
                name: "Spain"
                image: "countries_flags/Spain.png"
                selection: "FM(SPAIN)"
            }
        }
        // This is for EPG
        Component {
            id: selectionComponentDelegate
            Item {
                width: 205; height: 150
                Column {
                    Rectangle {
                        border.color: "black"
                        width: 205; height: 150
                        //color: index % 2 == 0 ? "Dark Gray" : "Light Gray"
                        color: "#0e6082"
                        Text {
                            id: countryName
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
                        BorderImage {
                            id: flagImage
                            x: 5
                            y: 30
                            width: 175
                            height: 110
                            opacity: 1
                            source: image

                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                rectangle1.sendSelectionChanged( selection )
                            }
                        }
                    }
                }
            }
        }

        Text {
            id: stateImageId
            objectName: "stateImageObject"
            x: 456
            y: 413
            width: 214
            height: 21
            color: "#ffffff"
            text: qsTr("")
            font.pixelSize: 15
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
