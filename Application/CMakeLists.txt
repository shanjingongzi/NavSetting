cmake_minimum_required(VERSION 3.16.1)

PROJECT(NavSetting)
include(${CMAKE_MODULE_DIR}/BuildQtExecutable.cmake)

AddQtModule(Widgets)
AddQtModule(SerialPort)
include_directories(
    ${ROOT_DIR}/Shared/Public
)
