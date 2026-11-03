#include "../editing_ww.h"
#include "../../globals.h"
#include "../../../shared/commonFunc.h"
#include <cmath>
#include "../../databanks/tiles.h"
#include "../../cBrush.h"
#include "../../windows/tilePicker.h"

extern HGE *hge;

bool State::EditingWW::TileThink(bool pbConsumed) {
    int tilePicked = MDI->GetActiveDoc()->iTilePicked[MDI->GetActiveDoc()->iTileSlot];
    int brushPicked = MDI->GetActiveDoc()->iBrushPicked;

    if (hge->Input_KeyDown(HGEK_ESCAPE)) {
        if (conWriteID->isVisible()) {
            conWriteID->setShow(0);
            iTileWriteIDx = iTileWriteIDy = -1;
            vPort->MarkToRedraw();
        }
        SetTool(EWW_TOOL_NONE);
        vTileGhosting.clear();
        return 0;
    }

    if (!hge->Input_IsMouseOver())
        return 0;

    float mx, my;
    hge->Input_GetMousePos(&mx, &my);

    if (iActiveTool == EWW_TOOL_WRITEID || iActiveTool == EWW_TOOL_PIPETTE) {
        PipetteTileHighlight();
    }

    if (iActiveTool == EWW_TOOL_NONE && iCurTileCbE != CLIPBOARD_IS_EMPTY) {
        TileClipboardGhostPreview(mx, my);
    }

    if (iActiveTool != EWW_TOOL_PENCIL && iActiveTool != EWW_TOOL_FILL && iActiveTool != EWW_TOOL_BRUSH &&
            iActiveTool != EWW_TOOL_WRITEID && iActiveTool != EWW_TOOL_PIPETTE)
        return 0;

    bool bVPfocused = (conMain->getWidgetAt(mx, my) == vPort->GetWidget() && !pbConsumed);
    int tgSize = vTileGhosting.size();

    if (wintpFillColor->isVisible() != (tilePicked == EWW_TILE_FILL))
        wintpFillColor->setVisible((tilePicked == EWW_TILE_FILL));

    if ((iActiveTool == EWW_TOOL_BRUSH && !bVPfocused)) {
        vTileGhosting.clear();
        return 0;
    }

    if (bVPfocused) {
        int hx = Scr2WrdX(GetActivePlane(), mx) / GetActivePlane()->GetTileWidth(),
            hy = Scr2WrdY(GetActivePlane(), my) / GetActivePlane()->GetTileHeight(),
            hxUnwrapped = hx, hyUnwrapped = hy;

        if (hx >= GetActivePlane()->GetPlaneWidth()) {
            if (!GetActivePlane()->GetFlag(WWD::Flag_p_XWrapping)) {
                hx = -1;
            } else if (iTileDrawStartX == -1 && iTileDrawStartY == -1) {
                hx = hx % GetActivePlane()->GetPlaneWidth();
            }
        }

        if (hy >= GetActivePlane()->GetPlaneHeight()) {
            if (!GetActivePlane()->GetFlag(WWD::Flag_p_YWrapping)) {
                hy = -1;
            } else if (iTileDrawStartX == -1 && iTileDrawStartY == -1) {
                hy = hy % GetActivePlane()->GetPlaneHeight();
            }
        }

        if (iActiveTool == EWW_TOOL_BRUSH && (hx != lastbrushx || hy != lastbrushy))
            vTileGhosting.clear();

        iPipetteTileHL = EWW_TILE_NONE;
        if (iActiveTool == EWW_TOOL_PIPETTE || iActiveTool == EWW_TOOL_WRITEID) {
            auto tile = GetActivePlane()->GetTile(hx, hy);
            if (tile) {
                AddTileToTileGhosting(EWW_TILE_HIGHLIGHT, hx, hy);

                if (tile->IsFilled()) {
                    iPipetteTileHL = EWW_TILE_FILL;
                } else if (tile->IsInvisible()) {
                    iPipetteTileHL = EWW_TILE_ERASE;
                } else {
                    cTileImageSet* set = hTileset->GetSet(GetActivePlane()->GetImageSet(0));
                    if (!set) return 0;
                    for (int z = 0; z < set->GetTilesCount(); z++) {
                        if (set->GetTileByIterator(z)->GetID() == tile->GetID()) {
                            iPipetteTileHL = z;
                            break;
                        }
                    }
                }
            }
        } else if (iActiveTool == EWW_TOOL_BRUSH) {
            if ((hx != lastbrushx || hy != lastbrushy) && brushPicked >= 0) {
                cBrush *brush = hTileset->GetSet(GetActivePlane()->GetImageSet(0))->GetBrushByIterator(
                        brushPicked);
                if (brush)
                    brush->ApplyGhosting(GetActivePlane(), hx, hy, &vTileGhosting);
            }
        }

        if (lockDrawing) {
            lockDrawing = hge->Input_GetKeyState(HGEK_LBUTTON);
        } else if (hge->Input_GetKeyState(HGEK_SPACE)) {
            if (!vTileGhosting.empty()) {
                vTileGhosting.clear();
                vPort->MarkToRedraw();
            }
            return 0;
        } else if (iActiveTool == EWW_TOOL_PENCIL) {
            PencilThink(hx, hy, hxUnwrapped, hyUnwrapped);
        } else if (iActiveTool == EWW_TOOL_WRITEID && (hge->Input_KeyDown(HGEK_LBUTTON) ||
                                                        hge->Input_KeyDown(HGEK_ENTER) &&
                                                        !conWriteID->isVisible())) {
            WriteIDThink(hx, hy);
        } else if (hge->Input_GetKeyState(HGEK_LBUTTON)) {
            if (iActiveTool == EWW_TOOL_PIPETTE) {
                lockDrawing = 1;
                if (!PipetteThink(hx, hy)) return 0;
            } else if (iActiveTool == EWW_TOOL_FILL) {
                if (hge->Input_GetKeyState(HGEK_LBUTTON))
                    FloodFill(hx, hy, tilePicked);
                hPlaneData[GetActivePlaneID()]->bUpdateBuffer = 1;
                vPort->MarkToRedraw();
            } else if (iActiveTool == EWW_TOOL_BRUSH && brushPicked >= 0) {
                if (iLastBrushPlacedX != hx || iLastBrushPlacedY != hy) {
                    vTileGhosting.clear();
                    cBrush *brush = hTileset->GetSet(GetActivePlane()->GetImageSet(0))->GetBrushByIterator(
                            brushPicked);
                    if (brush->Apply(GetActivePlane(), hx, hy)) {
                        MarkUnsaved();
                        vPort->MarkToRedraw();
                    }
                    iLastBrushPlacedX = hx;
                    iLastBrushPlacedY = hy;
                }
            }
        }

        if (iActiveTool == EWW_TOOL_BRUSH && hx != -1 && hy != -1) {
            lastbrushx = hx;
            lastbrushy = hy;
        }

        if (hx != iTilePreviewX || hy != iTilePreviewY) {
            vPort->MarkToRedraw();
            iTilePreviewX = hx;
            iTilePreviewY = hy;
        }
    } else {
        if (iActiveTool == EWW_TOOL_PENCIL && iTileDrawStartX != -1 && iTileDrawStartY != -1) {
            iTileDrawStartX = iTileDrawStartY = -1;
            vTileGhosting.clear();
        }
    }
    if (tgSize != vTileGhosting.size()) {
        vPort->MarkToRedraw();
    }
    return 1;
}

