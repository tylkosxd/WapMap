#include "tilePicker.h"
#include "../langID.h"
#include "../states/editing_ww.h"
#include "../../shared/commonFunc.h"
#include "../states/imageImport.h"
#include "../databanks/tiles.h"
#include "../cBrush.h"

#define TILE_PICKER_WIDTH 348

winTilePicker::winTilePicker() : cWindow(GETL2S("TilePicker", "WinCaption"), TILE_PICKER_WIDTH, 300) {
    vp = new WIDG::Viewport(this, 0);
    myWin.add(vp, 0, 0);

    int x = 3 + iInventorySeparator,
        y = 15,
        offx = 35;

    butPoint    = MakeButton(x + offx * 0, y, Icon_Point);
    butLine     = MakeButton(x + offx * 1, y, Icon_Line);
    butRect     = MakeButton(x + offx * 2, y, Icon_Rect);
    butEllipse  = MakeButton(x + offx * 3, y, Icon_Ellipse);
    butSpray    = MakeButton(x + offx * 4, y, Icon_Spray);
    butFlood    = MakeButton(x + offx * 5, y, Icon_Fill);
    butPipette  = MakeButton(x + offx * 6, y, Icon_Pipette);
    butBrush    = MakeButton(x + offx * 7, y, Icon_Brush);

    x += 2;
    y = 55;

    char labtmp[128];
    sprintf(labtmp, "%s: 1", GETL2S("TilePicker", "Pointsize"));
    labPointSize = new SHR::Lab(labtmp);
    labPointSize->adjustSize();
    myWin.add(labPointSize, x, y);

    sliPointSize = new SHR::Slider(1, 20);
    sliPointSize->setOrientation(SHR::Slider::HORIZONTAL);
    sliPointSize->setStyle(SHR::Slider::POINTER);
    sliPointSize->setDimension(gcn::Rectangle(0, 0, 270, 14));
    sliPointSize->addActionListener(this);
    myWin.add(sliPointSize, x, y + 20);

    sprintf(labtmp, "%s: 1", GETL2S("TilePicker", "LineThickness"));
    labLineThickness = new SHR::Lab(labtmp);
    labLineThickness->adjustSize();
    myWin.add(labLineThickness, x, y);

    sliLineThickness = new SHR::Slider(1, 20);
    sliLineThickness->setOrientation(SHR::Slider::HORIZONTAL);
    sliLineThickness->setStyle(SHR::Slider::POINTER);
    sliLineThickness->setDimension(gcn::Rectangle(0, 0, 270, 14));
    sliLineThickness->addActionListener(this);
    myWin.add(sliLineThickness, x, y + 20);

    cbRectFilled = new SHR::CBox(GV->hGfxInterface, GETL2S("TilePicker", "Filled"));
    cbRectFilled->setSelected(1);
    cbRectFilled->adjustSize();
    myWin.add(cbRectFilled, x, y);

    cbEllipseFilled = new SHR::CBox(GV->hGfxInterface, GETL2S("TilePicker", "Filled"));
    cbEllipseFilled->setSelected(1);
    cbEllipseFilled->adjustSize();
    myWin.add(cbEllipseFilled, x, y);

    sprintf(labtmp, "%s: 2", GETL2S("TilePicker", "SpraySize"));
    labSpraySize = new SHR::Lab(labtmp);
    labSpraySize->adjustSize();
    myWin.add(labSpraySize, x, y);

    sliSpraySize = new SHR::Slider(2, 20);
    sliSpraySize->setOrientation(SHR::Slider::HORIZONTAL);
    sliSpraySize->setStyle(SHR::Slider::POINTER);
    sliSpraySize->addActionListener(this);
    sliSpraySize->setDimension(gcn::Rectangle(0, 0, 125, 14));
    myWin.add(sliSpraySize, x, y + 20);

    sprintf(labtmp, "%s: 50%%", GETL2S("TilePicker", "SprayDensity"));
    labSprayDensity = new SHR::Lab(labtmp);
    labSprayDensity->adjustSize();
    myWin.add(labSprayDensity, x + 135, y);

    sliSprayDensity = new SHR::Slider(10, 100);
    sliSprayDensity->setOrientation(SHR::Slider::HORIZONTAL);
    sliSprayDensity->setStyle(SHR::Slider::POINTER);
    sliSprayDensity->addActionListener(this);
    sliSprayDensity->setValue(50);
    sliSprayDensity->setDimension(gcn::Rectangle(0, 0, 125, 14));
    myWin.add(sliSprayDensity, x + 135, y + 20);

    butReloadBrush = MakeButton(x, 10, Icon_Reload);
    labReloadBrush = new SHR::Lab(GETL2S("TilePicker", "ReloadBrushes"));
    labReloadBrush->adjustSize();
    myWin.add(labReloadBrush, x + 35, 10);

    cbShowTileID = new SHR::CBox(GV->hGfxInterface, GETL2S("TilePicker", "ShowTileID"));
    cbShowTileID->adjustSize();
    cbShowTileID->addActionListener(this);
    myWin.add(cbShowTileID, x, 10);

    cbShowProperties = new SHR::CBox(GV->hGfxInterface, GETL2S("TilePicker", "ShowTileProp"));
    cbShowProperties->adjustSize();
    cbShowProperties->addActionListener(this);
    myWin.add(cbShowProperties, x, 10);

    sliPicker = new SHR::Slider(0);
    sliPicker->setOrientation(SHR::Slider::VERTICAL);
    sliPicker->setDimension(gcn::Rectangle(0, 0, 15, 100));
    sliPicker->setMarkerLength(40);
    myWin.add(sliPicker, TILE_PICKER_WIDTH - 20, 100);
}

