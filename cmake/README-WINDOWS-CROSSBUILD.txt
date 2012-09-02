Support for cross-compiling for windows is based on the Tomahawk music player
(http://www.tomahawk-player.org/,https://github.com/tomahawk-player/tomahawk/) build system.

A big thanks go to Tomahawk's developers!

-------------------------------------------------------------------------------

= Setting up the build environment
(based on http://wiki.tomahawk-player.org/mediawiki/index.php/Building_Windows_Binary_on_openSUSE_11.4)

Rough steps, currently. Some of these might not be needed. Tested on openSUSE 11.4 *only*.

# Add repositories
sudo zypper addrepo http://download.opensuse.org/repositories/windows:/mingw/openSUSE_11.4/windows:mingw.repo
sudo zypper addrepo http://download.opensuse.org/repositories/windows:/mingw:/win32/openSUSE_11.4/windows:mingw:win32.repo
sudo zypper addrepo http://download.tomahawk-player.org/packman/mingw:32/openSUSE_11.4/mingw:32.repo

# Upgrade openSUSE's Qt to 4.8
sudo zypper ar http://download.opensuse.org/repositories/KDE:/Qt48/openSUSE_11.4/KDE:Qt48.repo
sudo zypper refresh && sudo zypper dup -r 1

# Install needed stuff
sudo zypper install ccache git wine gcc libqt4-devel mingw32-cross-nsis-plugin-uac \
	mingw32-cross-nsis-plugin-processes libpoppler-qt4-devel mingw32-libpoppler-qt4-devel \
	mingw32-libpoppler mingw32-libpoppler-qt4
sudo zypper source-install --build-deps-only mingw32-tomahawk-unstable
sudo zypper source-install --build-deps-only mingw32-tomahawk-player-installer

= Building

git clone https://code.google.com/p/open-pdf-presenter/ && cd open-pdf-presenter
# Assuming here a build of the master branch, otherwise change to another
mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain/Toolchain-mingw32-openSUSE.cmake ..
make

= Packaging
make package
