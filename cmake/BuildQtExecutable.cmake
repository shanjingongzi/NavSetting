include(${CMAKE_MODULE_DIR}/Tools.cmake)
include(${CMAKE_MODULE_DIR}/FindQt.cmake)


CollectDirectories(TARGET_DIR ${CMAKE_CURRENT_SOURCE_DIR}) 
CollectSources(SRCS "${TARGET_DIR}" ${CMAKE_CURRENT_SOURCE_DIR})
CollectMOCSources(RES "${TARGET_DIR}" ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/Public
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(
        ${PROJECT_NAME} 
        MANUAL_FINALIZATION
        ${SRCS}
        ${RES}
    )

# Define target properties for Android with Qt 6 as:
#    set_property(TARGET template APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(${PROJEECT_NAME} SHARED
            ${SRCS}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(${PROJECT_NAME}
            ${SRCS}
        )
    endif()
endif()

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()

include(${CMAKE_MODULE_DIR}/QtInstall.cmake)