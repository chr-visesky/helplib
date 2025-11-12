#include "main.h"
#include "hacklib/PatternScanner.h"
#include "hacklib/Logging.h"

#define VERIFY_CB [](uintptr_t o, uintptr_t val)->bool
#define SHIFT_CB [](uintptr_t o)->uintptr_t

namespace GW2LIB {

    /*
    * DataScan():
    *  pattern = the pattern to search for, passed to hl::FindPattern() or hl::PatternScanner::find()
    *  shiftCb = custom func to call for shifting the offset, "shift" and "size" are ignored
    *  shift = the relative offset from the pattern to the target value
    *  size = number of bytes to extract from the target offset
    *  isString = which scan type to use hl::FindPattern() or hl::PatternScanner::find()
    *  verify = callback to verify if offset is correct
    */
    uintptr_t DataScan(const char *pattern, uintptr_t(*shiftCb)(uintptr_t offset), int32_t shift, uint8_t size, int instance, bool isString, bool(*verify)(uintptr_t offset, uintptr_t value), bool cache) {
        static std::unordered_map <std::string, uintptr_t> OffsetScanMap;
        uintptr_t val = 0;
        uintptr_t offset = 0;

        if (cache && OffsetScanMap.count(pattern)) {
            offset = OffsetScanMap[pattern];
        } else {
            try {
                if (isString) {
                    static hl::PatternScanner scanner;
                    offset = scanner.findString(pattern, "", instance);
                } else {
                    offset = hl::FindPattern(pattern, "", instance);
                }

                OffsetScanMap[pattern] = offset;
            } catch (...) {
                offset = 0;
            }
        }

        if (!offset) {
            HL_LOG_ERR("[GW2LIB::DataScan] Could not find pattern: \"%s\"\n", pattern);
            throw STATUS_ACCESS_VIOLATION;
        }

        if (shiftCb) { // PointerScan
            try {
                val = shiftCb(offset);
            } catch (...) {
                HL_LOG_ERR("[GW2LIB::DataScan] Exception occured in callback for pattern: \"%s\"\n", pattern);
                throw STATUS_ACCESS_VIOLATION;
            }
        } else { // OffsetScan
            offset += shift;

            // extract target value and byte-swap
            while (size--) val |= (*(uint8_t*)(offset + size)) << (size * 8);
        }

        if (verify && !verify(offset, val)) {
            HL_LOG_ERR("[GW2LIB::DataScan] Verifying pattern failed (offset: 0x%p, value: 0x%X, pattern: \"%s\")\n", offset, val, pattern);
            throw STATUS_ACCESS_VIOLATION;
        }

        return val;
    }

    uintptr_t OffsetScan(const char *pattern, int32_t shift, uint8_t size, int instance, bool isString, bool(*verify)(uintptr_t offset, uintptr_t value), bool cache) {
        return DataScan(pattern, nullptr, shift, size, instance, isString, verify, cache);
    }

    uintptr_t PointerScan(const char *pattern, uintptr_t(*cb)(uintptr_t offset), int instance, bool isString, bool(*verify)(uintptr_t offset, uintptr_t value), bool cache) {
        return DataScan(pattern, cb, 0, 0, instance, isString, verify, cache);
    }
};

