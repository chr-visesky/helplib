#include "main.h"

#include "hacklib/Main.h"
#include "hacklib/PatternScanner.h"
#include "hacklib/D3DDeviceFetcher.h"
#include "hacklib/Hooker.h"
#include "hacklib/ImplementMember.h"
#include "hacklib/ForeignClass.h"
#include "hacklib/Logging.h"
#include "glm/gtc/matrix_transform.hpp"

#include <mutex>
#include <thread>
#include <chrono>


void __fastcall hkGameThread(uintptr_t, int, int);
HRESULT __stdcall hkPresentDx11(IDXGISwapChain* pSwap, UINT, UINT);
HRESULT __stdcall hkResizeBuffDx11(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);


hl::StaticInit<Gw2HackMain> g_initObj;


Gw2HackMain *GetMain()
{
    return g_initObj.getMain();
}

int64_t GetTimestamp() {
    return std::chrono::system_clock::now().time_since_epoch().count() / 10000;
}

DWORD ExceptHandler(const char *msg, DWORD code, EXCEPTION_POINTERS *ep, const char *file, const char *func, int line) {
    EXCEPTION_RECORD *er = ep->ExceptionRecord;
    CONTEXT *ctx = ep->ContextRecord;
    const char *fmt_dbg = "%s: 0x%08X - addr: 0x%p\n";
    const char *fmt_rel = "%s\n";

    TRACER_PRINT;

#ifdef _DEBUG
    hl::LogError(file, func, line, fmt_dbg, msg, code, er->ExceptionAddress);
#else
    hl::LogError(fmt_rel, msg);
#endif

    return EXCEPTION_EXECUTE_HANDLER;
}

void Gw2HackMain::debug_tracer_list_push(const char* format) {
    if (debug_tracer_list_idx + 1 > DEBUG_TRACER_LIST_MAX) {
        debug_tracer_list_idx = 0;
    }

    debug_tracer_list[debug_tracer_list_idx++] = format;
}

void Gw2HackMain::debug_tracer_list_print() {
    size_t start = debug_tracer_list_idx;

    for (int i = 0; i < DEBUG_TRACER_LIST_MAX; i++) {
        if (start >= DEBUG_TRACER_LIST_MAX) start = 0;
        const char* x = debug_tracer_list[start++];
        if (x == 0) continue;
        HL_LOG_RAW(x);
    }

    HL_LOG_RAW("debug_string: %s\n", debug_tracer_string.c_str());
}