void State::EditingWW::PipetteTileHighlight() {
    vTileGhosting.clear();
    if (iTileWriteIDx != -1 && iTileWriteIDy != -1) {
        AddTileToTileGhosting(EWW_TILE_HIGHLIGHT, iTileWriteIDx, iTileWriteIDy);
        int posX = iTileWriteIDx * GetActivePlane()->GetTileWidth(),
            posY = iTileWriteIDy * GetActivePlane()->GetTileHeight();
        posX = Wrd2ScrX(GetActivePlane(), posX);
        posY = Wrd2ScrY(GetActivePlane(), posY);
        int tileW = (GetActivePlane()->GetTileWidth() * fZoom - 48.0f) / 2.0f,
            tileH = GetActivePlane()->GetTileHeight() * fZoom;
        conWriteID->setPosition(posX + tileW, posY + tileH / 2 - 10);
    }
}

void State::EditingWW::TileClipboardGhostPreview(int mouseX, int mouseY) {
    if (hge->Input_GetKeyState(HGEK_ALT)) {
        int tx = Scr2WrdX(GetActivePlane(), mouseX) / GetActivePlane()->GetTileWidth(),
            ty = Scr2WrdY(GetActivePlane(), mouseY) / GetActivePlane()->GetTileHeight();
        if (vTileGhosting.empty() || (!vTileGhosting.empty() && (tx != vTileGhosting[0].x || ty != vTileGhosting[0].y))) {
            vTileGhosting.clear();
            cTileClipboardEntry *cbEntry = arTileClipboard[iCurTileCbE];
            for (int i = 0, y = 0; y < cbEntry->height; ++y)
                for (int x = 0; x < cbEntry->width; ++x, ++i) {
                    int tileID = cbEntry->tiles[i].IsFilled() ? EWW_TILE_FILL
                               : cbEntry->tiles[i].IsInvisible() ? EWW_TILE_ERASE
                               : cbEntry->tiles[i].GetID();
                    AddTileToTileGhosting(tileID, tx + x, ty + y);
                }
            vPort->MarkToRedraw();
        }
    } else if (!vTileGhosting.empty()) {
        vTileGhosting.clear();
        vPort->MarkToRedraw();
    }
}

