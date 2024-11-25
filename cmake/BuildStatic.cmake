include(${CMAKE_MODULE_DIR}/Tools.cmake)

CollectDirectories(TARGET_DIR ${CMAKE_CURRENT_SOURCE_DIR}) 
CollectSources(SRCS "${TARGET_DIR}" ${CMAKE_CURRENT_SOURCE_DIR})
add_library(${PROJECT_NAME}  STATIC ${SRCS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Public)

include(${CMAKE_MODULE_DIR}/Install.cmake)