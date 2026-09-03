#include "tilePicker.h"
#include "../langID.h"
#include "../states/editing_ww.h"
#include "../../shared/commonFunc.h"
#include "../states/imageImport.h"
#include "../databanks/tiles.h"
#include "../cBrush.h"

winTilePicker::winTilePicker() : cWindow(GETL2S("TilePicker", "WinCaption"), 292, 300) {
    vp = new WIDG::Viewport(this, 0);
    myWin.add(vp, 0, 0);

    butPoint = MakeButton(5, 15, Icon_Point);
    butLine = MakeButton(5 + 32, 15, Icon_Line);
    butRect = MakeButton(5 + 32 * 2, 15, Icon_Rect);
    butEllipse = MakeButton(5 + 32 * 3, 15, Icon_Ellipse);
    butFlood = MakeButton(5 + 32 * 4, 15, Icon_Fill);
    butSpray = MakeButton(5 + 32 * 5, 15, Icon_Spray);

    labToolOptions = new SHR::Lab(GETL2S("TilePicker", "ToolOptions"));
    labToolOptions->adjustSize();
    myWin.add(labToolOptions, 5, 55);

    char labtmp[128];
    sprintf(labtmp, "%s: 1", GETL2S("TilePicker", "Pointsize"));
    labPointSize = new SHR::Lab(labtmp);
    labPointSize->adjustSize();
    myWin.add(labPointSize, 5, 75);

    sliPointSize = new SHR::Slider(1, 20);
    sliPointSize->setOrientation(SHR::Slider::HORIZONTAL);
    sliPointSize->setStyle(SHR::Slider::POINTER);
    sliPointSize->setDimension(gcn::Rectangle(0, 0, 270, 14));
    sliPointSize->addActionListener(this);
    myWin.add(sliPointSize, 5, 75 + 20);

    sprintf(labtmp, "%s: 1", GETL2S("TilePicker", "LineThickness"));
    labLineThickness = new SHR::Lab(labtmp);
    labLineThickness->adjustSize();
    myWin.add(labLineThickness, 5, 75);

    sliLineThickness = new SHR::Slider(1, 20);
    sliLineThickness->setOrientation(SHR::Slider::HORIZONTAL);
    sliLineThickness->setStyle(SHR::Slider::POINTER);
    sliLineThickness->setDimension(gcn::Rectangle(0, 0, 270, 14));
    sliLineThickness->addActionListener(this);
    myWin.add(sliLineThickness, 5, 75 + 20);

    cbRectFilled = new SHR::CBox(GV->hGfxInterface, GETL2S("TilePicker", "Filled"));
    cbRectFilled->setSelected(1);
    cbRectFilled->adjustSize();
    myWin.add(cbRectFilled, 5, 75);

    cbEllipseFilled = new SHR::CBox(GV->hGfxInterface, GETL2S("TilePicker", "Filled"));
    cbEllipseFilled->setSelected(1);
    cbEllipseFilled->adjustSize();
    myWin.add(cbEllipseFilled, 5, 75);

    sprintf(labtmp, "%s: 2", GETL2S("TilePicker", "SpraySize"));
    labSpraySize = new SHR::Lab(labtmp);
    labSpraySize->adjustSize();
    myWin.add(labSpraySize, 5, 75);

    sliSpraySize = new SHR::Slider(2, 20);
    sliSpraySize->setOrientation(SHR::Slider::HORIZONTAL);
    sliSpraySize->setStyle(SHR::Slider::POINTER);
    sliSpraySize->addActionListener(this);
    sliSpraySize->setDimension(gcn::Rectangle(0, 0, 270, 14));
    myWin.add(sliSpraySize, 5, 75 + 20);

    sprintf(labtmp, "%s: 50%%", GETL2S("TilePicker", "SprayDensity"));
    labSprayDensity = new SHR::Lab(labtmp);
    labSprayDensity->adjustSize();
    myWin.add(labSprayDensity, 5, 75 + 20 * 2);

    sliSprayDensity = new SHR::Slider(10, 100);
    sliSprayDensity->setOrientation(SHR::Slider::HORIZONTAL);
    sliSprayDensity->setStyle(SHR::Slider::POINTER);
    sliSprayDensity->addActionListener(this);
    sliSprayDensity->setValue(50);
    sliSprayDensity->setDimension(gcn::Rectangle(0, 0, 270, 14));
    myWin.add(sliSprayDensity, 5, 75 + 20 * 3);

    butReloadBrush = MakeButton(5, 10, Icon_Reload);
    labReloadBrush = new SHR::Lab(GETL2S("TilePicker", "ReloadBrushes"));
    labReloadBrush->adjustSize();
    myWin.add(labReloadBrush, 5 + 35, 10);

    cbShowTileID = new SHR::CBox(GV->hGfxInterface, GETL2S("TilePicker", "ShowTileID"));
    cbShowTileID->adjustSize();
    cbShowTileID->addActionListener(this);
    myWin.add(cbShowTileID, 5, 10);

    cbShowProperties = new SHR::CBox(GV->hGfxInterface, GETL2S("TilePicker", "ShowTileProp"));
    cbShowProperties->adjustSize();
    cbShowProperties->addActionListener(this);
    myWin.add(cbShowProperties, 5, 10);

    sliPicker = new SHR::Slider(0);
    sliPicker->setOrientation(SHR::Slider::VERTICAL);
    sliPicker->setDimension(gcn::Rectangle(0, 0, 15, 100));
    sliPicker->setMarkerLength(40);
    myWin.add(sliPicker, 272, 100);
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
    auto *es = GV->editState;
    int at = es->iActiveTool;
    if (at == EWW_TOOL_BRUSH || at == EWW_TOOL_FILL || at == EWW_TOOL_PENCIL) {
        if (at == EWW_TOOL_BRUSH && es->MDI->GetActiveDoc() != NULL && es->iTilePicked != EWW_TILE_NONE)
            HandleBrushSwitch(at, EWW_TILE_NONE);
        es->iTilePicked = EWW_TILE_NONE;
    }
}

