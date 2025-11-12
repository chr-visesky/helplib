#include "main.h"
#include "hacklib/ForeignClass.h"
#include "hacklib/Logging.h"

void Gw2HackMain::RefreshDataAgent(GameData::AgentData *pAgentData, hl::ForeignClass agent) {
    __try {
        pAgentData->pAgent = agent;
        TRACER("RefreshDataAgent: 1\n");
        pAgentData->category = agent.call<GW2LIB::GW2::AgentCategory>(m_pubmems.agentVtGetCategory); TRACER("RefreshDataAgent: 2\n");
        pAgentData->type = agent.get<GW2LIB::GW2::AgentType>(m_pubmems.agentType); TRACER("RefreshDataAgent: 3\n");
        pAgentData->agentId = agent.get<uint32_t>(m_pubmems.agentId); TRACER("RefreshDataAgent: 4\n");
        agent.call<void>(m_pubmems.agentVtGetPos, &pAgentData->pos); TRACER("RefreshDataAgent: 5\n");

        if (!pAgentData->nameResolved || !pAgentData->name.size()) {
            TRACER("RefreshDataAgent: 6\n");
            pAgentData->nameResolved = true;
            uintptr_t *codedName = (uintptr_t*)pAgentData->wmAgent.call<uintptr_t*>(m_pubmems.wmAgVtGetCodedName); TRACER("RefreshDataAgent: 7\n");
            if (codedName) DecodeText(codedName, &pAgentData->name); TRACER("RefreshDataAgent: 8\n");
        }
        TRACER("RefreshDataAgent: 9\n");
        GW2LIB::Vector4 rot;
        agent.call<uintptr_t>(m_pubmems.agentVtGetRot, &rot); TRACER("RefreshDataAgent: 10\n");
        pAgentData->rot = atan2(rot.y, rot.x);
        TRACER("RefreshDataAgent: 11\n");
        hl::ForeignClass transform = agent.get<void*>(m_pubmems.agentTransform); TRACER("RefreshDataAgent: 12\n");
        if (transform) {
            TRACER("RefreshDataAgent: 13\n");
            if (pAgentData->category == GW2LIB::GW2::AGENT_CATEGORY_KEYFRAMED) {
                TRACER("RefreshDataAgent: 14\n");
                pAgentData->token = agent.call<uint64_t>(m_pubmems.gd_agentVtGetToken); TRACER("RefreshDataAgent: 15\n");
                agent.call<void*>(m_pubmems.gd_agentVtGetSeq, &pAgentData->seq); TRACER("RefreshDataAgent: 16\n");
            }TRACER("RefreshDataAgent: 17\n");
        }
        TRACER("RefreshDataAgent: 18\n");
    } __except (HLGW2_EXCEPTION("[RefreshDataAgent] access violation")) {
        ;
    }
}
void Gw2HackMain::RefreshDataCharacter(GameData::CharacterData *pCharData, hl::ForeignClass character) {
    __try {
        hl::ForeignClass subChar = character + m_pubmems.charSubClass;
        pCharData->pCharacter = character;

        pCharData->isAlive = subChar.call<bool>(m_pubmems.charVtAlive);
        pCharData->isDowned = character.call<bool>(m_pubmems.charVtDowned);
        pCharData->isControlled = character.call<bool>(m_pubmems.charVtControlled);
        pCharData->isPlayer = subChar.call<bool>(m_pubmems.charVtPlayer);
        pCharData->isInWater = character.call<bool>(m_pubmems.charVtInWater);
        pCharData->isMonster = character.call<bool>(m_pubmems.charVtMonster);
        pCharData->isClone = character.call<bool>(m_pubmems.charVtClone);
        pCharData->isRangerPet = character.call<bool>(m_pubmems.charVtRangerPet);

        pCharData->attitude = character.get<GW2LIB::GW2::Attitude>(m_pubmems.charAttitude);
        pCharData->gliderPercent = character.get<float>(m_pubmems.charGliderPercent);

        uint32_t flags = character.get<uint32_t>(m_pubmems.charFlags);
        pCharData->isInCombat = !!(flags & GameData::CHAR_FLAG_IN_COMBAT);
        pCharData->isWeapDrawn = !!(flags & GameData::CHAR_FLAG_WEAP_DRAWN);

        pCharData->asFlags = character.call<uint32_t>(m_pubmems.charVtAsFlags);
        pCharData->id = character.get<uint16_t>(m_pubmems.charID);

        hl::ForeignClass health = character.get<void*>(m_pubmems.charHealth);
        if (health) {
            pCharData->currentHealth = health.get<float>(m_pubmems.healthCurrent);
            pCharData->maxHealth = health.get<float>(m_pubmems.healthMax);
            pCharData->currBarrier = health.call<float>(m_pubmems.healthVtGetBarrier);
        }

        hl::ForeignClass endurance = character.get<void*>(m_pubmems.charEndurance);
        if (endurance) {
            pCharData->currentEndurance = static_cast<float>(endurance.get<int>(m_pubmems.endCurrent));
            pCharData->maxEndurance = static_cast<float>(endurance.get<int>(m_pubmems.endMax));
        }

        hl::ForeignClass corestats = character.get<void*>(m_pubmems.charCoreStats);
        if (corestats) {
            pCharData->profession = corestats.get<GW2LIB::GW2::Profession>(m_pubmems.statsProfession);
            pCharData->level = corestats.get<int>(m_pubmems.statsLevel);
            pCharData->scaledLevel = corestats.get<int>(m_pubmems.statsScaledLevel);
            pCharData->gender = (GW2LIB::GW2::CharacterGender)corestats.get<BYTE>(m_pubmems.statsGender);
            pCharData->race = (GW2LIB::GW2::Race)corestats.get<BYTE>(m_pubmems.statsRace);
            pCharData->stats = corestats.get<GW2LIB::GW2::CharacterStats>(m_pubmems.statsStats);
        }
        TRACER("RefreshDataCharacter: 1\n");
        hl::ForeignClass inventory = character.call<void*>(m_pubmems.charVtGetInv); TRACER("RefreshDataCharacter: 2\n");
        if (inventory) {
            TRACER("RefreshDataCharacter: 3\n");
            pCharData->pInventory = inventory;
            pCharData->wvwsupply = inventory.call<int>(m_pubmems.invVtGetSupply);
            TRACER("RefreshDataCharacter: 4\n");
            for (size_t i = 0; i < GW2LIB::GW2::EQUIP_SLOT_END; i++) {
                hl::ForeignClass equip = inventory.get<void*>(m_pubmems.invEquipArr + i * sizeof(void*));
                if (equip) {
                    if (!pCharData->equipItemDataList[i]) {
                        [pCharData, i]() -> void {
                            pCharData->equipItemDataList[i] = std::make_unique<GameData::EquipItemData>();
                        } ();
                    }

                    GameData::EquipItemData *data = pCharData->equipItemDataList[i].get();

                    data->equipSlot = (GW2LIB::GW2::EquipmentSlot)i;
                    data->pCharData = pCharData;

                    RefreshDataEquipItem(data, equip);
                } else {
                    pCharData->equipItemDataList[i] = nullptr;
                }
            }
        }
        TRACER("RefreshDataCharacter: 5\n");

        hl::ForeignClass skillbar = character.get<void*>(m_pubmems.charSkillbar);
        if (skillbar) {
            pCharData->pSkillbar = skillbar;
        }


        hl::ForeignClass breakbar = character.get<void*>(m_pubmems.charBreakbar);
        if (breakbar) {
            pCharData->breakbarState = breakbar.get<GW2LIB::GW2::BreakbarState>(m_pubmems.breakbarState);
            pCharData->breakbarPercent = breakbar.get<float>(m_pubmems.breakbarPercent);
        } else {
            pCharData->breakbarState = GW2LIB::GW2::BREAKBAR_STATE_NONE;
            pCharData->breakbarPercent = 0;
        }

        hl::ForeignClass combatant = character.call<void*>(m_pubmems.charVtGetCmbtnt);
        if (combatant) {
            pCharData->pCombatant = combatant;
            hl::ForeignClass buffBar = combatant.call<void*>(m_pubmems.cmbtntVtGetBuffBar);
            if (buffBar) {
                pCharData->pBuffBar = buffBar;
                auto buffs = buffBar.get<GameData::ANet::Collection<GameData::BuffEntry, false>>(m_pubmems.buffbarBuffArr);

                if (buffs.IsValid()) {
                    uint32_t sizeBuffsArray = buffs.Capacity();

                    for (uint32_t i = 0; i < sizeBuffsArray; i++) {
                        GameData::BuffEntry be = buffs[i];
                        hl::ForeignClass pBuff = be.pBuff;
                        size_t buffId = be.buffId;

                        if (pBuff) {
                            bool is_new = false;
                            // new buff, add to array
                            if (!pCharData->buffDataList[i] || pCharData->buffDataList[i]->pBuff != pBuff) {
                                [pCharData, i]() -> void {
                                    pCharData->buffDataList[i] = std::make_unique<GameData::BuffData>();
                                } ();
                                is_new = true;
                            }

                            GameData::BuffData *pBuffData = pCharData->buffDataList[i].get();
                            RefreshDataBuff(pBuffData, pBuff);
                            pBuffData->pCharData = pCharData;

                            if (is_new) pCharData->AddBuff(pBuffData);
                        }

                        // remove invalid buffs from our array
                        if (pCharData->buffDataList[i] && !pBuff) {
                            GameData::BuffData *b = pCharData->buffDataList[i].get();
                            pCharData->RemoveBuff(b);

                            b->pSrcAgData = nullptr;
                            pCharData->buffDataList[i] = nullptr;
                        }
                    }

                }
            }
        }

        if (pCharData->isPlayer) {
            hl::ForeignClass player = character.call<void*>(m_pubmems.charVtGetPlayer);
            if (player) {
                pCharData->pPlayer = player;
                hl::ForeignClass prof = character.get<void*>(m_pubmems.charProfession);
                if (prof && pCharData->profession != GW2LIB::GW2::PROFESSION_ENGINEER) {
                    bool toInt =
                        pCharData->profession == GW2LIB::GW2::PROFESSION_MESMER ||
                        pCharData->profession == GW2LIB::GW2::PROFESSION_WARRIOR ||
                        pCharData->profession == GW2LIB::GW2::PROFESSION_ELEMENTALIST;

                    pCharData->stance = prof.get<GW2LIB::GW2::ProfessionStance>(m_pubmems.profStance);
                    pCharData->currentEnergy = toInt ? prof.get<int>(m_pubmems.profEnergy) : prof.get<float>(m_pubmems.profEnergy);
                    pCharData->maxEnergy = toInt ? prof.get<int>(m_pubmems.profEnergyMax) : prof.get<float>(m_pubmems.profEnergyMax);
                }
            }
        }
    } __except (HLGW2_EXCEPTION("[RefreshDataCharacter] access violation")) {
        ;
    }
}