void State::EditingWW::PencilThink(int hx, int hy, int hxUnwrapped, int hyUnwrapped) {
    bool bPlacing;
    if (iTileDrawMode == EWW_DRAW_RECT || iTileDrawMode == EWW_DRAW_LINE ||
        iTileDrawMode == EWW_DRAW_ELLIPSE) {
        if (iTileDrawStartX == -1 && iTileDrawStartY == -1 && hge->Input_KeyDown(HGEK_LBUTTON)) {
            iTileDrawStartX = hxUnwrapped;
            iTileDrawStartY = hyUnwrapped;
        }
        bPlacing = !hge->Input_GetKeyState(HGEK_LBUTTON);
    } else {
        bPlacing = (hge->Input_KeyDown(HGEK_LBUTTON) ||
                    ((hx != iLastBrushPlacedX || hy != iLastBrushPlacedY) &&
                        hge->Input_GetKeyState(HGEK_LBUTTON)));
    }

    if (hx != iLastBrushPlacedX || hy != iLastBrushPlacedY) {
        vTileGhosting.clear();

        if (iTileDrawMode == EWW_DRAW_POINT || iTileDrawMode == EWW_DRAW_SPRAY) {
            PencilPointThink(&hx, &hy, hxUnwrapped, hyUnwrapped);
        } else if (iTileDrawStartX != -1 && iTileDrawStartY != -1) {
            if (iTileDrawMode == EWW_DRAW_LINE) {
                PencilLineThink(hx, hy);
            } else {
                if (hge->Input_GetKeyState(HGEK_SHIFT)) {
                    int dx = hx - iTileDrawStartX,
                        dy = hy - iTileDrawStartY,
                        maxlen = std::max(abs(dx), abs(dy));
                    if (dx >= 0 && dy >= 0) {
                        hx = iTileDrawStartX + maxlen;
                        hy = iTileDrawStartY + maxlen;
                    } else if (dx < 0 && dy >= 0) {
                        hx = iTileDrawStartX - maxlen;
                        hy = iTileDrawStartY + maxlen;
                    } else if (dx < 0 && dy < 0) {
                        hx = iTileDrawStartX - maxlen;
                        hy = iTileDrawStartY - maxlen;
                    } else if (dx >= 0 && dy < 0) {
                        hx = iTileDrawStartX + maxlen;
                        hy = iTileDrawStartY - maxlen;
                    }
                }
                if (iTileDrawMode == EWW_DRAW_RECT)
                    PencilRectThink(hx, hy);
                else if (iTileDrawMode == EWW_DRAW_ELLIPSE)
                    PencilEllipseThink(hx, hy);
            }
        }
        iLastBrushPlacedX = hx;
        iLastBrushPlacedY = hy;
    }

    if (bPlacing) {
        PlacePencilTiles();
    }
}