bool Gw2HackMain::init()
{
    m_con.create("Gw2lib Console");

    logConfig.logFunc = [this](const std::string& str){
        m_con.printf("%s", str.c_str());
    };
    hl::ConfigLog(logConfig);

    HL_LOG_DBG("Scanning for offsets...\n");
    int64_t scan_start = GetTimestamp();

    if (!ScanForOffsets()) {
        HL_LOG_ERR("[Core::Init] One or more offset scans are invalid\n");
        return false;
    }

    uintptr_t MapIdSig = hl::FindPattern("00 00 08 00 89 0d ?? ?? ?? ?? c3");
    uintptr_t ping = hl::FindPattern("CC 33 C0 4C 8D 0D ?? ?? ?? ?? 4C 8B DA");
    uintptr_t fps = hl::FindPattern("CC 83 0D ?? ?? ?? ?? 20 89 0D ?? ?? ?? ?? C3 CC");
    uintptr_t ifHide = hl::FindPattern("CC 33 C0 39 05 ?? ?? ?? ?? 0F 9D C0 C3 CC");
    uintptr_t mapOpen = hl::FindPattern("B8 11 00 00 00 E9 ?? ?? 00 00 83 3D ?? ?? ?? ?? 00 74 0A");
    uintptr_t actionCam = hl::FindPattern("48 83 C4 ?? 5B C3 BA 06 00 00 00 C7 05 ?? ?? ?? ?? 01 00 00 00 4C 8D 4C 24");
    uintptr_t buildId = hl::FindPattern("B9 01 00 00 00 E8 ?? ?? ?? ?? E8 ?? ?? ?? ?? 44 8B C0 48 8D 15");
    uintptr_t ipAddr = hl::FindPattern("0F 10 47 ?? 48 8B 0D ?? ?? ?? ?? 0F 11 05 ?? ?? ?? ?? F2 0F 10 4F ?? F2 0F 11");
    uintptr_t fGetCT = GW2LIB::PointerScan("CParser::Validate(buffer->Ptr(), buffer->Term(), true ) == buffer->Term()", [](uintptr_t o)->uintptr_t { return o - 0x74; }, 5, true);
    uintptr_t fGetItemCName = hl::FindPattern("57 41 54 41 55 41 56 41 57 48 83 EC ?? 4D 8B F9 4D 8B E0 4C 8B EA 48 8B F1 48 85 C9 75");
    GetContext = hl::FindPattern("65 48 8b 04 25 58 00 00 00 ba 10 00 00 00") - 0x6;
    SetContext = hl::FindPattern("65 48 8B 04 25 58 00 00 00 41 B8 10 00 00 00 48 8B 04 D0 49 89 0C 00 C3") - 0x6;
    GUID2Str = hl::FollowRelativeAddress(GW2LIB::PointerScan("NULL REWARD TRACK: %s", [](uintptr_t o)->uintptr_t { return o - 0xc; }, 0, true));
    /* from 32-bit
    LockCamera = hl::FindPattern("5E 8B E5 5D C2 10 00 CC 55 8B EC 8B 4D 08 E8 ?? ?? ?? ?? 5D C2 04 00 CC") + 0x8;
    LockCamera2 = hl::FindPattern("55 8B EC 56 8B F1 83 3E 00 74 ?? 8B 4E 04 85 C9 74");
    AddDrunkLevel = hl::FollowRelativeAddress(hl::FindPattern("CC 56 8B F2 E8 ?? ?? ?? ?? 8B 48 48 0F B6 46 02 50 E8 ?? ?? ?? ?? B8 01 00 00 00 5E C3 CC") + 0x12);
    */

    hl::PatternScanner scanner;

    auto results = scanner.find({
        "ViewAdvanceDevice",
        "ViewAdvanceAgentSelect",
        "ViewAdvanceAgentView",
        "ViewAdvanceWorldView",
        "CompassManager()->IsCompassFixed()",
        "ViewAdvanceUi",
        "ultimateMasterCharacter",
        "m_currCamera",
        "guid != MEM_CATEGORY_INVALID && guid < m_headGUID. guid: %u m_headGuid: %u",
        "resultFunc",
        "Validate(id)",
        "ViewAdvanceAudioMumble"
    });

    if (![&](){
        __try {
            m_mems.pAlertCtx = (void*)hl::FollowRelativeAddress(hl::FollowRelativeAddress(results[0] + 0xa) + 0x3);
            m_mems.pAgentSelectionCtx = (void*)hl::FollowRelativeAddress(hl::FollowRelativeAddress(results[1] + 0xa) + 0x3);
            m_mems.pAgentViewCtx = (void*)hl::FollowRelativeAddress(hl::FollowRelativeAddress(results[2] + 0xa) + 0x3);
            m_mems.pWorldView = (void*)hl::FollowRelativeAddress(hl::FollowRelativeAddress(results[3] + 0xa) + 0x7);
            m_mems.pCompass = (void*)hl::FollowRelativeAddress(hl::FollowRelativeAddress(results[4] + 0xa) + 0x3);
            m_mems.pUiOpts = (void*)hl::FollowRelativeAddress(hl::FollowRelativeAddress(results[5] + 0xa) + 0x3);
            m_mems.pCam = *(void**)hl::FollowRelativeAddress(results[7] - 0x1e);
            m_mems.pMumble = (void*)hl::FollowRelativeAddress(hl::FollowRelativeAddress(results[11] + 0xa) + 0x3);
            m_mems.avctxAgentArray = (uintptr_t)m_mems.pAgentViewCtx + m_pubmems.avctxAgentArray;
            if (MapIdSig) m_mems.pMapId = (int*)hl::FollowRelativeAddress(MapIdSig + 0x6);
            if (ping) m_mems.pPing = (int*)hl::FollowRelativeAddress(ping + 0x6);
            if (fps) m_mems.pFps = (int*)hl::FollowRelativeAddress(fps + 0xa);
            if (ifHide) m_mems.pIfHide = (int*)hl::FollowRelativeAddress(ifHide + 0x5);
            if (mapOpen) m_mems.pMapOpen = (int*)hl::FollowRelativeAddress(mapOpen + 0xc);
            if (actionCam) m_mems.pActionCam = (int*)hl::FollowRelativeAddress(actionCam + 0xd, 4);
            if (buildId) m_gameData.buildId = *(uint32_t*)(hl::FollowRelativeAddress(buildId + 0xB) + 1);
            if (ipAddr) m_mems.pIpAddr = (sockaddr_in*)hl::FollowRelativeAddress(ipAddr + 0xe);

            DecodeGameText = results[9] - 0x6a;
            GetCharacter = hl::FollowRelativeAddress(results[6] - 0x1c);
            if (fGetItemCName) GetCodedItemName = fGetItemCName - 0xf;
            /* from 32-bit
            m_gameData.objData.memCatList = *(uintptr_t*)(results[8] + 0xc);
            m_gameData.objData.memCatSize = *(size_t**)(results[8] - 0x11);
            */
            m_gameData.objData.frameListPtr = (uintptr_t)hl::FollowRelativeAddress(results[10] - 0x2f);

            if (fGetCT) GetCodedText = fGetCT;
            m_mems.pWorldViewContext = *(void**)m_mems.pWorldView;

        } __except (EXCEPTION_EXECUTE_HANDLER) {
            return false;
        }

        return true;
    }())
    {
        HL_LOG_ERR("[Core::Init] One or more patterns are invalid\n");
        return false;
    }

    int32_t scan_time = (int32_t)(GetTimestamp() - scan_start);
    HL_LOG_DBG("Offset scan complete - Time: %ums\n", scan_time);

    HL_LOG_DBG("base:    %p\n", hl::GetCurrentModule());
    HL_LOG_DBG("aa:      %p\n", m_mems.pAgentViewCtx);
    HL_LOG_DBG("actx:    %p\n", m_mems.pAlertCtx);
    HL_LOG_DBG("asctx:   %p\n", m_mems.pAgentSelectionCtx);
    HL_LOG_DBG("wv:      %p\n", m_mems.pWorldView);
    HL_LOG_DBG("wvctx:   %p\n", m_mems.pWorldViewContext);
    HL_LOG_DBG("comp:    %p\n", m_mems.pCompass);
    HL_LOG_DBG("uiOpts:  %p\n", m_mems.pUiOpts);
    HL_LOG_DBG("cam:     %p\n", m_mems.pCam);
    HL_LOG_DBG("mpid:    %p\n", m_mems.pMapId);
    HL_LOG_DBG("ping:    %p\n", m_mems.pPing);
    HL_LOG_DBG("fps:     %p\n", m_mems.pFps);
    HL_LOG_DBG("ifHide:  %p\n", m_mems.pIfHide);
    HL_LOG_DBG("mapOpen: %p\n", m_mems.pMapOpen);
    HL_LOG_DBG("actCam:  %p\n", m_mems.pActionCam);
    HL_LOG_DBG("ipaddr:  %p\n", m_mems.pIpAddr);
    HL_LOG_DBG("mumble:  %p\n", m_mems.pMumble);
    HL_LOG_DBG("decdtxt: %p\n", DecodeGameText);
    HL_LOG_DBG("frList:  %p\n", m_gameData.objData.frameListPtr);
    HL_LOG_DBG("agArr:   %p\n", m_mems.avctxAgentArray);

    // hook functions
#ifdef NOD3DHOOK
    HL_LOG("Compiled to NOT hook D3D!\n");
#else

    // get d3d11 coms
    m_d3d11coms = hl::D3DDeviceFetcher::GetD3D11Device();
    HL_LOG_DBG("dx11Swp: %p\n", m_d3d11coms.pSwapChain);
    HL_LOG_DBG("dx11Dev: %p\n", m_d3d11coms.pDevice);
    HL_LOG_DBG("dx11Ctx: %p\n", m_d3d11coms.pDeviceContext);

    m_hkPresent = m_hooker.hookVT((uintptr_t)m_d3d11coms.pSwapChain, 8, (uintptr_t)hkPresentDx11);
    if (!m_hkPresent) {
        HL_LOG_ERR("[Core::Init] Hooking DX11 render thread failed\n");
        return false;
    }

    m_hkResizeBuff = m_hooker.hookVT((uintptr_t)m_d3d11coms.pSwapChain, 13, (uintptr_t)hkResizeBuffDx11);
    if (!m_hkResizeBuff) {
        HL_LOG_ERR("[Core::Init] Hooking DX11 resize buffers failed\n");
        return false;
    }

#endif

    m_hkAlertCtx = m_hooker.hookVT(*(uintptr_t*)m_mems.pAlertCtx, 0, (uintptr_t)hkGameThread);
    if (!m_hkAlertCtx) {
        HL_LOG_ERR("[Core::Init] Hooking game thread failed\n");
        return false;
    }

    if (![this](){
        __try {
            m_gw2Hook.init_hooks();
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            return false;
        }
        return true;
    }()) {
        HL_LOG_ERR("[Core::Init] Could not initialize GW2 game hooks\n");
        return false;
    }


    HL_LOG_DBG("Init ESP data\n");

    // no drawer for dx11
    //if (!m_isDX11) m_drawer.setContext(pDevice);

    //extern bool InitEsp(); // EspDraw.cpp
    //bool result = InitEsp();
    bool result = true;

#ifndef NOD3DHOOK
    if (!result)
        return false;
#endif


    GW2LIB::gw2lib_main();

    return false;
}

