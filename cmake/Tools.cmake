function(CollectDirectories result cur_dir)
    file(GLOB children RELATIVE ${cur_dir} ${cur_dir}/*)
    set(subdirs "")
    if(IS_DIRECTORY ${cur_dir})
        list(APPEND subdirs ${cur_dir}/${child})
    endif()
    foreach(child ${children})
        if(IS_DIRECTORY ${cur_dir}/${child}) 
            string(TOLOWER ${child} low_child) 
            if(NOT low_child STREQUAL "build" )
                if(NOT low_child STREQUAL "cmakefiles")
                    list(APPEND subdirs ${cur_dir}/${child})
                    CollectDirectories(subsubdirs ${cur_dir}/${child})
                    list(APPEND subdirs ${subsubdirs})
                endif()
            endif()
        endif()
    endforeach()
    set(${result} ${subdirs} PARENT_SCOPE)
endfunction()

function(CollectSources RESULT ALL_DIRECTORIES BASE_DIR)
    set(SRCS "")
    foreach(DIR  IN LISTS ALL_DIRECTORIES)
        file(GLOB CURRENT_SOURCES
            ${DIR}/*.h
            ${DIR}/*.hpp
            ${DIR}/*.inl
            ${DIR}/*.cpp
            ${DIR}/*.c
            ${DIR}/*.cxx
        )
        if(CURRENT_SOURCES)
            list(APPEND SRCS ${CURRENT_SOURCES})
            file(RELATIVE_PATH GROUP_NAME ${BASE_DIR} ${DIR})
            if(GROUP_NAME)
                source_group(${GROUP_NAME} FILES ${CURRENT_SOURCES})
            endif()
        endif()
    endforeach()
    set(${RESULT} ${SRCS} PARENT_SCOPE)
endfunction()

function(CollectMOCSources RESULT ALL_DIRECTORIES BASE_DIR)
    set(SRCS "")
    foreach(DIR  IN LISTS ALL_DIRECTORIES)
        file(GLOB CURRENT_SOURCES
            ${DIR}/*.rc
            ${DIR}/*.ui
        )
        if(CURRENT_SOURCES)
            list(APPEND SRCS ${CURRENT_SOURCES})
            file(RELATIVE_PATH GROUP_NAME ${BASE_DIR} ${DIR})
            if(GROUP_NAME)
                source_group(${GROUP_NAME} FILES ${CURRENT_SOURCES})
            endif()
        endif()
    endforeach()
    set(${RESULT} ${SRCS} PARENT_SCOPE)
endfunction()