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
            ListElement {
                programme: "A Masterpiece"
                station: "Gabriel"
            }
        }

        TableView {
            x: 77
            y: 45
            width: 600
            height: 291
            visible: true
            TableViewColumn {
                role: "programme"
                title: "Programme Name"
                width: 200
            }
            TableViewColumn {
                role: "station"
                title: "Station Name"
                width: 200
            }
            TableViewColumn {
                role: "when"
                title: "Date And Time"
                width: 200
            }
            model: libraryModel
        }

}