void Gw2HackMain::shutdown()
{
    m_hooker.unhook(m_hkResizeBuff);
    m_hooker.unhook(m_hkPresent);
    m_hooker.unhook(m_hkAlertCtx);
    m_gw2Hook.cleanup();

    std::lock_guard<std::mutex> lock(m_gameDataMutex);
}

uint64_t Gw2HackMain::GetWindowHandle() {
    if (!m_hwnd) {
        m_hwnd = hl::WindowOverlay::GetTargetWindow();
    }

    return (uint64_t)m_hwnd;
}

GW2LIB::D3D11COMs *Gw2HackMain::GetD3D11Coms() {
    return &m_d3d11coms;
}

const GameData::GameData *Gw2HackMain::GetGameData() const
{
    return &m_gameData;
}

void Gw2HackMain::SetRenderCallback(void(*cbRender)())
{
    m_cbRender = cbRender;
}

void Gw2HackMain::SetGameHookCallback(void(*cbGameHook)()) {
    m_cbGameHook = cbGameHook;
}

void Gw2HackMain::SetResizeBuffCallback(void(*cbResizeBuff)()) {
    m_cbResizeBuff = cbResizeBuff;
}


void Gw2HackMain::GameCtxCheck() {
    // make tls ctx from main thread available in this thread
    if (!GetContext()) {
        if(m_mems.pCtx) SetContext(m_mems.pCtx);
    }
}

