cmake_minimum_required(VERSION 3.20)
project(helplib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
  add_definitions(-DARCH_64BIT)
endif()

if (WIN32)
  add_definitions(-DUNICODE -D_UNICODE -DWIN32_LEAN_AND_MEAN)
endif()

# 头文件目录（你的仓库）
include_directories(${CMAKE_SOURCE_DIR}/include)

# -------- helplib: 注入/内存读取 的核心静态库（无任何 Overlay/D3D 源） --------
if (WIN32)
  set(PLATFORM_SOURCES
    src/CrashHandler_WIN32.cpp
    src/ExeFile_WIN32.cpp
    src/Injector_WIN32.cpp
    src/Main_WIN32.cpp
    src/Memory_WIN32.cpp
    src/MessageBox_WIN32.cpp
    src/Process_WIN32.cpp
  )
  set(PLATFORM_LIBS psapi)
else()
  set(PLATFORM_SOURCES
    src/CrashHandler_UNIX.cpp
    src/ExeFile_UNIX.cpp
    src/Injector_UNIX.cpp
    src/Main_UNIX.cpp
    src/Memory_UNIX.cpp
    src/MessageBox_UNIX.cpp
    src/Process_UNIX.cpp
  )
  set(PLATFORM_LIBS dl pthread)
endif()

set(HELPLIB_COMMON_SOURCES
  src/CrashHandler.cpp
  src/ExeFile.cpp
  src/Hooker.cpp
  src/Hooker_VEH.cpp
  src/Input.cpp
  src/Logging.cpp
  src/Main.cpp
  src/Patch.cpp
  src/PatternScanner.cpp
  src/StringManip.cpp
  ${PLATFORM_SOURCES}
)

if (WIN32)
  list(APPEND HELPLIB_COMMON_SOURCES src/ConsoleEx.cpp)
endif()

add_library(helplib STATIC
  ${HELPLIB_COMMON_SOURCES}
  ${PLATFORM_SOURCES}
)

# 仅链接必要系统库（不拉任何 D3D/OpenGL）
target_link_libraries(helplib PRIVATE ${PLATFORM_LIBS})

# -------- gw2 目录（去掉 SampleApp 与所有 Overlay 源）--------
add_subdirectory(src/gw2)