void State::EditingWW::WriteIDThink(int hx, int hy) {
    iTileWriteIDx = hx;
    iTileWriteIDy = hy;
    int posX = hx * GetActivePlane()->GetTileWidth(),
        posY = hy * GetActivePlane()->GetTileHeight();
    posX = Wrd2ScrX(GetActivePlane(), posX);
    posY = Wrd2ScrY(GetActivePlane(), posY);
    int tileH = GetActivePlane()->GetTileHeight() * fZoom;
    int tileW = (GetActivePlane()->GetTileWidth() * fZoom - 48.0f) / 2.0f;
    WWD::Tile *tile = GetActivePlane()->GetTile(hx, hy);
    if (tile) {
        if (tile->IsFilled()) {
            tfWriteID->setText("FILL");
        } else if (tile->IsInvisible()) {
            tfWriteID->setText("0");
        } else {
            tfWriteID->setText(std::to_string(tile->GetID()));
        }

        conWriteID->setVisible(0);
        conWriteID->setShow(1);
        tfWriteID->requestFocus();
        tfWriteID->setSelectionPosition(0);
        tfWriteID->setCaretPosition(tfWriteID->getText().length());
        conWriteID->setPosition(posX + tileW, posY + tileH / 2 - 10);
    }
}

bool State::EditingWW::PipetteThink(int hx, int hy) {
    int pipTile;
    if (GetActivePlane()->GetTile(hx, hy)->IsFilled())
        pipTile = EWW_TILE_FILL;
    else if (GetActivePlane()->GetTile(hx, hy)->IsInvisible())
        pipTile = EWW_TILE_ERASE;
    else {
        cTileImageSet *set = hTileset->GetSet(GetActivePlane()->GetImageSet(0));
        if (!set) return 0;
        for (int z = 0; z < set->GetTilesCount(); z++)
            if (set->GetTileByIterator(z)->GetID() == GetActivePlane()->GetTile(hx, hy)->GetID()) {
                pipTile = z;
                break;
            }
    }
    if (hge->Input_GetKeyState(HGEK_LBUTTON))
        MDI->GetActiveDoc()->iTilePicked[MDI->GetActiveDoc()->iTileSlot] = pipTile;
    hwinTilePicker->Rebuild();
    return 1;
}

void State::EditingWW::PencilPointThink(int *hx, int *hy, int hxUnwrapped, int hyUnwrapped) {
    int x1 = iTileDrawStartX,
        y1 = iTileDrawStartY,
        x2 = *hx,
        y2 = *hy;
    int tilePicked = MDI->GetActiveDoc()->iTilePicked[MDI->GetActiveDoc()->iTileSlot];
    int pointsize = (iTileDrawMode == EWW_DRAW_POINT ? hwinTilePicker->GetPointSize()
                                                                : hwinTilePicker->GetSpraySize());
    if (pointsize == 1) {
        AddTileToTileGhosting(tilePicked, *hx, *hy);
    } else {
        *hx = hxUnwrapped;
        *hy = hyUnwrapped;
        pointsize--;
        for (int y = hyUnwrapped - pointsize; y <= hyUnwrapped + pointsize; y++)
            for (int x = hxUnwrapped - pointsize; x <= hxUnwrapped + pointsize; x++) {
                float dist = sqrt(pow(x - hxUnwrapped, 2) + pow(y - hyUnwrapped, 2));
                if (dist < (pointsize + 1))
                    AddTileToTileGhosting(tilePicked, x, y);
            }
        if (iTileDrawMode == EWW_DRAW_SPRAY) {
            float density = hwinTilePicker->GetSprayDensity() / 100.0f;
            int clearnum = float(vTileGhosting.size()) * (1.0f - density);
            for (int i = 0; i < clearnum; i++) {
                int idx = hge->Random_Int(0, vTileGhosting.size() - 1);
                vTileGhosting.erase(vTileGhosting.begin() + idx);
            }
        }
    }
}