void Gw2HackMain::RenderHookDx11() {
    if (m_gameData.camData.valid) {

        if (m_cbRender) {
            m_bPublicDrawer = true;

            [&]()
            {
                __try {
                    m_cbRender();
                }
                __except (HLGW2_EXCEPTION("[ESP DX11 callback] Exception in ESP code")) {
                    ;
                }
            }();

            m_bPublicDrawer = false;
        }
    }

}

void Gw2HackMain::ResizeBuffersDx11() {
    if (m_cbResizeBuff) {
        [&]()
        {
            __try {
                m_cbResizeBuff();
            }
            __except (HLGW2_EXCEPTION("Resize Buffers callback error")) {
                ;
            }
        }();
    }
}

std::string ConvertWcharToStr(const wchar_t *in) {
    std::string txt = "";
    int num = WideCharToMultiByte(CP_UTF8, 0, in, -1, NULL, 0, NULL, 0);
    char *name = new char[num + 1];

    WideCharToMultiByte(CP_UTF8, 0, in, -1, name, num, NULL, 0);

    int i = 0;
    while (name[i]) {
        txt += name[i];
        i++;
    }

    delete[] name;
    return txt;
}

void __fastcall DecodeTextCB(std::string* ctx, wchar_t *decodedText) {
    auto main = GetMain();
    if (!decodedText || !decodedText[0] || !ctx || !main || !main->m_decodedStrings.count(ctx)) return;
    *ctx = ConvertWcharToStr(decodedText);
}

void __fastcall DecodeTextStripColorCB(std::string* ctx, wchar_t *decodedText) {
    auto main = GetMain();
    if (!decodedText || !decodedText[0] || !ctx || !main || !main->m_decodedStrings.count(ctx)) return;
    *ctx = ConvertWcharToStr(decodedText);
    if(ctx->size()) main->StripColorTag(ctx);
}

void Gw2HackMain::StripColorTag(std::string* txt) {
    if (!txt) return;
    if (txt->find_first_of("<c=#") != std::string::npos)
        txt->assign(txt->substr(11, txt->size() - 15));
}

void Gw2HackMain::DecodeText(void* codedTxt, std::string* dst, bool stripColor) {
    if(!stripColor) DecodeGameText(codedTxt, DecodeTextCB, dst);
    else DecodeGameText(codedTxt, DecodeTextStripColorCB, dst);
}

