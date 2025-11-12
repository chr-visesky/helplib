#include <sstream>
#include <iomanip>
#include "main.h"


using namespace GW2LIB;


Agent::Agent()
{
    m_ptr = nullptr;
}

Agent::Agent(uintptr_t* pAgent)
{
    m_ptr = GameData::GetAgentData(pAgent);
}

Agent::Agent(uintptr_t pAgent) {
    m_ptr = GameData::GetAgentData((uintptr_t*)pAgent);
}


Agent::Agent(const Agent &ag)
{
    m_ptr = ag.m_ptr;
}

Agent &Agent::operator= (const Agent &ag)
{
    if (this != &ag) {
        m_ptr = ag.m_ptr;
    }
    return *this;
}

bool Agent::operator== (const Agent &ag)
{
    return ag.m_ptr == this->m_ptr;
}

bool Agent::operator!= (const Agent &ag)
{
    return !(*this == ag);
}

bool Agent::IsValid() const
{
    return m_ptr && GetMain()->GetGameData()->objData.agentDataList[m_ptr->agentId];
}

Agent::operator bool() const {
    return IsValid();
}


bool Agent::BeNext()
{
    const auto& agents = GetMain()->GetGameData()->objData.agentDataList;

    while (iterator < agents.size()) {
        if (agents[iterator]) {
            m_ptr = agents[iterator].get();
            iterator++;
            return true;
        }
        iterator++;
    }

    m_ptr = nullptr;
    return false;
}

void Agent::BeSelf()
{
    if (GetMain()->GetGameData()->objData.ownCharacter) {
        m_ptr = GetMain()->GetGameData()->objData.ownCharacter->pAgentData;
    } else {
        m_ptr = 0;
    }
}


Character Agent::GetCharacter() const
{
    Character chr;
    if (m_ptr)
        chr.m_ptr = m_ptr->pCharData;
    return chr;
}

Player Agent::GetPlayer() const
{
    Player player;
    if (m_ptr)
        player.m_ptr = m_ptr->pPlayerData;
    return player;
}

Gadget Agent::GetGadget() const
{
    Gadget gd;
    if (m_ptr)
        gd.m_ptr = m_ptr->gadgetData.get();
    return gd;
}

AttackTarget Agent::GetAttackTarget() const
{
    AttackTarget tgt;
    if (m_ptr)
        tgt.m_ptr = m_ptr->attackTgtData.get();
    return tgt;
}


GW2::AgentCategory Agent::GetCategory() const {
    return m_ptr ? m_ptr->category : GW2::AGENT_CATEGORY_CHAR;
}

GW2::AgentType Agent::GetType() const {
    return m_ptr ? m_ptr->type : GW2::AGENT_TYPE_CHAR;
}

uint32_t Agent::GetAgentId() const {
    return m_ptr ? m_ptr->agentId : 0;
}


Vector3 Agent::GetPos() const
{
    Vector3 pos = { 0, 0, 0 };
    if (m_ptr)
    {
        D3DXVECTOR3 dxPos = m_ptr->pos;
        pos.x = dxPos.x;
        pos.y = dxPos.y;
        pos.z = dxPos.z;
    }
    return pos;
}

float Agent::GetRot() const {
    return m_ptr ? m_ptr->rot : 0;
}

uint64_t Agent::GetToken() const {
    return m_ptr ? m_ptr->token : 0;
}

uint64_t Agent::GetSequence() const {
    return m_ptr ? m_ptr->seq : 0;
}

float Agent::GetSpeed() const {
    return m_ptr ? m_ptr->speed : 0;
}

float Agent::GetMaxSpeed() const {
    return m_ptr ? m_ptr->maxSpeed : 0;
}

float Agent::GetRealSpeed() const {
    return m_ptr ? m_ptr->realSpeed : 0;
}

float Agent::GetCalcSpeed() const {
    if (!m_ptr) return 0;
    Vector3 pos = GetPos();
    float dist = Dist3D(m_ptr->prevCalcSpeedPos, pos);
    m_ptr->prevCalcSpeedPos = pos;
    return dist;
}

bool Agent::IsSelectable() const {
    return m_ptr && m_ptr->selectable;
}

std::string Agent::GetName() const {
    if (!m_ptr) return "";
    if (m_ptr->name != "") return m_ptr->name;
    if (m_ptr->pCharData && m_ptr->category == GW2::AGENT_CATEGORY_CHAR) {
        Character ch;
        ch.m_ptr = m_ptr->pCharData;
        return ch.GetName();
    }

    return "";
}

// must be called from main thread
void Agent::SetSequence(uint64_t state) {
    if (m_ptr && m_ptr->pAgent) {
        const GW2LIB::Mems* offsets = GetMain()->GetGameOffsets();
        hl::ForeignClass transform = m_ptr->pAgent.get<void*>(offsets->agentTransform);
        if (transform) {
            transform.call<void>(offsets->agtranVtSetSeqSt, state);
        }
    }
}


void Agent::SetActiveSelection() {
    if (!m_ptr) return;

    auto mems = GetMain()->GetGamePointers();
    if (!mems) return;

    hl::ForeignClass asctx = mems->pAgentSelectionCtx;
    if (!asctx) return;

    //asctx.call<void>(gw2Offsets.asctxVtSetAct, m_ptr->pAgent);
}

bool Agent::LockCamera() {
    /*auto pmems = GetMain()->GetGameOffsets();
    auto mems = GetMain()->GetGamePointers();

    void *offset = (void*)((uintptr_t)mems->pCam + pmems->camWmAgent);
    auto tgt = GetMain()->GetGameData()->objData.lockedSelection;

    GetMain()->LockCamera2(offset, tgt->wmAgent.data());*/
    return true;
}