void winTilePicker::action(const ActionEvent &actionEvent) {
    auto *es = GV->editState;
    if (actionEvent.getSource() == butPoint) {
        es->iTileDrawMode = EWW_DRAW_POINT;
        if (es->iActiveTool == EWW_TOOL_FILL)
            es->SetTool(EWW_TOOL_PENCIL);
        Rebuild();
    } else if (actionEvent.getSource() == butLine) {
        es->iTileDrawMode = EWW_DRAW_LINE;
        if (es->iActiveTool == EWW_TOOL_FILL)
            es->SetTool(EWW_TOOL_PENCIL);
        Rebuild();
    } else if (actionEvent.getSource() == butRect) {
        es->iTileDrawMode = EWW_DRAW_RECT;
        if (es->iActiveTool == EWW_TOOL_FILL)
            es->SetTool(EWW_TOOL_PENCIL);
        Rebuild();
    } else if (actionEvent.getSource() == butEllipse) {
        es->iTileDrawMode = EWW_DRAW_ELLIPSE;
        if (es->iActiveTool == EWW_TOOL_FILL)
            es->SetTool(EWW_TOOL_PENCIL);
        Rebuild();
    } else if (actionEvent.getSource() == butSpray) {
        es->iTileDrawMode = EWW_DRAW_SPRAY;
        if (es->iActiveTool == EWW_TOOL_FILL)
            es->SetTool(EWW_TOOL_PENCIL);
        Rebuild();
    } else if (actionEvent.getSource() == butFlood) {
        es->SetTool(EWW_TOOL_FILL);
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
        HandleBrushSwitch(es->iTilePicked, EWW_TILE_NONE);
        es->iTilePicked = EWW_TILE_NONE;
        es->hTileset->ReloadBrushes();
        Rebuild();
        es->gui->focusNone();
    }
}

void winTilePicker::Think() {
    auto *vport = GV->editState->vPort;
    if (myWin.getHeight() != vport->GetHeight())
        Rebuild();
    if (myWin.getY() != vport->GetY())
        myWin.setY(vport->GetY());
}

