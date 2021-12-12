#
# This file is part of Escain QTopMenu library
#
# QTopMenu library is free software: you can redistribute it and/or modify
# it under ther terms of the GNU General Public License as published by
# the Free Software Foundation, version 3 of the License.
#
# Escain Documentor is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this library. If not, see <https://www.gnu.org/licenses/>.
#
# Author: Adrian Maire escain (at) gmail.com
#

cmake_minimum_required(VERSION 3.13)
cmake_policy( SET CMP0076 NEW )

project("QTopMenu")

macro( "EscainSetWarningPedantic" TARGET)
	if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
		target_compile_options( ${TARGET} PRIVATE -Wall -pedantic -Wno-padded -Werror=return-type)
	endif()

	if(CMAKE_BUILD_TOOL MATCHES "(msdev|devenv|nmake)")
		target_compile_options( ${TARGET} PRIVATE /W2)
	endif()
endmacro()

macro( SetupQtForProject )
	set(CMAKE_AUTOMOC ON)
	set(CMAKE_AUTORCC ON)
	set(CMAKE_AUTOUIC ON)
	set(CMAKE_INCLUDE_CURRENT_DIR ON)

	set(QT_USE_QMAIN true) #Avoid console, need WIN32 in the add_executable on Windows

	find_package(Qt5Widgets REQUIRED)
	find_package(Qt5Core REQUIRED)
	find_package(Qt5Svg REQUIRED)
	find_package(Qt5Xml REQUIRED)

	include_directories(${Qt5Widgets_INCLUDE_DIRS})
	add_definitions(${Qt5Widgets_DEFINITIONS})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
endmacro()
SetupQtForProject() #Before add_executable


set(TARGET "QTopMenu")
add_library(${TARGET})

EscainSetWarningPedantic(${TARGET})
target_compile_features( ${TARGET} PUBLIC cxx_std_17)

add_subdirectory("${CMAKE_SOURCE_DIR}/QCustomUtils")
include_directories("${CMAKE_SOURCE_DIR}/QCustomUtils")
add_subdirectory("${CMAKE_SOURCE_DIR}/QSvgPixmap")
include_directories("${CMAKE_SOURCE_DIR}/QSvgPixmap")

#Build this project
set ( SOURCES 
	QTopMenu.cpp
	QTopMenuTab.cpp
	QTopMenuGrid.cpp
	QTopMenuGridGroup.cpp
	QTopMenuGridGroupPopup.cpp
	QTopMenuAction.cpp
	QTopMenuWidget.cpp
	QTopMenuButton.cpp
	QTopMenuButtonWidget.cpp
	)

set ( HEADERS 
	QTopMenu.hpp
	QTopMenuTab.hpp
	QTopMenuGrid.hpp
	QTopMenuGridGroup.hpp
	QTopMenuGridGroupPopup.hpp
	QTopMenuAction.hpp
	QTopMenuWidget.hpp
	QTopMenuWidgetTypes.hpp
	QTopMenuButton.hpp
	QTopMenuButtonWidget.hpp
	)

set ( LIBS  
	Qt5::Widgets
	Qt5::Core
	Qt5::Gui
	Qt5::Svg
	)

# Project name
target_sources( ${TARGET} PRIVATE ${SOURCES} PRIVATE ${HEADERS})
target_link_libraries(${TARGET} ${LIBS})

# GUI TEST for GridGroup
set(Test_GridGroup "UnitTest_GridGroup")
add_executable(${Test_GridGroup})
EscainSetWarningPedantic(${Test_GridGroup})
target_compile_features( ${Test_GridGroup} PUBLIC cxx_std_17)
qt5_wrap_ui( UI_GRID_GROUP_HEADERS "tests/Test_QTopMenuGridGroup.ui" )
target_sources( ${Test_GridGroup} PRIVATE "tests/Test_QTopMenuGridGroup.cpp" ${UI_GRID_GROUP_HEADERS})
target_link_libraries(${Test_GridGroup} QTopMenu QCustomUtils QSvgPixmap)

# GUI TEST for Grid
set(Test_Grid "UnitTest_Grid")
add_executable(${Test_Grid})
EscainSetWarningPedantic(${Test_Grid})
target_compile_features( ${Test_Grid} PUBLIC cxx_std_17)
qt5_wrap_ui( UI_GRID_HEADERS "tests/Test_QTopMenuGrid.ui" )
target_sources( ${Test_Grid} PRIVATE "tests/Test_Grid.cpp" ${UI_GRID_HEADERS})
target_link_libraries(${Test_Grid} QTopMenu QCustomUtils QSvgPixmap)

set(Test_TopMenuButton "UnitTest_TopMenuButton")
add_executable(${Test_TopMenuButton})
EscainSetWarningPedantic(${Test_TopMenuButton})
target_compile_features( ${Test_TopMenuButton} PUBLIC cxx_std_17)
qt5_wrap_ui( UI_BUTTON_HEADERS "tests/Test_QTopMenuButton.ui" )
target_sources( ${Test_TopMenuButton} PRIVATE "tests/Test_QTopMenuButton.cpp" ${UI_BUTTON_HEADERS})
target_link_libraries(${Test_TopMenuButton} QTopMenu QCustomUtils QSvgPixmap)

# # GUI TEST for all the widget
set(Test_All "UnitTest_All")
add_executable(${Test_All})
EscainSetWarningPedantic(${Test_All})
target_compile_features( ${Test_All} PUBLIC cxx_std_17)
qt5_wrap_ui( UI_MENU_HEADERS "tests/Test_QTopMenu.ui" )
target_sources( ${Test_All} PRIVATE "tests/Test_QTopMenu.cpp" ${UI_MENU_HEADERS})
target_link_libraries(${Test_All} ${LIBS} QTopMenu QCustomUtils QSvgPixmap)

# Simple example
set(Test_Example "UnitTest_Example")
add_executable(${Test_Example})
EscainSetWarningPedantic(${Test_Example})
target_compile_features( ${Test_Example} PUBLIC cxx_std_17)
target_sources( ${Test_Example} PRIVATE "tests/Test_Example.cpp" ${UI_EXAMPLE_HEADERS})
target_link_libraries(${Test_Example} ${LIBS} QTopMenu QCustomUtils QSvgPixmap)