SHR::But *winTilePicker::MakeButton(int x, int y, Gfx32Icons icon) {
    SHR::But *but = new SHR::But(GV->hGfxInterface, GV->sprIcons[icon]);
    but->setDimension(gcn::Rectangle(0, 0, 32, 32));
    but->addActionListener(this);
    myWin.add(but, x, y);
    return but;
}

void winTilePicker::Open() {
    myWin.setPosition(GV->editState->vPort->GetX(), GV->editState->vPort->GetY());
    myWin.setHeight(GV->editState->vPort->GetHeight());
    myWin.setVisible(1);
    myWin.getParent()->moveToTop(&myWin);
}

void winTilePicker::OnDocumentChange() {
    sliPicker->setValue(0);
}

void winTilePicker::action(const ActionEvent &actionEvent) {
    auto *es = GV->editState;
    if (actionEvent.getSource() == butPoint) {
        es->iTileDrawMode = EWW_DRAW_POINT;
        if (es->iActiveTool != EWW_TOOL_PENCIL)
            es->SetTool(EWW_TOOL_PENCIL);
        Rebuild();
    } else if (actionEvent.getSource() == butLine) {
        if (es->iActiveTool != EWW_TOOL_PENCIL)
            es->SetTool(EWW_TOOL_PENCIL);
        es->iTileDrawMode = EWW_DRAW_LINE;
        Rebuild();
    } else if (actionEvent.getSource() == butRect) {
        if (es->iActiveTool != EWW_TOOL_PENCIL)
            es->SetTool(EWW_TOOL_PENCIL);
        es->iTileDrawMode = EWW_DRAW_RECT;
        Rebuild();
    } else if (actionEvent.getSource() == butEllipse) {
        if (es->iActiveTool != EWW_TOOL_PENCIL)
            es->SetTool(EWW_TOOL_PENCIL);
        es->iTileDrawMode = EWW_DRAW_ELLIPSE;
        Rebuild();
    } else if (actionEvent.getSource() == butSpray) {
        if (es->iActiveTool != EWW_TOOL_PENCIL)
            es->SetTool(EWW_TOOL_PENCIL);
        es->iTileDrawMode = EWW_DRAW_SPRAY;
        Rebuild();
    } else if (actionEvent.getSource() == butFlood) {
        if (es->iActiveTool != EWW_TOOL_FILL)
            es->SetTool(EWW_TOOL_FILL);
    } else if (actionEvent.getSource() == butPipette) {
        if (es->iActiveTool != EWW_TOOL_PIPETTE)
            es->SetTool(EWW_TOOL_PIPETTE);
    } else if (actionEvent.getSource() == butBrush) {
        if (es->iActiveTool != EWW_TOOL_BRUSH)
            es->SetTool(EWW_TOOL_BRUSH);
    } else if (actionEvent.getSource() == sliPointSize) {
        char tmp[128];
        sprintf(tmp, "%s: %d", GETL2S("TilePicker", "PointSize"), int(sliPointSize->getValue()));
        labPointSize->setCaption(tmp);
        labPointSize->adjustSize();
    } else if (actionEvent.getSource() == sliLineThickness) {
        char tmp[128];
        sprintf(tmp, "%s: %d", GETL2S("TilePicker", "LineThickness"), int(sliLineThickness->getValue()));
        labLineThickness->setCaption(tmp);
        labLineThickness->adjustSize();
    } else if (actionEvent.getSource() == sliSpraySize) {
        char tmp[128];
        sprintf(tmp, "%s: %d", GETL2S("TilePicker", "SpraySize"), int(sliSpraySize->getValue()));
        labSpraySize->setCaption(tmp);
        labSpraySize->adjustSize();
    } else if (actionEvent.getSource() == sliSprayDensity) {
        char tmp[128];
        sprintf(tmp, "%s: %d%%", GETL2S("TilePicker", "SprayDensity"), int(sliSprayDensity->getValue()));
        labSprayDensity->setCaption(tmp);
        labSprayDensity->adjustSize();
    } else if (actionEvent.getSource() == butReloadBrush) {
        HandleBrushSwitch(es->MDI->GetActiveDoc()->iBrushPicked, EWW_TILE_NONE);
        es->MDI->GetActiveDoc()->iBrushPicked = EWW_TILE_NONE;
        es->hTileset->ReloadBrushes();
        Rebuild();
        es->gui->focusNone();
    }
}

