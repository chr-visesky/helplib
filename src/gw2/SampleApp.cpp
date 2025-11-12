#include "gw2lib.h"
#include "GameData.h"
#include <sstream>
#include <thread>
#include <chrono>
#include <codecvt>
#include <locale>

using namespace GW2LIB;

float memTotal = 0;
int64_t lastTime = 0;
std::stringstream memStr, memStr2;

float offsety = 0;
#define LINEHEIGHT 13
#define OFFSETY (offsety-=LINEHEIGHT)

std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
GW2LIB::Font font;
static const DWORD fontColor = 0xffffffff;
std::string chat;

std::string strProf[] = { "Error", "Guardian", "Warrior", "Engineer", "Ranger", "Thief", "Elementalist", "Mesmer", "Necromancer", "Revenant", "None" };
std::string strStance[] = { "None", "Fire", "Water", "Air", "Earth", "Unknown1", "Shroud", "Adrenaline 1", "Adrenaline 2", "Adrenaline 3", "Druid", "Astral", "Dragon", "Assassin", "Dwarf", "Demon", "TEST11", "Centaur", "TEST13", "TEST14", "Alliance", "TEST16", "TEST17" };
std::string charSex[] = { "Male", "Female", "None", "Error" };
std::string charRace[] = { "Asura", "Charr", "Human", "Norn", "Sylvari", "None", "Race 6" };
std::string charRank[] = { "Normal", "Ambient", "Veteran", "Elite", "Champion", "Legendary" };
std::string itemRarity[] = { "Junk", "Common", "Fine", "Masterwork", "Rare", "Exotic", "Ascended", "Legendary", "Rarity 8" };

float dist(GW2LIB::Vector3 p1, GW2LIB::Vector3 p2)
{
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2) + pow(p1.z - p2.z, 2));
}

int64_t GetSystemTime() {
    return std::chrono::system_clock::now().time_since_epoch().count() / 10000;
}