void winTilePicker::Draw(int piCode) {
    auto *es = GV->editState;
    float mx, my;
    hge->Input_GetMousePos(&mx, &my);
    bool hasMouse = es->conMain->getWidgetAt(mx, my) == &myWin;
    int dx, dy;
    myWin.getAbsolutePosition(dx, dy);
    int dx2 = dx + myWin.getWidth() - 1,
        dy2 = dy + myWin.getHeight() - 1;

    hge->Gfx_RenderLine(dx, dy + iTilePickerOffUp + 13, dx2, dy + iTilePickerOffUp + 13, GV->colLineDark);
    hge->Gfx_RenderLine(dx, dy + iTilePickerOffUp + 14, dx2, dy + iTilePickerOffUp + 14, GV->colLineBright);

    hge->Gfx_RenderLine(dx, dy2 - iTilePickerOffDown + 20, dx2, dy2 - iTilePickerOffDown + 20, GV->colLineDark);
    hge->Gfx_RenderLine(dx, dy2 - iTilePickerOffDown + 21, dx2, dy2 - iTilePickerOffDown + 21, GV->colLineBright);

    cTileImageSet *set = es->hTileset->GetSet(es->GetActivePlane()->GetImageSet(0));
    if (!set)
        return;

    if (es->iActiveTool == EWW_TOOL_BRUSH && es->iTilePicked >= 0 && set->GetBrushByIterator(es->iTilePicked)->HasSettings()) {
        hge->Gfx_RenderLine(dx, dy2 - 40, dx2, dy2 - 40, GV->colLineDark);
        hge->Gfx_RenderLine(dx, dy2 - 39, dx2, dy2 - 39, GV->colLineBright);
    }

    int itpx = dx + 2,
        itpy = dy + iTilePickerOffUp + 16;
    int clipx = myWin.getWidth() - 20;
    hge->Gfx_SetClipping(itpx, itpy, clipx, sliPicker->getHeight());

    if (sliPicker->isEnabled() && mx > itpx && mx < dx + clipx && my > itpy && my < itpy + sliPicker->getHeight()) {
        sliPicker->setValue(sliPicker->getValue() - 100 * hge->Input_GetMouseWheel());
    }

    if (es->iActiveTool == EWW_TOOL_BRUSH && set->GetBrushesCount() == 0) {
        GV->fntMyriad16->SetColor(0xFFe1e1e1);
        GV->fntMyriad16->printf(itpx + (myWin.getWidth() - 20) / 2, itpy + sliPicker->getHeight() / 2,
                HGETEXT_CENTER, GETL(Lang_NoBrushes), 0);
        return;
    }

    int scroll = sliPicker->getValue();
    if (es->iActiveTool == EWW_TOOL_BRUSH) {
        int ioldsel = es->iTilePicked;
        int loopst = scroll / (iTileDim + 3),
            loopen = sliPicker->getHeight() / (iTileDim + 3) + 2;
        for (int i = 0; i < set->GetBrushesCount() && i < (loopst + loopen); i++) {
            int drx = itpx + 2 + 4,
                dry = itpy + 4 + i * (iTileDim + 4) - scroll;
            cBrush *brush = set->GetBrushByIterator(i);
            hgeSprite *spr = NULL;
            if (brush->GetIcon() == BRUSHICON_INVALID) {
                spr = GV->gcnParts.sprIconError;
            } else {
                cTile *tile = set->GetTile(brush->GetIcon());
                if (tile == NULL) {
                    spr = GV->sprSmiley;
                } else {
                    spr = tile->GetImage();
                    spr->SetColor(0xFFFFFFFF);
                    spr->SetFlip(0, 0);
                    spr->SetHotSpot(0, 0);
                }
            }
            DWORD colBorder = GV->colLineBright;
            if (hasMouse && (mx > dx && my > dry - 1 && mx < dx + clipx && my < dry + iTileDim + 1)
                    && my > itpy && my < itpy + sliPicker->getHeight()) {
                colBorder = TILE_HIGHLIGHT_COLOR;
                if (hge->Input_KeyDown(HGEK_LBUTTON))
                    es->iTilePicked = i;
            } else if (i == es->iTilePicked) {
                colBorder = TILE_PICK_COLOR;
            }
            hge->Gfx_RenderLine(drx - 1, dry, drx + iTileDim + 1, dry, colBorder);
            hge->Gfx_RenderLine(drx, dry + iTileDim + 1, drx + iTileDim + 1, dry + iTileDim + 1, colBorder);
            hge->Gfx_RenderLine(drx, dry, drx, dry + iTileDim + 1, colBorder);
            hge->Gfx_RenderLine(drx + iTileDim + 1, dry, drx + iTileDim + 1, dry + iTileDim + 1, colBorder);

            GV->fntMyriad16->SetColor(colBorder == GV->colLineBright ? 0xFFe1e1e1 : colBorder);
            GV->fntMyriad16->Render(drx + iTileDim + 8, dry + iTileDim/2 - 10, HGETEXT_LEFT, brush->GetName(), 0);

            spr->RenderStretch(drx, dry, drx + iTileDim, dry + iTileDim);
            hge->Gfx_RenderLine(drx - 2, dry - 1, drx + iTileDim + 2, dry - 1, GV->colLineDark);
            hge->Gfx_RenderLine(drx - 1, dry + iTileDim + 2, drx + iTileDim + 2, dry + iTileDim + 2, GV->colLineDark);
            hge->Gfx_RenderLine(drx - 1, dry - 1, drx - 1, dry + iTileDim + 2, GV->colLineDark);
            hge->Gfx_RenderLine(drx + iTileDim + 2, dry - 1, drx + iTileDim + 2, dry + iTileDim + 2, GV->colLineDark);
        }
        if (ioldsel != es->iTilePicked)
            HandleBrushSwitch(ioldsel, es->iTilePicked);
    } 
    else {
        int tilesCount = set->GetTilesCount();
        int loopst = scroll / (iTileDim + 4)  * 4,
            loopen = sliPicker->getHeight() / (iTileDim + 4) * 4 + 8;
        if (loopst < 0) loopst = 0;
        for (int i = -3 + loopst; i < tilesCount && i < (-2 + loopst + loopen); i++) {
            int idx = (i + 3) % 4,
                idy = (i + 3) / 4;
            int drx = itpx + 2 + idx * (iTileDim + 4),
                dry = itpy + 2 + idy * (iTileDim + 4) - scroll;
            if (i == -3) {
                GV->sprIcons[Icon_Pipette]->Render(drx + iTileDim/4, dry + iTileDim/4);
            } else if (i == -2) {
                GV->sprIcons[Icon_Erase]->Render(drx + iTileDim/4, dry + iTileDim/4);
            } else if (i == -1) {
                SHR::SetQuad(&q, es->hDataCtrl->GetPalette()->GetColor(es->GetActivePlane()->GetFillColor()), drx, dry,
                                drx + iTileDim, dry + iTileDim);
                hge->Gfx_RenderQuad(&q);
            } else {
                cTile *tile = set->GetTileByIterator(i);
                tile->GetImage()->SetFlip(0, 0);
                tile->GetImage()->SetHotSpot(0, 0);
                tile->GetImage()->SetColor(0xFFFFFFFF);
                int tileW = GV->editState->GetActivePlane()->GetTileWidth(),
                    tileH = GV->editState->GetActivePlane()->GetTileHeight(),
                    maxTileW =  GV->editState->hTileset->GetMaxTileWidth(),
                    maxTileH = GV->editState->hTileset->GetMaxTileHeight();
                tile->GetImage()->RenderStretch(drx, dry, drx + (iTileDim*maxTileW/tileW), dry + (iTileDim*maxTileH/tileH));
                if (cbShowProperties->isSelected()) {
                    float scaleX = (float)iTileDim / tileW,
                        scaleY = (float)iTileDim / tileH;
                    es->DrawTileAttributes(tile->GetID(), drx, dry, scaleX, scaleY);
                }
                if (cbShowTileID->isSelected()) {
                    GV->fntMyriad16->SetColor(0xFF000000);
                    GV->fntMyriad16->printf(drx + iTileDim/2 + 1, dry + 1 + 0.75f*iTileDim, HGETEXT_CENTER, "%d", 0, tile->GetID());
                    GV->fntMyriad16->SetColor(0xFFFFFFFF);
                    GV->fntMyriad16->printf(drx + iTileDim/2, dry + 0.75f*iTileDim, HGETEXT_CENTER, "%d", 0, tile->GetID());
                }
            }
            hge->Gfx_RenderLine(drx - 2, dry - 1, drx + iTileDim + 2, dry - 1, GV->colLineDark);
            hge->Gfx_RenderLine(drx - 1, dry + iTileDim + 2, drx + iTileDim + 2, dry + iTileDim +2, GV->colLineDark);
            hge->Gfx_RenderLine(drx - 1, dry - 1, drx - 1, dry + iTileDim + 2, GV->colLineDark);
            hge->Gfx_RenderLine(drx + iTileDim + 2, dry - 1, drx + iTileDim + 2, dry + iTileDim + 2, GV->colLineDark);
            DWORD colBorder = GV->colLineBright;
            if (es->iTilePicked == EWW_TILE_PIPETTE && es->iPipetteTileHL != EWW_TILE_NONE)
                if (es->iPipetteTileHL == EWW_TILE_ERASE && i == -2)
                    colBorder = TILE_HIGHLIGHT_COLOR;
                else if (es->iPipetteTileHL == EWW_TILE_FILL && i == -1)
                    colBorder = TILE_HIGHLIGHT_COLOR;
                else if (es->iPipetteTileHL == i)
                    colBorder = TILE_HIGHLIGHT_COLOR;
            if (mx > drx && my > dry && mx < drx + iTileDim && my < dry + iTileDim && hasMouse && my > itpy &&
                my < itpy + sliPicker->getHeight()) {
                colBorder = TILE_HIGHLIGHT_COLOR;
                if (hge->Input_KeyDown(HGEK_LBUTTON)) {
                    es->iTilePicked = (i < 0 ? i - 1 : i);
                    if (i == -1)
                        es->wintpFillColor->setVisible(1);
                }
            } else if (i < 0 && i - 1 == es->iTilePicked || i >= 0 && i == es->iTilePicked ||
                        es->iTilePicked == EWW_TILE_PIPETTE && i == -3) {
                colBorder = TILE_PICK_COLOR;
            }
            hge->Gfx_RenderLine(drx - 1, dry, drx + iTileDim + 1, dry, colBorder);
            hge->Gfx_RenderLine(drx, dry + iTileDim + 1, drx + iTileDim + 1, dry + iTileDim + 1, colBorder);
            hge->Gfx_RenderLine(drx, dry, drx, dry + iTileDim + 1, colBorder);
            hge->Gfx_RenderLine(drx + iTileDim + 1, dry, drx + iTileDim + 1, dry + iTileDim + 1, colBorder);
        }
    }
}

