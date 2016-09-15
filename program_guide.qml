import QtQuick 2.0

Item {
    Rectangle {
        id: rectangle1
        x: 131
        y: 81
        width: 427
        height: 347
        color: "#ffffff"
        Component {
               id: contactDelegate
               Item {
                   width: 180; height: 40
                   Column {
                       Text { text: '<b>Name:</b> ' + name }
                       Text { text: '<b>Number:</b> ' + number }
                   }
               }
        }
    }

}