void winTilePicker::Think() {
    if (!myWin.isVisible())
        return;
    auto *vport = GV->editState->vPort;
    if (myWin.getHeight() != vport->GetHeight())
        Rebuild();
    if (myWin.getY() != vport->GetY())
        myWin.setY(vport->GetY());
    if (GV->editState->MDI->GetActiveDoc() == NULL)
        myWin.setVisible(false);
}

void winTilePicker::Draw(int piCode) {
    auto *es = GV->editState;
    auto *ad = es->MDI->GetActiveDoc();
    float mx, my;
    hge->Input_GetMousePos(&mx, &my);
    bool hasMouse = es->conMain->getWidgetAt(mx, my) == &myWin;
    int dx, dy;
    myWin.getAbsolutePosition(dx, dy);
    int dx2 = dx + myWin.getWidth() - 1,
        dy2 = dy + myWin.getHeight() - 1;

    // vertical inventory separator
    hge->Gfx_RenderLine(dx + iInventorySeparator - 1, dy + 25, dx + iInventorySeparator - 1, dy2, GV->colLineDark);
    hge->Gfx_RenderLine(dx + iInventorySeparator, dy + 25, dx + iInventorySeparator, dy2, GV->colLineBright);

    // horizontal tools/tiles
    hge->Gfx_RenderLine(dx + iInventorySeparator, dy + iTilePickerOffUp + 13, dx2, dy + iTilePickerOffUp + 13, GV->colLineDark);
    hge->Gfx_RenderLine(dx + iInventorySeparator, dy + iTilePickerOffUp + 14, dx2, dy + iTilePickerOffUp + 14, GV->colLineBright);

    // horizontal tiles settings separator
    hge->Gfx_RenderLine(dx + iInventorySeparator, dy2 - iTilePickerOffDown + 20, dx2, dy2 - iTilePickerOffDown + 20, GV->colLineDark);
    hge->Gfx_RenderLine(dx + iInventorySeparator, dy2 - iTilePickerOffDown + 21, dx2, dy2 - iTilePickerOffDown + 21, GV->colLineBright);

    for (int i = 0; i < TILE_INVENTORY_SLOTS; i++) {
        int tilePv = 48,
            offY = tilePv + 3;
        int drx = dx + 3,
            dry = dy + 25 + i*offY;
        DWORD colBorder = GV->colLineBright;
        if (i == ad->iTileSlot && 
                (es->iActiveTool == EWW_TOOL_PENCIL || es->iActiveTool == EWW_TOOL_FILL || es->iActiveTool == EWW_TOOL_PIPETTE)) {
            colBorder = TILE_PICK_COLOR;
        } else if (hasMouse && mx >= drx && my >= dry && mx <= drx + tilePv && my <= dry + tilePv) {
            colBorder = TILE_HIGHLIGHT_COLOR;
            if (hge->Input_KeyDown(HGEK_LBUTTON)) {
                ad->iTileSlot = i;
                if (es->iActiveTool == EWW_TOOL_BRUSH)
                    es->SetTool(EWW_TOOL_PENCIL);
            }
                
        }
        RenderSingleTile(ad->iTilePicked[i], drx, dry, tilePv, colBorder, 0);

        GV->fntMyriad16->SetColor(0xFF000000);
        GV->fntMyriad16->printf(drx + 7, dry + 3, HGETEXT_CENTER, "%d", 0, i+1);
        GV->fntMyriad16->SetColor(0xFFFFFFFF);
        GV->fntMyriad16->printf(drx + 6, dry + 2, HGETEXT_CENTER, "%d", 0, i+1);
    }

    cTileImageSet *set = es->hTileset->GetSet(es->GetActivePlane()->GetImageSet(0));
    if (!set)
        return;

    int itpx = dx + 2 + iInventorySeparator,
        itpy = dy + iTilePickerOffUp + 16;
    int clipx = myWin.getWidth() - 20;
    hge->Gfx_SetClipping(itpx, itpy, clipx, sliPicker->getHeight());

    if (sliPicker->isEnabled() && mx > itpx && mx < dx + clipx && my > itpy && my < itpy + sliPicker->getHeight()) {
        sliPicker->setValue(sliPicker->getValue() - 100 * hge->Input_GetMouseWheel());
    }

    if (es->iActiveTool == EWW_TOOL_BRUSH && set->GetBrushesCount() == 0) {
        GV->fntMyriad16->SetColor(0xFFe1e1e1);
        GV->fntMyriad16->printf(itpx + (myWin.getWidth() - iInventorySeparator - 20) / 2, itpy + sliPicker->getHeight() / 2,
                HGETEXT_CENTER, GETL(Lang_NoBrushes), 0);
        return;
    }

    int scroll = sliPicker->getValue();
    if (es->iActiveTool == EWW_TOOL_BRUSH) {
        int ioldsel = ad->iBrushPicked;
        int loopst = scroll / (iTileDim + 3),
            loopen = sliPicker->getHeight() / (iTileDim + 3) + 2;
        for (int i = 0; i < set->GetBrushesCount() && i < (loopst + loopen); i++) {
            int drx = itpx + 2 + 4,
                dry = itpy + 4 + i * (iTileDim + 4) - scroll;
            cBrush *brush = set->GetBrushByIterator(i);
            DWORD colBorder = GV->colLineBright;
            if (hasMouse && (mx > dx && my > dry - 1 && mx < dx + clipx && my < dry + iTileDim + 1)
                    && my > itpy && my < itpy + sliPicker->getHeight()) {
                colBorder = TILE_HIGHLIGHT_COLOR;
                if (hge->Input_KeyDown(HGEK_LBUTTON))
                    ad->iBrushPicked = i;
            } else if (i == ad->iBrushPicked) {
                colBorder = TILE_PICK_COLOR;
            }
            RenderSingleTile(brush->GetIcon(), drx, dry, iTileDim, colBorder, 0, 1);
            GV->fntMyriad16->SetColor(colBorder == GV->colLineBright ? 0xFFe1e1e1 : colBorder);
            GV->fntMyriad16->Render(drx + iTileDim + 8, dry + iTileDim/2 - 10, HGETEXT_LEFT, brush->GetName(), 0);
        }
        if (ioldsel != ad->iBrushPicked)
            HandleBrushSwitch(ioldsel, ad->iBrushPicked);
        
        hge->Gfx_RenderLine(dx + iInventorySeparator, dy2 - 40, dx2, dy2 - 40, GV->colLineDark);
        hge->Gfx_RenderLine(dx + iInventorySeparator, dy2 - 39, dx2, dy2 - 39, GV->colLineBright);
    } 
    else {
        int tilesCount = set->GetTilesCount();
        int loopst = scroll / (iTileDim + 4)  * 4,
            loopen = sliPicker->getHeight() / (iTileDim + 4) * 4 + 8;
        if (loopst < 0) loopst = 0;
        for (int i = -2 + loopst; i < tilesCount && i < (-1 + loopst + loopen); i++) {
            int idx = (i + 2) % 4,
                idy = (i + 2) / 4;
            int drx = itpx + 2 + idx * (iTileDim + 4),
                dry = itpy + 2 + idy * (iTileDim + 4) - scroll;
            int tileI = i;
            if (i == -2)
                tileI = EWW_TILE_ERASE;
            else if (i == -1)
                tileI = EWW_TILE_FILL;

            DWORD colBorder = GV->colLineBright;
            if (es->iActiveTool == EWW_TOOL_PIPETTE && es->iPipetteTileHL != EWW_TILE_NONE) {
                if (es->iPipetteTileHL == EWW_TILE_ERASE && tileI == EWW_TILE_ERASE)
                    colBorder = TILE_HIGHLIGHT_COLOR;
                else if (es->iPipetteTileHL == EWW_TILE_FILL && tileI == EWW_TILE_FILL)
                    colBorder = TILE_HIGHLIGHT_COLOR;
                else if (es->iPipetteTileHL == tileI)
                    colBorder = TILE_HIGHLIGHT_COLOR;
            }
            if (mx > drx && my > dry && mx < drx + iTileDim && my < dry + iTileDim && hasMouse && my > itpy &&
                my < itpy + sliPicker->getHeight()) {
                colBorder = TILE_HIGHLIGHT_COLOR;
                if (hge->Input_KeyDown(HGEK_LBUTTON)) {
                    ad->iTilePicked[ad->iTileSlot] = tileI; 
                    if (tileI == EWW_TILE_FILL)
                        es->wintpFillColor->setVisible(1);
                }
            } else if (tileI == ad->iTilePicked[ad->iTileSlot]) {
                colBorder = TILE_PICK_COLOR;
            }
            RenderSingleTile(tileI, drx, dry, iTileDim, colBorder, 1);
        }
    }
}