void cbESP()
{
    Agent agAuto = GetAutoSelection();
    Agent agHover = GetHoverSelection();
    Agent agLocked = GetLockedSelection();
    Character chrAuto = agAuto.GetCharacter();
    Character chrHover = agHover.GetCharacter();
    Character chrLocked = agLocked.GetCharacter();

    Vector3 camPos = GetCameraPosition();
    Vector3 camView = GetViewVector();

    int64_t currTime = GetSystemTime();
    if (currTime - lastTime > 500) { // every 500ms
        lastTime = currTime;
        memTotal = 0;
        Memory memCat;
        bool switchMemStr = 0;
        memStr.str("");
        memStr.clear();
        memStr2.str("");
        memStr2.clear();
        memStr.imbue(std::locale("en-US"));
        memStr2.imbue(std::locale("en-US"));

        while (memCat.BeNext()) {
            uint32_t memCount = memCat.GetCount();
            //if (!memCount) continue;

            uint32_t memSize = memCat.GetSize();
            /*size_t catid = memCat.GetCategory();
            std::string name = converter.to_bytes(memCat.GetName());

            if (catid == 127) switchMemStr++;*/
            memTotal += memSize;

            /*switch (switchMemStr) {
            case 0: memStr  << catid << " - " << name << " - " << memCount << " - " << memSize << "\n"; break;
            case 1: memStr2 << catid << " - " << name << " - " << memCount << " - " << memSize << "\n"; break;
            }*/
        }
    }

    font.Draw(150, 60, fontColor, "mem total (MB): %.2f", memTotal/1024/1024);
    //font.Draw(300, 40, fontColor, "%s", memStr.str().c_str());
    //font.Draw(600, 40, fontColor, "%s", memStr2.str().c_str());

    if (agLocked.IsValid()) {
        agLocked.LockCamera();
    }

    font.Draw(10, 25, fontColor, "%s", chat.c_str());
    font.Draw(10, 40, fontColor, "(ui opts) TGT_PCT: %i, DBL_TAP: %i, AOE_LOOT: %i",
        GetUiOptionFlag(GW2::UI_OPT_TGT_PCT),
        GetUiOptionFlag(GW2::UI_OPT_DBL_TAP),
        GetUiOptionFlag(GW2::UI_OPT_AOE_LOOT)
    );
    font.Draw(25, 75, fontColor, "MapId: %i - BuildId: %u", GetCurrentMapId(), GetBuildId());
    font.Draw(25, 100, fontColor, "(Cam) ActionCam: %i | Zoom: %.1f - %.1f : %.1f | (Pos) X: %.1f, Y: %.1f, Z: %.1f | (View) X: %.2f, Y: %.2f, Z: %.2f", 
        ActionCamOn(), GetCamMinZoom(), GetCamMaxZoom(), GetCamCurrentZoom(),
        camPos.x, camPos.y, camPos.z, camView.x, camView.y, camView.z);
    font.Draw(25, 125, fontColor, "Mouse: %.1f %.1f %.1f", GetMouseInWorld().x, GetMouseInWorld().y, GetMouseInWorld().z);
    if (agAuto.m_ptr)
        font.Draw(25, 150, fontColor, "AutoSelection: agptr %p chrptr %p", agAuto.m_ptr->pAgent, chrAuto.m_ptr ? chrAuto.m_ptr->pCharacter : nullptr);
    if (agHover.m_ptr)
        font.Draw(25, 175, fontColor, "HoverSelection: agptr %p chrptr %p", agHover.m_ptr->pAgent, chrHover.m_ptr ? chrHover.m_ptr->pCharacter : nullptr);
    if (agLocked.m_ptr)
        font.Draw(25, 200, fontColor, "LockedSelection: agptr %p chrptr %p", agLocked.m_ptr->pAgent, chrLocked.m_ptr ? chrLocked.m_ptr->pCharacter : nullptr);

    Character me = GetOwnCharacter();
    Vector3 mypos = me.GetAgent().GetPos();

    Agent ag;
    while (ag.BeNext())
    {
        Character chr = ag.GetCharacter();
        Player player = ag.GetPlayer();
        Vector3 pos = ag.GetPos();

        if (dist(mypos, pos) > 2000.0f)
            continue;

        offsety = 0;

        float x, y;
        if (WorldToScreen(pos, &x, &y)) {

            float circleSize = 10.0f;
            DWORD color = 0x66ffffff;

            if (chr.IsValid())
            {
                color = 0xcc000000;

                // lower opacity for non players
                if (!chr.IsPlayer())
                    color -= 0x66000000;

                // assign colors
                if (chr == GetOwnCharacter()) {
                    color |= 0x000000ff;
                } else {
                    switch (chr.GetAttitude()) {
                    case GW2::ATTITUDE_FRIENDLY:
                        color |= 0x0033ff00;
                        break;
                    case GW2::ATTITUDE_HOSTILE:
                        color |= 0x00ff3300;
                        break;
                    case GW2::ATTITUDE_INDIFFERENT:
                        color |= 0x00dddd00;
                        break;
                    case GW2::ATTITUDE_NEUTRAL:
                        color |= 0x00dddddd;
                        break;
                    }
                }

                circleSize = 20;
            }

            Vector3 rotArrow = {
                pos.x + cos(ag.GetRot()) * 50.0f,
                pos.y + sin(ag.GetRot()) * 50.0f,
                pos.z
            };

            DrawCircleProjected(pos, circleSize, color);
            DrawCircleFilledProjected(pos, circleSize, color);
            DrawLineProjected(pos, rotArrow, color);

            font.Draw(x, y, fontColor, "pos: %.1f %.1f %.1f", pos.x, pos.y, pos.z);
            font.Draw(x, y + OFFSETY, fontColor, "rot: %.5f", ag.GetRot());
            font.Draw(x, y + OFFSETY, fontColor, "speed: %.5f / %.5f - %.5f", ag.GetSpeed(), ag.GetMaxSpeed(), ag.GetRealSpeed());
            font.Draw(x, y + OFFSETY, fontColor, "agentId: %i / 0x%04X", ag.GetAgentId(), ag.GetAgentId());
            font.Draw(x, y + OFFSETY, fontColor, "category: %i, type: %i", ag.GetCategory(), ag.GetType());

            if (ag.IsValid()) {
                std::string name = ag.GetName();
                if (name.size()) font.Draw(x, y + OFFSETY, fontColor, "%s", name.c_str());
                font.Draw(x, y + OFFSETY, fontColor, "wmAgent: %p", ag.m_ptr->wmAgent);
                font.Draw(x, y + OFFSETY, fontColor, "agentptr: %p", ag.m_ptr->pAgent);
                font.Draw(x, y + OFFSETY, fontColor, "selectable: %i", ag.IsSelectable());

                if (ag.GetType() == GW2::AGENT_TYPE_GADGET) {
                    Gadget gd = ag.GetGadget();
                    if (gd.IsValid()) {
                        font.Draw(x, y + OFFSETY, fontColor, "gadget: %p - type: %i", gd.m_ptr->pGadget, gd.GetType());
                        font.Draw(x, y + OFFSETY, fontColor, "gadget hp: %.0f / %.0f", gd.GetCurrentHealth(), gd.GetMaxHealth());
                        font.Draw(x, y + OFFSETY, fontColor, "wvw teamId: %i", gd.GetWvwTeamId());

                        if (gd.GetType() == GW2::GADGET_TYPE_RESOURCE_NODE) {
                            ResourceNode node = gd.GetResourceNode();
                            if (node.IsValid()) {
                                font.Draw(x, y + OFFSETY, fontColor, "resource: %p - type: %i", node.m_ptr->pResourceNode, node.GetType());
                                font.Draw(x, y + OFFSETY, fontColor, "rnode depleted: %i", !node.IsGatherable());

                            }
                        }
                    }
                }

                if (ag.GetType() == GW2::AGENT_TYPE_GADGET_ATTACK_TARGET) {
                    AttackTarget tgt = ag.GetAttackTarget();
                    if (tgt.IsValid()) {
                        font.Draw(x, y + OFFSETY, fontColor, "atk tgt: %p", tgt.m_ptr->pAttackTgt);
                        font.Draw(x, y + OFFSETY, fontColor, "atk hp: %.0f / %.0f", tgt.GetCurrentHealth(), tgt.GetMaxHealth());
                    }
                }

                if (ag.GetCategory() == GW2::AGENT_CATEGORY_KEYFRAMED) {
                    font.Draw(x, y + OFFSETY, fontColor, "seq: 0x%llX", ag.GetSequence());
                    font.Draw(x, y + OFFSETY, fontColor, "token: 0x%llX", ag.GetToken());
                }
            }

            if (chr.IsValid()) {
                font.Draw(x, y + OFFSETY, fontColor, "%i %i %i %i %i %i %i %i",
                    chr.IsAlive(),
                    chr.IsControlled(),
                    chr.IsDowned(),
                    chr.IsInWater(),
                    chr.IsMonster(),
                    chr.IsClone(),
                    chr.IsPlayer(),
                    chr.IsRangerPet()
                );
                font.Draw(x, y + OFFSETY, fontColor, "A C D W M L P R");
                font.Draw(x, y + OFFSETY, fontColor, "char rank: %s - 0x%08X", charRank[chr.GetRank()].c_str(), chr.m_ptr->asFlags);
                font.Draw(x, y + OFFSETY, fontColor, "char hp: %.0f / %.0f", chr.GetCurrentHealth(), chr.GetMaxHealth());
                font.Draw(x, y + OFFSETY, fontColor, "barrier: %.0f", chr.GetCurrentBarrier());
                font.Draw(x, y + OFFSETY, fontColor, "in combat: %i", chr.IsInCombat());
                font.Draw(x, y + OFFSETY, fontColor, "weap drawn: %i", chr.IsWeaponDrawn());
                font.Draw(x, y + OFFSETY, fontColor, "%s %s %s", charRace[chr.GetRace()].c_str(), charSex[chr.GetGender()].c_str(), strProf[chr.GetProfession()].c_str());
                font.Draw(x, y + OFFSETY, fontColor, "char ID: %i - ptr: %p", chr.GetID(), chr.m_ptr->pCharacter);
                font.Draw(x, y + OFFSETY, fontColor, "inv: %p", chr.m_ptr->pInventory);
                font.Draw(x, y + OFFSETY, fontColor, "buff bar: %p", chr.m_ptr->pBuffBar);
                font.Draw(x, y + OFFSETY, fontColor, "skill bar: %p", chr.m_ptr->pSkillbar);
                font.Draw(x, y + OFFSETY, fontColor, "wvw supply: %i", chr.GetWvwSupply());

                font.Draw(x, y + OFFSETY, fontColor, "gliding: %i - %0.2f", chr.GetBuffStackCount(GW2::EFFECT_GLIDING), chr.GetGliderPercent());
                /*font.Draw(x, y + OFFSETY, fontColor, "might: %0.1f", chr.GetBuffTimeLeft(GW2::EFFECT_MIGHT) / 1000.0f);
                font.Draw(x, y + OFFSETY, fontColor, "portal: %0.1f", chr.GetBuffTimeLeft(GW2::EFFECT_PORTAL) / 1000.0f);
                font.Draw(x, y + OFFSETY, fontColor, "alacrity: %0.1f", chr.GetBuffTimeLeft(GW2::EFFECT_ALACRITY) / 1000.0f);
                font.Draw(x, y + OFFSETY, fontColor, "swiftness: %0.1f", chr.GetBuffTimeLeft(GW2::EFFECT_SWIFTNESS) / 1000.0f);

                Buff buff = chr.GetBuffs();
                while (buff.BeNext()) {
                    Agent agSrc = buff.GetSource();
                    int32_t duration = buff.GetDuration();
                    bool bDur = (duration && duration != -1);
                    font.Draw(x, y + OFFSETY, fontColor, "dur: %i - id: 0x%04x - type: %i - effect: %i - source: %s", duration, buff.m_ptr->id, buff.m_ptr->stackType, buff.GetEffectType(), agSrc.GetName().c_str());
                }

                EquipItem eItem = chr.GetEquipItems();
                while (eItem.BeNext()) {
                    Item item = eItem.GetItem();
                    font.Draw(x, y + OFFSETY, fontColor, "(equipment) ptr: %p - slot: %2i - item_id: %i - rarity: %s - name: %s",
                        eItem.m_ptr->pEItem, eItem.GetEquipSlot() , item.GetID(), itemRarity[item.GetRarity()].c_str(), item.GetName().c_str());
                }*/
            }

            if (player.IsValid()) {
                if (chr.GetStance()) font.Draw(x, y + OFFSETY, fontColor, "stance: %s", strStance[chr.GetStance()].c_str());
                font.Draw(x, y + OFFSETY, fontColor, "energy: %.1f / %.1f", chr.GetCurrentEnergy(), chr.GetMaxEnergy());
                font.Draw(x, y + OFFSETY, fontColor, "level: %i (actual: %i)", chr.GetScaledLevel(), chr.GetLevel());
                font.Draw(x, y + OFFSETY, fontColor, "playerPtr: %p", player.m_ptr->pPlayer);
                font.Draw(x, y + OFFSETY, fontColor, "UUID: %s", player.GetUUID2Str().c_str());
                font.Draw(x, y + OFFSETY, fontColor, "coins: %i", player.GetCurrency(GW2::CURRENCY_COIN));
                font.Draw(x, y + OFFSETY, fontColor, "mastery: %p - %i", player.m_ptr->pTrainMgr, player.GetMasteryLevel());
                font.Draw(x, y + OFFSETY, fontColor, "ach: %p - %i", player.m_ptr->pAchMgr, player.GetAP());

                std::stringstream ss, ts;
                for (int s = 0; s < GW2::SPEC_SLOT_END; s++) {
                    ss << player.GetSpecType((GW2::SpecSlot)s) << ", ";
                    ts << "line" << (s+1) << ": ";

                    for (int t = 0; t < GW2::TRAIT_SLOT_END; t++) {
                        ts << player.GetTraitType((GW2::SpecSlot)s, (GW2::TraitSlot)t) << ", ";
                    }

                    ts << "\n";
                }

                font.Draw(x, y + OFFSETY, fontColor, "spec: %p - %s", player.m_ptr->pSpecMgr, ss.str().c_str());
                font.Draw(x, y + (offsety -= (LINEHEIGHT*GW2::SPEC_SLOT_END+GW2::SPEC_SLOT_END)), fontColor, "%s", ts.str().c_str());
            }
        }
    }
}

