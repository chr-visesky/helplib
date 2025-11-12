#include "main.h"


GW2LIB::Character GW2LIB::GetOwnCharacter()
{
    Character chr;
    chr.m_ptr = GetMain()->GetGameData()->objData.ownCharacter;
    return chr;
}

GW2LIB::Agent GW2LIB::GetOwnAgent()
{
    Agent ag;
    ag.m_ptr = GetMain()->GetGameData()->objData.ownAgent;
    return ag;
}

GW2LIB::Agent GW2LIB::GetAutoSelection()
{
    Agent agent;
    agent.m_ptr = GetMain()->GetGameData()->objData.autoSelection;
    return agent;
}

GW2LIB::Agent GW2LIB::GetHoverSelection()
{
    Agent agent;
    agent.m_ptr = GetMain()->GetGameData()->objData.hoverSelection;
    return agent;
}

GW2LIB::Agent GW2LIB::GetLockedSelection()
{
    Agent agent;
    agent.m_ptr = GetMain()->GetGameData()->objData.lockedSelection;
    return agent;
}

GW2LIB::Vector3 GW2LIB::GetMouseInWorld()
{
    Vector3 pos;
    pos.x = GetMain()->GetGameData()->mouseInWorld.x;
    pos.y = GetMain()->GetGameData()->mouseInWorld.y;
    pos.z = GetMain()->GetGameData()->mouseInWorld.z;
    return pos;
}

int GW2LIB::GetCurrentMapId()
{
    return GetMain()->GetGameData()->mapId;
}

int GW2LIB::GetPing() {
    return GetMain()->GetGameData()->ping;
}

float GW2LIB::Dist3D(GW2LIB::Vector3 p1, GW2LIB::Vector3 p2) {
    return (float)sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2) + pow(p1.z - p2.z, 2));
}

GW2LIB::GW2::UiIntefaceSize GW2LIB::GetUiInterfaceSize() {
    return GetMain()->GetGameData()->uiIntSize;
}

bool GW2LIB::GetUiOptionFlag(GW2LIB::GW2::UiOptionFlag opt) {
    if (opt >= GW2LIB::GW2::UI_OPT_END || opt < 0) return false;
    const GameData::GameData *data = GetMain()->GetGameData();

    if (opt < 32) return data->uiFlags1 & (1 << opt);
    if (opt < 64) return data->uiFlags2 & (1 << (opt - 32));

    return data->uiFlags3 & (1 << (opt - 64));
}

GW2LIB::Compass GW2LIB::GetCompass() {
    Compass comp;
    comp.m_ptr = GetMain()->GetGameData()->objData.compData.get();
    return comp;
}

int GW2LIB::GetFPS() {
    return GetMain()->GetGameData()->fps;
}

Gw2GameHook* get_hook() {
    return &GetMain()->m_gw2Hook;
}

Gw2Hooks* get_hook_list() {
    return &get_hook()->m_hookList;
}

bool GW2LIB::IsInterfaceHidden() {
    return GetMain()->GetGameData()->ifHide;
}

bool GW2LIB::IsMapOpen() {
    return GetMain()->GetGameData()->mapOpen;
}

bool GW2LIB::IsInCutscene() {
    return GetMain()->GetGameData()->asCtxMode == 1;
}

bool GW2LIB::ActionCamOn() {
    return GetMain()->GetGameData()->actionCam;
}

std::string GW2LIB::GetIPAddress() {
    static sockaddr_in cache_addr = { 0 };
    static std::string addr;

    auto gd = GetMain()->GetGameData();

    if (memcmp(&cache_addr, &gd->ipAddr, sizeof(sockaddr_in))) {
        cache_addr = gd->ipAddr;
        addr = inet_ntoa(cache_addr.sin_addr);
    }

    return addr;
}


void GW2LIB::AddDrunkLevel(int lvl) {
    auto pmems = GetMain()->GetGameOffsets();
    auto mems  = GetMain()->GetGamePointers();

    hl::ForeignClass ctx = mems->pCtx;
    if (!ctx) return;
    hl::ForeignClass charctx = ctx.get<void*>(pmems->contextChar);
    if (!charctx) return;

    GetMain()->AddDrunkLevel(charctx.data(), lvl);
}

uint32_t GW2LIB::GetBuildId() {
    return GetMain()->GetGameData()->buildId;
}

void GW2LIB::EnableGameHook(void(*cbGameHook)()) {
    GetMain()->SetGameHookCallback(cbGameHook);
}

void GW2LIB::GameCtxCheck() {
    GetMain()->GameCtxCheck();
}
