#include "hacklib/Input.h"
#include <cstddef>
#ifdef _WIN32
#include <Windows.h>
#endif


using namespace hl;


void Input::update()
{
    for (std::size_t i = 0; i < m_status.size(); i++)
    {
        bool keyDown = false;
#ifdef _WIN32
        keyDown = GetAsyncKeyState(static_cast<int>(i)) < 0;
#endif

        if (keyDown && !m_status[i].isDown)
            m_status[i].state = InputState::WentDown;
        else if (!keyDown && m_status[i].isDown)
            m_status[i].state = InputState::WentUp;
        else
            m_status[i].state = InputState::Idle;

        m_status[i].isDown = keyDown;
    }
}


bool Input::isDown(int vkey) const
{
    return m_status[vkey].isDown;
}

bool Input::wentDown(int vkey) const
{
    return m_status[vkey].state == InputState::WentDown;
}

bool Input::wentUp(int vkey) const
{
    return m_status[vkey].state == InputState::WentUp;
}