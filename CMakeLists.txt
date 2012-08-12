project (open-pdf-presenter)

cmake_minimum_required(VERSION 2.6)

# Project Definitions
set(PROJECT_VERSION "0.2.99")

add_definitions(-DOPP_VERSION=\"${PROJECT_VERSION}\")

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

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmakemodules/)
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
