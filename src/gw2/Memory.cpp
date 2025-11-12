#include "main.h"

using namespace GW2LIB;

Memory::Memory() {
    m_ptr = nullptr;
}

Memory::Memory(size_t cat) {
    SetCategory(cat);
}

Memory &Memory::operator=(const size_t &cat) {
    SetCategory(cat);
    return *this;
}

bool Memory::BeNext() {
    const auto &objData = GetMain()->GetGameData()->objData;
    if (!objData.memCatList) {
        m_ptr = nullptr;
        return false;
    }

    uintptr_t memCats = objData.memCatList;
    size_t size = *objData.memCatSize;

    while (iterator < size) {
        void *mem = *(void**)(memCats + (sizeof(uintptr_t) * iterator));
        if (mem) {
            m_ptr = mem;
            iterator++;
            return true;
        }
        iterator++;
    }

    m_ptr = nullptr;
    return false;
}

bool Memory::SetCategory(size_t cat) {
    const auto &objData = GetMain()->GetGameData()->objData;
    if (!objData.memCatList) {
        m_ptr = nullptr;
        return false;
    }

    uintptr_t memCats = objData.memCatList;
    size_t size = *objData.memCatSize;

    if (cat < size) {
        void *mem = *(void**)(memCats + (sizeof(uintptr_t) * cat));
        if (mem) {
            m_ptr = mem;
            return true;
        }
    }

    m_ptr = nullptr;
    return false;
}

uint32_t Memory::GetCategory() {
    if (!m_ptr) return -1;
    hl::ForeignClass mem = m_ptr;
    return mem.get<uint32_t>(0x4);
}

wchar_t* Memory::GetName() {
    if (!m_ptr) return L"";
    hl::ForeignClass mem = m_ptr;
    return mem.call<wchar_t*>(0x0);
}

uint32_t Memory::GetSize() {
    if (!m_ptr) return 0;
    hl::ForeignClass mem = m_ptr;
    uintptr_t st = mem.call<uintptr_t>(0x8);
    return *(uint32_t*)(st + 0x0);
}

uint32_t Memory::GetCount() {
    if (!m_ptr) return 0;
    hl::ForeignClass mem = m_ptr;
    uintptr_t st = mem.call<uintptr_t>(0x8);
    return *(uint32_t*)(st + 0x4);
}


