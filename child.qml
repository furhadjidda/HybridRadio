import QtQuick 2.3
import QtQuick.Window 2.2
import QtQuick.Controls 1.0

Window {
    id: window1
        width: 800
        height: 460
        visible: true
        ListModel {
            id: libraryModel
            objectName: "listModelObject"
        }

        TableView {
            x: 8
            y: 51
            width: 784
            height: 389
            visible: true
            TableViewColumn {
                role: "programme"
                title: "Programme Description"
                width: 200
            }
            TableViewColumn {
                role: "pName"
                title: "Programme Name"
                width: 200
            }
            TableViewColumn {
                role: "when"
                title: "Date And Time"
                width: 200
            }
            TableViewColumn {
                role: "duration"
                title: "Duration"
                width: 100
            }
            model: libraryModel
        }

        Label {
            id: label1
            x: 199
            y: 12
            width: 377
            height: 33
            color: "#1360da"
            text: qsTr("Electronic Programme Guide")
            style: Text.Sunken
            font.bold: true
            font.underline: false
            font.pointSize: 15
            horizontalAlignment: Text.AlignHCenter
        }

        function clearListElement()
        {
            libraryModel.clear()
            return 0;
        }

        function addListElement(programName,description,when,duration)
        {
            libraryModel.append({"programme":description,"pName":programName,"when":when,"duration":duration})
            return 0;
        }
}

