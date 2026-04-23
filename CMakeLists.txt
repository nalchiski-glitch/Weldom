cmake_minimum_required(VERSION 3.20)
project(Weldom_Prototype)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3.0 REQUIRED COMPONENTS graphics window system audio)

add_executable(Weldom_Prototype main.cpp)

target_link_libraries(Weldom_Prototype PRIVATE sfml-graphics sfml-window sfml-system sfml-audio)

# Копируем картинки в папку с EXE
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/craft_bg.png DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/stick.png DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/diamond.png DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/pickaxe.png DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/duck_stage_bg.png DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/roasted_chicken.png DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/eye_exam_bg.png DESTINATION ${CMAKE_BINARY_DIR})