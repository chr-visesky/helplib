cmake_minimum_required(VERSION 3.20)
project(helplib LANGUAGES CXX)

# MSVC / Win32 假定
if(NOT WIN32)
  message(FATAL_ERROR "This stripped build targets Windows only.")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DUNICODE -D_UNICODE -DWIN32_LEAN_AND_MEAN)

# 头文件目录（你的仓库）
include_directories(${CMAKE_SOURCE_DIR}/include)

# -------- helplib: 注入/内存读取 的核心静态库（无任何 Overlay/D3D 源） --------
add_library(helplib STATIC
  src/ConsoleEx.cpp
  src/CrashHandler.cpp
  src/CrashHandler_WIN32.cpp
  src/ExeFile.cpp
  src/ExeFile_WIN32.cpp
  src/Hooker.cpp
  src/Hooker_VEH.cpp
  src/Injector_WIN32.cpp
  src/Input.cpp
  src/Logging.cpp
  src/Main.cpp
  src/Main_WIN32.cpp
  src/Memory_WIN32.cpp
  src/MessageBox_WIN32.cpp
  src/Patch.cpp
  src/PatternScanner.cpp
  src/Process_WIN32.cpp
  src/StringManip.cpp
)

# 仅链接必要系统库（不拉任何 D3D/OpenGL）
target_link_libraries(helplib PRIVATE psapi)

# -------- gw2 目录（去掉 SampleApp 与所有 Overlay 源）--------
add_subdirectory(src/gw2)
