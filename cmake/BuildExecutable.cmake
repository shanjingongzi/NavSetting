include(${CMAKE_MODULE_DIR}/Tools.cmake)
CollectDirectories(TARGET_DIR ${CMAKE_CURRENT_SOURCE_DIR}) 
CollectSources(SRCS "${TARGET_DIR}" ${CMAKE_CURRENT_SOURCE_DIR})
add_executable(${PROJECT_NAME} ${SRCS})
include(${CMAKE_MODULE_DIR}/Install.cmake)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/Public
)