void winTilePicker::RenderSingleTile(int i, int x, int y, int d, DWORD borderColor, bool drawInfo, bool byTileID) {
    auto *es = GV->editState;
    auto *ad = es->MDI->GetActiveDoc();
    if (i == EWW_TILE_ERASE) {
        int iconD = 32;
        GV->sprIcons[Icon_Erase]->Render(x + d/2 - iconD/2, y + d/2 - iconD/2);
    } else if (i == EWW_TILE_FILL) {
        int qColor = es->hDataCtrl->GetPalette()->GetColor(es->GetActivePlane()->GetFillColor());
        SHR::SetQuad(&q, qColor, x, y, x + d, y + d);
        hge->Gfx_RenderQuad(&q); 
    } else if (i == EWW_TILE_NONE) {
        GV->gcnParts.sprIconError->RenderStretch(x, y, x + d, y + d);
    } else if (i >= 0) {
        cTileImageSet *set = es->hTileset->GetSet(es->GetActivePlane()->GetImageSet(0));
        if (!set) return;
        cTile *tile = byTileID ? set->GetTile(i) : set->GetTileByIterator(i);
        hgeSprite *spr = tile ? tile->GetImage() : GV->sprUnknownTile;
        spr->SetFlip(0, 0);
        spr->SetHotSpot(0, 0);
        spr->SetColor(0xFFFFFFFF);
        int tileW = GV->editState->GetActivePlane()->GetTileWidth(),
            tileH = GV->editState->GetActivePlane()->GetTileHeight(),
            maxTileW =  GV->editState->hTileset->GetMaxTileWidth(),
            maxTileH = GV->editState->hTileset->GetMaxTileHeight();
        spr->RenderStretch(x, y, x + (d*maxTileW/tileW), y + (d*maxTileH/tileH));
        if (tile && drawInfo) {
            if (cbShowProperties->isSelected()) {
                float scaleX = (float)d / tileW,
                    scaleY = (float)d / tileH;
                es->DrawTileAttributes(tile->GetID(), x, y, scaleX, scaleY);
            }
            if (cbShowTileID->isSelected()) {
                GV->fntMyriad16->SetColor(0xFF000000);
                GV->fntMyriad16->printf(x + d/2 + 1, y + 1 + 0.75f*d, HGETEXT_CENTER, "%d", 0, tile->GetID());
                GV->fntMyriad16->SetColor(0xFFFFFFFF);
                GV->fntMyriad16->printf(x + d/2, y + 0.75f*d, HGETEXT_CENTER, "%d", 0, tile->GetID());
            }
        }
    }
    hge->Gfx_RenderLine(x - 1, y, x + d + 1, y, borderColor); //top
    hge->Gfx_RenderLine(x, y + d + 1, x + d + 1, y + d + 1, borderColor); //bottom
    hge->Gfx_RenderLine(x, y, x, y + d + 1, borderColor); //left
    hge->Gfx_RenderLine(x + d + 1, y, x + d + 1, y + d + 1, borderColor); //right

    hge->Gfx_RenderLine(x - 2, y - 1, x + d + 2, y - 1, GV->colLineDark);
    hge->Gfx_RenderLine(x - 1, y + d + 2, x + d + 2, y + d + 2, GV->colLineDark);
    hge->Gfx_RenderLine(x - 1, y - 1, x - 1, y + d + 2, GV->colLineDark);
    hge->Gfx_RenderLine(x + d + 2, y - 1, x + d + 2, y + d + 2, GV->colLineDark);
}

