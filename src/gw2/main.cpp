#define NOMINMAX
#include <Windows.h>
#include <TlHelp32.h>
#include <cstdio>
#include <cstdint>
#include <functional>
#include <vector>
#include <string>
#include <algorithm>

#include "gw2lib.h"
#include "offsets.h"
#include "GameData.h"

#include "hacklib/Logging.h"
#include "hacklib/PatternScanner.h"
#include "hacklib/ExeFile.h"
#include "hacklib/Process.h"
#include "hacklib/Hooker.h"
#include "hacklib/ConsoleEx.h"

#include "main.h"

// 关闭一切 D3D / OpenGL Hook 路径；不再需要 D3DDeviceFetcher / WindowOverlay
// 不依赖外部开关，直接在源码里固定为“禁用”
#ifndef NOD3DHOOK
#define NOD3DHOOK 1
#endif

// 全局对象
Gw2HackMain g_Main;

static bool IsTopLevelVisibleWindowOfCurrentProcess(HWND h)
{
    if (!IsWindow(h)) return false;
    if (!IsWindowVisible(h)) return false;
    if (GetWindow(h, GW_OWNER) != NULL) return false;

    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    return pid == GetCurrentProcessId();
}

static HWND FindTopLevelWindowOfCurrentProcess()
{
    HWND result = NULL;
    EnumWindows([](HWND h, LPARAM lp)->BOOL {
        HWND* pout = reinterpret_cast<HWND*>(lp);
        if (IsTopLevelVisibleWindowOfCurrentProcess(h)) {
            *pout = h;
            return FALSE; // stop
        }
        return TRUE; // continue
    }, reinterpret_cast<LPARAM>(&result));
    return result;
}

Gw2HackMain::Gw2HackMain()
    : m_inited(false)
    , m_hwnd(NULL)
    , m_hkPresentAddr(0)
    , m_hkResizeAddr(0)
    , m_hkDX12PresentAddr(0)
    , m_shouldExit(false)
    , m_consoleVisible(false)
{
}

Gw2HackMain::~Gw2HackMain()
{
    // 防御
    if (m_inited)
        destroy();
}

bool Gw2HackMain::init()
{
    std::lock_guard<std::mutex> _g(m_lock);
    if (m_inited) return true;

    hl::log::info("GW2 helper starting (no overlay/no D3D).");
    if (!init_internal())
        return false;

    m_inited = true;
    return true;
}

void Gw2HackMain::destroy()
{
    std::lock_guard<std::mutex> _g(m_lock);
    if (!m_inited) return;

    destroy_internal();
    m_inited = false;
}

bool Gw2HackMain::init_internal()
{
    // 打开控制台（可选）
    hl::ConsoleEx::Open("GW2 Helper Console");
    m_consoleVisible = true;

    // 这里不再做任何 D3D / OpenGL 相关逻辑
    // 只做与进程/模块/偏移扫描相关的初始化（如果你后续需要，可在此处调用 DataScan 等）

    // 定位一次窗口（可用于后续只读功能里做坐标/鼠标换算等）
    m_hwnd = FindTopLevelWindowOfCurrentProcess();
    if (!m_hwnd) {
        hl::log::warn("Window handle not found. Proceeding without it.");
    } else {
        hl::log::info("Window handle acquired: 0x%p", m_hwnd);
    }

    return true;
}

void Gw2HackMain::destroy_internal()
{
    // 释放钩子（当前版本没有渲染钩子；如果将来加了内存读相关硬件断点等，这里统一清理）
    m_hooker.unhookAll();

    if (m_consoleVisible) {
        hl::ConsoleEx::Close();
        m_consoleVisible = false;
    }
}

HWND Gw2HackMain::GetWindowHandle()
{
    if (m_hwnd != NULL) return m_hwnd;
    m_hwnd = FindTopLevelWindowOfCurrentProcess();
    return m_hwnd;
}

GW2LIB::D3D11COMs* Gw2HackMain::GetD3D11Coms()
{
    // 为了与既有接口兼容，仍然返回结构体指针，但内部全部为 NULL
    // 外部如有调用，应先判断是否非空再使用（本版本始终为空）
    return &m_d3d11coms;
}

void Gw2HackMain::ExceptHandler(const char* msg, unsigned int code, _EXCEPTION_POINTERS* /*ep*/, const char* file, const char* func, int line)
{
    hl::log::error("Exception caught: %s (code=0x%08X) at %s:%d in %s", msg ? msg : "(null)", code, file ? file : "(null)", line, func ? func : "(null)");
}
