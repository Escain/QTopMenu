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
	find_package(Qt5Svg REQUIRED)

	include_directories(${Qt5Widgets_INCLUDE_DIRS})
	add_definitions(${Qt5Widgets_DEFINITIONS})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
endmacro()
SetupQtForProject()

set(TARGET "QSvgPixmap")
add_library(${TARGET})

EscainSetWarningPedantic(${TARGET})
target_compile_features( ${TARGET} PUBLIC cxx_std_17)

target_include_directories(${TARGET} PRIVATE "${CMAKE_SOURCE_DIR}/QCustomUtils")

set ( SOURCES 
	QSvgPixmap.cpp
	QSvgPixmapCache.cpp
	QSvgIcon.cpp
	)
set ( HEADERS 
	QSvgPixmap.hpp
	QSvgPixmapCache.hpp
	QSvgIcon.hpp
	)
	
set ( LIBS  
	Qt5::Core 
	Qt5::Svg
	)
	
# Project name
target_sources( ${TARGET} PRIVATE ${SOURCES} PRIVATE ${HEADERS})
target_link_libraries(${TARGET} ${LIBS})

set(Test_QSvgPixmap "UnitTest_QSvgPixmap")
add_executable(${Test_QSvgPixmap})
EscainSetWarningPedantic(${Test_QSvgPixmap})
target_compile_features( ${Test_QSvgPixmap} PUBLIC cxx_std_17)
target_sources( ${Test_QSvgPixmap} PRIVATE "UnitTest.cpp")
target_link_libraries(${Test_QSvgPixmap} Qt5::Widgets QCustomUtils ${LIBS} "QSvgPixmap")
