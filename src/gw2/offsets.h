#ifndef OFFSETS_H
#define OFFSETS_H

namespace GW2LIB {
    struct Mems {
        /*
        If you update gw2lib and the patterns are still working it can be useful to know
        the current game object pointers for debugging. The following example shows how to
        get them. If the patterns break, see Gw2HackMain::init the strings are unlikly to
        change but maybe the offsets if code generation is changed.

        #include "main.h"

        void dummy()
        {
            auto ptrs = GetMain()->GetGamePointers();
            void *pCtx = ptrs->pCtx;
        }

        It may also be helpful if you attach a debugger and break on thrown Win32 exeptions, so
        you know the member or function offset that causes the access violation.
        */

        // CContext
        // CharClient::CContext* m_charContext;
        uintptr_t contextChar = 0x98; // os

        /*
        This is the main game state object from which you can get to almost anything else. It can
        only be safely accessed from within the game thread.
        A pointer to this object can be found in the thread-local storage of the game thread. Windows
        stores these in FS:[0x2c].
        The current location is: (unlikely to change)
        CContext ***localStorage = FS:[0x2c];
        CContext *ctx = localStorage[0][1];

        It consists of only pointers to other sub contexts and nulled out space.

        The char context offset can be found by looking at the objects before and after the offset
        where it was before and compare to the CharClient::CContext description.
        "!IsPlayer() || GetPlayer()" + 0xE = 32-bit mov opcode (mov ebx,[eax+??])
        */

        // AgentView::CContext
        // ANet::Array<Agent::CAvAgent*> m_agentArray;
        uintptr_t avctxAgentArray = 0x70; // os (start at "ViewAdvanceAgentView")

        /*
        This is the context that deals with agents, the base of all entities in the game. It is optimized
        out of the main context, but there are static references to it.

        The agent array offset is found by looking for arrays in this object.

        An array has a base pointer that points to the raw data array and two following integers that
        describe the capacity and current size of the array. This layout is really easy to recognize.
        "52 FF 50 ?? C7 86 ?? ?? ?? ?? 00 00 00 00 56 E8 ?? ?? ?? ?? 8B C8 E8" + 0x10 = 32-bit call to Agent Array fn
        */

        // AgentView::CAvAgent
        // Agent::CAgentBase* GetAgent();
        uintptr_t avagVtGetAgent = 0x60; // os

        /*
        We are not interested in this object, but it is used to get from CAvAgent to CAgentBase which
        holds useful information.

        The offset of the virtual function can be found with the following assert string:
        "wmAgent->GetAgent() != agent"
        */
        uintptr_t wmAgVtGetCodedName = 0x0; // os

        // Agent::CAgentBase
        // AgentCategory GetCategory();
        uintptr_t agentVtGetCategory = 0x20; // os
        // int GetAgentId();
        uintptr_t agentId = 0xc; // os
        // AgentType GetType();
        uintptr_t agentType = 0x8; // os
        // void GetPos(D3DXVECTOR3* pPos);
        uintptr_t agentVtGetPos = 0x158; // os
        uintptr_t agentVtGetRot = 0x1A0; // os
        uintptr_t gd_agentVtGetToken = 0x280; // os - L"Content is referencing a propID"
        uintptr_t gd_agentVtGetSeq = 0x228; // os
        // Agent::CAgentTransform* m_transform;
        uintptr_t agentTransform = 0x50; // os

        /*
        Contains agent data that is important to this lib.

        Assert strings for virtual functions in order:
        "agent->GetCategory() == AGENT_CATEGORY_CHAR"
        "targetAgent && targetAgent->GetAgentId()"
        "m_outOfRangeActivationTargetAgent->GetType() == AGENT_TYPE_GADGET_ATTACK_TARGET"
        GetPos I don't remember, but should be easy to trial and error.
        "55 8B EC 8B 49 1C FF 75 08 8B 11 FF 52 3C 8B 45 08 5D C2 04 00" = 32-bit agentVtGetPos func def

        The agentTransform member is very easy to recognize, because many numbers in it
        move when your character moves (when looking at own agent data). See CAgentTransform.
        */

        // Agent::CAgentTransform
        // float x;
        uintptr_t agtransX = 0x30; // os
        // float y;
        uintptr_t agtransY = 0x34; // os
        // float z;
        uintptr_t agtransZ = 0x38; // os
        uintptr_t gd_agtransVtGetRot = 0x130; // os
        uintptr_t agtransVtGetMoveRot = 0x158; // os
        uintptr_t agtranVtSetSeqSt = 0x220; // os
        uintptr_t agtransToken = 0xf0; // os
        uintptr_t agtransSeq = 0xf8; // os
        uintptr_t npc_agtransSpeed = 0x148; // os
        uintptr_t agtransRealSpeed = 0x204; // os
        uintptr_t agtransSpeed = 0x208; // os
        uintptr_t agtransMaxSpeed = 0x20c; // os

