#include <sstream>
#include <iomanip>
#include "main.h"


using namespace GW2LIB;


Frame::Frame()
{
    m_ptr = nullptr;
}

Frame& Frame::operator= (const Frame& ag)
{
    if (this != &ag) {
        m_ptr = ag.m_ptr;
    }
    return *this;
}

bool Frame::IsValid() const
{
    return m_ptr && GetMain()->GetGameData()->objData.agentDataList[frameId];
}

Frame::operator bool() const {
    return IsValid();
}

uint32_t Frame::ListSize() {
    GameData::ANet::Collection<void*> arr(GetMain()->GetGameData()->objData.frameListPtr);
    return arr.Count();
}

bool Frame::BeNext()
{
    GameData::ANet::Collection<void*> arr(GetMain()->GetGameData()->objData.frameListPtr);
    uint32_t arrSize = arr.Count();

    while (iterator < arrSize) {
        if (arr[iterator]) {
            m_ptr = arr[iterator];
            frameId = iterator;
            iterator++;
            return true;
        }
        iterator++;
    }

    m_ptr = nullptr;
    return false;
}


