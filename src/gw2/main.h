#ifndef GW2HACK_MAIN_H
#define GW2HACK_MAIN_H

#include "gw2lib.h"
#include "offsets.h"
#include "GameData.h"

#include "hacklib/Main.h"
#include "hacklib/ConsoleEx.h"
#include "hacklib/Hooker.h"
#include "hacklib/DrawerD3D.h"
#include "hacklib/WindowOverlay.h"

#include <mutex>

#define HLGW2_EXCEPTION(msg) ExceptHandler(msg, GetExceptionCode(), GetExceptionInformation(), __FILE__, __FUNCTION__, __LINE__)

#define DEBUG_TRACER_LIST_MAX 10
#define TRACER(format) (GetMain()->debug_tracer_list_push(format))
#define TRACER_PRINT (GetMain()->debug_tracer_list_print())

class Gw2HackMain *GetMain();
int64_t GetTimestamp();
DWORD ExceptHandler(const char*, DWORD, EXCEPTION_POINTERS*, const char*, const char*, int);
std::string ConvertWcharToStr(const wchar_t *in);

template<typename T>
void __fastcall DecodeTextCB(std::string* ctx, wchar_t *decodedText);
void __fastcall DecodeTextStripColorCB(std::string* ctx, wchar_t *decodedText);

#define CALL_FN(conv) ((T(conv*)(Ts...))m_ptr)(args...)
enum CallingConvention {
    CALL_CONV_FASTCALL,
    CALL_CONV_CDECL,
    CALL_CONV_STDCALL,
    CALL_CONV_THISCALL,
    CALL_CONV_VECTORCALL
};

template <typename T, CallingConvention cv = CALL_CONV_CDECL>
class ForeignFunction {
public:
    ForeignFunction() : m_ptr(nullptr) {}
    ForeignFunction(void *ptr) : m_ptr(ptr) {}
    ForeignFunction(uintptr_t ptr) { m_ptr = (void*)ptr; }

    template <typename... Ts>
    T operator()(Ts... args) {
        if (!m_ptr) throw EXCEPTION_ACCESS_VIOLATION;

        switch (cv) {
        case CALL_CONV_FASTCALL:
            return CALL_FN(__fastcall);
            break;

        case CALL_CONV_CDECL:
            return CALL_FN(__cdecl);
            break;

        case CALL_CONV_STDCALL:
            return CALL_FN(__stdcall);
            break;

        case CALL_CONV_THISCALL:
            return CALL_FN(__thiscall);
            break;

        case CALL_CONV_VECTORCALL:
            return CALL_FN(__vectorcall);
            break;

        default:
            throw EXCEPTION_ACCESS_VIOLATION;
        }
    }

    explicit operator bool() const {
        return !!m_ptr;
    }

private:
    void *m_ptr;

};


struct RenderState {
    D3DRENDERSTATETYPE state;
    DWORD value;
};

struct GamePointers
{
    int *pMapId = nullptr;
    int *pPing = nullptr;
    int *pFps = nullptr;
    int *pIfHide = nullptr;
    int *pMapOpen = nullptr;
    int *pActionCam = nullptr;
    void *pAlertCtx = nullptr;
    void *pCtx = nullptr;
    void *pAgentViewCtx = nullptr;
    void *pWorldView = nullptr;
    void *pWorldViewContext = nullptr;
    void *pAgentSelectionCtx = nullptr;
    void *pCompass = nullptr;
    void *pUiOpts = nullptr;
    void *pCam = nullptr;
    void* pMumble = nullptr;
    sockaddr_in *pIpAddr = nullptr;

    uintptr_t avctxAgentArray = 0;
};


class Gw2HackMain : public hl::Main
{
public:
    bool init() override;
    void shutdown() override;

    const GamePointers *GetGamePointers() const { return &m_mems; }
    const GW2LIB::Mems *GetGameOffsets() const { return &m_pubmems; }

    GW2LIB::D3D11COMs *GetD3D11Coms();
    const GameData::GameData *GetGameData() const;

    uint64_t GetWindowHandle();

    void SetRenderCallback(void (*cbRender)());
    void SetGameHookCallback(void(*cbGameHook)());
    void SetResizeBuffCallback(void(*cbResizeBuff)());

