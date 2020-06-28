unix:!android {

    isEmpty(target.path) {
        data.files = countries.csv
        qnx {
            target.path = /tmp/$${TARGET}/bin
        } else {
            target.path = /home/pi/qt-apps/$${TARGET}/bin
            data.path = /home/pi/qt-apps/$${TARGET}/bin
        }
        export(target.path)
    }
    INSTALLS += target
    INSTALLS += data
}

export(INSTALLS)

