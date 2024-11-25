include(${CMAKE_MODULE_DIR}/Tools.cmake)

CollectDirectories(TARGET_DIR ${CMAKE_CURRENT_SOURCE_DIR}) 
CollectSources(SRCS "${TARGET_DIR}" ${CMAKE_CURRENT_SOURCE_DIR})
add_library(${PROJECT_NAME}  SHARED ${SRCS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Public)
target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_LIBRARY)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Public)

include(${CMAKE_MODULE_DIR}/Install.cmake)