void sample_chat_cb(wchar_t *wtxt) {
    chat = converter.to_bytes(wtxt);
}

void sample_combat_log_cb(Agent &agSrc, Agent &agTgt, int hit, GW2::CombatLogType type, GW2::EffectType ef) {
    HL_LOG_DBG("src: %4i, tgt: %4i, type: %2i, ef: %5i, hit: %i\n",
        agSrc.GetAgentId(), agTgt.GetAgentId(), type, ef, hit);
}

void sample_log_cb(char *txt) {
    HL_LOG_DBG("log: %s\n", txt);
}

void sample_dmg_log_cb(Agent &src, Agent &tgt, int hit, GW2::EffectType ef) {
    HL_LOG_DBG("src: %4i, tgt: %4i, ef: %5i, hit: %i\n", src.GetAgentId(), tgt.GetAgentId(), ef, hit);
}

GW2::ScreenshotMode sample_screenshot_cb(GW2::ScreenshotMode mode) {
    return GW2::SCREENSHOT_HIGHRES_NOUI;
}

bool ag_can_be_sel_cb(bool &call_orig, Agent &ag) {
    /*if (ag.GetAgentId() == GetOwnAgent().GetAgentId()) {
        return true; // select self :)
    }*/
    if (ag.GetCategory() == GW2::AGENT_CATEGORY_KEYFRAMED) {
        return true;
    }

    call_orig = true;
    return true; // ignored (prevents compiler warning)
}

