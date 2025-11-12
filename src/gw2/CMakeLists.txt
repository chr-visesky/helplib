# 只保留“注入 + 内存读取 + 扫描 + 逻辑包装”，移除 SampleApp.cpp
if (WIN32)
  add_library(helplib_gw2 SHARED
    Agent.cpp
    AttackTarget.cpp
    Buff.cpp
    Camera.cpp
    Character.cpp
    Compass.cpp
    DataScan.cpp
    EquipItem.cpp
    Frame.cpp
    Gadget.cpp
    GameData.cpp
    General.cpp
    Gw2Hook.cpp
    Item.cpp
    Memory.cpp
    Player.cpp
    RefreshData.cpp
    ResourceNode.cpp
    stub.cpp
    main.cpp
  )

  target_include_directories(helplib_gw2 PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
  )

  target_link_libraries(helplib_gw2 PRIVATE helplib ws2_32 comctl32)
else()
  add_library(helplib_gw2 SHARED
    stub.cpp
  )

  target_include_directories(helplib_gw2 PRIVATE
    ${CMAKE_SOURCE_DIR}/include
  )

  target_link_libraries(helplib_gw2 PRIVATE helplib)
endif()

set_target_properties(helplib_gw2 PROPERTIES
  OUTPUT_NAME "helplib_gw2"
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
