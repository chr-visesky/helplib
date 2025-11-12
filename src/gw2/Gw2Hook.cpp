
#include "main.h"
#include <sstream>

#define SHIFT_CB [](uintptr_t o)->uintptr_t

size_t __fastcall hkCanBeSel(uintptr_t, uintptr_t, uintptr_t);

void hkProcessText(hl::CpuContext*);
void hkDmgLog(hl::CpuContext*);
void hkCombatLog(hl::CpuContext*);
void hkAllocator(hl::CpuContext*);
void hkLogger(hl::CpuContext*);
void hkLogger2(hl::CpuContext*);
void hkFrameText(hl::CpuContext*);
void hkMsgConn(hl::CpuContext*);
void hkMsgSend(hl::CpuContext*);
void hkMsgRecv(hl::CpuContext*);
void hkScreenshot(hl::CpuContext*);
void hkWndProc(hl::CpuContext*);
void hkWndProcDx11(hl::CpuContext*);
void hkTextDec(hl::CpuContext*);
void hkMntSock(hl::CpuContext*);
void hkMoveLock(hl::CpuContext*);

bool Gw2GameHook::init_hooks() {
    uintptr_t pLogger  = hl::FindPattern("48 89 5C 24 08 57 48 83 EC 20 4D 8B 09 48 8B DA 48 8B F9 E8");
    uintptr_t pLogger2 = hl::FindPattern("5F C3 CC CC CC CC CC CC CC 48 89 5C 24 08 57 48 83 EC 20 4D 8B 09 48 8B DA 48 8B F9 E8");
    uintptr_t pScreenshot = GW2LIB::PointerScan("!( screenshotFlags & SCREENSHOT_FLAG_STEREOSCOPIC && screenshotFlags & SCREENSHOT_FLAG_HIGH_RES )", SHIFT_CB{ return o - 0x21; }, 0, true);
    uintptr_t pAllocator = GW2LIB::PointerScan("bytes < MAX_ALLOC", SHIFT_CB{ return o - 0x53; }, 2, true);
    //uintptr_t pWndProc = GW2LIB::PointerScan("ArenaNet_Dx_Window_Class", SHIFT_CB{ return hl::FollowRelativeAddress(o - 0x7); }, 1, true);
    uintptr_t pWndProcDx11 = GW2LIB::PointerScan("ArenaNet_Gr_Window_Class", SHIFT_CB{ return hl::FollowRelativeAddress(o - 0x16); }, 0, true);
    uintptr_t pDmgLog = hl::FindPattern("48 89 5C 24 18 55 56 41 56 48 83 EC 40 41 8B E9 49 8B D8 48 8B F2 48 85 D2 75"); // "targetAgent" in ChCliCombatLog.cpp
    uintptr_t pMntSock = hl::FindPattern("CC 48 89 5C 24 08 48 89 74 24 18 57 48 83 EC 30 48 8D 79 14 8B DA 48 8B F1 3B 17 73 02"); // "socketInfo.socketedAgents.Count() > socketIndex" in RidingContext.cpp (0x2C has max count)

    // scan for uint32_t - 1 = chair controls unlocked, 0 = controls locked
    uintptr_t pMoveLock = hl::FindPattern("CC 48 89 5C 24 08 57 48 83 EC 20 48 8B D9 48 8B FA 48 8B 89 ?? ?? ?? ?? E8 ?? ?? ?? ?? 85 C0 74 0A");

    /* from 32-bit
    uintptr_t pFrameTxt = hl::FindPattern("CC CC CC CC CC CC CC CC CC CC CC CC 52 51 B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? C3 CC CC CC");
    */

    hl::PatternScanner scanner;
    auto results = scanner.find({
        "codedProcessedText",
        "logType < UI_COMBAT_LOG_TYPES",
        "bytes < MAX_ALLOC",
        "mc->recvMsgPacked->defArray[0].defSize",
        "mc->mode == MSGCONN_MODE_ENCRYPTED",
        "Raw dispatch failed"
    });

    uintptr_t pProcessText = NULL;
    uintptr_t pCombatLog = NULL;
    uintptr_t pMsgConn = NULL;
    uintptr_t pMsgSend = NULL;
    uintptr_t pMsgRecv = NULL;

    pProcessText = results[0] - 0x4f;
    pCombatLog = results[1] - 0x26;
    pLogger = pLogger + 0x4c;
    pLogger2 = pLogger2 + 0x55;
    //pWndProc = pWndProc + 0x25;
    pMsgSend = results[4] + 0x42;
    pMsgRecv = results[5] + 0xb0;
    pMntSock = pMntSock + 1;
    pMoveLock = pMoveLock + 1;

    /* from 32-bit
    pDmgLog = (pDmgLog + 0x4);
    pMsgConn = (results[3] - 0x17);
    pFrameTxt = (pFrameTxt + 0xc);
    pScreenshot = (pScreenshot + 0x3);
    */

    m_hkProcessText = m_hooker.hookDetour(pProcessText, 17, hkProcessText);
    m_hkDmgLog = m_hooker.hookDetour(pDmgLog, 16, hkDmgLog);
    m_hkCombatLog = m_hooker.hookDetour(pCombatLog, 16, hkCombatLog);
    m_hkAllocator = m_hooker.hookDetour(pAllocator, 14, hkAllocator);
    m_hkLogger = m_hooker.hookDetour(pLogger, 14, hkLogger);
    m_hkLogger2 = m_hooker.hookDetour(pLogger2, 14, hkLogger2);
    m_hkScreenshot = m_hooker.hookDetour(pScreenshot, 15, hkScreenshot);
    //m_hkWndProc = m_hooker.hookDetour(pWndProc, 16, hkWndProc);
    m_hkWndProcDx11 = m_hooker.hookDetour(pWndProcDx11, 16, hkWndProcDx11);
    m_hkMsgSend = m_hooker.hookDetour(pMsgSend, 17, hkMsgSend);
    m_hkMsgRecv = m_hooker.hookDetour(pMsgRecv, 17, hkMsgRecv);
    m_hkMntSock = m_hooker.hookDetour(pMntSock, 15, hkMntSock);
    m_hkMoveLock = m_hooker.hookDetour(pMoveLock, 16, hkMoveLock);

    /* from 32-bit
    m_hkFrTxt = m_hooker.hookDetour(pFrameTxt, 7, hkFrameText);
    m_hkMsgConn = m_hooker.hookDetour(pMsgConn, 6, hkMsgConn);
    */

    auto mems = GetMain()->GetGamePointers();

    m_hkCanBeSel = m_hooker.hookVT((uintptr_t)mems->pAgentSelectionCtx, 1, (uintptr_t)hkCanBeSel);

    if (!m_hkCanBeSel) {
        HL_LOG_ERR("[Hook::Init] Hooking m_hkCanBeSel failed\n");
        return false;
    }

    if (!m_hkProcessText) {
        HL_LOG_ERR("[Hook::Init] Hooking chat log failed\n");
        return false;
    }

    if (!m_hkDmgLog) {
        HL_LOG_ERR("[Hook::Init] Hooking damage log failed\n");
        return false;
    }

    if (!m_hkCombatLog) {
        HL_LOG_ERR("[Hook::Init] Hooking combat log failed\n");
        return false;
    }

    if (!m_hkLogger) {
        HL_LOG_ERR("[Hook::Init] Hooking game logger failed\n");
        return false;
    }

    if (!m_hkLogger2) {
        HL_LOG_ERR("[Hook::Init] Hooking game logger 2 failed\n");
        return false;
    }

    /*if (!m_hkFrTxt) {
        HL_LOG_ERR("[Hook::Init] Hooking frame text proc failed\n");
        return false;
    }*/

    if (!m_hkScreenshot) {
        HL_LOG_ERR("[Hook::Init] Hooking screenshot proc failed\n");
        return false;
    }

    /*if (!m_hkWndProc) {
        HL_LOG_ERR("[Hook::Init] Hooking window proc failed\n");
        return false;
    }*/

    if (!m_hkWndProcDx11) {
        HL_LOG_ERR("[Hook::Init] Hooking DX11 window proc failed\n");
        return false;
    }

    return true;
}

