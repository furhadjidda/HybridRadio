
import os
import shutil
import sys
import git
from git import Repo

# git clone git://code.qt.io/qt/<qt-module>.git -b <qt-version>

QtMain = "git://code.qt.io/qt/"
QtModules = [
    "qtbase",
    "qtxmlpatterns",
    "qtsvg",
    "qtdeclarative",
    "qtimageformats",
    "qtgraphicaleffects",
    "qtquickcontrols",
    "qtquickcontrols2",
    "qtvirtualkeyboard",
    "qtwebsockets",
    "qtwebglplugin",
    "qtcharts",
    "qtconnectivity",
    "qtmultimedia",
    "qtlocation"
]

DIR_NAME = "~/raspi/"

if os.path.isdir(DIR_NAME):
    shutil.rmtree(DIR_NAME)

os.mkdir(DIR_NAME)


for module in QtModules:
    qtRepo = QtMain + module
    repo = git.Repo.init(module)
    origin = repo.create_remote('origin',qtRepo)
    print("Fetching " + qtRepo)
    origin.fetch()
    origin.pull(origin.refs[0].remote_head)

print ("---- DONE ----")