        /*
        Holds metric information about an agent.

        The offsets are easy to figure out if you look at data of your own agent and
        move or rotate.
        */

        //uintptr_t wmagTransScale = 0x9f4;

        // CharClient::CContext
        // ANet::Array<CharClient::CCharacter*> m_charArray;
        uintptr_t charctxCharArray = 0x60; // os
        // ANet::Array<CharClient::CPlayer*> m_playerArray;
        uintptr_t charctxPlayerArray = 0x80; // os
        // CharClient::CCharacter* m_controlledCharacter;
        uintptr_t charctxControlled = 0x98; // os
        /*
        Context that contains data about CCharacters.

        Easy to recognize by the arrays. The order of them is unlikely to change.
        */

        // CharClient::CCharacter

        // Agent::CAgentBase* GetAgent();
        uintptr_t charVtGetAgent = 0x0; // os - moved to sub-class
        // int GetAgentId();
        uintptr_t charVtGetAgentId = 0x198; // os
        // CharClient::CPlayer* GetPlayer();
        uintptr_t charVtGetPlayer = 0x290; // os
        // bool IsAlive();
        uintptr_t charVtAlive = 0x8; // os - moved to sub-class
        // bool IsControlled();
        uintptr_t charVtControlled = 0x2e8; // os
        // bool IsDowned();
        uintptr_t charVtDowned = 0x300; // os
        // bool IsInWater();
        uintptr_t charVtInWater = 0x370; // os
        // bool IsMonster();
        uintptr_t charVtMonster = 0x388; // os
        // bool IsMonsterClone();
        uintptr_t charVtClone = 0x388; // os
        // bool IsPlayer();
        uintptr_t charVtPlayer = 0x60; // os
        // bool IsRangerPet();
        uintptr_t charVtRangerPet = 0x4a8; // os
        // Attitude m_attitudeTowardControlled;
        uintptr_t charAttitude = 0xb0; // os
        // CharClient::CBreakBar* m_breakBar;
        uintptr_t charBreakbar = 0xc8; // os
        // CharClient::CCoreStats* m_coreStats;
        uintptr_t charCoreStats = 0x378; // os
        // CharClient::CEndurance* m_endurance;
        uintptr_t charEndurance = 0x2e8; // os
        // CharClient::CHealth* m_health;
        uintptr_t charHealth = 0x3e8; // os
        // CharClient::CInventory* m_inventory;
        uintptr_t charInventory = 0x3f0; // os
        uintptr_t charVtGetInv = 0x88; // os
        uintptr_t charVtGetCmbtnt = 0x118; // os
        uintptr_t charSkillbar = 0x520; // os
        uintptr_t charGliderPercent = 0x138; // os
        uintptr_t charProfession = 0x510; // os
        uintptr_t charName = 0x190; // os
        uintptr_t charFlags = 0x130; // os
        uintptr_t charVtAsFlags = 0x2b8; // os
        uintptr_t charID = 0x90; // os
        uintptr_t charSubClass = 0x8; // os - some offsets have moved to this sub-class
        /*
        Represents a player.

        The name is very easy to find by just comparing to your name.
        "TextValidateLiteral(m_name.Ptr())"
        "8B C8 8B 10 FF 52 ?? 8B C8 8B 10 FF 92 ?? ?? ?? ?? 8B C8 8B 10 FF 52 ?? 6A 00 50 BA" + 0xD = 32-bit GetWallet fn call
        "m_trainingMgr"
        "achievementMgr"
        "specializationMgr"
        */

        uintptr_t cmbtntVtGetBuffBar = 0x40;
        uintptr_t cmbtntBuffBar = 0x88;

        // buff bar struct
        uintptr_t buffbarBuffArr = 0x20;
        uintptr_t buffbarBuffStatArr = 0x44;

        // buff struct
        uintptr_t buffEfType = 0x8;
        uintptr_t buffSkillDef = 0x10;
        uintptr_t buffBuffId = 0x18;
        uintptr_t buffBuffBar = 0x20;
        uintptr_t buffSrcAg = 0x28;
        uintptr_t buffDuration = 0x40;
        uintptr_t buffActive = 0x4c;

