cmake_minimum_required(VERSION 3.5)

project(Test1 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY Build)

add_subdirectory(DirInfo)

set(QT_DEPENDS Widgets)
find_package(QT NAMES Qt5 COMPONENTS ${QT_DEPENDS} REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${QT_DEPENDS} REQUIRED)

file(GLOB SOURCES *.h *.cpp)

add_executable(Test1 ${SOURCES})

foreach(QT_DEPEND ${QT_DEPENDS})
    target_link_libraries(Test1 PRIVATE Qt${QT_VERSION_MAJOR}::${QT_DEPEND})
endforeach()

target_link_libraries(Test1 PRIVATE DirInfo)
target_include_directories(Test1 PRIVATE DirInfo)