void State::EditingWW::PencilLineThink(int hx, int hy) {
    int x1 = iTileDrawStartX,
        y1 = iTileDrawStartY,
        x2 = hx,
        y2 = hy;
    if (hge->Input_GetKeyState(HGEK_SHIFT)) {
        if (abs(x2 - x1) - abs(y2 - y1) > 1)
            y2 = y1;
        else if (abs(x2 - x1) - abs(y2 - y1) < -1)
            x2 = x1;
        else if ((x2 > x1 && y2 > y1) || (x2 < x1 && y2 < y1))
            y2 = y1 + x2 - x1;
        else
            y2 = y1 - x2 + x1;
    }
    int thickness = hwinTilePicker->GetLineThickness();
    int tilePicked = MDI->GetActiveDoc()->iTilePicked[MDI->GetActiveDoc()->iTileSlot];

    auto bline = [&](int x1, int y1, int x2, int y2) {
        int dx = x2 - x1;
        int dy = y2 - y1;
        int incXH, incXL;
        if (dx >= 0) incXH = incXL = 1;
        else {
            dx = -dx;
            incXH = incXL = -1;
        }
        int incYH, incYL;
        if (dy >= 0) incYH = incYL = 1;
        else {
            dy = -dy;
            incYH = incYL = -1;
        }
        int longD, shortD;
        if (dx >= dy) {
            longD = dx;
            shortD = dy;
            incYL = 0;
        } else {
            longD = dy;
            shortD = dx;
            incXL = 0;
        }
        int d = 2 * shortD - longD;
        int incDL = 2 * shortD;
        int incDH = 2 * shortD - 2 * longD;
        for (int i = 0; i <= longD; i++) {
            AddTileToTileGhosting(tilePicked, x1, y1);
            if (d >= 0) {
                x1 += incXH;
                y1 += incYH;
                d += incDH;
            } else {
                x1 += incXL;
                y1 += incYL;
                d += incDL;
            }
        }
    };

    bline(x1, y1, x2, y2);
    if (thickness > 1) {
        if (thickness > 2) thickness *= 2;
        if (x1 == x2) {
            thickness = thickness / 2 + 1;
            for (int i = 0; i < thickness; i++) {
                bline(x1 - i, y1, x2 - i, y2);
                bline(x1 + i, y1, x2 + i, y2);
            }
        } else if (y1 == y2) {
            thickness = thickness / 2 + 1;
            for (int i = 0; i < thickness; i++) {
                bline(x1, y1 - i, x2, y2 - i);
                bline(x1, y1 + i, x2, y2 + i);
            }
        } else if (abs(y2 - y1) / abs(x2 - x1) < 1) {
            double wy = thickness * sqrt(pow((x2 - x1), 2)
                        + pow((y2 - y1), 2)) / (2 * fabs(x2 - x1));
            for (int i = 0; i < wy; i++) {
                bline(x1, y1 - i, x2, y2 - i);
                bline(x1, y1 + i, x2, y2 + i);
            }
        } else {
            double wx = thickness * sqrt(pow((x2 - x1), 2)
                        + pow((y2 - y1), 2)) / (2 * fabs(y2 - y1));
            for (int i = 0; i < wx; i++) {
                bline(x1 - i, y1, x2 - i, y2);
                bline(x1 + i, y1, x2 + i, y2);
            }
        }
    }
}

void State::EditingWW::PencilRectThink(int hx, int hy) {
    int tilePicked = MDI->GetActiveDoc()->iTilePicked[MDI->GetActiveDoc()->iTileSlot];
    int x1 = (iTileDrawStartX > hx ? hx : iTileDrawStartX),
        y1 = (iTileDrawStartY > hy ? hy : iTileDrawStartY),
        x2 = (iTileDrawStartX > hx ? iTileDrawStartX : hx),
        y2 = (iTileDrawStartY > hy ? iTileDrawStartY : hy);
    for (int y = y1; y <= y2; y++) {
        for (int x = x1; x <= x2; x++) {
            bool fill = hwinTilePicker->GetRectFill();
            if (fill || (!fill && (x == x1 || x == x2 || y == y1 || y == y2)))
                AddTileToTileGhosting(tilePicked, x, y);
        }
    }
}