bool Gw2HackMain::SetupCamData() {
    // get cam data
    m_gameData.camData.valid = false;
    if (m_mems.pWorldViewContext)
    {
        hl::ForeignClass wvctx = m_mems.pWorldViewContext;
        if (wvctx && wvctx.get<int>(m_pubmems.wvctxStatus) == 1)
        {
            hl::Vec3 lookAt, upVec;
            wvctx.call<void>(m_pubmems.wvctxVtGetMetrics, 1, &m_gameData.camData.camPos, &lookAt, &upVec, &m_gameData.camData.fovy);
            m_gameData.camData.viewVec = glm::normalize(lookAt - m_gameData.camData.camPos);
            m_gameData.camData.valid = true;

            m_gameData.camData.currZoom = wvctx.get<float>(m_pubmems.camCurrZoom);
            m_gameData.camData.minZoom  = wvctx.get<float>(m_pubmems.camMinZoom);
            m_gameData.camData.maxZoom  = wvctx.get<float>(m_pubmems.camMaxZoom);
        }
    }

    return true;
}

bool Gw2HackMain::SetupAgentArray() {
    hl::ForeignClass ctx = m_mems.pCtx;
    hl::ForeignClass avctx = m_mems.pAgentViewCtx;
    hl::ForeignClass asctx = m_mems.pAgentSelectionCtx;
    if (!ctx || !avctx || !asctx) return false;

    hl::ForeignClass gdctx = ctx.get<void*>(m_pubmems.contextGadget);
    if (!gdctx) return false;

    GameData::ANet::Collection<void*> agentArray(m_mems.avctxAgentArray);
    if (!agentArray.IsValid()) return false;

    bool bOwnAgentFound = false;
    bool bAutoSelectionFound = false;
    bool bHoverSelectionFound = false;
    bool bLockedSelectionFound = false;

    // add agents from game array to own array and update data
    uint32_t sizeAgentArray = agentArray.Count();
    if (sizeAgentArray != m_gameData.objData.agentDataList.size()) {
        m_gameData.objData.agentDataList.resize(sizeAgentArray);
    }

    for (uint32_t i = 0; i < sizeAgentArray; i++)
    {
        hl::ForeignClass avAgent = agentArray[i];
        if (!avAgent) continue;

        hl::ForeignClass pAgent = avAgent.call<void*>(m_pubmems.avagVtGetAgent);
        if (!pAgent) continue;

        // agent is not in our array. add and fix ptr
        if (!m_gameData.objData.agentDataList[i] || m_gameData.objData.agentDataList[i]->pAgent != pAgent) {
            m_gameData.objData.agentDataList[i] = std::make_unique<GameData::AgentData>();
        }

        GameData::AgentData *pAgentData = m_gameData.objData.agentDataList[i].get();
        if (!pAgentData->wmAgent) pAgentData->wmAgent = avAgent;

        // update values
        RefreshDataAgent(pAgentData, pAgent);

        pAgentData->pCharData = nullptr;
        pAgentData->pPlayerData = nullptr;
        pAgentData->selectable = asctx.call<bool>(m_pubmems.asctxVtAgCanSel, pAgent);

        // gadget update
        if (pAgentData->type == GW2LIB::GW2::AGENT_TYPE_GADGET) {
            hl::ForeignClass pGadget = gdctx.call<void*>(m_pubmems.ctxgdVtGetGadget, pAgentData->agentId);
            if (!pAgentData->gadgetData) pAgentData->gadgetData = std::make_unique<GameData::GadgetData>();
            GameData::GadgetData *pGadgetData = pAgentData->gadgetData.get();
            RefreshDataGadget(pGadgetData, pGadget);
            pGadgetData->pAgentData = pAgentData;

            // resource node update
            if (pGadgetData->type == GW2LIB::GW2::GADGET_TYPE_RESOURCE_NODE) {
                hl::ForeignClass pRNode = pGadget.call<void*>(m_pubmems.gdVtGetRNode);
                if (!pGadgetData->rNodeData) pGadgetData->rNodeData = std::make_unique<GameData::ResourceNodeData>();
                GameData::ResourceNodeData *pRNodeData = pGadgetData->rNodeData.get();
                RefreshDataResourceNode(pRNodeData, pRNode);
                pRNodeData->pAgentData = pAgentData;
            }
        }

        // gadget attack target update
        if (pAgentData->type == GW2LIB::GW2::AGENT_TYPE_GADGET_ATTACK_TARGET) {
            hl::ForeignClass pAttackTgt = gdctx.call<void*>(m_pubmems.ctxgdVtGetAtkTgt, pAgentData->agentId);
            if (!pAgentData->attackTgtData) pAgentData->attackTgtData = std::make_unique<GameData::AttackTargetData>();
            GameData::AttackTargetData *pAttackTgtData = pAgentData->attackTgtData.get();
            RefreshDataAttackTarget(pAttackTgtData, pAttackTgt);
            pAttackTgtData->pAgentData = pAgentData;
        }

        // set own agent
        if (m_gameData.objData.ownCharacter && m_gameData.objData.ownCharacter->pAgentData == pAgentData) {
            m_gameData.objData.ownAgent = pAgentData;
            bOwnAgentFound = true;
        }

        // set selection agents
        if (pAgent == asctx.get<void*>(m_pubmems.asctxAuto)) {
            m_gameData.objData.autoSelection = pAgentData;
            bAutoSelectionFound = true;
        }
        if (pAgent == asctx.get<void*>(m_pubmems.asctxHover)) {
            m_gameData.objData.hoverSelection = pAgentData;
            bHoverSelectionFound = true;
        }
        if (pAgent == asctx.get<void*>(m_pubmems.asctxLocked)) {
            m_gameData.objData.lockedSelection = pAgentData;
            bLockedSelectionFound = true;
        }
    }

    // remove non valid agents from list
    for (uint32_t i = 0; i < m_gameData.objData.agentDataList.size(); i++) {
        if (!m_gameData.objData.agentDataList[i]) {
            continue;
        }

        // check if agent in our array is in game data
        hl::ForeignClass avAgent = agentArray[i];

        if (i >= sizeAgentArray || !avAgent || avAgent.call<void*>(m_pubmems.avagVtGetAgent) != m_gameData.objData.agentDataList[i]->pAgent) {
            GameData::AgentData *a = m_gameData.objData.agentDataList[i].get();

            // reset camera
            /*hl::ForeignClass cam = m_mems.pCam;
            if (avAgent.data() == cam.get<void*>(m_pubmems.camWmAgent)) {
                cam.set<void*>(m_pubmems.camWmAgent, m_gameData.objData.ownAgent->wmAgent.data());
            }*/

            // agent was not found in game. remove from our array and unlink corresponding data objects
            a->wmAgent = nullptr;
            a->pCharData = nullptr;
            a->pPlayerData = nullptr;
            m_gameData.objData.agentDataList[i] = nullptr;
        }
    }

    if (!bOwnAgentFound)
        m_gameData.objData.ownAgent = nullptr;
    if (!bAutoSelectionFound)
        m_gameData.objData.autoSelection = nullptr;
    if (!bHoverSelectionFound)
        m_gameData.objData.hoverSelection = nullptr;
    if (!bLockedSelectionFound)
        m_gameData.objData.lockedSelection = nullptr;

    return true;
}

