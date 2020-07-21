unix:!android {

    isEmpty(target.path) {
        data.files = countries.csv
        data.files += pi-code-location-data-usa.csv
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

DISTFILES += \
    $$PWD/pi-code-location-data-usa.csv

