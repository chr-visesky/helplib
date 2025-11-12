#ifndef GW2_HACK_MAIN_H
#define GW2_HACK_MAIN_H

#include <Windows.h>
#include <mutex>

#include "hacklib/Main.h"
#include "hacklib/ConsoleEx.h"
#include "hacklib/Hooker.h"
#include "hacklib/Logging.h"
#include "hacklib/PatternScanner.h"
#include "hacklib/Process.h"
#include "hacklib/ExeFile.h"

#include "gw2lib.h"
#include "offsets.h"
#include "GameData.h"

// 统一异常记录（gw2 其他源码可能会用到该宏）
#define HLGW2_EXCEPTION(msg) ExceptHandler(msg, GetExceptionCode(), GetExceptionInformation(), __FILE__, __FUNCTION__, __LINE__)

class Gw2HackMain : public hl::Main
{
public:
    Gw2HackMain();
    virtual ~Gw2HackMain();

    // hl::Main
    virtual bool init() override;
    virtual void destroy() override;

    // 提供真实的窗口句柄（用 Win32 在当前进程内搜索顶层窗口）
    HWND GetWindowHandle();

private:
    bool init_internal();
    void destroy_internal();

    static void ExceptHandler(const char* msg,
                              unsigned int code,
                              _EXCEPTION_POINTERS* ep,
                              const char* file,
                              const char* func,
                              int line);

    static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lp);

private:
    std::mutex m_lock;
    bool m_inited;
    HWND m_hwnd;

    hl::Hooker m_hooker; // 仅保留通用钩子器（与渲染无关）
};

extern Gw2HackMain g_Main;

#endif // GW2_HACK_MAIN_H
