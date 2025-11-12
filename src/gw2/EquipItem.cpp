#include "main.h"

using namespace GW2LIB;

EquipItem::EquipItem() {
    m_ptr = nullptr;
}

EquipItem::EquipItem(const EquipItem &i) {
    m_ptr = i.m_ptr;
}

EquipItem &EquipItem::operator= (const EquipItem &i) {
    if (this != &i) {
        m_ptr = i.m_ptr;
    }
    return *this;
}

bool EquipItem::operator== (const EquipItem &i) {
    return i.m_ptr == this->m_ptr;
}

bool EquipItem::IsValid() const {
    return !!m_ptr;
}

bool EquipItem::BeNext() {
    if (!dataList) return false;
    const auto& list = *dataList;

    while (iterator != list.end()) {
        if (iterator->second) {
            m_ptr = iterator->second.get();
            iterator++;
            return true;
        }
        iterator++;
    }

    m_ptr = nullptr;
    return false;
}

Character EquipItem::GetSource() {
    Character src;
    if (m_ptr && m_ptr->pCharData)
        src.m_ptr = m_ptr->pCharData;
    return src;
}

GW2::EquipmentSlot GW2LIB::EquipItem::GetEquipSlot() {
    return m_ptr ? m_ptr->equipSlot : GW2::EQUIP_SLOT_NONE;
}

Item GW2LIB::EquipItem::GetItem() {
    Item item;
    if (m_ptr && m_ptr->itemDef.pItemDef)
        item.m_ptr = &m_ptr->itemDef;
    return item;
}