bool Gw2HackMain::SetupCharacterArray() {
    hl::ForeignClass ctx = m_mems.pCtx;
    if (!ctx) return false;

    hl::ForeignClass charctx = ctx.get<void*>(m_pubmems.contextChar);
    if (!charctx) return false;

    auto charArray = charctx.get<GameData::ANet::Collection<void*>>(m_pubmems.charctxCharArray);
    if (!charArray.IsValid()) return false;

    bool bOwnCharFound = false;

    // add characters from game array to own array and update data
    uint32_t sizeCharArray = charArray.Count();
    if (sizeCharArray != m_gameData.objData.charDataList.size()) {
        m_gameData.objData.charDataList.resize(sizeCharArray);
    }

    for (uint32_t i = 0; i < sizeCharArray; i++)
    {
        hl::ForeignClass pCharacter = charArray[i];
        if (!pCharacter) continue;

        int agentId = pCharacter.call<int>(m_pubmems.charVtGetAgentId);

        if (!m_gameData.objData.charDataList[i] || m_gameData.objData.charDataList[i]->pCharacter != pCharacter) {
            m_gameData.objData.charDataList[i] = std::make_unique<GameData::CharacterData>();
        }

        GameData::CharacterData *pCharData = m_gameData.objData.charDataList[i].get();

        // update values
        RefreshDataCharacter(pCharData, pCharacter);

        bool bAgentDataFound = false;

        // link agentdata of corresponding agent
        if (m_gameData.objData.agentDataList.size() && m_gameData.objData.agentDataList[agentId]) {
            pCharData->pAgentData = m_gameData.objData.agentDataList[agentId].get();
            pCharData->pAgentData->pCharData = pCharData;

            if (pCharData->pAgentData->pAgent) {
                hl::ForeignClass transform = pCharData->pAgentData->pAgent.get<void*>(m_pubmems.agentTransform);
                if (transform) {
                    if (pCharData->isPlayer) {
                        pCharData->pAgentData->realSpeed = transform.get<float>(m_pubmems.agtransRealSpeed);
                    } else {
                        pCharData->pAgentData->speed =
                        pCharData->pAgentData->maxSpeed =
                        pCharData->pAgentData->realSpeed =
                        transform.get<float>(m_pubmems.npc_agtransSpeed);
                    }
                }
            }

            bAgentDataFound = true;
        }

        if (!bAgentDataFound) {
            pCharData->pAgentData = nullptr;
        }

        // set own character
        if (pCharacter == charctx.get<void*>(m_pubmems.charctxControlled)) {
            m_gameData.objData.ownCharacter = pCharData;
            bOwnCharFound = true;
        }
    }

    // remove non valid chars from list
    for (uint32_t i = 0; i < m_gameData.objData.charDataList.size(); i++) {
        if (!m_gameData.objData.charDataList[i]) {
            continue;
        }

        hl::ForeignClass pChar = charArray[i];

        if (i >= sizeCharArray || !pChar || pChar != m_gameData.objData.charDataList[i]->pCharacter) {
            GameData::CharacterData *c = m_gameData.objData.charDataList[i].get();
            c->pAgentData = nullptr;
            m_gameData.objData.charDataList[i] = nullptr;
        }
    }

    if (!bOwnCharFound)
        m_gameData.objData.ownCharacter = nullptr;

    return true;
}

