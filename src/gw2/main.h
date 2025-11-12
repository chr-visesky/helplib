#ifndef GW2HACK_MAIN_H
#define GW2HACK_MAIN_H

#include "gw2lib.h"
#include "offsets.h"
#include "GameData.h"

#include "hacklib/Main.h"
#include "hacklib/ConsoleEx.h"
#include "hacklib/Hooker.h"
#include <mutex>

// 统一的异常宏
#define HLGW2_EXCEPTION(msg) ExceptHandler(msg, GetExceptionCode(), GetExceptionInformation(), __FILE__, __FUNCTION__, __LINE__)

// 轻量日志追踪
#define DEBUG_TRACER_LIST_MAX 10
#define TRACER(format) 

namespace GW2LIB
{
    // 直接沿用原来结构；用于对外返回 D3D 句柄（此版本不再填充）
    struct D3D11COMs
    {
        void* pSwapChain;
        void* pDevice;
        void* pDeviceContext;
        D3D11COMs() : pSwapChain(NULL), pDevice(NULL), pDeviceContext(NULL) {}
    };
}

class Gw2HackMain : public hl::Main
{
public:
    Gw2HackMain();
    virtual ~Gw2HackMain();

    // hl::Main 接口
    virtual bool init() override;
    virtual void destroy() override;

    // 外部可能调用的查询
    HWND GetWindowHandle();
    GW2LIB::D3D11COMs* GetD3D11Coms();

private:
    // 初始化/释放
    bool init_internal();
    void destroy_internal();

    // —— 下面是原有成员（去掉了 Overlay 与 Drawer 相关依赖）——
    std::mutex m_lock;
    bool m_inited;

    // 窗口句柄（用 Win32 枚举得到）
    HWND m_hwnd;

    // 保留 D3D 结构体给外部接口，但不再赋值（全部为 NULL）
    GW2LIB::D3D11COMs m_d3d11coms;

    // 钩子器等（内存读相关不受影响）
    hl::Hooker m_hooker;

    // 以下是渲染相关原先的 Present 钩子地址与对象（现不再使用，但保留占位避免连锁改动）
    uintptr_t m_hkPresentAddr;
    uintptr_t m_hkResizeAddr;
    uintptr_t m_hkDX12PresentAddr;

    // 其他业务状态位
    bool m_shouldExit;
    bool m_consoleVisible;

    // 异常处理（原宏会用到）
    void ExceptHandler(const char* msg, unsigned int code, _EXCEPTION_POINTERS* ep, const char* file, const char* func, int line);
};

extern Gw2HackMain g_Main;

#endif // GW2HACK_MAIN_H