void Gw2HackMain::RefreshDataEquipItem(GameData::EquipItemData *pItemData, hl::ForeignClass item) {
    __try {
        if (pItemData->pEItem != item) pItemData->ResetData();
        pItemData->pEItem = item;

        hl::ForeignClass itemDef = item.get<void*>(m_pubmems.equipItemDef);
        if (itemDef) {
            pItemData->itemDef.skinName = item.call<uintptr_t*>(m_pubmems.equipItemVtSkinName);

            hl::ForeignClass prepost = item.call<void*>(m_pubmems.equipItemVtPrePost);
            if (prepost) {
                pItemData->itemDef.prefixName = prepost.call<uintptr_t*>(m_pubmems.itemPrePostVtGetPrefix);

                hl::ForeignClass suffix = prepost.call<uintptr_t*>(m_pubmems.itemPrePostVtGetSuffix, 1);
                if (suffix) {
                    pItemData->itemDef.suffixName = suffix.get<uintptr_t*>(m_pubmems.itemSuffixName);
                }
            }

            RefreshDataItem(&pItemData->itemDef, itemDef);
        }
    } __except (HLGW2_EXCEPTION("[RefreshDataEquipItem] access violation")) {
        ;
    }
}

void Gw2HackMain::RefreshDataItem(GameData::ItemData* pItemData, hl::ForeignClass item) {
    __try {
        pItemData->pItemDef = item;
        pItemData->id = item.get<uint32_t>(m_pubmems.itemDefId);
        pItemData->rarity = item.get<GW2LIB::GW2::ItemRarity>(m_pubmems.itemDefRarity);
        pItemData->type = item.get<uint32_t>(m_pubmems.itemDefType);
        pItemData->reqLvl = item.get<uint32_t>(m_pubmems.itemDefReqLvl);
        pItemData->sellValue = item.get<uint32_t>(m_pubmems.itemDefSelVal);

        uint32_t type = pItemData->type;
        if (!pItemData->nameResolved || !pItemData->name.size()) {
            pItemData->nameResolved = true;
            uint32_t hashId = item.get<uint32_t>(m_pubmems.itemDefHashId);
            if (hashId) {
                void* codedName = GetCodedText(hashId, 0);
                if (codedName) DecodeText(codedName, &pItemData->name);
            } else {
                //void* codedName2 = GetCodedItemName(item.data(), pItemData->skinName, pItemData->prefixName, pItemData->suffixName, 1, (uintptr_t)0, 0);
                void* codedName2 = GetCodedItemName(item.data(), (uintptr_t)0, pItemData->prefixName, pItemData->suffixName, 1, (uintptr_t)0, 0);
                if (codedName2) DecodeText(codedName2, &pItemData->name, true);
            }
        }

    } __except (HLGW2_EXCEPTION("[RefreshDataItem] access violation")) {
        ;
    }
}

