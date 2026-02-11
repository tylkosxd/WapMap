#include "../editing_ww.h"
#include "../../cObjectUserData.h"

void State::EditingWW::CopyTiles() {
    if (hTileClipboard != NULL) {
        delete[] hTileClipboard;
        delete[] hTileClipboardImageSet;
    }

    auto* plane = GetActivePlane();

    hTileClipboardImageSet = new char[strlen(plane->GetImageSet(0)) + 1];
    strcpy(hTileClipboardImageSet, plane->GetImageSet(0));

    if (iTileSelectX2 >= plane->GetPlaneWidth()) {
        iTileSelectX2 = plane->GetPlaneWidth() - 1;
    }
    if (iTileSelectY2 >= plane->GetPlaneHeight()) {
        iTileSelectY2 = plane->GetPlaneHeight() - 1;
    }

    iTileCBw = iTileSelectX2 - iTileSelectX1 + 1;
    iTileCBh = iTileSelectY2 - iTileSelectY1 + 1;

    hTileClipboard = new WWD::Tile[iTileCBw*iTileCBh];

    for (int i = 0, y = iTileSelectY1; y <= iTileSelectY2; y++)
        for (int x = iTileSelectX1; x <= iTileSelectX2; x++, i++)
            hTileClipboard[i] = *plane->GetTile(x, y);
}

void State::EditingWW::CutTiles() {
    CopyTiles();
    auto* plane = GetActivePlane();
    bool bChanges = false;
    for (int x = iTileSelectX1; x <= iTileSelectX2; x++)
        for (int y = iTileSelectY1; y <= iTileSelectY2; y++) {
            WWD::Tile *tile = plane->GetTile(x, y);
            if (!tile->IsInvisible()) {
                bChanges = true;
                tile->SetInvisible(true);
            }
        }
    if (bChanges) {
        vPort->MarkToRedraw();
        MarkUnsaved();
    }
}

void State::EditingWW::PasteTiles() {
    if (!hTileClipboard)
        return;
    bool bChanges = false;
    auto* plane = GetActivePlane();
    int tx = Scr2WrdX(plane, contextX) / plane->GetTileWidth(),
        ty = Scr2WrdY(plane, contextY) / plane->GetTileHeight();

    for (int i = 0, y = ty; y < ty + iTileCBh; ++y) {
        for (int x = tx; x < tx + iTileCBw; ++x, ++i) {
            WWD::Tile *tile = plane->GetTile(x, y);
            if (tile && *tile != hTileClipboard[i]) {
                bChanges = true;
                *tile = hTileClipboard[i];
            }
        }
    }
    if (bChanges) {
        vPort->MarkToRedraw();
        MarkUnsaved();
    }
}

void State::EditingWW::CopyObjects() {
    for (auto object : vObjectClipboard) {
        delete object;
    }
    vObjectClipboard.clear();
    for (auto object : vObjectsPicked) {
        if (object != hStartingPosObj) {
            auto nObject = new WWD::Object(object);
            vObjectClipboard.push_back(nObject);
        }
    }
}

void State::EditingWW::CutObjects() {
    CopyObjects();
    auto plane = GetActivePlane();
    std::vector<WWD::Object*> temp = vObjectsPicked;
    for (auto &obj : temp) {
        if (obj != hStartingPosObj) {
            plane->DeleteObject(obj);
        }
    }
    vObjectsPicked.clear();
    vPort->MarkToRedraw();
    MarkUnsaved();
}

void State::EditingWW::PasteObjects() {
    if (vObjectClipboard.empty())
        return;
    vObjectsPicked.clear();
    float x = int(fCamX * (GetActivePlane()->GetMoveModX() / 100.0f) * fZoom) + contextX - vPort->GetX();
    float y = int(fCamY * (GetActivePlane()->GetMoveModY() / 100.0f) * fZoom) + contextY - vPort->GetY();

    x = x / fZoom;
    y = y / fZoom;

    for (auto &clipboardObject : vObjectClipboard) {
        float diffX = clipboardObject->GetX() - vObjectClipboard[0]->GetX();
        float diffY = clipboardObject->GetY() - vObjectClipboard[0]->GetY();

        auto *object = new WWD::Object(clipboardObject);
        GetActivePlane()->AddObjectAndCalcID(object);
        object->SetUserData(new cObjUserData(object));
        vObjectsPicked.push_back(object);
        GetUserDataFromObj(object)->SetPos(x + diffX, y + diffY);
    }

    vPort->MarkToRedraw();
    if (UpdateMovedObjectWithRects(vObjectsPicked)) {
        MarkUnsaved();
        vPort->MarkToRedraw();
    } else {
        std::vector<WWD::Object *> tmp = vObjectsPicked;
        for (auto &i : tmp) {
            GetActivePlane()->DeleteObject(i);
        }
    }
}