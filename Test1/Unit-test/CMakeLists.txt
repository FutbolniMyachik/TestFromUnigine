set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY Unit-test)

set(QT_DEPENDS Test Widgets)
find_package(QT NAMES Qt5 COMPONENTS ${QT_DEPENDS} REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${QT_DEPENDS} REQUIRED)

file(GLOB SOURCES *.h *.cpp)

add_executable(Unit-test ${SOURCES})

foreach(QT_DEPEND ${QT_DEPENDS})
    target_link_libraries(Unit-test PRIVATE Qt${QT_VERSION_MAJOR}::${QT_DEPEND})
endforeach()

target_link_libraries(Unit-test PRIVATE DirInfo)
target_include_directories(Unit-test PRIVATE ../DirInfo)