void winTilePicker::Rebuild(bool forceSliderRefresh) {
    auto *es = GV->editState;
    int at = es->iActiveTool,
        tp = es->MDI->GetActiveDoc()->iTilePicked[es->MDI->GetActiveDoc()->iTileSlot],
        bp = es->MDI->GetActiveDoc()->iBrushPicked,
        dm = es->iTileDrawMode;
    bool isBrush = at == EWW_TOOL_BRUSH,
        isFill = at == EWW_TOOL_FILL,
        isPencil = at == EWW_TOOL_PENCIL,
        isPipette = at == EWW_TOOL_PIPETTE;

    myWin.setY(es->vPort->GetY());
    myWin.setHeight(es->vPort->GetHeight());
    butReloadBrush->setY(myWin.getHeight() - 50);
    labReloadBrush->setY(myWin.getHeight() - 50 + 6);
    cbShowTileID->setY(myWin.getHeight() - 60);
    cbShowProperties->setY(myWin.getHeight() - 40);
    
    butPoint->setHighlight(!isFill && !isBrush && !isPipette && dm == EWW_DRAW_POINT);
    butPipette->setHighlight(!isFill && !isBrush && isPipette);
    butLine->setHighlight(!isFill && !isBrush && !isPipette && dm == EWW_DRAW_LINE);
    butRect->setHighlight(!isFill && !isBrush && !isPipette && dm == EWW_DRAW_RECT);
    butEllipse->setHighlight(!isFill && !isBrush && !isPipette && dm == EWW_DRAW_ELLIPSE);
    butFlood->setHighlight(isFill && !isPipette);
    butSpray->setHighlight(!isFill && !isBrush && !isPipette && dm == EWW_DRAW_SPRAY);
    butBrush->setHighlight(isBrush);

    butReloadBrush->setVisible(isBrush);
    labReloadBrush->setVisible(isBrush);

    labPointSize->setVisible(isPencil && dm == EWW_DRAW_POINT);
    sliPointSize->setVisible(isPencil && dm == EWW_DRAW_POINT);
    labLineThickness->setVisible(isPencil && dm == EWW_DRAW_LINE);
    sliLineThickness->setVisible(isPencil && dm == EWW_DRAW_LINE);
    cbRectFilled->setVisible(isPencil && dm == EWW_DRAW_RECT);
    cbEllipseFilled->setVisible(isPencil && dm == EWW_DRAW_ELLIPSE);
    labSpraySize->setVisible(isPencil && dm == EWW_DRAW_SPRAY);
    labSprayDensity->setVisible(isPencil && dm == EWW_DRAW_SPRAY);
    sliSpraySize->setVisible(isPencil && dm == EWW_DRAW_SPRAY);
    sliSprayDensity->setVisible(isPencil && dm == EWW_DRAW_SPRAY);

    iTilePickerOffUp = 65;
    iTilePickerOffDown = 60;
    if (at == EWW_TOOL_BRUSH) {
        auto *set = es->hTileset->GetSet(es->GetActivePlane()->GetImageSet(0));
        if (set && bp < set->GetBrushesCount() && bp >= 0) {
            cBrush *br = set->GetBrushByIterator(bp);
            if (br && br->HasSettings())
                iTilePickerOffDown += 10 + br->GetSettingsHeight();
        }
    } else {
        if (at == EWW_TOOL_PENCIL)
            iTilePickerOffUp += 45;
        iTilePickerOffDown += 10;
    }

    cbShowTileID->setVisible(!isBrush);
    cbShowProperties->setVisible(!isBrush);
    sliPicker->setY(iTilePickerOffUp);
    sliPicker->setHeight(myWin.getHeight() - iTilePickerOffUp - iTilePickerOffDown);
    RefreshSlider(forceSliderRefresh);
}

