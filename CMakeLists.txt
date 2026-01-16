cmake_minimum_required(VERSION 4.1)
project(1)

set(CMAKE_CXX_STANDARD 20)
add_executable(1 main.cpp)

# 关键：适配字符编码+链接Windows透明/图形库
if(MINGW)
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
    target_link_libraries(1 user32 gdi32 kernel32)  # 透明/图形/窗口依赖
    set_target_properties(1 PROPERTIES LINK_FLAGS "-Wl,-subsystem,console")
endif()