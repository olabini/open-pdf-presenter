project (open-pdf-presenter)

cmake_minimum_required(VERSION 2.6)

# Project Definitions
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 3)
set(PROJECT_VERSION_PATCH 00)
set(PROJECT_WEBSITE "https://code.google.com/p/open-pdf-presenter/")

set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

add_definitions(-DOPP_VERSION=\"${PROJECT_VERSION}\")

set(PLUGIN_PATH "${CMAKE_INSTALL_PREFIX}/lib/open-pdf-presenter/")
add_definitions(-DOPP_PLUGIN_PATH=\"${PLUGIN_PATH}\")

# Macros
# Find header files with Q_OBJECT macros
macro(FIND_HEADERS_TO_MOC return_list header_list)
	foreach(file_path ${${header_list}})
		file(STRINGS ${file_path} file_result REGEX "Q_OBJECT")
		if(file_result MATCHES "Q_OBJECT")
			# return_list contains the name of the list where the result will be
			# written to, so we need to "get" its value (the list name) for the set,
			# and we need to "get" its value twice to access the values of the list
			# contained therein
			set(${return_list} ${${return_list}} ${file_path})
			#message(${file_path} " added to moc list")
		endif()
	endforeach()
endmacro()

# Libraries
find_package(Qt4 COMPONENTS QtGui QtXml QtXmlPatterns REQUIRED)
include(UseQt4)
include_directories(${QT_INCLUDES})

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules/)
find_package(PopplerQt4 REQUIRED)
include_directories(${POPPLER_QT4_INCLUDE_DIR})

# Input files
file(GLOB_RECURSE OPP_HEADERS	 src/*.h)
file(GLOB_RECURSE OPP_SOURCES 	 src/*.cpp)
file(GLOB_RECURSE OPP_FORMS 	 ui/*.ui)
file(GLOB_RECURSE OPP_RESOURCES resources/*.qrc)

include_directories(src/views/ src/events/ include/)

# Build
#set (CMAKE_CXX_FLAGS "-g -Wall -Wextra")
set (CMAKE_CXX_FLAGS "-g")

include_directories(${CMAKE_CURRENT_BINARY_DIR})

find_headers_to_moc(MOC_HEADERS OPP_HEADERS)  # identify headers that need moc'ing
qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})      # run moc
qt4_wrap_ui(FORMS ${OPP_FORMS})               # process forms
qt4_add_resources(RESOURCES ${OPP_RESOURCES}) # process resources

add_executable(open-pdf-presenter ${MOC_SOURCES} ${OPP_SOURCES} ${RESOURCES} ${FORMS})
target_link_libraries(open-pdf-presenter ${QT_LIBRARIES} ${POPPLER_QT4_LIBRARIES})

# Install
install(TARGETS open-pdf-presenter DESTINATION bin)

# Packaging
if(WIN32)
	set(MING_BIN_PATH "${CMAKE_FIND_ROOT_PATH}/bin") # CMAKE_FIND_ROOT_PATH is set by the toolchain file

	# Include needed libraries in "make install"
	install (FILES
		# Qt4
		${MING_BIN_PATH}/QtCore4.dll
		${MING_BIN_PATH}/QtGui4.dll
		${MING_BIN_PATH}/QtXml4.dll
		${MING_BIN_PATH}/QtXmlPatterns4.dll
		${MING_BIN_PATH}/QtNetwork4.dll
		${MING_BIN_PATH}/QtSvg4.dll
		# mingw/c++ runtimes
		${MING_BIN_PATH}/libgcc_s_sjlj-1.dll
		${MING_BIN_PATH}/libstdc++-6.dll
		# Qt4/poppler dependencies
		${MING_BIN_PATH}/libfreetype-6.dll
		${MING_BIN_PATH}/libjpeg-8.dll
		${MING_BIN_PATH}/liblcms-1.dll
		${MING_BIN_PATH}/libopenjpeg-1.dll
		${MING_BIN_PATH}/libpng15-15.dll
		${MING_BIN_PATH}/libtiff-5.dll
		${MING_BIN_PATH}/zlib1.dll
		${MING_BIN_PATH}/liblzma-5.dll
		${MING_BIN_PATH}/libtiff-5.dll
		# poppler
		${MING_BIN_PATH}/libpoppler-26.dll
		${MING_BIN_PATH}/libpoppler-qt4-4.dll
		DESTINATION bin)

	include(InstallRequiredSystemLibraries)
	set(CPACK_GENERATOR NSIS)

	set(CPACK_NSIS_PACKAGE_NAME            ${PROJECT_NAME})
	set(CPACK_NSIS_DISPLAY_NAME            ${PROJECT_NAME})
	set(CPACK_NSIS_CONTACT                 ${PROJECT_WEBSITE})
	set(CPACK_NSIS_MENU_LINKS              ${PROJECT_WEBSITE} "${PROJECT_NAME} Homepage")
	set(CPACK_NSIS_URL_INFO_ABOUT          ${PROJECT_WEBSITE})
	set(CPACK_NSIS_HELP_LINK               ${PROJECT_WEBSITE})
	set(CPACK_NSIS_COMPRESSOR              "/SOLID lzma")

	set(CPACK_PACKAGE_INSTALL_DIRECTORY    ${PROJECT_NAME})
	set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY ${PROJECT_NAME}-${PROJECT_VERSION})

	set(CPACK_RESOURCE_FILE_LICENSE        "${CMAKE_SOURCE_DIR}/COPYING")
	set(CPACK_PACKAGE_VERSION_MAJOR        ${PROJECT_VERSION_MAJOR})
	set(CPACK_PACKAGE_VERSION_MINOR        ${PROJECT_VERSION_MINOR})
	set(CPACK_PACKAGE_VERSION_PATCH        ${PROJECT_VERSION_PATCH})
	set(CPACK_PACKAGE_EXECUTABLES          "${PROJECT_NAME};${PROJECT_NAME}")

	set(CPACK_STRIP_FILES                  TRUE) # remove debug information from deployed files

	include(CPack)
ENDIF( WIN32 )