void winTilePicker::RefreshSlider(bool forceSliderRefresh) {
    auto *es = GV->editState;
    cTileImageSet *set = es->hTileset->GetSet(es->GetActivePlane()->GetImageSet(0));
    int scale = 0;
    if (!set)
        scale = 0;
    else if (es->iActiveTool == EWW_TOOL_BRUSH)
        scale = set->GetBrushesCount() * (iTileDim + 4) - sliPicker->getHeight();
    else
        scale = int((set->GetTilesCount() + 2) / 4 + 1) * (iTileDim + 4) - sliPicker->getHeight() + 10;

    sliPicker->setEnabled(scale > 0);
    if (scale > 0)
        sliPicker->setScaleEnd(scale);
    else
        sliPicker->setValue(0);

    if (!myWin.isVisible() || forceSliderRefresh) {
        sliPicker->setValue(0);
    }
}

void winTilePicker::HandleBrushSwitch(int itOld, int itNew) {
    int off = 0;
    auto *plane = GV->editState->hParser->GetPlane(GV->editState->MDI->GetActiveDoc()->iSelectedPlane);
    cTileImageSet *set = GV->editState->hTileset->GetSet(plane->GetImageSet(0));
    if (!set) return;
    if (itOld >= 0 && itOld < set->GetBrushesCount()) {
        auto *br = set->GetBrushByIterator(itOld);
        if (br && br->HasSettings()) {
            off = br->GetSettingsHeight() + 10;
            br->RemoveSettingsFromContainer(&myWin);
            if (sliPicker->isEnabled())
                sliPicker->setValue(sliPicker->getValue() + off);
            Rebuild();
        }
    }
    if (itNew >= 0 && itOld < set->GetBrushesCount()) {
        auto *br = set->GetBrushByIterator(itNew);
        if (br && br->HasSettings()) {
            off = br->GetSettingsHeight() + 10;
            br->AddSettingsToContainer(&myWin, 5 + iInventorySeparator, myWin.getHeight() - 60 - br->GetSettingsHeight());
            if (sliPicker->isEnabled())
                sliPicker->setValue(sliPicker->getValue() - off);
            Rebuild();
        }
    }
}

void winTilePicker::CloseBrush() {
    auto *es = GV->editState;
    if (!es->MDI->GetActiveDoc())
        return;
    int brushPicked = es->MDI->GetActiveDoc()->iBrushPicked;
    if (brushPicked >= 0) {
        auto set = es->hTileset->GetSet(es->GetActivePlane()->GetImageSet(0));
        if (!set || set->GetBrushesCount() == 0 || brushPicked >= set->GetBrushesCount()) {
            es->MDI->GetActiveDoc()->iBrushPicked = EWW_TILE_NONE;
            return;
        }
        auto *br = set->GetBrushByIterator(brushPicked);
        if (br && br->HasSettings())
            br->RemoveSettingsFromContainer(&myWin);
        es->MDI->GetActiveDoc()->iBrushPicked = EWW_TILE_NONE;
    }
}