void Gw2GameHook::cleanup() {
    if (m_hkCanBeSel) m_hooker.unhook(m_hkCanBeSel);
    if (m_hkProcessText) m_hooker.unhook(m_hkProcessText);
    if (m_hkDmgLog) m_hooker.unhook(m_hkDmgLog);
    if (m_hkCombatLog) m_hooker.unhook(m_hkCombatLog);
    if (m_hkAllocator) m_hooker.unhook(m_hkAllocator);
    if (m_hkLogger) m_hooker.unhook(m_hkLogger);
    if (m_hkLogger2) m_hooker.unhook(m_hkLogger2);
    if (m_hkFrTxt) m_hooker.unhook(m_hkFrTxt);
    if (m_hkScreenshot) m_hooker.unhook(m_hkScreenshot);
    if (m_hkWndProc) m_hooker.unhook(m_hkWndProc);
    if (m_hkWndProcDx11) m_hooker.unhook(m_hkWndProcDx11);
    if (m_hkMsgSend) m_hooker.unhook(m_hkMsgSend);
    if (m_hkMsgRecv) m_hooker.unhook(m_hkMsgRecv);
    if (m_hkMntSock) m_hooker.unhook(m_hkMntSock);
    if (m_hkMoveLock) m_hooker.unhook(m_hkMoveLock);
}