void sample_alloc_cb(int type, size_t size, int cat, int line, char *file) {
    HL_LOG_DBG("type: %1i, size: %5i, cat: %3i, line: %4i, file: %s\n", type, size, cat, line, file);
}


// true  = will pass message back to game
// false = will prevent message from being passed back to game (hacklib_gw2 will set msg to WM_NULL, this effectively suppresses the msg)
bool sample_wnd_proc_cb(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_KEYDOWN:
        HL_LOG_DBG("keyboard: 0x%x\n", wParam);
        switch (wParam) {
        case VK_OEM_MINUS: AddDrunkLevel(-100); break;
        case VK_OEM_PLUS: AddDrunkLevel(100); break;
        }
        break;
    }

    return true;
}

void GameHook() {

}

void GW2LIB::gw2lib_main()
{
    float minZoom = GetCamMinZoom();
    float maxZoom = GetCamMaxZoom();
    SetCamMinZoom(60);
    SetCamMaxZoom(2000);

    EnableEsp(cbESP);
    EnableGameHook(GameHook);
    if (!font.Init(14, "Consolas", false))
    {
        //DbgOut("could not create font");
        return;
    }

    SetGameHook(HOOK_CHAT, sample_chat_cb);
    //SetGameHook(HOOK_COMBAT_LOG, sample_combat_log_cb);
    //SetGameHook(HOOK_LOGGER, sample_log_cb);
    //SetGameHook(HOOK_DAMAGE_LOG, sample_dmg_log_cb);
    //SetGameHook(HOOK_AG_CAN_BE_SEL, ag_can_be_sel_cb);
    //SetGameHook(HOOK_SCREENSHOT, sample_screenshot_cb);
    //SetGameHook(HOOK_ALLOCATOR, sample_alloc_cb);
    //SetGameHook(HOOK_WINDOW_PROC, sample_wnd_proc_cb);

    while (GetAsyncKeyState(VK_F12) >= 0)
        std::this_thread::sleep_for(std::chrono::milliseconds(25));

    if(minZoom) SetCamMinZoom(minZoom);
    if(maxZoom) SetCamMaxZoom(maxZoom);
}