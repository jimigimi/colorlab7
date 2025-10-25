cmake_minimum_required(VERSION 3.16)
project(ColorLab7 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(ColorLab7
    main.cpp
)

target_link_libraries(ColorLab7 PRIVATE Qt6::Widgets)


if(WIN32)
  add_custom_command(TARGET ColorLab7 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Для упаковки используйте windeployqt:"
    COMMAND ${CMAKE_COMMAND} -E echo "windeployqt --release $<TARGET_FILE:ColorLab7>"
  )
endif()