        // CharClient::CSkill* m_skillDef
        uintptr_t skillDefEffect = 0x28; // os
        uintptr_t skillDefInfo = 0x60; // os
        uintptr_t skillStackType = 0xc;
        /*
        Represents a character in the game. Generally stuff that can move around like
        players, npcs or monsters.

        Almost everything in here can be found through assert strings. In order:
        "character->GetAgent() == m_agent"
        "m_agent && (m_agent->GetAgentId() == character->GetAgentId() || m_masterCharacter == character)"
        "m_ownerCharacter->GetPlayer() == CharClientContext()->GetControlledPlayer()"
        Three at once! "character->IsAlive() || (character->IsDowned() && character->IsInWater())"
        "IsControlled()"
        "character->IsAlive() || (character->IsDowned() && character->IsInWater())"
        "character->IsAlive() || (character->IsDowned() && character->IsInWater())"
        Two at once! "IsPlayer() || IsMonster()"
        Two at once! "character->IsPlayer() || character->IsMonsterClone()"
        "character->IsPlayer() || character->IsMonsterClone()"
        "m_kennel" then look up for "speciesDef", 3rd vt call counting up

        "m_attitudeTowardControlled < Content::AFFINITY_ATTITUDES"
        "55 8B EC 8B 49 ?? 85 C9 74 ?? 8B 55 ?? 56 8B 31 0F B6 42" +0x5 (32-bit)
        "m_coreStats"
        "!m_endurance"
        "m_health"
        "m_inventory"
        "m_combatant" = char struct shifted +0x18
        "m_skillbar"
        Glider found by looking for float value between 0.0 - 1.0 in your own character data while gliding.
        "m_profession"
        "TextValidateLiteral(m_nameOverride.Ptr())"
        */

        // CharClient::CPlayer
        // char* m_name
        uintptr_t playerName = 0x68; // os
        uintptr_t playerVtGetWallet = 0x1f0; // os
        uintptr_t playerVtGetTrainMgr = 0x398; // os - "m_trainingMgr"
        uintptr_t playerVtGetAchMgr = 0x130; // os
        uintptr_t playerVtGetSpecMgr = 0x340; // os
        uintptr_t playerVtGetChar = 0x0; // os
        uintptr_t playerVtGetUUID = 0x90; // os
        uintptr_t playerChar = 0x20; // os

        // CharClient::CSkillBar* m_skillBar;
        uintptr_t skillbarSkillsArray = 0x0; // os
        uintptr_t skillbarPressedSkill = 0x0; // os
        uintptr_t skillbarHoveredSkill = 0x0; // os
        uintptr_t skillbarVtSkillDown = 0x1a8; // os
        uintptr_t skillbarVtSkillUp = 0x1b8; // os

        uintptr_t walletVtGetCurrency = 0x0;
        uintptr_t trmgrVtGetMLvl = 0x18;
        uintptr_t achMgrVtGetAP = 0x18;
        uintptr_t specMgrSpecsArr = 0x58;
        uintptr_t specMgrTraitsArr = 0x78;
        uintptr_t specType = 0x28;
        uintptr_t traitType = 0x28;

        // CharClient::CCoreStats
        // race
        uintptr_t statsRace = 0x33;
        // gender
        uintptr_t statsGender = 0x35;
        // CharacterStats stats;
        uintptr_t statsStats = 0xb4;
        // int m_level;
        uintptr_t statsLevel = 0x200;
        // int m_scaledLevel;
        uintptr_t statsScaledLevel = 0x234;
        // Profession m_profession;
        uintptr_t statsProfession = 0x280;
        /*
        Some general stat information about a character.

        Offsets are found by looking at the data. Also see profession enum.
        */

        // CharClient::CEndurance
        // int m_currentValue;
        uintptr_t endCurrent = 0x8;
        // int m_maxValue;
        uintptr_t endMax = 0xc;

        // CharClient::CHealth
        // float m_currentValue;
        uintptr_t healthCurrent = 0xc;
        // float m_maxValue;
        uintptr_t healthMax = 0x10;
        uintptr_t healthVtGetBarrier = 0x28;

        // CharClient::CInventory
        // int m_supply
        uintptr_t invVtGetSupply = 0x298;
        uintptr_t invEquipArr = 0x160; // "m_equipmentSlots[equipSlot] == nullptr"

        uintptr_t equipItemDef = 0x40; // "m_itemDef"
        uintptr_t equipItemInv = 0x58; // pointer back to char's inventory
        uintptr_t equipItemVtGetUpgrades = 0x208; // returns sigils/runes struct
        uintptr_t equipItemUpgradesArr = 0x28; // call GetUpgrades first
        uintptr_t equipItemVtGetSkin = 0x70; // returns item def
        uintptr_t equipItemVtPrePost = 0x260; // os
        uintptr_t equipItemVtSkinName = 0x70;