bool Gw2HackMain::ScanForOffsets() {
    using namespace GW2LIB;
    GW2LIB::Mems &m = m_pubmems;

    try {
        m.contextChar = OffsetScan("!IsPlayer() || GetPlayer()", 0x11, 4, 0, true, VERIFY_CB{ return val >= 0x10 && val <= 0x300; });
        m.avagVtGetAgent = OffsetScan("centerRadius < agentRadius", 0x11, 1, 0, true, VERIFY_CB{ return val >= 0x24 && val <= 0xc0; });
        m.wmAgVtGetCodedName = OffsetScan("48 85 C0 74 ?? 48 8B 10 48 8B C8 FF 92 ?? ?? ?? ?? 48 85 C0 74 ?? 48 8B D0 48 8B CB E8 ?? ?? ?? ?? 48 8B D8", 0xd, 4, 0, false, VERIFY_CB{ return val >= 0x30 && val <= 0x100; });
        m.agentVtGetCategory = OffsetScan("agent->GetCategory() == AGENT_CATEGORY_KEYFRAMED", -0x16, 1, 0, true, VERIFY_CB{ return val > 0 && val <= 0x50; });
        m.agentId = OffsetScan("targetAgent && targetAgent->GetAgentId()", -0x11, 1, 0, true, VERIFY_CB{ return val >= 0x0 && val <= 0x20; });
        //m.agentId = OffsetScan("!m_activeFloaters.BitTest(agent->GetAgentId())", -0x2c, 1, 0, true, VERIFY_CB{ return val >= 0x0 && val <= 0x20; }); // alt pattern
        m.agentType = OffsetScan("m_outOfRangeActivationTargetAgent->GetType() == AGENT_TYPE_GADGET_ATTACK_TARGET", -0x14, 1, 0, true, VERIFY_CB{ return val >= 0x0 && val <= 0x20; });
        m.agentVtGetPos = OffsetScan("48 8B 03 48 8D 54 24 ?? FF 90 ?? ?? ?? ?? 8B 44 24 ?? 25 00 00 00", 0xa, 4, 0, false, VERIFY_CB{ return val >= 0x40 && val <= 0x400; });
        m.agentVtGetRot = OffsetScan("48 8D 54 24 ?? 4C 8B 81 ?? ?? ?? ?? 48 8B C8 41 FF D0 48 8B C8", 0x8, 4, 0, false, VERIFY_CB{ return val >= 0x40 && val <= 0x400; });
        m.gd_agentVtGetToken = OffsetScan("49 8B 16 48 8B F8 48 8B 08 48 8B 59 ?? 49 8B CE FF 92 ?? ?? ?? ?? 48 8B D0 48 8B CF FF D3 4C 8B 74 24 ?? 48 8B", 0x12, 4, 0, false, VERIFY_CB{ return val >= 0xd0 && val <= 0x500; });
        m.gd_agentVtGetSeq = OffsetScan("40 53 48 83 EC 20 48 8B D9 48 8D 54 24 30 48 8B 49 38 48 8B 01 FF 90", 0x17, 4, 0, false, VERIFY_CB{ return val >= 0xf0 && val <= 0x500; });
        m.agentTransform = OffsetScan("85 C0 74 14 48 8B 4F ?? 48 8D 54 24 ?? 48 8B 01 FF 90", 7, 1, 0, false, VERIFY_CB{ return val > 0 && val <= 0x70; });
        m.agtransX = OffsetScan("F3 0F 10 59 ?? 8B 41 ?? F3 0F 10 49 ?? F3 0F 10 51 ?? 0F 57 15", 0x4, 1, 0, false, VERIFY_CB{ return val > 0 && val <= 0x74; });
        m.agtransY = m.agtransX + 4;
        m.agtransZ = m.agtransX + 8;
        //m.gd_agtransVtGetRot = OffsetScan("48 83 EC 48 48 8B 49 ?? 48 8D 54 24 ?? 48 8B 01 FF 90 ?? ?? ?? ?? 48 8B D0", 0x12, 4, 0, false, VERIFY_CB{ return val > 0x70 && val <= 0x400; });
        m.agtransVtGetMoveRot = OffsetScan("48 8B 03 48 8D 54 24 ?? 48 8B CB FF 90 ?? ?? ?? ?? F3 0F 10 48 ?? F3 0F 10 00", 0xd, 4, 0, false, VERIFY_CB{ return val > 0x20 && val <= 0x400; });
        m.agtranVtSetSeqSt = OffsetScan("48 8B 52 ?? 48 8B 01 FF 90 ?? ?? ?? ?? 48 8B 4B ?? 4C", 0x9, 4, 0, false, VERIFY_CB{ return val > 0x20 && val <= 0x400; });
        m.npc_agtransSpeed = OffsetScan("48 89 84 24 ?? ?? ?? ?? F3 0F 10 91 ?? ?? ?? ?? 0F 57 E4 F3", 0xc, 4, 0, false, VERIFY_CB{ return val > 0x80 && val <= 0x500; });
        m.agtransRealSpeed = OffsetScan("0F 57 D2 0F 28 C1 F3 0F 5C C2 0F 2F C2 73 03 0F 57 C9 F3 0F 11 8B", 0x16, 4, 0, false, VERIFY_CB{ return val > 0x1a0 && val <= 0x700; });
        m.agtransSpeed    = m.agtransRealSpeed + 4;
        m.agtransMaxSpeed = m.agtransRealSpeed + 8;
        m.charctxCharArray = OffsetScan("m_characterArray.Count() <= agentId || !m_characterArray[agentId]", -0x1b, 1, 0, true, VERIFY_CB{ return val >= 0x20 && val <= 0xc0; });
        m.charctxPlayerArray = OffsetScan("m_playerArray.Count() <= playerId || !m_playerArray[playerId]", -0x1f, 4, 0, true, VERIFY_CB{ return val >= 0x20 && val <= 0xc0; });
        m.charctxControlled = OffsetScan("CC 48 8B 89 ?? ?? ?? ?? 48 85 C9 74 0B 48 8B 41 ?? 48 83 C1 08 48 FF 20 33 C0 C3 CC", 4, 4, 0, false, VERIFY_CB{ return val > 0x50 && val <= 0x300; });
        m.charVtGetAgent = OffsetScan("character->GetAgent() == m_agent", -0x18, 2, 0, true, VERIFY_CB{ return val == 0x10ff; }) - 0x10ff;
        m.charVtGetAgentId = OffsetScan("m_agent && (m_agent->GetAgentId() == character->GetAgentId() || m_masterCharacter == character)", -0x27, 4, 0, true, VERIFY_CB{ return val > 0 && val <= 0x400; });
        m.charVtGetPlayer = OffsetScan("m_ownerCharacter->GetPlayer() == CharClientContext()->GetControlledPlayer()", -0x26, 4, 0, true, VERIFY_CB{ return val > 0 && val <= 0x500; });
        m.charVtAlive = OffsetScan("character->IsAlive() || (character->IsDowned() && character->IsInWater())", -0x35, 1, 0, true, VERIFY_CB{ return val > 0 && val <= 0x40; });
        m.charVtDowned = OffsetScan("character->IsAlive() || (character->IsDowned() && character->IsInWater())", -0x28, 4, 0, true, VERIFY_CB{ return val > 0 && val <= 0x700; });
        m.charVtInWater = OffsetScan("character->IsAlive() || (character->IsDowned() && character->IsInWater())", -0x18, 4, 0, true, VERIFY_CB{ return val > 0 && val <= 0x700; });
        m.charVtControlled = OffsetScan("IsControlled()", -0x18, 4, 0, true, VERIFY_CB{ return val > 0 && val <= 0x700; });
        m.charVtMonster = OffsetScan("IsPlayer() || IsMonster()", -0x18, 4, 0, true, VERIFY_CB{ return val > 0 && val <= 0x700; });
        m.charVtPlayer = OffsetScan("character->IsPlayer() || character->IsMonsterClone()", -0x25, 1, 0, true, VERIFY_CB{ return val > 0 && val <= 0x81; });
        m.charVtClone = OffsetScan("character->IsPlayer() || character->IsMonsterClone()", -0x18, 4, 0, true, VERIFY_CB{ return val > 0 && val <= 0x900; });
        m.charVtRangerPet = OffsetScan("CC CC CC CC CC 40 53 48 83 EC 20 48 8B D9 48 8B 89 ?? ?? ?? ?? 48 8B 01 FF 90 ?? ?? ?? ?? 85 C0 74", 0x1a, 4, 0, false, VERIFY_CB{ return val > 0 && val <= 0x900; });
        m.charAttitude = OffsetScan("m_attitudeTowardControlled < Content::AFFINITY_ATTITUDES", -0x1a, 4, 0, true, VERIFY_CB{ return val > 0 && val <= 0x200; });
        m.charBreakbar = OffsetScan("83 7B 04 08 76 03 48 8B 36 48 83 BF ?? ?? ?? ?? 00", 0xc, 4, 0, false, VERIFY_CB{ return val > 0 && val <= 0x200; });
        m.charCoreStats = OffsetScan("!m_coreStats", -0x1a, 4, 0, true, VERIFY_CB{ return val > 0 && val <= 0x500; });
        m.charEndurance = OffsetScan("!m_endurance", -0x22, 4, 0, true, VERIFY_CB{ return val > 0 && val <= 0x600; });
        m.charHealth = OffsetScan("m_health", -0x1a, 4, 0, true, VERIFY_CB{ return val > 0 && val <= 0x800; });
        m.charVtGetInv = OffsetScan("m_inventory == character->GetInventory()", -0x19, 4, 0, true, VERIFY_CB{ return val > 0 && val <= 0xC0; });
        m.charVtGetCmbtnt = OffsetScan("48 8B 13 48 8B CB 48 8B 5C 24 ?? 48 83 C4 20 5F 48 FF A2 ?? ?? ?? ?? CC", 0x13, 4, 0, false, VERIFY_CB{ return val > 0 && val <= 0x400; });
        m.charSkillbar = OffsetScan("!m_skillbar", -0x1a, 4, 0, true, VERIFY_CB{ return val > 0 && val <= 0x900; });
        m.charGliderPercent = OffsetScan("CC 48 83 EC 28 F3 0F 10 81 ?? ?? ?? ?? 4C 8D 4C 24 ?? F3 0F 11 89 ?? ?? ?? ?? 4C 8D 44 24", 9, 4, 0, false, VERIFY_CB{ return val > 0 && val <= 0x400; });
        m.charProfession = OffsetScan("m_profession", -0x1a, 4, 0, true, VERIFY_CB{ return val > 0 && val <= 0x900; });
        m.charName = OffsetScan("TextValidateLiteral(m_name.Ptr())", -0x25, 1, 0, true, VERIFY_CB{ return val > 0 && val <= 0xa0; });
        m.charFlags = OffsetScan("CC 48 8B 81 ?? ?? ?? ?? 48 85 C0 74 0E 8B 88 ?? ?? ?? ?? C0 E9 04 F6 C1 01", 0xf, 4, 0, false, VERIFY_CB{ return val > 0 && val <= 0x400; });
        m.charVtAsFlags = OffsetScan("48 8B 03 BA 01 00 00 00 48 8B CB FF 90 ?? ?? ?? ?? C1 E8 0C 83 E0 01 48 83 C4 20 5B C3", 0xd, 4, 0, false, VERIFY_CB{ return val > 0 && val <= 0x500; });
        //m.charID = OffsetScan("41 B9 01 00 00 00 48 8B 50 ?? 48 8D 48 ?? 4C 63 42 ?? 48 8B 93", 0x15, 4, 0, false, VERIFY_CB{ return val >= 0x30 && val <= 0x300; });
        m.charSubClass = OffsetScan("character->GetAgent() == m_agent", -0x1f, 1, 0, true, VERIFY_CB{ return val > 0 && val <= 0x30; });
        m.playerName = OffsetScan("TextValidateLiteral(m_name.Ptr())", -0x25, 1, 0, true, VERIFY_CB{ return val > 0 && val <= 0xa0; });
        m.playerVtGetWallet = OffsetScan("48 8B 10 FF 92 ?? ?? ?? ?? 48 8B C8 48 8B 10 FF 52 ?? 44 8B C0 45 33 C9", 5, 4, 0, false, VERIFY_CB{ return val > 0 && val <= 0x400; });
        m.playerVtGetTrainMgr = OffsetScan("48 8B 03 48 8B CB FF 90 ?? ?? ?? ?? 48 8B C8 4C 8B 00 41 FF 90 ?? ?? ?? ?? 48 89 46", 0x15, 4, 0, false, VERIFY_CB{ return val > 0 && val <= 0x600; });
        m.playerVtGetAchMgr = OffsetScan("achievementMgr", -0x1c, 4, 0, true, VERIFY_CB{ return val > 0 && val <= 0x400; });
        m.playerVtGetSpecMgr = OffsetScan("specializationMgr", -0x1c, 4, 0, true, VERIFY_CB{ return val > 0 && val <= 0x500; });
        m.playerVtGetChar = OffsetScan("FF 90 ?? ?? ?? ?? 48 85 C0 74 27 48 8B 10 48 8B C8 FF 52 ?? 48 85 C0", 0x13, 1, 0, false, VERIFY_CB{ return val >= 0 && val <= 0x50; });
        m.playerVtGetUUID = OffsetScan("48 8B 02 48 8B CA FF 90 ?? ?? ?? ?? 48 8B D0 48 8B CB 48 83 C4 ?? 5B", 0x8, 4, 0, false, VERIFY_CB{ return val > 0 && val <= 0x300; });
        m.cmbtntVtGetBuffBar = OffsetScan("m_direction2d < arrsize(s_stateGroundLimp)", -0x3b, 1, 0, true, VERIFY_CB{ return val >= 0 && val <= 0x80; });
        m.buffbarBuffArr = OffsetScan("83 79 ?? 00 48 8B F2 48 8B D9 74 ?? 44 8B 02 44 3B 41 ?? 73 ?? 0F 1F", 0x12, 1, 0, false, VERIFY_CB{ return val >= 0 && val <= 0x60; });
        //m.buffbarBuffStatArr = OffsetScan("48 89 5C 24 ?? 48 8D 99 ?? ?? ?? ?? 48 83 7B ?? 00 89 7C 24 ?? 74", 0x8, 4, 0, false, VERIFY_CB{ return val >= 0 && val <= 0x300; });
        /* - these offsets don't change very frequently, fallback to offsets.h defaults
        m.buffEfType = OffsetScan("44 8B 50 ?? 48 8D 05 ?? ?? ?? ?? 48 89 01 44 89 51 ?? 4C 89 79", 0x11, 1, 0, false, VERIFY_CB{ return val >= 0 && val <= 0x40; });
        m.buffBuffId = OffsetScan("49 89 7E ?? 48 8B 06 49 89 46 ?? 8B 44 24 ?? 41 89 46 ?? 8B 44 24 ?? 41 89 46 ?? E8", 0x12, 1, 0, false, VERIFY_CB{ return val >= 0 && val <= 0x50; });
        m.buffBuffBar = OffsetScan("49 89 7E ?? 48 8B 06 49 89 46 ?? 8B 44 24 ?? 41 89 46 ?? 8B 44 24 ?? 41 89 46 ?? E8", 3, 1, 0, false, VERIFY_CB{ return val >= 0 && val <= 0x50; });
        m.buffSrcAg = OffsetScan("48 89 ?? 33 C0 48 89 41 ?? 48 89 41 ?? 48 89 41 ?? 48 89 41 ?? 48 83 C1 ?? E8", 0xc, 1, 0, false, VERIFY_CB{ return val >= 0 && val <= 0x60; });
        m.buffDuration = OffsetScan("41 89 46 ?? E8 ?? ?? ?? ?? 48 8B 5C 24 ?? 48 8B 74 24 ?? 48 8B 7C 24", 3, 1, 0, false, VERIFY_CB{ return val >= 0 && val <= 0x70; });
        */
        m.buffSkillDef = OffsetScan("!buffData->isActive", 0x13, 1, 0, true, VERIFY_CB{ return val >= 0 && val <= 0x40; });
        m.buffActive = OffsetScan("!buffData->isActive", -0x13, 1, 0, true, VERIFY_CB{ return val >= 0 && val <= 0x70; });
        m.skillbarSkillsArray = OffsetScan("skillbarSlot < arrsize(m_slotSkillDefs)", 0xd, 4, 0, true, VERIFY_CB{ return val > 0 && val <= 0x400; });
        m.skillbarPressedSkill = OffsetScan("skillbarSlot < CHAR_SKILLBAR_SLOTS", 0xd, 1, 0, true, VERIFY_CB{ return val >= 0 && val <= 0xc0; });
        m.skillbarHoveredSkill = OffsetScan("48 8B 02 48 3B 81 ?? ?? ?? ?? 74 21 48 89 81 ?? ?? ?? ?? 4C 8D 81 ?? ?? ?? ?? 48 81 C1", 6, 4, 0, false, VERIFY_CB{ return val >= 0 && val <= 0x300; });
        m.skillbarVtSkillDown = OffsetScan("45 8B CD 4C 8B C6 41 8B D7 48 8B C8 4C 8B 10 41 FF 92 ?? ?? ?? ?? 48 8B", 0x12, 4, 0, false, VERIFY_CB{ return val >= 0 && val <= 0x400; });
        m.skillbarVtSkillUp = OffsetScan("8B D6 48 8B C8 4C 8B 00 41 FF 90 ?? ?? ?? ?? 48 8B 5C", 0xb, 4, 0, false, VERIFY_CB{ return val >= 0 && val <= 0x400; });
        m.skillDefEffect = OffsetScan("48 8B 07 8B 48 ?? 8B C1 C1 E8 05 3B 43 ?? 73", 0x5, 1, 0, false, VERIFY_CB{ return val >= 0 && val <= 0x60; });
        m.skillDefInfo = OffsetScan("skillBuff", -0x1a, 1, 0, true, VERIFY_CB{ return val >= 0 && val <= 0x90; });
        m.healthVtGetBarrier = OffsetScan("49 8B ?? 49 8B CE FF 50 ?? E8 ?? ?? ?? ?? 8B 4E ?? BA 01 00 00 00 8B F8 E8", 0x8, 1, 0, false, VERIFY_CB{ return val >= 0 && val <= 0x4c; });
        m.invVtGetSupply = OffsetScan("48 8B C8 48 8B 10 FF 92 ?? ?? ?? ?? 8B 4F ?? 33 D2 8B E8 E8", 0x8, 4, 0, false, VERIFY_CB{ return val >= 0x60 && val <= 0x600; });
        m.invEquipArr = OffsetScan("m_equipmentSlots[equipSlot] == nullptr", -0x17, 4, 0, true, VERIFY_CB{ return val > 0x10 && val <= 0x400; });
        m.equipItemVtPrePost = OffsetScan("49 8B 00 49 8B C8 FF 90 ?? ?? ?? ?? 48 8B F0 48 85 C0", 0x8, 4, 0, false, VERIFY_CB{ return val >= 0x100 && val <= 0x600; });
        m.contextGadget = PointerScan("ViewAdvanceGadget", SHIFT_CB{ return *(uint32_t*)(hl::FollowRelativeAddress(o + 0xa) + 0xc); }, 0, true, VERIFY_CB{ return val >= 0x80 && val <= 0x400; });
        m.ctxgdVtGetGadget = OffsetScan("8B 53 02 48 8B 88 ?? ?? ?? ?? 48 8B 01 FF 50 ?? 48 8B F8", 0xf, 1, 0, false, VERIFY_CB{ return val >= 0 && val <= 0x40; });
        m.atkTgtVtGetGd = OffsetScan("attackTarget", 0x11, 1, 0, true, VERIFY_CB{ return val > 0 && val <= 0x40; });
        m.gdHealth = OffsetScan("IsCombatant() && m_healthMeter", -0x24, 4, 0, true, VERIFY_CB{ return val > 0 && val <= 0x400; });
        m.gdVtGetType = OffsetScan("gadget->GetType() == Content::GADGET_TYPE_RESOURCE_NODE", -0x19, 4, 0, true, VERIFY_CB{ return val > 0 && val <= 0x200; });
        m.gdVtGetRNode = OffsetScan("resourceNode", -0x1c, 4, 0, true, VERIFY_CB{ return val >= 0 && val <= 0x400; });
        m.uiFlags1 = OffsetScan("flag < UI_FLAGS", 0x16, 4, 1, true, VERIFY_CB{ return val >= 0 && val <= 0x400; });
        m.uiFlags2 = m.uiFlags1 + 4;
        m.uiFlags3 = m.uiFlags1 + 8;
        m.asctxAuto = OffsetScan("!agent || AgentCanBeSelection(agent)", 0xc, 1, 2, true, VERIFY_CB{ return val >= 0 && val <= 0xb0; }, false);
        m.asctxHover = OffsetScan("!agent || AgentCanBeSelection(agent)", 0xf, 4, 1, true, VERIFY_CB{ return val >= 0 && val <= 0x400; }, false);
        m.asctxLocked = OffsetScan("!agent || AgentCanBeSelection(agent)", 0x2e, 4, 3, true, VERIFY_CB{ return val >= 0 && val <= 0x500; }, false);
        m.asctxStoW = OffsetScan("41 0F 10 45 28 41 0F 11 84 24 ?? ?? ?? ?? 49 8B 4D 48 48 85 C9", 0xa, 4, 0, false, VERIFY_CB{ return val >= 0 && val <= 0x500; });
        m.asctxCtxMode = OffsetScan("No valid case for switch variable 'm_contextMode'", -0x12, 4, 1, true, VERIFY_CB{ return val >= 0 && val <= 0x400; });
    } catch (...) {
        return false;
    }

    return true;
}


