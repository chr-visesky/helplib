#include "main.h"

using namespace GW2LIB;

GW2LIB::Vector3 GW2LIB::GetCameraPosition() {
    auto& cam = GetMain()->GetGameData()->camData;
    Vector3 vec;
    vec.x = cam.camPos.x;
    vec.y = cam.camPos.y;
    vec.z = cam.camPos.z;
    return vec;
}

GW2LIB::Vector3 GW2LIB::GetViewVector() {
    auto& cam = GetMain()->GetGameData()->camData;
    Vector3 vec;
    vec.x = cam.viewVec.x;
    vec.y = cam.viewVec.y;
    vec.z = cam.viewVec.z;
    return vec;
}

float GW2LIB::GetFieldOfViewY() {
    return GetMain()->GetGameData()->camData.fovy;
}

float GW2LIB::GetCamCurrentZoom() {
    return GetMain()->GetGameData()->camData.currZoom;
}

float GW2LIB::GetCamMinZoom() {
    return GetMain()->GetGameData()->camData.minZoom;
}

float GW2LIB::GetCamMaxZoom() {
    return GetMain()->GetGameData()->camData.maxZoom;
}

void GW2LIB::SetCamMinZoom(float zoom) {
    auto mems = GetMain()->GetGamePointers();
    auto pubmems = GetMain()->GetGameOffsets();
    if (!mems->pWorldViewContext) return;

    hl::ForeignClass wvctx = mems->pWorldViewContext;
    if (wvctx && wvctx.get<int>(pubmems->wvctxStatus) == 1) {
        wvctx.set<>(pubmems->camMinZoom, zoom);
    }
}

void GW2LIB::SetCamMaxZoom(float zoom) {
    auto mems = GetMain()->GetGamePointers();
    auto pubmems = GetMain()->GetGameOffsets();
    if (!mems->pWorldViewContext) return;

    hl::ForeignClass wvctx = mems->pWorldViewContext;
    if (wvctx && wvctx.get<int>(pubmems->wvctxStatus) == 1) {
        wvctx.set<>(pubmems->camMaxZoom, zoom);
    }
}