        uintptr_t itemDefId = 0x28;
        uintptr_t itemDefType = 0x2c; // "item->GetDef()->GetItemType() == Content::ITEM_TYPE_GATHERING"
        uintptr_t itemDefRarity = 0x60;
        uintptr_t itemDefReqLvl = 0x74;
        uintptr_t itemDefHashId = 0x80;
        uintptr_t itemDefSelVal = 0x88;

        uintptr_t itemPrePostVtGetPrefix = 0x0;
        uintptr_t itemPrePostVtGetSuffix = 0x10;
        uintptr_t itemSuffixName = 0x30;

        // AgentSelection::CContext
        // Agent::CAgentBase* m_autoSelection;
        uintptr_t asctxAuto = 0x70; // os
        // Agent::CAgentBase* m_hoverSelection;
        uintptr_t asctxHover = 0x130; // os
        // Agent::CAgentBase* m_lockedSelection;
        uintptr_t asctxLocked = 0x298; // os
        // D3DXVECTOR3 m_screenToWorld;
        uintptr_t asctxStoW = 0x300; // os
        uintptr_t asctxCtxMode = 0x90; // os - enum
        uintptr_t asctxVtAgCanSel = 0x8;

        /*
        The offsets can be found in a function containing lots of asserts for them. Strings in order:
        "!m_autoSelection"
        "!m_hoverSelection"
        "!m_lockedSelection"
        screenToWorld is easy to find by just moving the cursor around. It's the first vector, because updated more frequently and correct z.
        "m_contextMode == CONTEXT_MODE_NULL"
        "!agent || AgentCanBeSelection(agent)"
        */

        // WorldView::CContext
        // void GetMetrics(int one, D3DXVECTOR3* camPos, D3DXVECTOR3* lookAt, D3DXVECTOR3* upVec float* fov);
        uintptr_t wvctxVtGetMetrics = 0x88;
        // int m_camStatus;
        uintptr_t wvctxStatus = 0x58;
        /*
        Context for camera information.

        The GetMetrics function contains the strings "eye", "center", "up", "fov". There are two. Currently
        it is the first one. Set a breakpoint and go up the call stack until you see a virtual call
        that contains the offset.

        camStatus is a bit that flips in loading screens and can be found by inspection.
        */

        uintptr_t camWmAgent = 0x580;
        uintptr_t camCurrZoom = 0x48;
        uintptr_t camMinZoom = 0x38;
        uintptr_t camMaxZoom = 0x3c;

        // BreakbarState
        uintptr_t breakbarState = 0x40;
        uintptr_t breakbarPercent = 0x44;

        // compass (minimap) offsets
        uintptr_t compWidth = 0x40;
        uintptr_t compHeight = 0x44;
        uintptr_t compZoom = 0x48;
        uintptr_t compFlags = 0x28;
        uintptr_t compMaxWidth = 0x18;  // max width?
        uintptr_t compMaxHeight = 0x1c; // max height?

        // ui options
        uintptr_t uiIntSize = 0x54;
        uintptr_t uiFlags1 = 0x1cc; // os - "flag < UI_FLAGS"
        uintptr_t uiFlags2 = 0x1d0; // os
        uintptr_t uiFlags3 = 0x1d4; // os
        uintptr_t uiFlags4 = 0x1d8; // os
        uintptr_t uiFlags5 = 0x1dc; // os

        // gadget stuff
        uintptr_t contextGadget = 0x138;
        uintptr_t ctxgdVtGetGadget = 0x10; // os
        uintptr_t ctxgdVtGetAtkTgt = 0x18;

        uintptr_t atkTgtVtGetGd = 0x10; // os
        uintptr_t gdHealth = 0x210; // os
        uintptr_t gdWvwTeamId = 0x470;
        uintptr_t gdVtGetType = 0x100; // os - "gadget->GetType() == Content::GADGET_TYPE_RESOURCE_NODE"
        uintptr_t gdVtGetRNode = 0x150; // os - "Content::GADGET_TYPE_RESOURCE_NODE"

        // resource node stuff
        uintptr_t rnodeType = 0xc;
        uintptr_t rnodeFlags = 0x10;

        // profession stuff
        uintptr_t profStance = 0x40;
        uintptr_t profEnergy = 0x50;
        uintptr_t profEnergyMax = 0x54;

        // squad stuff
        uintptr_t contextSquad = 0x130;

        uintptr_t contextWorldTime = 0x148;

        uintptr_t contextItem = 0xb0;
        uintptr_t ctxItemVtGetItem = 0x8; // Item* ctx->GetItem(uint itemID);
    };
};

#endif