// this hook will intercept calls to "AgentCanBeSelection()" and run a user provided function.
// setting "call_orig" to true inside your callback will call the original game function and
// ignore any return value provided by your callback
size_t __fastcall hkCanBeSel(uintptr_t pInst, uintptr_t ag64, uintptr_t ag32) {
    uintptr_t agptr = ag64;

    Gw2GameHook *hk = get_hook();
    Gw2Hooks* list = get_hook_list();
    auto pCore = GetMain();
    static auto orgFunc = ((bool(__thiscall*)(uintptr_t, uintptr_t))hk->m_hkCanBeSel->getLocation());

    bool call_orig = false;
    bool ret = false;

    try {
        if (list->AgCanBeSel) {
            GW2LIB::Agent ag(agptr);
            ret = list->AgCanBeSel(call_orig, ag);
        } else {
            call_orig = true;
        }

        if (call_orig) return orgFunc(pInst, agptr);
        return ret;
    } catch (int e) {
        HL_LOG_ERR("[hkCanBeSel Exception]: %i\n", e);
    }

    return false;
}

struct mc_msg_def {
    uintptr_t unk1;
    uintptr_t unk2;
    uintptr_t type;
    uintptr_t unk3;
    uintptr_t size;
};

struct mc_msg {
    uintptr_t arg1;
    mc_msg_def *def; // ptr to msg def
    uintptr_t arg3;
    uintptr_t func;  // msg handler
};

void hkMsgConn(hl::CpuContext *ctx) {
/* from 32-bit
    // mc->recvMsgPacked->defArray[0].defSize
    // ctx->ESI = mc struct
    mc_msg *msg = *(mc_msg**)(ctx->ESI + 0x28);

    // recvMsgPacked
    uintptr_t arg1  = msg->arg1;
    mc_msg_def *def = msg->def;
    uintptr_t arg3  = msg->arg3;
    uintptr_t func  = msg->func;

    // msg def
    uintptr_t unk1 = def->unk1;
    uintptr_t type = def->type;
    uintptr_t size = def->size;

    //HL_LOG_DBG("msg: %p, arg1: %p, def: %p, func: %p, type: %p, size: %p\n", msg, arg1, def, func, type, size);
*/
}

void hkMsgRecv(hl::CpuContext *ctx) {
    uintptr_t *buff = (uintptr_t*)ctx->RBX;
    uint32_t size = (uint32_t)ctx->RSI;

    Gw2Hooks* list = get_hook_list();
    if (list->PacketRecvHook) list->PacketRecvHook(size, buff);
}

void hkMsgSend(hl::CpuContext *ctx) {
    uintptr_t *buff = (uintptr_t*)ctx->RSI;
    uint32_t size = (uint32_t)ctx->RDI;

    Gw2Hooks* list = get_hook_list();
    if (list->PacketSendHook) list->PacketSendHook(size, buff);
}

void hkWndProc(hl::CpuContext *ctx) {
    HWND &hWnd = (HWND&)(ctx->RCX);
    UINT &msg = (UINT&)(ctx->RDX);
    WPARAM &wParam = (WPARAM&)(ctx->R8);
    LPARAM &lParam = (LPARAM&)(ctx->R9);

    Gw2Hooks* list = get_hook_list();
    bool call_orig = true;
    if (list->WndProcHook) call_orig = list->WndProcHook(hWnd, msg, wParam, lParam);
    if (!call_orig) msg = WM_NULL;
}

void hkWndProcDx11(hl::CpuContext* ctx) {
    HWND& hWnd = (HWND&)(ctx->RCX);
    UINT& msg = (UINT&)(ctx->RDX);
    WPARAM& wParam = (WPARAM&)(ctx->R8);
    LPARAM& lParam = (LPARAM&)(ctx->R9);

    Gw2Hooks* list = get_hook_list();
    bool call_orig = true;
    if (list->WndProcHook) call_orig = list->WndProcHook(hWnd, msg, wParam, lParam);
    if (!call_orig) msg = WM_NULL;
}

