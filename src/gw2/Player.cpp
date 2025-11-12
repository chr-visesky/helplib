#include "main.h"


using namespace GW2LIB;


Player::Player()
{
    m_ptr = nullptr;
}

Player::Player(const Player &player)
{
    m_ptr = player.m_ptr;
}

Player &Player::operator= (const Player &player)
{
    if (this != &player) {
        m_ptr = player.m_ptr;
    }
    return *this;
}

bool Player::operator== (const Player &player) {
    return player.m_ptr == this->m_ptr;
}


bool Player::IsValid() const {
    return m_ptr && GameData::GetPlayerData(m_ptr->pPlayer);
}


bool Player::BeNext()
{
    const auto& players = GetMain()->GetGameData()->objData.playerDataList;

    iterator++;
    while (iterator < players.size()) {
        if (players[iterator]) {
            m_ptr = players[iterator].get();
            return true;
        }
        iterator++;
    }

    m_ptr = nullptr;
    return false;
}

void Player::BeSelf()
{
    if (GetMain()->GetGameData()->objData.ownAgent) {
        m_ptr = GetMain()->GetGameData()->objData.ownAgent->pPlayerData;
    } else {
        m_ptr = nullptr;
    }
}


Agent Player::GetAgent() const
{
    Agent agent;
    if (m_ptr)
        agent.m_ptr = m_ptr->pAgentData;
    return agent;
}

Character Player::GetCharacter() const
{
    Character ch;
    if (m_ptr)
        ch.m_ptr = m_ptr->pCharData;
    return ch;
}

int Player::GetCurrency(GW2::CurrencyType type) {
    if (type >= GW2::CURRENCY_END) return 0;
    if (m_ptr && m_ptr->pWallet) {
        return m_ptr->pWallet.call<int>(GetMain()->GetGameOffsets()->walletVtGetCurrency, type);
    }
    return 0;
}

int Player::GetMasteryLevel() const {
    return m_ptr ? m_ptr->masteryLvl : 0;
}

int Player::GetAP() const {
    return m_ptr ? m_ptr->ap : 0;
}

GW2::Specialization GW2LIB::Player::GetSpecType(GW2::SpecSlot slot) {
    return m_ptr && slot < GW2LIB::GW2::SPEC_SLOT_END ? m_ptr->specs[slot] : GW2LIB::GW2::SPEC_NONE;
}

GW2::Trait GW2LIB::Player::GetTraitType(GW2::SpecSlot sslot, GW2::TraitSlot tslot) {
    using namespace GW2LIB::GW2;
    if (!m_ptr || sslot >= SPEC_SLOT_END || tslot >= TRAIT_SLOT_END) return TRAIT_NONE;
    return m_ptr->traits[sslot][tslot];
}

bool GW2LIB::Player::HasEliteSpec() {
    using namespace GW2LIB::GW2;

    if (!m_ptr) return false;

    switch (m_ptr->specs[SPEC_SLOT_3]) {
    case SPEC_GUARD_DRAGONHUNTER:
    case SPEC_MES_CHRONOMANCER:
    case SPEC_ELE_TEMPEST:
    case SPEC_ENGI_SCRAPPER:
    case SPEC_THIEF_DAREDEVIL:
    case SPEC_NECRO_REAPER:
    case SPEC_RANGER_DRUID:
    case SPEC_WAR_BERSERKER:
    case SPEC_REV_HERALD:
        return true;
    }

    return false;
}

bool GW2LIB::Player::HasPOFEliteSpec() {
    using namespace GW2LIB::GW2;

    if (!m_ptr) return false;

    switch (m_ptr->specs[SPEC_SLOT_3]) {
    case SPEC_GUARD_FIREBRAND:
    case SPEC_MES_MIRAGE:
    case SPEC_ELE_WEAVER:
    case SPEC_ENGI_HOLOSMITH:
    case SPEC_THIEF_DEADEYE:
    case SPEC_NECRO_SCOURGE:
    case SPEC_RANGER_SOULBEAST:
    case SPEC_WAR_SPELLBREAKER:
    case SPEC_REV_RENEGADE:
        return true;
    }

    return false;
}

bool GW2LIB::Player::HasEODEliteSpec() {
    using namespace GW2LIB::GW2;

    if (!m_ptr) return false;

    switch (m_ptr->specs[SPEC_SLOT_3]) {
    case SPEC_GUARD_WILLBENDER:
    case SPEC_MES_VIRTUOSO:
    case SPEC_NECRO_HARBINGER:
    case SPEC_ELE_CATALYST:
    case SPEC_WAR_BLADESWORN:
    case SPEC_REV_VINDICATOR:
    case SPEC_RANGER_UNTAMED:
    case SPEC_THIEF_SPECTER:
    case SPEC_ENGI_MECHANIST:
        return true;
    }

    return false;
}

std::string Player::GetName() const {
    return m_ptr ? m_ptr->name : "";
}

UUID GW2LIB::Player::GetUUID() const {
    if (!m_ptr) return { 0 };
    return m_ptr->uuid;
}

std::string GW2LIB::Player::GetUUID2Str() const {
    if (!m_ptr) return "";

    UUID *u = &m_ptr->uuid;
    char uuid[37];

    GetMain()->GUID2Str(u, sizeof(uuid), uuid);

    return std::string(uuid);
}
