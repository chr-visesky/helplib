#include "main.h"

static bool IsTopLevelVisibleWindowOfCurrentProcess(HWND hWnd)
{
    if (!::IsWindow(hWnd) || !::IsWindowVisible(hWnd)) return false;
    if (::GetWindow(hWnd, GW_OWNER) != NULL) return false;
    DWORD pid = 0;
    ::GetWindowThreadProcessId(hWnd, &pid);
    return pid == ::GetCurrentProcessId();
}

BOOL CALLBACK Gw2HackMain::EnumWindowsProc(HWND hWnd, LPARAM lp)
{
    HWND* out = reinterpret_cast<HWND*>(lp);
    if (IsTopLevelVisibleWindowOfCurrentProcess(hWnd))
    {
        *out = hWnd;
        return FALSE; // stop
    }
    return TRUE; // continue
}

Gw2HackMain g_Main;

Gw2HackMain::Gw2HackMain()
    : m_inited(false),
      m_hwnd(NULL)
{
}

Gw2HackMain::~Gw2HackMain()
{
    if (m_inited) destroy();
}

bool Gw2HackMain::init()
{
    std::lock_guard<std::mutex> _g(m_lock);
    if (m_inited) return true;

    hl::ConsoleEx::Open("helplib console");
    hl::log::info("helplib: init begin (no overlay, no D3D).");

    if (!init_internal())
    {
        hl::log::error("helplib: init failed.");
        return false;
    }

    m_inited = true;
    hl::log::info("helplib: init ok.");
    return true;
}

void Gw2HackMain::destroy()
{
    std::lock_guard<std::mutex> _g(m_lock);
    if (!m_inited) return;

    destroy_internal();
    hl::ConsoleEx::Close();

    m_inited = false;
}

bool Gw2HackMain::init_internal()
{
    // 仅做与“注入/内存读取/模式扫描”相关的准备。完全不触碰 D3D/Overlay。
    // 若你需要在此处调用 DataScan 初始化偏移，可安全添加。
    m_hwnd = NULL;
    ::EnumWindows(&Gw2HackMain::EnumWindowsProc, reinterpret_cast<LPARAM>(&m_hwnd));
    if (!m_hwnd)
        hl::log::warn("helplib: window handle not found (still ok for memory-only).");
    else
        hl::log::info("helplib: window handle = 0x%p", m_hwnd);

    return true;
}

void Gw2HackMain::destroy_internal()
{
    // 若后续添加了内存/断点等钩子，在这里统一撤销
    m_hooker.unhookAll();
    m_hwnd = NULL;
}

HWND Gw2HackMain::GetWindowHandle()
{
    if (m_hwnd && ::IsWindow(m_hwnd)) return m_hwnd;
    m_hwnd = NULL;
    ::EnumWindows(&Gw2HackMain::EnumWindowsProc, reinterpret_cast<LPARAM>(&m_hwnd));
    return m_hwnd;
}

void Gw2HackMain::ExceptHandler(const char* msg,
                                unsigned int code,
                                _EXCEPTION_POINTERS* /*ep*/,
                                const char* file,
                                const char* func,
                                int line)
{
    hl::log::error("EXCEPTION: %s, code=0x%08X at %s:%d (%s)",
                   msg ? msg : "(null)", code,
                   file ? file : "(null)",
                   line, func ? func : "(null)");
}