void Gw2HackMain::RefreshDataBuff(GameData::BuffData *pBuffData, hl::ForeignClass buff) {
    __try {
        pBuffData->pBuff = buff;
        pBuffData->id = buff.get<uint32_t>(m_pubmems.buffBuffId);
        pBuffData->effectType = buff.get<GW2LIB::GW2::EffectType>(m_pubmems.buffEfType);
        pBuffData->duration = buff.get<int32_t>(m_pubmems.buffDuration);

        hl::ForeignClass srcAg = buff.get<void*>(m_pubmems.buffSrcAg);
        if (srcAg) {
            auto srcAgId = srcAg.get<uint32_t>(m_pubmems.agentId);
            pBuffData->pSrcAgData = GameData::GetAgentDataById(srcAgId);
        } else {
            pBuffData->pSrcAgData = nullptr;
        }

        int64_t ts = GetTimestamp();
        if (!pBuffData->applyTime)
            pBuffData->applyTime = ts;

        hl::ForeignClass pSkill = buff.get<void*>(m_pubmems.buffSkillDef);
        if (pSkill) {
            hl::ForeignClass pSkillInfo = pSkill.get<void*>(m_pubmems.skillDefInfo);
            if (pSkillInfo) {
                pBuffData->stackType = pSkillInfo.get<GW2LIB::GW2::BuffStackType>(m_pubmems.skillStackType);
            }
        }
    } __except (HLGW2_EXCEPTION("[RefreshDataBuff] access violation")) {
        ;
    }
}