void State::EditingWW::PencilEllipseThink(int hx, int hy) {
    int tilePicked = MDI->GetActiveDoc()->iTilePicked[MDI->GetActiveDoc()->iTileSlot];
    int x1 = (iTileDrawStartX > hx ? hx : iTileDrawStartX),
        y1 = (iTileDrawStartY > hy ? hy : iTileDrawStartY),
        x2 = (iTileDrawStartX > hx ? iTileDrawStartX : hx),
        y2 = (iTileDrawStartY > hy ? iTileDrawStartY : hy);
    if (!(abs(x1 - x2) > 2 && abs(y1 - y2) > 2))
        return;
    int radiusX = (x2 - x1) / 2,
        radiusY = (y2 - y1) / 2;
    int centerX = x1 + radiusX,
        centerY = y1 + radiusY;
    int x, y, changeX, changeY, e, twoASquare, twoBSquare, stoppingX, stoppingY;

    twoASquare = 2 * radiusX * radiusX;
    twoBSquare = 2 * radiusY * radiusY;
    x = radiusX;
    y = 0;
    changeX = radiusY * radiusY * (1 - 2 * radiusX);
    changeY = radiusX * radiusX;
    e = 0;
    stoppingX = twoBSquare * radiusX;
    stoppingY = 0;

    while (stoppingX >= stoppingY) {
        AddTileToTileGhosting(tilePicked, centerX + x, centerY + y);
        AddTileToTileGhosting(tilePicked, centerX - x, centerY - y);
        AddTileToTileGhosting(tilePicked, centerX + x, centerY - y);
        AddTileToTileGhosting(tilePicked, centerX - x, centerY + y);
        if (hwinTilePicker->GetEllipseFill()) {
            for (int z = centerX - x; z < centerX + x; z++) {
                AddTileToTileGhosting(tilePicked, z, centerY - y);
                AddTileToTileGhosting(tilePicked, z, centerY + y);
            }
        }

        y++;
        stoppingY += twoASquare;
        e += changeY;
        changeY += twoASquare;
        if ((2 * e + changeX) > 0) {
            x--;
            stoppingX -= twoBSquare;
            e += changeX;
            changeX += twoBSquare;
        }
    }

    x = 0;
    y = radiusY;
    changeX = radiusY * radiusY;
    changeY = radiusX * radiusX * (1 - 2 * radiusY);
    e = 0;
    stoppingX = 0;
    stoppingY = twoASquare * radiusY;
    while (stoppingX <= stoppingY) {
        AddTileToTileGhosting(tilePicked, centerX + x, centerY + y);
        AddTileToTileGhosting(tilePicked, centerX - x, centerY - y);
        AddTileToTileGhosting(tilePicked, centerX + x, centerY - y);
        AddTileToTileGhosting(tilePicked, centerX - x, centerY + y);
        if (hwinTilePicker->GetEllipseFill()) {
            for (int z = centerX - x; z < centerX + x; z++) {
                AddTileToTileGhosting(tilePicked, z, centerY - y);
                AddTileToTileGhosting(tilePicked, z, centerY + y);
            }
        }
        x++;
        stoppingX += twoBSquare;
        e += changeX;
        changeX += twoBSquare;
        if ((2 * e + changeY) > 0) {
            y--;
            stoppingY -= twoASquare;
            e += changeY;
            changeY += twoASquare;
        }
    }
}

void State::EditingWW::AddTileToTileGhosting(int i, int x, int y) {
    TileGhost tg;
    tg.pl = GetActivePlane();
    tg.x = tg.pl->ClampX(x);
    tg.y = tg.pl->ClampY(y);
    if (i >= 0) {
        auto *tile = hTileset->GetSet(GetActivePlane()->GetImageSet(0))->GetTileByIterator(i);
        tg.id = tile ? tile->GetID() : EWW_TILE_UNKNOWN;
    } else
        tg.id = i;
    vTileGhosting.push_back(tg);
}

