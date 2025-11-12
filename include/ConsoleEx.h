#ifndef HACKLIB_CONSOLEEX_H
#define HACKLIB_CONSOLEEX_H

#include <cstdarg>
#include <functional>
#include <future>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace hl
{
#ifdef _WIN32

struct CONSOLEEX_PARAMETERS
{
    int cellsX = 80;
    int cellsYVisible = 20;
    int cellsYBuffer = 500;
    COLORREF bgColor = RGB(0, 0, 0);
    COLORREF textColor = RGB(192, 192, 192);
    bool closemenu = false;
};

class ConsoleEx
{
    ConsoleEx(const ConsoleEx&) = delete;
    ConsoleEx& operator=(const ConsoleEx&) = delete;

public:
    static CONSOLEEX_PARAMETERS GetDefaultParameters();

    ConsoleEx(HINSTANCE hModule = NULL);
    ~ConsoleEx();

    bool create(const std::string& windowTitle, CONSOLEEX_PARAMETERS* parameters = nullptr);

    void registerInputCallback(void (*cbInput)(std::string));
    template <class F>
    void registerInputCallback(F cbInput)
    {
        m_cbInput = cbInput;
    }

    void close();
    bool isOpen() const;
    HWND getWindowHandle() const;

    void vprintf(const char* format, va_list valist);
    void printf(const char* format, ...);
    void print(const std::string& str);

private:
    static LRESULT CALLBACK s_WndProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
    static LRESULT CALLBACK s_EditOutSubclassProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam,
                                                  UINT_PTR subclassId, DWORD_PTR refData);

    LRESULT wndProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
    LRESULT editOutSubclassProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
    void threadProc(std::promise<bool>& p);

    void writeString(const char* strOut, int len);
    void writeStringToRawBuffer(const char* strOut, int len);

    void updateScrollState(bool checkTrackPos);

    std::thread m_thread;
    HINSTANCE m_hModule = NULL;
    HWND m_hWnd = NULL;
    HWND m_hEditOut = NULL;
    HWND m_hEditIn = NULL;
    HBRUSH m_hbrBackGround = NULL;
    std::string m_title;
    CONSOLEEX_PARAMETERS m_parameters = GetDefaultParameters();
    std::function<void(std::string)> m_cbInput;
    bool m_isOpen = false;
    bool m_isScrolledToBottom = true;
    std::vector<char> m_bufferForScrollLock;
    std::vector<int> m_bufferForScrollLockLengths;

    class RingBuffer
    {
    public:
        void resize(int width, int height)
        {
            m_wid = width;
            m_hei = height;
            m_buffer.resize((width + 2) * height);
        }
        void writeChar(wchar_t c, int pos) { m_buffer[translatePos(pos)] = c; }
        void writeToLine(wchar_t* str, int len, int pos)
        {
            memcpy(&m_buffer[translatePos(pos)], str, len * sizeof(wchar_t));
        }
        void scrollUp()
        {
            if (m_start == (m_wid + 2) * (m_hei - 1))
            {
                m_start = 0;
            }
            else
            {
                m_start += m_wid + 2;
            }
        }
        void copyTo(wchar_t* dst)
        {
            int startBlockSz = (m_wid + 2) * m_hei - m_start;
            memcpy(dst, &m_buffer[m_start], startBlockSz * sizeof(wchar_t));
            memcpy(dst + startBlockSz, &m_buffer[0], (m_start - 2) * sizeof(wchar_t));
        }

    private:
        int translatePos(int pos)
        {
            pos += m_start;
            int sz = (m_wid + 2) * m_hei;
            return pos >= sz ? pos - sz : pos;
        }

        int m_start = 0;
        int m_wid = 0, m_hei = 0;
        std::vector<wchar_t> m_buffer;
    };

    class ConsoleBuffer
    {
        ConsoleBuffer(const ConsoleBuffer&) = delete;
        ConsoleBuffer& operator=(const ConsoleBuffer&) = delete;

    public:
        ConsoleBuffer() = default;
        ~ConsoleBuffer();

        void free();
        HLOCAL getBuffer() const;
        void setBuffer(HLOCAL buffer);

        void lock();
        void unlock();

        bool resize(int width, int height, int heightVisible);
        void clear();
        void flushBackBuffer();

        void write(wchar_t* str);
        void scrollUp();

    private:
        int getBufferOffsetFromCursorPos();
        void writeChar(wchar_t c, int pos)
        {
            m_buffer[pos] = c;
            m_backBuffer.writeChar(c, pos);
        }
        void writeToLine(wchar_t* str, int len, int pos)
        {
            memcpy(&m_buffer[pos], str, len * sizeof(wchar_t));
            m_backBuffer.writeToLine(str, len, pos);
        }

        mutable std::mutex m_writeMutex;
        HLOCAL m_bufferHandle = NULL;
        wchar_t* m_buffer = nullptr;
        int m_cursorPos = 0;
        int m_wid = 0, m_hei = 0, m_heiVisible = 0;

        RingBuffer m_backBuffer;

    } m_rawBuffer;
};

#else

struct CONSOLEEX_PARAMETERS
{
    int cellsX = 80;
    int cellsYVisible = 20;
    int cellsYBuffer = 500;
    unsigned int bgColor = 0;
    unsigned int textColor = 0;
    bool closemenu = false;
};

class ConsoleEx
{
public:
    static CONSOLEEX_PARAMETERS GetDefaultParameters()
    {
        return {};
    }

    explicit ConsoleEx(void* = nullptr) {}
    ~ConsoleEx() = default;

    bool create(const std::string&, CONSOLEEX_PARAMETERS* = nullptr) { return false; }

    void registerInputCallback(void (*)(std::string)) {}
    template <class F>
    void registerInputCallback(F) {}

    void close() {}
    bool isOpen() const { return false; }
    void* getWindowHandle() const { return nullptr; }

    void vprintf(const char*, va_list) {}
    void printf(const char*, ...) {}
    void print(const std::string&) {}
};

#endif // _WIN32
}

#endif