void Gw2HackMain::RefreshDataPlayer(GameData::PlayerData *pPlayerData, hl::ForeignClass player) {
    using namespace GW2LIB::GW2;
    __try {

        pPlayerData->pPlayer = player;

        hl::ForeignClass wallet = player.call<void*>(m_pubmems.playerVtGetWallet);
        if (wallet) {
            pPlayerData->pWallet = wallet;
        }

        hl::ForeignClass trainMgr = player.call<void*>(m_pubmems.playerVtGetTrainMgr);
        if (trainMgr) {
            pPlayerData->pTrainMgr = trainMgr;
            pPlayerData->masteryLvl = trainMgr.call<int>(m_pubmems.trmgrVtGetMLvl);
        }

        hl::ForeignClass achMgr = player.call<void*>(m_pubmems.playerVtGetAchMgr);
        if (achMgr) {
            pPlayerData->pAchMgr = achMgr;
            pPlayerData->ap = achMgr.call<int>(m_pubmems.achMgrVtGetAP);
        }

        hl::ForeignClass specMgr = player.call<void*>(m_pubmems.playerVtGetSpecMgr);
        if (specMgr) {
            pPlayerData->pSpecMgr = specMgr;

            for (size_t i = 0; i < SPEC_SLOT_END; i++) {
                hl::ForeignClass spec = specMgr.get<void*>(m_pubmems.specMgrSpecsArr + (i * sizeof(void*)));
                if (spec) {
                    pPlayerData->specs[i] = spec.get<Specialization>(m_pubmems.specType);
                } else {
                    pPlayerData->specs[i] = SPEC_NONE;
                }

                for (size_t j = 0; j < TRAIT_SLOT_END; j++) {
                    hl::ForeignClass trait = specMgr.get<void*>(m_pubmems.specMgrTraitsArr + (i * TRAIT_SLOT_END + j) * sizeof(void*));
                    if (trait) {
                        pPlayerData->traits[i][j] = trait.get<Trait>(m_pubmems.traitType);
                    } else {
                        pPlayerData->traits[i][j] = TRAIT_NONE;
                    }
                }
            }


        }

        wchar_t *name2 = player.get<wchar_t*>(m_pubmems.playerName);
        if (name2) {
            int num = WideCharToMultiByte(CP_ACP, 0, name2, -1, NULL, 0, NULL, 0);
            char *name = new char[num + 1];
            WideCharToMultiByte(CP_ACP, 0, name2, -1, name, num, NULL, 0);
            int i = 0;
            pPlayerData->name = "";
            while (name[i]) {
                pPlayerData->name += name[i];
                i++;
            }
            delete[] name;
        }

        pPlayerData->uuid = *player.call<UUID*>(m_pubmems.playerVtGetUUID);

    } __except (HLGW2_EXCEPTION("[RefreshDataPlayer] access violation")) {
        ;
    }
}