void winTilePicker::Rebuild(bool forceSliderRefresh) {
    auto *es = GV->editState;
    int at = es->iActiveTool,
        tp = es->iTilePicked,
        dm = es->iTileDrawMode;

    myWin.setY(es->vPort->GetY());
    myWin.setHeight(es->vPort->GetHeight());
    butReloadBrush->setY(myWin.getHeight() - 50);
    labReloadBrush->setY(myWin.getHeight() - 50 + 6);
    cbShowTileID->setY(myWin.getHeight() - 60);
    cbShowProperties->setY(myWin.getHeight() - 40);

    butPoint->setHighlight(at != EWW_TOOL_FILL && at != EWW_TILE_PIPETTE && dm == EWW_DRAW_POINT);
    butLine->setHighlight(at != EWW_TOOL_FILL && at != EWW_TILE_PIPETTE && dm == EWW_DRAW_LINE);
    butRect->setHighlight(at != EWW_TOOL_FILL && at != EWW_TILE_PIPETTE && dm == EWW_DRAW_RECT);
    butEllipse->setHighlight(at != EWW_TOOL_FILL && at != EWW_TILE_PIPETTE && dm == EWW_DRAW_ELLIPSE);
    butFlood->setHighlight(at != EWW_TILE_PIPETTE && at == EWW_TOOL_FILL);
    butSpray->setHighlight(at != EWW_TOOL_FILL && at != EWW_TILE_PIPETTE && dm == EWW_DRAW_SPRAY);

    butPoint->setVisible(at != EWW_TOOL_BRUSH);
    butLine->setVisible(at != EWW_TOOL_BRUSH);
    butRect->setVisible(at != EWW_TOOL_BRUSH);
    butEllipse->setVisible(at != EWW_TOOL_BRUSH);
    butFlood->setVisible(at != EWW_TOOL_BRUSH);
    butSpray->setVisible(at != EWW_TOOL_BRUSH);
    butReloadBrush->setVisible(at == EWW_TOOL_BRUSH);
    labReloadBrush->setVisible(at == EWW_TOOL_BRUSH);

    labToolOptions->setVisible(at == EWW_TOOL_PENCIL);
    labPointSize->setVisible(at == EWW_TOOL_PENCIL && dm == EWW_DRAW_POINT);
    sliPointSize->setVisible(at == EWW_TOOL_PENCIL && dm == EWW_DRAW_POINT);
    labLineThickness->setVisible(at == EWW_TOOL_PENCIL && dm == EWW_DRAW_LINE);
    sliLineThickness->setVisible(at == EWW_TOOL_PENCIL && dm == EWW_DRAW_LINE);
    cbRectFilled->setVisible(at == EWW_TOOL_PENCIL && dm == EWW_DRAW_RECT);
    cbEllipseFilled->setVisible(at == EWW_TOOL_PENCIL && dm == EWW_DRAW_ELLIPSE);
    labSpraySize->setVisible(at == EWW_TOOL_PENCIL && dm == EWW_DRAW_SPRAY);
    labSprayDensity->setVisible(at == EWW_TOOL_PENCIL && dm == EWW_DRAW_SPRAY);
    sliSpraySize->setVisible(at == EWW_TOOL_PENCIL && dm == EWW_DRAW_SPRAY);
    sliSprayDensity->setVisible(at == EWW_TOOL_PENCIL && dm == EWW_DRAW_SPRAY);

    if (at == EWW_TOOL_BRUSH) {
        iTilePickerOffUp = 0;
        iTilePickerOffDown = 60;
        if (at != -1) {
            cBrush *br = es->hTileset->GetSet(es->GetActivePlane()->GetImageSet(0))->GetBrushByIterator(at);
            if (at >= 0 && br->HasSettings())
                iTilePickerOffDown += 10 + br->GetSettingsHeight();
        }
    } else {
        iTilePickerOffUp = 40 + 32 * 2;
        iTilePickerOffDown = 70;
    }
    iTilePickerOffUp += 15 + 40;

    cbShowTileID->setVisible(at != EWW_TOOL_BRUSH);
    cbShowProperties->setVisible(at != EWW_TOOL_BRUSH);
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
    bool changed = false;
    int oOff = 0, nOff = 0;
    cTileImageSet *set = GV->editState->hTileset->GetSet(GV->editState->GetActivePlane()->GetImageSet(0));
    if (itOld >= 0 && set->GetBrushByIterator(itOld)->HasSettings()) {
        oOff = set->GetBrushByIterator(itOld)->GetSettingsHeight() + 10;
        set->GetBrushByIterator(itOld)->RemoveSettingsFromContainer(&myWin);
        changed = true;
    }
    if (changed) {
        Rebuild();
        if (nOff != oOff && sliPicker->isEnabled())
            sliPicker->setValue(sliPicker->getValue() + (nOff - oOff));
        changed = false;
    }
    if (itNew >= 0 && set->GetBrushByIterator(itNew)->HasSettings()) {
        nOff = set->GetBrushByIterator(itNew)->GetSettingsHeight() + 10;
        set->GetBrushByIterator(itNew)->AddSettingsToContainer(
                &myWin, 5, myWin.getHeight() - 60 - set->GetBrushByIterator(itNew)->GetSettingsHeight());
        changed = true;
    }
    if (changed) {
        Rebuild();
        if (nOff != oOff && sliPicker->isEnabled())
            sliPicker->setValue(sliPicker->getValue() + (nOff - oOff));
    }
}

void winTilePicker::CloseBrush() {
    auto *es = GV->editState;
    if (es->iTilePicked >= 0 && es->MDI->GetActiveDoc() != NULL) {
        auto set = es->GetActivePlane()->GetImageSet(0);
        es->hTileset->GetSet(set)->GetBrushByIterator(es->iTilePicked)->RemoveSettingsFromContainer(&myWin);
    }
}