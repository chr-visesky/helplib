#include "main.h"

using namespace GW2LIB;

Item::Item() {
    m_ptr = nullptr;
}

Item::Item(const Item &i) {
    m_ptr = i.m_ptr;
}

Item &Item::operator= (const Item &i) {
    if (this != &i) {
        m_ptr = i.m_ptr;
    }
    return *this;
}

bool Item::operator== (const Item &i) {
    return i.m_ptr == this->m_ptr;
}

bool Item::IsValid() const {
    return !!m_ptr;
}

uint32_t GW2LIB::Item::GetID() {
    return m_ptr ? m_ptr->id : 0;
}

GW2::ItemRarity Item::GetRarity() {
    return m_ptr ? m_ptr->rarity : GW2::RARITY_NONE;
}

std::string GW2LIB::Item::GetName() const {
    if (!m_ptr) return "";
    return m_ptr->name;
}
