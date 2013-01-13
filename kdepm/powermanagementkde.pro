TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../src/
HEADERS = powermanagementkde.h
SOURCES = powermanagementkde.cpp
TARGET = $$qtLibraryTarget(powermanagementkde)

# SOLID (KDE Power Management)
# Inspired by http://www.qtcentre.org/threads/36521-configuring-optional-dependencies-in-qmake-or-cmake
# Check for KDE Solid headers and for libsolid
KDEPATH_OPTIONS = /usr/include/KDE/ /usr/include/kde4/KDE
for(dir, KDEPATH_OPTIONS):exists($${dir}):KDEPATH=$${dir}

lessThan(QT_MAJOR_VERSION, 5) \ # KDE is not expected to have a stable release with Qt5 anytime soon
exists($$KDEPATH/Solid/PowerManagement) \
system("grep beginSuppressingScreenPowerManagement $$KDEPATH/../solid/powermanagement.h >> /dev/null") \
system("ld -lsolid -o /dev/null 2> /dev/null") {
#  message("KDE Power Management FOUND")
  DEFINES += ENABLE_SOLID
  INCLUDEPATH += $$KDEPATH
  LIBS += -lsolid
} else {
  error("KDE Power Management support cannot be built (KDE not found/too old)")
}

# Suport for make install
unix {
  isEmpty(PREFIX) {
    PREFIX=/usr/local/
  }
  message("Install prefix is $$PREFIX")

  PLUGIN_PATH=$$PREFIX/lib/open-pdf-presenter/

  target.path += $$PLUGIN_PATH
  INSTALLS += target
}
