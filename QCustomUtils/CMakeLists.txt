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

	include_directories(${Qt5Widgets_INCLUDE_DIRS})
	add_definitions(${Qt5Widgets_DEFINITIONS})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
endmacro()
SetupQtForProject()

set(TARGET "QCustomUtils")
add_library(${TARGET})

EscainSetWarningPedantic(${TARGET})
target_compile_features( ${TARGET} PUBLIC cxx_std_17)

set ( SOURCES 
	QPaletteExt.cpp 
	QClickManager.cpp
	)
set ( HEADERS 
	QPaletteExt.hpp 
	QClickManager.hpp
	)
	
set ( LIBS  
	Qt5::Core 
	)
	
# Project name
target_sources( ${TARGET} PRIVATE ${SOURCES} PRIVATE ${HEADERS})
target_link_libraries(${TARGET} ${LIBS})

set( TestClickManager UnitTest_ClickManager)
add_executable(${TestClickManager})
EscainSetWarningPedantic(${TestClickManager})
target_compile_features( ${TestClickManager} PUBLIC cxx_std_17)
target_sources( ${TestClickManager} PRIVATE "UnitTest_ClickManager.cpp")
target_link_libraries(${TestClickManager} Qt5::Widgets ${LIBS} "QCustomUtils")

set( TestPaletteExt UnitTest_PaletteExt)
add_executable(${TestPaletteExt})
EscainSetWarningPedantic(${TestPaletteExt})
target_compile_features( ${TestPaletteExt} PUBLIC cxx_std_17)
target_sources( ${TestPaletteExt} PRIVATE "UnitTest_PaletteExt.cpp")
target_link_libraries(${TestPaletteExt} Qt5::Widgets ${LIBS} "QCustomUtils")