bool Gw2HackMain::SetupPlayerArray() {
    hl::ForeignClass ctx = m_mems.pCtx;
    if (!ctx) return false;

    hl::ForeignClass charctx = ctx.get<void*>(m_pubmems.contextChar);
    if (!charctx) return false;

    auto playerArray = charctx.get<GameData::ANet::Collection<void*>>(m_pubmems.charctxPlayerArray);
    if (!playerArray.IsValid()) return false;

    uint32_t sizePlayerArray = playerArray.Count();
    if (sizePlayerArray != m_gameData.objData.playerDataList.size()) {
        m_gameData.objData.playerDataList.resize(sizePlayerArray);
    }

    for (uint32_t i = 0; i < sizePlayerArray; i++) {
        hl::ForeignClass pPlayer = playerArray[i];

        if (pPlayer) {
            hl::ForeignClass pChar = pPlayer.call<void*>(m_pubmems.playerVtGetChar);
            if (pChar) {
                int agentId = pChar.call<int>(m_pubmems.charVtGetAgentId);

                if (!m_gameData.objData.playerDataList[i] || m_gameData.objData.playerDataList[i]->pPlayer != pPlayer) {
                    m_gameData.objData.playerDataList[i] = std::make_unique<GameData::PlayerData>();
                }

                GameData::PlayerData *pPlayerData = m_gameData.objData.playerDataList[i].get();

                pPlayerData->pChar = pChar;

                // update values
                RefreshDataPlayer(pPlayerData, pPlayer);

                bool playerDataFound = false;

                // link agentdata of corresponding agent
                if (m_gameData.objData.agentDataList.size() && m_gameData.objData.agentDataList[agentId]) {
                    pPlayerData->pAgentData = m_gameData.objData.agentDataList[agentId].get();
                    pPlayerData->pCharData = pPlayerData->pAgentData->pCharData;
                    pPlayerData->pAgentData->pPlayerData = pPlayerData;

                    if (pPlayerData->pAgentData->pAgent) {
                        hl::ForeignClass transform = pPlayerData->pAgentData->pAgent.get<void*>(m_pubmems.agentTransform);
                        if (transform) {
                            pPlayerData->pAgentData->speed = transform.get<float>(m_pubmems.agtransSpeed);
                            pPlayerData->pAgentData->maxSpeed = transform.get<float>(m_pubmems.agtransMaxSpeed);
                        }
                    }

                    playerDataFound = true;
                }

                if (!playerDataFound) {
                    pPlayerData->pAgentData = nullptr;
                    pPlayerData->pCharData = nullptr;
                }
            }
        }
    }

    // remove non valid players from list
    for (uint32_t i = 0; i < m_gameData.objData.playerDataList.size(); i++) {
        if (!m_gameData.objData.playerDataList[i]) {
            continue;
        }

        hl::ForeignClass pPlayer = playerArray[i];

        if (i >= sizePlayerArray || !pPlayer || pPlayer != m_gameData.objData.playerDataList[i]->pPlayer) {
            GameData::PlayerData *p = m_gameData.objData.playerDataList[i].get();
            p->pAgentData = nullptr;
            p->pCharData = nullptr;
            m_gameData.objData.playerDataList[i] = nullptr;
        }
    }

    return true;
}


