include(${CMAKE_MODULE_DIR}/Install.cmake)


set(pack_command "$ENV{QT_SDK}/bin/windeployqt.exe ${BINARY_DIR}/${PROJECT_NAME}.exe")
install(CODE "execute_process(COMMAND powershell -NoProfile -ExecutionPolicy Bypass -Command \"${pack_command}\" WORKING_DIRECTORY ${CMAKE_BINARY_DIR})")