void State::EditingWW::PlacePencilTiles() {
    bool changes = 0;
    for (auto &t : vTileGhosting) {
        WWD::Tile *tl = t.pl->GetTile(t.x, t.y);
        if (!tl) 
            continue;
        
        if (t.id == EWW_TILE_ERASE && !tl->IsInvisible()) {
            tl->SetInvisible();
            changes = 1;
        }
        else if (t.id == EWW_TILE_FILL && !tl->IsFilled()) {
            tl->SetFilled();
            changes = 1;
        }
        else if (t.id >= 0 && tl->GetID() != t.id) {
            tl->SetID(t.id);
            changes = 1;
        }
    }
    if (changes) {
        MarkUnsaved();
        hPlaneData[GetActivePlaneID()]->bUpdateBuffer = 1;
    }
    vTileGhosting.clear();
    if (iTileDrawMode == EWW_DRAW_RECT || iTileDrawMode == EWW_DRAW_LINE || iTileDrawMode == EWW_DRAW_ELLIPSE) {
        iTileDrawStartX = iTileDrawStartY = -1;
    }
}

void State::EditingWW::FloodFill(int x, int y, int tile) {
    if (x < 0 || y < 0) return;
    x = GetActivePlane()->ClampX(x);
    y = GetActivePlane()->ClampY(y);
    WWD::Tile *t = GetActivePlane()->GetTile(x, y);
    if (!t) return;

    int base = 0;
    if (t->IsInvisible())
        base = EWW_TILE_ERASE;
    else if (t->IsFilled())
        base = EWW_TILE_FILL;
    else
        base = t->GetID();

    if (tile == base || tile == EWW_TILE_NONE) return;

    int width = GetActivePlane()->GetPlaneWidth(),
        height = GetActivePlane()->GetPlaneHeight(),
        size = width * height,
        it = y * width + x;

    bool *bFloodFillBuf = new bool[size]{};

    std::function < bool() > checkTile;
    if (base == EWW_TILE_ERASE) {
        checkTile = [&t]() { return t->IsInvisible(); };
    } else if (base == EWW_TILE_FILL) {
        checkTile = [&t]() { return t->IsFilled(); };
    } else {
        checkTile = [&t, base]() { return t->GetID() == base; };
    }

    std::function < void() > fillTile;
    if (tile == EWW_TILE_ERASE) {
        fillTile = [&t]() { t->SetInvisible(); };
    } else if (tile == EWW_TILE_FILL) {
        fillTile = [&t]() { t->SetFilled(); };
    } else {
        // imgSet
        int id = hTileset->GetSet(GetActivePlane()->GetImageSet(0))->GetTileByIterator(tile)->GetID();
        fillTile = [&t, id]() { t->SetID(id); };
    }

    struct Placement { int x; int it; };
    std::stack<Placement> queue;
    queue.push({x, it});

    bFloodFillBuf[it] = 1;
    MarkUnsaved();

    while (!queue.empty()) {
        Placement p = queue.top();
        queue.pop();

        t = GetActivePlane()->GetTile(p.it);
        if (!t || !checkTile()) continue;

        fillTile();

        if (p.x + 1 < width && !bFloodFillBuf[p.it + 1]) {
            bFloodFillBuf[p.it + 1] = 1;
            queue.push({p.x + 1, p.it + 1});
        }

        if (p.x > 0 && !bFloodFillBuf[p.it - 1]) {
            bFloodFillBuf[p.it - 1] = 1;
            queue.push({p.x - 1, p.it - 1});
        }

        if (p.it + width < size && !bFloodFillBuf[p.it + width]) {
            bFloodFillBuf[p.it + width] = 1;
            queue.push({p.x, p.it + width});
        }

        if (p.it - width > 0 && !bFloodFillBuf[p.it - width]) {
            bFloodFillBuf[p.it - width] = 1;
            queue.push({p.x, p.it - width});
        }
    }

    delete[] bFloodFillBuf;
}
