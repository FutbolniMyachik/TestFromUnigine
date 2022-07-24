set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(QT_DEPENDS Widgets Core)
find_package(QT NAMES Qt5 COMPONENTS ${QT_DEPENDS} REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${QT_DEPENDS} REQUIRED)

file(GLOB SOURCES *.h *.cpp)

add_library(DirInfo STATIC ${SOURCES})

foreach(QT_DEPEND ${QT_DEPENDS})
    target_link_libraries(DirInfo PRIVATE Qt${QT_VERSION_MAJOR}::${QT_DEPEND})
endforeach()