void Gw2HackMain::GameHook()
{
    m_mems.pCtx = GetContext();

    SetupCamData();

    if (m_gameData.camData.valid)
    {
        //m_gameData.objData.agentList.Update();
        SetupAgentArray();
        SetupCharacterArray();
        SetupPlayerArray();
    }

    hl::ForeignClass asctx = m_mems.pAgentSelectionCtx;

    m_gameData.mouseInWorld = asctx.get<D3DXVECTOR3>(m_pubmems.asctxStoW);
    m_gameData.asCtxMode = asctx.get<int>(m_pubmems.asctxCtxMode);

    if (m_mems.pMapId) m_gameData.mapId = *m_mems.pMapId;
    if (m_mems.pPing) m_gameData.ping = *m_mems.pPing;
    if (m_mems.pFps) m_gameData.fps = *m_mems.pFps;
    if (m_mems.pIfHide) m_gameData.ifHide = *m_mems.pIfHide;
    if (m_mems.pMapOpen) m_gameData.mapOpen = *m_mems.pMapOpen;
    if (m_mems.pActionCam) m_gameData.actionCam = *m_mems.pActionCam;
    if (m_mems.pIpAddr) m_gameData.ipAddr = *m_mems.pIpAddr;

    hl::ForeignClass comp = m_mems.pCompass;
    if (!m_gameData.objData.compData) m_gameData.objData.compData = std::make_unique<GameData::CompassData>();
    RefreshDataCompass(m_gameData.objData.compData.get(), comp);

    hl::ForeignClass uiOpts = m_mems.pUiOpts;
    m_gameData.uiIntSize = uiOpts.get<GW2LIB::GW2::UiIntefaceSize>(m_pubmems.uiIntSize);
    m_gameData.uiFlags1 = uiOpts.get<uint32_t>(m_pubmems.uiFlags1);
    m_gameData.uiFlags2 = uiOpts.get<uint32_t>(m_pubmems.uiFlags2);
    m_gameData.uiFlags3 = uiOpts.get<uint32_t>(m_pubmems.uiFlags3);
    m_gameData.uiFlags4 = uiOpts.get<uint32_t>(m_pubmems.uiFlags4);
    m_gameData.uiFlags5 = uiOpts.get<uint32_t>(m_pubmems.uiFlags5);

    //m_gameData.uiFlags5 |= 0x1000; // server stats

    uiOpts.set<uint32_t>(m_pubmems.uiFlags5, m_gameData.uiFlags5);

    if (m_cbGameHook) {
        [&]() {
            __try {
                m_cbGameHook();
            } __except (HLGW2_EXCEPTION("[GameHook callback] Exception in GameHook code")) {
                ;
            }
        }();
    }
}


void __fastcall hkGameThread(uintptr_t pInst, int, int frame_time)
{
    auto pCore = g_initObj.getMain();

    static auto orgFunc = ((void(__thiscall*)(uintptr_t, int))pCore->m_hkAlertCtx->getLocation());

    if (pCore)
    {
        std::lock_guard<std::mutex> lock(pCore->m_gameDataMutex);

        [&]{
            __try {
                pCore->GameHook();
            } __except (HLGW2_EXCEPTION("[hkGameThread] Exception in game thread")) {
                ;
            }
        }();
    }

    orgFunc(pInst, frame_time);
}

HRESULT __stdcall hkPresentDx11(IDXGISwapChain* pSwap, UINT syncInterval, UINT flags)
{
    auto pCore = g_initObj.getMain();
    std::lock_guard<std::mutex> lock(pCore->m_gameDataMutex);
    static auto orgFunc = decltype(&hkPresentDx11)(pCore->m_hkPresent->getLocation());

    if (pCore)
    {
        [&] {
            __try {
                pCore->RenderHookDx11();
            }
            __except (HLGW2_EXCEPTION("[RenderHookDx11] Exception in render thread")) {
                ;
            }
        }();
    }

    return orgFunc(pSwap, syncInterval, flags);
}

HRESULT __stdcall hkResizeBuffDx11(IDXGISwapChain* pSwap, UINT buffCnt, UINT width, UINT height, DXGI_FORMAT fmt, UINT flags)
{
    auto pCore = g_initObj.getMain();
    static auto orgFunc = decltype(&hkResizeBuffDx11)(pCore->m_hkResizeBuff->getLocation());

    if (pCore)
    {
        [&] {
            __try {
                pCore->ResizeBuffersDx11();
            }
            __except (HLGW2_EXCEPTION("[ResizeBuffDx11] Exception in resize buffers")) {
                ;
            }
        }();
    }

    return orgFunc(pSwap, buffCnt, width, height, fmt, flags);
}
