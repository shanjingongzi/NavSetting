set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT_SDK $ENV{QT_SDK})

if(DEFINED ENV{QT_SDK})
    set(QT_SDK "$ENV{QT_SDK}")
endif()

if("${QT_SDK}" STREQUAL "")
    message(FATAL_ERROR "Count not find QT SDK.Please Set environment variable sush as D:/Qt/6.6.0/msvc2019_64")
else()
    message("Find QT SDK:${QT_SDK}")
endif()

set(QT_LIBRARY ${QT_SDK}/lib/cmake)

set(CMAKE_PREFIX_PATH 
    ${QT_LIBRARY}
)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)

set(QT_VERSION Qt${QT_VERSION_MAJOR})

set(CMAKE_PREFIX_PATH
    ${QT_LIBRARY}/${QT_VERSION}
    ${QT_LIBRARY}/${QT_VERSION}Widgets
    ${QT_LIBRARY}/${QT_VERSION}CoreTools
    ${QT_LIBRARY}/${QT_VERSION}GuiTools
)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

function(AddQtModule moduleName)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${moduleName})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${QT_VERSION}::${moduleName})
endfunction()