void Gw2HackMain::RefreshDataCompass(GameData::CompassData *pCompData, hl::ForeignClass comp) {
    __try {
        pCompData->pComp = comp;

        pCompData->width = comp.get<float>(m_pubmems.compWidth);
        pCompData->height = comp.get<float>(m_pubmems.compHeight);
        pCompData->zoom = (int)comp.get<float>(m_pubmems.compZoom);
        pCompData->maxWidth = comp.get<float>(m_pubmems.compMaxWidth);
        pCompData->maxHeight = comp.get<float>(m_pubmems.compMaxHeight);

        hl::ForeignClass mumble = m_mems.pMumble;
        pCompData->absCharLeft = mumble.get<float>(0x328);
        pCompData->absCharTop  = mumble.get<float>(0x32C);
        pCompData->absMapLeft  = mumble.get<float>(0x330);
        pCompData->absMapTop   = mumble.get<float>(0x334);
        pCompData->mapScale    = mumble.get<float>(0x338);

        DWORD flags = comp.get<DWORD>(m_pubmems.compFlags);
        pCompData->flags.rotation = !!(flags & GameData::COMP_ROTATION);
        pCompData->flags.position = !!(flags & GameData::COMP_POSITION);
        pCompData->flags.mouseOver = !!(flags & GameData::COMP_MOUSE_OVER);
    } __except (HLGW2_EXCEPTION("[RefreshDataCompass] access violation")) {
        ;
    }
}

void Gw2HackMain::RefreshDataGadget(GameData::GadgetData *pGadgetData, hl::ForeignClass gd) {
    __try {
        pGadgetData->pGadget = gd;

        pGadgetData->type = gd.call<GW2LIB::GW2::GadgetType>(m_pubmems.gdVtGetType);
        pGadgetData->wvwTeamId = gd.get<int>(m_pubmems.gdWvwTeamId);

        hl::ForeignClass health = gd.get<void*>(m_pubmems.gdHealth);
        if (health) {
            pGadgetData->currentHealth = health.get<float>(m_pubmems.healthCurrent);
            pGadgetData->maxHealth = health.get<float>(m_pubmems.healthMax);
        }
    } __except (HLGW2_EXCEPTION("[RefreshDataGadget] access violation")) {
        ;
    }
}

void Gw2HackMain::RefreshDataAttackTarget(GameData::AttackTargetData *pAtkTgtData, hl::ForeignClass atk) {
    __try {
        hl::ForeignClass gd = atk.call<void*>(m_pubmems.atkTgtVtGetGd); // bridge back to gadget
        pAtkTgtData->pAttackTgt = atk;
        hl::ForeignClass health = gd.get<void*>(m_pubmems.gdHealth);
        if (health) {
            pAtkTgtData->currentHealth = health.get<float>(m_pubmems.healthCurrent);
            pAtkTgtData->maxHealth = health.get<float>(m_pubmems.healthMax);
        }
    } __except (HLGW2_EXCEPTION("[RefreshDataAttackTarget] access violation")) {
        ;
    }
}

void Gw2HackMain::RefreshDataResourceNode(GameData::ResourceNodeData *pRNodeData, hl::ForeignClass node) {
    __try {
        pRNodeData->pResourceNode = node; TRACER("RefreshDataResourceNode: 1\n");
        pRNodeData->type = node.get<GW2LIB::GW2::ResourceNodeType>(m_pubmems.rnodeType); TRACER("RefreshDataResourceNode: 2\n");

        BYTE flags = node.get<BYTE>(m_pubmems.rnodeFlags); TRACER("RefreshDataResourceNode: 3\n");
        pRNodeData->flags.depleted = !(flags & GameData::RNODE_FLAG_DEPLETED); TRACER("RefreshDataResourceNode: 4\n");
    } __except (HLGW2_EXCEPTION("[RefreshDataResourceNode] access violation")) {
        ;
    }
}
