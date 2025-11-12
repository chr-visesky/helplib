
#include "hacklib/PatternScanner.h"
#include "hacklib/Logging.h"
#include "hacklib/Hooker.h"

#include <codecvt>
#include <locale>

#define ASSIGN_HOOK(h, c) if (typeid(h) == typeid(c)) h = decltype(h)((uintptr_t)c);

namespace GW2LIB {
    class Agent;
    class Character;
    class Player;
    class Compass;
    class Gadget;
    class AttackTarget;
    class ResourceNode;
};


struct Gw2Hooks {
    bool(*AgCanBeSel)(bool&, GW2LIB::Agent &ag) = nullptr;
    void(*ChatHook)(wchar_t*) = nullptr;
    void(*DmgLogHook)(GW2LIB::Agent &src, GW2LIB::Agent &tgt, int hit, GW2LIB::GW2::EffectType) = nullptr;
    void(*CombatLogHook)(GW2LIB::Agent &src , GW2LIB::Agent &tgt, int hit, GW2LIB::GW2::CombatLogType, GW2LIB::GW2::EffectType) = nullptr;
    void(*AllocatorHook)(int, size_t, int, int, char*) = nullptr;
    void(*LoggerHook)(char*) = nullptr;
    GW2LIB::GW2::ScreenshotMode(*ScreenshotHook)(GW2LIB::GW2::ScreenshotMode) = nullptr;
    bool(*WndProcHook)(HWND, UINT, WPARAM, LPARAM) = nullptr;
    void(*PacketSendHook)(uint32_t size, uintptr_t *packet) = nullptr;
    void(*PacketRecvHook)(uint32_t size, uintptr_t *packet) = nullptr;
};



class Gw2GameHook {
public:
    bool init_hooks();
    void cleanup();

    const hl::IHook *m_hkProcessText = nullptr;
    const hl::IHook *m_hkDmgLog = nullptr;
    const hl::IHook *m_hkCombatLog = nullptr;
    const hl::IHook *m_hkAllocator = nullptr;
    const hl::IHook *m_hkLogger = nullptr;
    const hl::IHook *m_hkLogger2 = nullptr;
    const hl::IHook *m_hkFrTxt = nullptr;
    const hl::IHook *m_hkMsgConn = nullptr;
    const hl::IHook *m_hkCanBeSel = nullptr;
    const hl::IHook *m_hkScreenshot = nullptr;
    const hl::IHook *m_hkWndProc = nullptr;
    const hl::IHook* m_hkWndProcDx11 = nullptr;
    const hl::IHook *m_hkMsgSend = nullptr;
    const hl::IHook *m_hkMsgRecv = nullptr;
    const hl::IHook* m_hkMntSock = nullptr;
    const hl::IHook* m_hkMoveLock = nullptr;

    Gw2Hooks m_hookList;
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;

private:
    hl::Hooker m_hooker;
};


Gw2GameHook* get_hook();
Gw2Hooks* get_hook_list();

namespace GW2LIB {
    template<typename T>
    void SetGameHook(Gw2Hook type, T cb) {
        Gw2Hooks *list = get_hook_list();

        switch (type) {
        case HOOK_AG_CAN_BE_SEL: ASSIGN_HOOK(list->AgCanBeSel, cb); break;
        case HOOK_CHAT: ASSIGN_HOOK(list->ChatHook, cb); break;
        case HOOK_DAMAGE_LOG: ASSIGN_HOOK(list->DmgLogHook, cb); break;
        case HOOK_COMBAT_LOG: ASSIGN_HOOK(list->CombatLogHook, cb); break;
        case HOOK_ALLOCATOR: ASSIGN_HOOK(list->AllocatorHook, cb); break;
        case HOOK_LOGGER: ASSIGN_HOOK(list->LoggerHook, cb); break;
        case HOOK_SCREENSHOT: ASSIGN_HOOK(list->ScreenshotHook, cb); break;
        case HOOK_WINDOW_PROC: ASSIGN_HOOK(list->WndProcHook, cb); break;
        case HOOK_SEND_PACKET: ASSIGN_HOOK(list->PacketSendHook, cb); break;
        case HOOK_RECV_PACKET: ASSIGN_HOOK(list->PacketRecvHook, cb); break;
        }
    }
};
