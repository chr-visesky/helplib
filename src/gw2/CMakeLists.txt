PROJECT(hacklib_gw2)

ADD_LIBRARY(${PROJECT_NAME} STATIC
    gw2lib.h
    enums.h
    General.cpp
    Camera.cpp
    Agent.cpp
    Character.cpp
    Player.cpp
    EspDraw.cpp
    Gadget.cpp
    AttackTarget.cpp
    Compass.cpp
    Memory.cpp
    Buff.cpp
    DataScan.cpp
    offsets.h
    GameData.h
    GameData.cpp
    Gw2Hook.h
    Gw2Hook.cpp
    ResourceNode.cpp
    Item.cpp
    EquipItem.cpp
    RefreshData.cpp
    main.h
    main.cpp
    )

SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES FOLDER ${PROJECT_NAME})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} hacklib ws2_32)

TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC .)


ADD_LIBRARY(${PROJECT_NAME}_sample SHARED SampleApp.cpp)

SET_TARGET_PROPERTIES(${PROJECT_NAME}_sample PROPERTIES FOLDER ${PROJECT_NAME}_sample)

TARGET_LINK_LIBRARIES(${PROJECT_NAME}_sample ${PROJECT_NAME})