void hkScreenshot(hl::CpuContext *ctx) {
    using namespace GW2LIB::GW2;
    auto &reg = ctx->RCX;

    auto mode = (ScreenshotMode)reg;

    Gw2Hooks* list = get_hook_list();
    if (list->ScreenshotHook) {
        mode = list->ScreenshotHook(mode);
    }

    if(mode >= 0 && mode < SCREENSHOT_END)
        reg = mode;
}


void hkFrameText(hl::CpuContext *ctx) {
    uintptr_t p1 = (uintptr_t)ctx->RCX;
    wchar_t *wtxt = (wchar_t*)ctx->RDX;

    Gw2GameHook *hk = get_hook();
    std::string test = hk->converter.to_bytes(wtxt);
    //if(test != "A+F1" && test != "A+F2" && test.size()) HL_LOG_DBG("ECX: %p - EDX: %p - %s\n", p1, wtxt, test.c_str());
}

void hkProcessText(hl::CpuContext *ctx)
{
    wchar_t *wtxt = *(wchar_t**)(ctx->R14 + 0x8);

    Gw2Hooks* list = get_hook_list();
    if (list->ChatHook) list->ChatHook(wtxt);
}


void hkDmgLog(hl::CpuContext *ctx)
{
    int hit = (int)ctx->R9;
    uintptr_t *pSrc = (uintptr_t*)(ctx->R8);
    uintptr_t *pTgt = (uintptr_t*)(ctx->RDX);
    hl::ForeignClass skillDef = **(uintptr_t***)(ctx->RSP + 0x28);

    const GW2LIB::Mems *offsets = GetMain()->GetGameOffsets();

    GW2LIB::GW2::EffectType ef = skillDef.get<GW2LIB::GW2::EffectType>(offsets->skillDefEffect);
    GW2LIB::Agent agSrc(pSrc);
    GW2LIB::Agent agTgt(pTgt);

    Gw2Hooks* list = get_hook_list();
    if (list->DmgLogHook) list->DmgLogHook(agSrc, agTgt, hit, ef);
}

void hkCombatLog(hl::CpuContext *ctx)
{
    GW2LIB::GW2::CombatLogType type = (GW2LIB::GW2::CombatLogType)(ctx->R8);
    int hit = (int)ctx->RBP;
    uintptr_t *pSrc = *(uintptr_t**)(ctx->RBX + 0x40);
    uintptr_t *pTgt = *(uintptr_t**)(ctx->RBX + 0x58);
    hl::ForeignClass skillDef = *(uintptr_t**)(ctx->RBX + 0x48);

    const GW2LIB::Mems *offsets = GetMain()->GetGameOffsets();

    GW2LIB::GW2::EffectType ef = skillDef.get<GW2LIB::GW2::EffectType>(offsets->skillDefEffect);
    GW2LIB::Agent agTgt(pTgt);
    GW2LIB::Agent agSrc(pSrc);

    Gw2Hooks* list = get_hook_list();
    if (list->CombatLogHook) list->CombatLogHook(agSrc, agTgt, hit, type, ef);
}

// PERFORMANCE WARNING: this function is called frequently by the client
void hkAllocator(hl::CpuContext *ctx) {
    int type = (int)(ctx->RCX);
    size_t size = ctx->RDX;
    int cat = *(int*)(ctx->RSP + 0x38);
    int line = *(int*)(ctx->RSP + 0x30);
    char* file = *(char**)(ctx->RSP + 0x28);

    Gw2Hooks* list = get_hook_list();
    if (list->AllocatorHook) list->AllocatorHook(type, size, cat, line, file);
}

void hkLogger(hl::CpuContext *ctx) {
    char* txt = (char*)(ctx->RDI);

    Gw2Hooks* list = get_hook_list();
    if (list->LoggerHook) list->LoggerHook(txt);
}

void hkLogger2(hl::CpuContext *ctx) {
    wchar_t* wtxt = (wchar_t*)(ctx->RDI);

    std::string txt = get_hook()->converter.to_bytes(wtxt);

    Gw2Hooks* list = get_hook_list();
    if (list->LoggerHook) list->LoggerHook((char*)txt.c_str());
}

void hkMntSock(hl::CpuContext* ctx) {
    ctx->RDX = 100;
}

void hkMoveLock(hl::CpuContext* ctx) {
    return;
    int oldval = *(int*)(ctx->RDX + 0x74);
    if (oldval == 0) {
        *(float*)(ctx->RDX + 0x74) = 367.5;
    }
}