    void RenderHookDx11();
    void ResizeBuffersDx11();
    void GameHook();
    void GameCtxCheck();

    //std::string DecodeText(void* codedTxt);
    void DecodeText(void* codedTxt, std::string* dst, bool stripColor = false);
    void StripColorTag(std::string* txt);

    const hl::IHook *m_hkPresent = nullptr;
    const hl::IHook *m_hkAlertCtx = nullptr;
    const hl::IHook* m_hkResizeBuff = nullptr;

    size_t debug_tracer_list_idx = 0;
    const char* debug_tracer_list[DEBUG_TRACER_LIST_MAX] = { 0 };
    void debug_tracer_list_push(const char* format);
    void debug_tracer_list_print();
    std::string debug_tracer_string;

    std::mutex m_gameDataMutex;
    Gw2GameHook m_gw2Hook;
    std::unordered_set<std::string*> m_decodedStrings;

    ForeignFunction<void, CALL_CONV_FASTCALL> DecodeGameText; // void __fastcall DecodeText(uint8_t *codedText, resultFunc, uintptr_t ctx)
    ForeignFunction<void*> GetContext;
    ForeignFunction<void, CALL_CONV_FASTCALL> SetContext; // void __fastcall SetContext(uintptr_t ctx);
    ForeignFunction<void, CALL_CONV_FASTCALL> GUID2Str; // void __fastcall GUID2Str(UUID *uuid, uint32_t dst_size, uintptr_t dst);
    ForeignFunction<void*> GetCharacter; // void *GetCharacter(void *agent);
    ForeignFunction<void> LockCamera; // void LockCamera(void *wmAgent);
    ForeignFunction<void, CALL_CONV_THISCALL> LockCamera2; // void __thiscall LockCamera2(pCam+camWmAgent, void *wmAgent);
    ForeignFunction<void, CALL_CONV_THISCALL> AddDrunkLevel; // void __thiscall AddDrunkLevel(ctxChar, uint32_t lvl);
    ForeignFunction<void*> GetCodedText; // void* GetCodedText(uint32_t hashId, uint32_t param2);
    ForeignFunction<void*, CALL_CONV_FASTCALL> GetCodedItemName; // void* __fastcall GetCodedItemName(void* baseItem, void* skinName, void* prefix, void* suffix, uint32_t count, void* unk1, uint32_t unk2)

private:
    void RefreshDataAgent(GameData::AgentData *pAgentData, hl::ForeignClass agent);
    void RefreshDataCharacter(GameData::CharacterData *pCharData, hl::ForeignClass character);
    void RefreshDataPlayer(GameData::PlayerData *pPlayerData, hl::ForeignClass player);
    void RefreshDataCompass(GameData::CompassData *pCompData, hl::ForeignClass comp);
    void RefreshDataGadget(GameData::GadgetData *pGadgetData, hl::ForeignClass gd);
    void RefreshDataAttackTarget(GameData::AttackTargetData *pGadgetTgtData, hl::ForeignClass gd);
    void RefreshDataResourceNode(GameData::ResourceNodeData *pRNodeData, hl::ForeignClass res);
    void RefreshDataBuff(GameData::BuffData *pBuffData, hl::ForeignClass buff);
    void RefreshDataEquipItem(GameData::EquipItemData *pItemData, hl::ForeignClass item);
    void RefreshDataItem(GameData::ItemData *pItemData, hl::ForeignClass item);

    bool SetupCamData();
    bool SetupAgentArray();
    bool SetupCharacterArray();
    bool SetupPlayerArray();

    hl::WindowHandle m_hwnd;

    hl::LogConfig logConfig;
    hl::ConsoleEx m_con;
    hl::Hooker m_hooker;

    GW2LIB::D3D11COMs m_d3d11coms;

    GameData::GameData m_gameData;

    bool m_bPublicDrawer = false;
    void(*m_cbRender)() = nullptr;
    void(*m_cbGameHook)() = nullptr;
    void(*m_cbResizeBuff)() = nullptr;

    bool ScanForOffsets();

    GamePointers m_mems;
    GW2LIB::Mems m_pubmems;

};

#endif