install(
    TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION ${LIBRARY_DIR}         
    LIBRARY DESTINATION ${LIBRARY_DIR}         
    RUNTIME DESTINATION ${BINARY_DIR}
) 

if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Public) 
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Public DESTINATION ${INCLUDE_DIR}/${PROJECT_NAME})
endif()
