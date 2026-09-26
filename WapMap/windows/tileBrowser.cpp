#include "tileBrowser.h"
#include "../langID.h"
#include "../states/editing_ww.h"
#include "../../shared/commonFunc.h"
#include "../states/imageImport.h"
#include "../databanks/tiles.h"
#include "../states/dialog.h"

#define TILES_GROUP_SPACE 32
#define WIN_TILE_BROWSER_WIDTH 850

winTileBrowser::winTileBrowser() : cWindow(GETL2S("Win_TileBrowser", "WinCaption"), WIN_TILE_BROWSER_WIDTH, 600) {
    iSelectedTileSet = iSelectedTile = 0;
    iHighlightedTS = iHighlightedT = -1;

    labTileSets = new SHR::Lab(GETL2S("Win_TileBrowser", "Tilesets"));
    labTileSets->adjustSize();
    myWin.add(labTileSets, 8, 15);

    conTileSets = new SHR::Container();
    conTileSets->setDimension(gcn::Rectangle(0, 0, 100, 100));
    conTileSets->setOpaque(0);
    saTileSets = new SHR::ScrollArea(conTileSets, SHR::ScrollArea::SHOW_AUTO, SHR::ScrollArea::SHOW_ALWAYS);
    saTileSets->setDimension(gcn::Rectangle(0, 0, 215, 520));
    myWin.add(saTileSets, 5, 35);
    conTileSets->setWidth(saTileSets->getChildrenArea().width);

    conTiles = new SHR::Container();
    conTiles->setDimension(gcn::Rectangle(0, 0, 100, 100));
    conTiles->setOpaque(0);
    saTiles = new SHR::ScrollArea(conTiles, SHR::ScrollArea::SHOW_AUTO, SHR::ScrollArea::SHOW_ALWAYS);
    saTiles->setDimension(gcn::Rectangle(0, 0, WIN_TILE_BROWSER_WIDTH - 226, 600 - 211));
    saTiles->setOpaque(0);
    myWin.add(saTiles, 220, 190);
    conTiles->setWidth(saTiles->getChildrenArea().width);

    butImportTileSet = MakeButton(L"ImportTileset", Icon16_Add, 3, 560, 145 + 74, 20);

    int x = 230, y = 15, caplen = 0;

    labTileSetName = MakeLabel(L"TilesetName", x, y);
    if (labTileSetName->getWidth() > caplen) caplen = labTileSetName->getWidth();

    labTileSetChecksum = MakeLabel(L"TilesetChecksum", x, y + 20);
    if (labTileSetChecksum->getWidth() > caplen) caplen = labTileSetChecksum->getWidth();

    labTileCount = MakeLabel(L"TileCount", x, y + 40);
    if (labTileCount->getWidth() > caplen) caplen = labTileCount->getWidth();

    x += caplen + 10;

    labTileSetNameV = new SHR::Lab("");
    labTileSetNameV->setColor(0xFFFFFFFF);
    myWin.add(labTileSetNameV, x, y);

    labTileSetChecksumV = new SHR::Lab("");
    labTileSetChecksumV->setColor(0xFFFFFFFF);
    myWin.add(labTileSetChecksumV, x, y + 20);

    labTileCountV = new SHR::Lab("");
    labTileCountV->setColor(0xFFFFFFFF);
    myWin.add(labTileCountV, x, y + 40);

    x = WIN_TILE_BROWSER_WIDTH - 155;

    butAddTiles = MakeButton(L"AddTiles", Icon16_Add, x, y);
    butRenameTileSet = MakeButton(L"RenameTileset", Icon16_Pencil, x, y + 20);
    butDeleteTileSet = MakeButton(L"DeleteTileset", Icon16_Trash, x, y + 40);

    x = 230 + 110;
    y = 85;
    caplen = 0;

    labTileID = MakeLabel(L"TileID", x, y);
    if (labTileID->getWidth() > caplen) caplen = labTileID->getWidth();

    labTileChecksum = MakeLabel(L"TileChecksum", x, y + 20);
    if (labTileChecksum->getWidth() > caplen) caplen = labTileChecksum->getWidth();

    labTileModDate = MakeLabel(L"TileModDate", x, y + 40);
    if (labTileModDate->getWidth() > caplen) caplen = labTileModDate->getWidth();

    labTileOrigin = MakeLabel(L"TileOrigin", x, y + 60);
    if (labTileOrigin->getWidth() > caplen) caplen = labTileOrigin->getWidth();

    labTileSize = MakeLabel(L"ImageSize", x, y + 80);
    if (labTileSize->getWidth() > caplen) caplen = labTileSize->getWidth();

    x += caplen + 10;

    labTileIDV = new SHR::Lab("");
    labTileIDV->setColor(0xFFFFFFFF);
    myWin.add(labTileIDV, x, y);

    labTileChecksumV = new SHR::Lab("");
    labTileChecksumV->setColor(0xFFFFFFFF);
    myWin.add(labTileChecksumV, x, y + 20);

    labTileModDateV = new SHR::Lab("");
    labTileModDateV->setColor(0xFFFFFFFF);
    myWin.add(labTileModDateV, x, y + 40);

    labTileOriginV = new SHR::Lab("");
    labTileOriginV->setColor(0xFFFFFFFF);
    myWin.add(labTileOriginV, x, y + 60);

    labTileSizeV = new SHR::Lab("");
    labTileSizeV->setColor(0xFFFFFFFF);
    myWin.add(labTileSizeV, x, y + 80);

    x = WIN_TILE_BROWSER_WIDTH - 155;
    y = 90;

    butChangeTileID = MakeButton(L"ChangeID", Icon16_WriteID, x, y);
    butBrowseFolder = MakeButton(L"BrowseDirectory", Icon16_Open, x, y + 20);
    butImageDetails = MakeButton(L"ImageDetails", Icon16_Flip, x, y + 40);
    butDeleteTile = MakeButton(L"DeleteTile", Icon16_Trash, x, y + 60);

    vpTileBrowser = new WIDG::Viewport(this, 0);
    myWin.add(vpTileBrowser, 5, 5);
}

SHR::But *winTileBrowser::MakeButton(const wchar_t* lstr, Gfx16Icons icon, int x, int y, int w, int h) {
    auto *but = new SHR::But(GV->hGfxInterface, GETL2SV("Win_TileBrowser", lstr));
    but->setDimension(gcn::Rectangle(0, 0, w, h));
    but->setIcon(GV->sprIcons16[icon]);
    but->addActionListener(this);
    myWin.add(but, x, y);
    return but;
}

SHR::Lab *winTileBrowser::MakeLabel(const wchar_t* lstr, int x, int y) {
    auto *lab = new SHR::Lab(GETL2SV("Win_TileBrowser", lstr));
    lab->adjustSize();
    myWin.add(lab, x, y);
    return lab;
}

void winTileBrowser::Think() {
    cBankTile *hTileset = GV->editState->hTileset;
    if (hTileset->GetSetsCount() == 0) {
        return;
    }

    if (hTileset->GetModFlag()) {
        Synchronize();
    }

    float mx, my;
    hge->Input_GetMousePos(&mx, &my);
    if (myWin.getParent()->getWidgetAt(mx, my) != &myWin) return;
    int dx, dy;
    myWin.getAbsolutePosition(dx, dy);
    mx -= dx;
    my -= dy;

    if (mx > 6 && mx < 210 && my > 45 && my < 45 + 530) {
        int tsid = (my + saTileSets->getVerticalScrollAmount() - 50) / 68;
        iHighlightedTS = (tsid < hTileset->GetSetsCount() ? tsid : -1);
    } else
        iHighlightedTS = -1;

    if (iHighlightedTS != -1 && hge->Input_KeyDown(HGEK_LBUTTON)) {
        iSelectedTileSet = iHighlightedTS;
        iSelectedTile = 0;
        Synchronize();
        saTiles->setVerticalScrollAmount(0);
    }

    cTileImageSet *ts = hTileset->GetSet((iSelectedTileSet));

    int tilePickX = dx + 230, tilePickW = myWin.getWidth() - 230 - 16,
        tilePickY = dy + 205, tilePickH = myWin.getHeight() - 210;
    int tilesPerRow = (tilePickW / 60);
    int borderOffset = (tilePickW - (tilesPerRow * 60)) / 2;
    int scroll = saTiles->getVerticalScrollAmount();

    mx += dx;
    my += dy;
    if (mx > tilePickX && my > tilePickY && mx < tilePickX + tilePickW && my < tilePickY + tilePickH) {
        if (bSingleGroup) {
            iHighlightedT = MouseHandleGroup(*hSingleGroup, tilePickX + borderOffset,
                    tilePickY + borderOffset - scroll, tilesPerRow);
        } else {
            iHighlightedT = -1;
            int yPos = -TILES_GROUP_SPACE;
            for (auto & vtGroup : vtGroups)
                if (!vtGroup.empty()) {
                    yPos += TILES_GROUP_SPACE;
                    int drawX = tilePickX + borderOffset,
                        drawY = tilePickY + borderOffset - scroll + yPos;
                    int hl = MouseHandleGroup(vtGroup, drawX, drawY + borderOffset * 3, tilesPerRow);
                    if (hl != -1 && hl < vtGroup.size()) {
                        cTile *t = vtGroup[hl];
                        for (int j = 0; j < ts->GetTilesCount(); j++)
                            if (ts->GetTileByIterator(j) == t) {
                                iHighlightedT = j;
                                break;
                            }
                    }
                    yPos += borderOffset * 4 + ((vtGroup.size() - 1) / tilesPerRow + 1) * 80;
                }
        }
    } else {
        iHighlightedT = -1;
    }

    if (iHighlightedT != -1 && hge->Input_KeyDown(HGEK_LBUTTON)) {
        iSelectedTile = iHighlightedT;
        Synchronize();
    }
}

void winTileBrowser::Draw(int piCode) {
    cBankTile *hTileset = GV->editState->hTileset;
    int dx, dy;
    myWin.getAbsolutePosition(dx, dy);

    unsigned char alpha = myWin.getAlpha();
    DWORD col = SETA(0xFFFFFFFF, alpha);
    DWORD colDark = SETA(GV->colLineDark, alpha);
    DWORD colBright = SETA(GV->colLineBright, alpha);

    int maxTileW =  GV->editState->hTileset->GetMaxTileWidth(),
        maxTileH = GV->editState->hTileset->GetMaxTileHeight();

    //tile sets list separator
    hge->Gfx_RenderLine(dx + 227, dy + 25, dx + 227, dy + myWin.getHeight(), colDark);
    hge->Gfx_RenderLine(dx + 228, dy + 25, dx + 228, dy + myWin.getHeight(), colBright);

    //tile set properties separator
    hge->Gfx_RenderLine(dx + 226, dy + 95, dx + myWin.getWidth(), dy + 95, colDark);
    hge->Gfx_RenderLine(dx + 226, dy + 96, dx + myWin.getWidth(), dy + 96, colBright);

    //tile preview separator
    hge->Gfx_RenderLine(dx + 336, dy + 96, dx + 336, dy + 205, colDark);
    hge->Gfx_RenderLine(dx + 337, dy + 96, dx + 337, dy + 205, colBright);

    //tiles separator
    hge->Gfx_RenderLine(dx + 226, dy + 205, dx + myWin.getWidth(), dy + 205, colDark);
    hge->Gfx_RenderLine(dx + 226, dy + 206, dx + myWin.getWidth(), dy + 206, colBright);

    hge->Gfx_SetClipping(dx + 8, dy + 51, 204, 520);

    if (hTileset->GetSetsCount() == 0)
        return;

    int startSet = saTileSets->getVerticalScrollAmount() / 68;
    for (int i = startSet; i < hTileset->GetSetsCount() && i < startSet + (saTileSets->getHeight() / 68) + 2; i++) {
        int drawX = dx + 8;
        int drawY = dy + 40 + 10 + i * 68 - saTileSets->getVerticalScrollAmount();
        cTileImageSet *ts = hTileset->GetSet(i);
        cTile *ico = ts->GetTileByIterator(0);
        int tileW = ts->GetTilesWidth(),
            tileH = ts->GetTilesHeight();

        if (i == iSelectedTileSet || i == iHighlightedTS) {
            SHR::SetQuad(&q, SETA(i == iSelectedTileSet ? 0x331796ff : 0x33a1a1a1, myWin.getAlphaModifier() * 0x33),
                            drawX, drawY, drawX + 198,drawY + 68);
            hge->Gfx_RenderQuad(&q);
        }

        hge->Gfx_RenderLine(drawX + 10 - 2, drawY + 10 - 1, drawX + 10 + 50, drawY + 10 - 1, colDark);
        hge->Gfx_RenderLine(drawX + 10 - 1, drawY + 10 + 50, drawX + 10 + 50, drawY + 10 + 50, colDark);
        hge->Gfx_RenderLine(drawX + 10 - 1, drawY + 10 - 1, drawX + 10 - 1, drawY + 10 + 50, colDark);
        hge->Gfx_RenderLine(drawX + 10 + 50, drawY + 10 - 1, drawX + 10 + 50, drawY + 10 + 50, colDark);

        hge->Gfx_RenderLine(drawX + 10 - 1, drawY + 10, drawX + 10 + 49, drawY + 10, colBright);
        hge->Gfx_RenderLine(drawX + 10, drawY + 10 + 49, drawX + 10 + 49, drawY + 10 + 49, colBright);
        hge->Gfx_RenderLine(drawX + 10, drawY + 10, drawX + 10, drawY + 10 + 49, colBright);
        hge->Gfx_RenderLine(drawX + 10 + 49, drawY + 10, drawX + 10 + 49, drawY + 10 + 49, colBright);

        ico->GetImage()->SetColor(col);
        ico->GetImage()->RenderStretch(drawX + 10, drawY + 10, drawX + 10 + 48*maxTileW/tileW, drawY + 10 + 48*maxTileH/tileH);
        GV->fntMyriad16->SetColor(SETA(0xFFc1c1c1, alpha));
        GV->fntMyriad16->Render(drawX + 20 + 48, drawY + 10 + 24 - 14, HGETEXT_LEFT, ts->GetName(), 0);
        GV->fntMyriad16->SetColor(SETA(0xFFe1e1e1, alpha));
        GV->fntMyriad16->printf(drawX + 20 + 48, drawY + 10 + 24, HGETEXT_LEFT, "%d %s", 0, ts->GetTilesCount(),
                                GETL2S("Win_TileBrowser", "tiles"));
    }

    int tilePickX = dx + 230, tilePickW = myWin.getWidth() - 230 - 16,
        tilePickY = dy + 205, tilePickH = myWin.getHeight() - 210;
    cTileImageSet *tsPick = hTileset->GetSet(iSelectedTileSet);
    m_iGroupTilesW = tsPick->GetTilesWidth();
    m_iGroupTilesH = tsPick->GetTilesHeight();

    int drx = dx + 249,
        dry = dy + 118;
    hge->Gfx_SetClipping(drx, dry, 64, 64);
    GV->sprCheckboard->SetColor(col);
    GV->sprCheckboard->Render(drx, dry);
    hge->Gfx_RenderLine(drx - 2, dry - 1, drx + 64, dry - 1, colDark);
    hge->Gfx_RenderLine(drx - 1, dry + 66, drx + 65, dry + 66, colDark);
    hge->Gfx_RenderLine(drx - 2, dry - 1, drx - 2, dry + 64, colDark);
    hge->Gfx_RenderLine(drx + 66, dry - 1, drx + 66, dry + 65, colDark);

    hge->Gfx_RenderLine(drx - 1, dry, drx + 63, dry, colBright);
    hge->Gfx_RenderLine(drx, dry + 65, drx + 64, dry + 65, colBright);
    hge->Gfx_RenderLine(drx - 1, dry, drx - 1, dry + 63, colBright);
    hge->Gfx_RenderLine(drx + 65, dry, drx + 65, dry + 64, colBright);
    hge->Gfx_SetClipping();
    cTile *seltile = tsPick->GetTileByIterator(iSelectedTile);
    if (seltile) {
        seltile->GetImage()->SetColor(col);
        int pvW = 64*maxTileW/m_iGroupTilesW,
            pvH = 64*maxTileH/m_iGroupTilesH;
        seltile->GetImage()->RenderStretch(drx, dry, drx + pvW, dry + pvH);
    }

    int tilesPerRow = (tilePickW / 60);
    int borderOffset = (tilePickW - (tilesPerRow * 60)) / 2;
    int scroll = saTiles->getVerticalScrollAmount();
    if (scroll < 0) scroll = 0;

    hge->Gfx_SetClipping(tilePickX, tilePickY, tilePickW, tilePickH);
    if (bSingleGroup) {
        RenderTileGroup(*hSingleGroup, tilePickX + borderOffset, tilePickY + borderOffset - scroll);
    } else {
        int yPos = -TILES_GROUP_SPACE;
        for (int i = 0; i < 3; i++)
            if (!vtGroups[i].empty()) {
                yPos += TILES_GROUP_SPACE;
                int drawX = tilePickX + borderOffset,
                    drawY = tilePickY + borderOffset - scroll + yPos;
                const char *label;
                if (i == 0) label = GETL2S("Win_TileBrowser", "GroupREZ");
                else if (i == 1) label = GETL2S("Win_TileBrowser", "GroupClaw");
                else if (i == 2) label = GETL2S("Win_TileBrowser", "GroupCustom");
                GV->fntMyriad16->SetColor(col);
                GV->fntMyriad16->Render(drawX + 3, drawY, HGETEXT_LEFT, label, 0);
                RenderTileGroup(vtGroups[i], drawX, drawY + borderOffset * 3);
                yPos += borderOffset * 4 + ((vtGroups[i].size() - 1) / tilesPerRow + 1) * 80;
            }
    }
}

void winTileBrowser::OnDocumentChange() {
    iSelectedTileSet = iSelectedTile = 0;
    iHighlightedTS = iHighlightedT = -1;
    Synchronize();
}

void winTileBrowser::Synchronize() {
    cBankTile *hTileset = GV->editState->hTileset;
    cDataController *hDataCtrl = GV->editState->hDataCtrl;

    if (hTileset->GetSetsCount() == 0)
        return;

    if (iSelectedTileSet < 0 || iSelectedTileSet >= hTileset->GetSetsCount())
        iSelectedTileSet = 0;

    cTileImageSet *ts = hTileset->GetSet(iSelectedTileSet);
    if (iSelectedTile < 0 || iSelectedTile >= ts->GetTilesCount())
        iSelectedTile = 0;

    labTileSetNameV->setCaption(ts->GetName());
    labTileSetNameV->adjustSize();

    char tmp[64];
    sprintf(tmp, "%d", ts->GetTilesCount());
    labTileCountV->setCaption(tmp);
    labTileCountV->adjustSize();

    labTileSetChecksumV->setCaption(ts->GetHash());
    labTileSetChecksumV->adjustSize();

    bool bCustomTS = 1;
    for (int i = 0; i < ts->GetTilesCount(); i++)
        if (ts->GetTileByIterator(i)->GetFile().hFeed != hDataCtrl->GetFeed(DB_FEED_CUSTOM)) {
            bCustomTS = 0;
            break;
        }

    butRenameTileSet->setVisible(bCustomTS);
    butDeleteTileSet->setVisible(bCustomTS);

    cTile *pickedtile = ts->GetTileByIterator(iSelectedTile);

    sprintf(tmp, "%d", pickedtile->GetID());
    labTileIDV->setCaption(tmp);
    labTileIDV->adjustSize();

    labTileChecksumV->setCaption(pickedtile->GetHash());
    labTileChecksumV->adjustSize();

    const char *orig = "NULL";
    cFileFeed *feed = pickedtile->GetFile().hFeed;
    if (feed == hDataCtrl->GetFeed(DB_FEED_REZ))
        orig = GETL2S("Win_TileBrowser", "OriginREZ");
    else if (feed == hDataCtrl->GetFeed(DB_FEED_DISC))
        orig = GETL2S("Win_TileBrowser", "OriginClaw");
    else if (feed == hDataCtrl->GetFeed(DB_FEED_CUSTOM))
        orig = GETL2S("Win_TileBrowser", "OriginCustom");
    labTileOriginV->setCaption(orig);
    labTileOriginV->adjustSize();


    if (feed == hDataCtrl->GetFeed(DB_FEED_CUSTOM)) {
        butChangeTileID->setVisible(1);
        butDeleteTile->setVisible(1);
        butBrowseFolder->setVisible(1);
        butImageDetails->setY(130);
    } else {
        butChangeTileID->setVisible(0);
        butDeleteTile->setVisible(0);
        butBrowseFolder->setVisible(0);
        butImageDetails->setY(90);
    }

    char *buf = SHR::FormatTimeFromUnix(
            pickedtile->GetFile().hFeed->GetFileModTime(pickedtile->GetFile().strPath.c_str()));
    labTileModDateV->setCaption(buf);
    labTileModDateV->adjustSize();
    delete[] buf;

    sprintf(tmp, "%dx%d", pickedtile->GetImageInfo().iWidth, pickedtile->GetImageInfo().iHeight);
    labTileSizeV->setCaption(tmp);
    labTileSizeV->adjustSize();

    for (auto & vtGroup : vtGroups)
        vtGroup.clear();

    for (int i = 0; i < ts->GetTilesCount(); i++) {
        cTile *t = ts->GetTileByIterator(i);
        if (t->GetFile().hFeed == hDataCtrl->GetFeed(DB_FEED_REZ))
            vtGroups[0].push_back(t);
        else if (t->GetFile().hFeed == hDataCtrl->GetFeed(DB_FEED_DISC))
            vtGroups[1].push_back(t);
        else if (t->GetFile().hFeed == hDataCtrl->GetFeed(DB_FEED_CUSTOM))
            vtGroups[2].push_back(t);
    }

    bSingleGroup = false;
    hSingleGroup = 0;
    if (!vtGroups[0].empty() && vtGroups[1].empty() && vtGroups[2].empty()) {
        bSingleGroup = true;
        hSingleGroup = &vtGroups[0];
    } else if (vtGroups[0].empty() && !vtGroups[1].empty() && vtGroups[2].empty()) {
        bSingleGroup = true;
        hSingleGroup = &vtGroups[1];
    } else if (vtGroups[0].empty() && vtGroups[1].empty() && !vtGroups[2].empty()) {
        bSingleGroup = true;
        hSingleGroup = &vtGroups[2];
    }

    conTileSets->setHeight((hTileset->GetSetsCount() * 68 + 10));

    int tilePickW = myWin.getWidth() - 230 - 16;
    int tilesPerRow = (tilePickW / 60);
    int borderOffset = (tilePickW - (tilesPerRow * 60)) / 2;

    if (bSingleGroup) {
        conTiles->setHeight(borderOffset * 2 + ((hSingleGroup->size() - 1) / tilesPerRow + 1) * 80);
    } else {
        int len = borderOffset - TILES_GROUP_SPACE;
        for (auto & vtGroup : vtGroups)
            if (!vtGroup.empty()) {
                len += borderOffset * 4 + ((vtGroup.size() - 1) / tilesPerRow + 1) * 80 + TILES_GROUP_SPACE;
            }
        conTiles->setHeight(len);
    }
}

void winTileBrowser::Open() {
    cWindow::Open();
    Synchronize();
}

void winTileBrowser::action(const ActionEvent &actionEvent) {
    if (actionEvent.getSource() == butBrowseFolder) {
        if (iSelectedTileSet != -1 && iSelectedTile != -1) {
            cFile f = GV->editState->hTileset->GetSet(iSelectedTileSet)->GetTileByIterator(iSelectedTile)->GetFile();
            std::string filepath = f.hFeed->GetAbsoluteLocation() + "/" + f.strPath;
            size_t pos = filepath.find('/');
            while (pos != std::string::npos) {
                filepath[pos] = '\\';
                pos = filepath.find('/');
            }
            char buf[1024];
            sprintf(buf, "Explorer.exe /select,%s", filepath.c_str());
            system(buf);
        }
    } else if (actionEvent.getSource() == butAddTiles) {
        GV->StateMgr->Push(
                new State::ImageImport(State::ImageImportTile, GV->editState->hTileset->GetSet(iSelectedTileSet)));
    } else if (actionEvent.getSource() == butImportTileSet) {
        GV->StateMgr->Push(new State::ImageImport(State::ImageImportTile, 0));
    } else if (actionEvent.getSource() == butDeleteTile) {
        if (iSelectedTileSet != -1 && iSelectedTile != -1) {
            cTileImageSet* ts = GV->editState->hTileset->GetSet(iSelectedTileSet);
            cTile* t = ts->GetTileByIterator(iSelectedTile);
            int spritesCount = ts->GetTilesCount() - 1;
            if (iSelectedTile == spritesCount)
                iSelectedTile--;

            std::string strPath = t->GetFile().hFeed->GetAbsoluteLocation() + "/" + t->GetFile().strPath;
            if (std::filesystem::remove(strPath.c_str())) {
                if (!spritesCount) {
                    strPath = strPath.substr(0, strPath.rfind('/'));
                    std::filesystem::remove(strPath.c_str());
                }
            } else {
                GV->Console->Printf("~r~Failed to remove: ~w~%s", strPath.c_str());
            }
        }
    } else if (actionEvent.getSource() == butDeleteTileSet) {
        if (iSelectedTileSet == -1)
            return;
        cTileImageSet* ts = GV->editState->hTileset->GetSet(iSelectedTileSet);
        int setsCount = GV->editState->hTileset->GetSetsCount() - 1;
        if (iSelectedTileSet == setsCount) {
            --iSelectedTileSet;
        }

        std::string strPath =
            GV->editState->hDataCtrl->GetFeed(DB_FEED_CUSTOM)->GetAbsoluteLocation() + "/TILES/" + ts->GetName();
        if (!std::filesystem::remove_all(strPath)) {
            GV->Console->Printf("~r~Failed to remove: ~w~%s", strPath.c_str());
        }
    } else if (actionEvent.getSource() == butRenameTileSet) {
        RenameTileSet(GV->editState->hTileset->GetSet(iSelectedTileSet));
    } else if (actionEvent.getSource() == butChangeTileID) {
        ChangeTileID(GV->editState->hTileset->GetSet(iSelectedTileSet));
    } else if (actionEvent.getSource() == butImageDetails) {

    }
}

void winTileBrowser::RenameTileSet(cTileImageSet* ts) {
    if (GV->editState->MDI->GetActiveDoc()->bSaved == 0) {
        State::MessageBox(GETL(Lang_Error), GETL2SV("Win_TileBrowser", L"RenameErrorUnsaved"), ST_DIALOG_ICON_ERROR);
        return;
    }
    std::string initName = ts->GetName();
    int ret = State::InputDialog(GETL(Lang_WM), GETL2SV("Win_TileBrowser", L"RenameTileset"), ST_DIALOG_BUT_OKCANCEL,
            initName.c_str());
    if (ret != RETURN_OK || initName == GV->szLastInputDialogText)
        return;

    std::string newName = GV->szLastInputDialogText;
    if (newName.empty()) {
        State::MessageBox(GETL(Lang_Error), GETL2SV("Win_TileBrowser", L"RenameErrorEmpty"), ST_DIALOG_ICON_ERROR);
        return;
    }
    cBankTile *hTileset = GV->editState->hTileset;
    for (int i = 0; i < hTileset->GetSetsCount(); i++) {
        const char *name = hTileset->GetSet(i)->GetName();
        if (!strcmp(newName.c_str(), name)) {
            State::MessageBox(GETL(Lang_Error), GETL2SV("Win_TileBrowser", L"RenameErrorExists"), ST_DIALOG_ICON_ERROR);
            return;
        }
    }
    std::string root = GV->editState->hDataCtrl->GetFeed(DB_FEED_CUSTOM)->GetAbsoluteLocation() + "/TILES/";
    std::filesystem::path newPath = root + newName;
    if (std::filesystem::exists(newPath)) {
        State::MessageBox(GETL(Lang_Error), GETL2SV("Win_TileBrowser", L"RenameErrorExists"), ST_DIALOG_ICON_ERROR);
        return;
    }
    std::error_code ec;
    std::filesystem::rename(root + ts->GetName(), newPath, ec);
    if (ec) {
        State::MessageBox(GETL(Lang_Error), GETL2SV("Win_TileBrowser", L"RenameErrorFilesystem"), ST_DIALOG_ICON_ERROR);
        return;
    }
    
    ts->SetName(newName.c_str());
    auto *hParser = GV->editState->hParser;
    for (int i = 0; i < hParser->GetPlanesCount(); i++) {
        auto *plane = hParser->GetPlane(i);
        if (!strcmp(plane->GetImageSet(0), initName.c_str())) {
            plane->ClearImageSets();
            plane->AddImageSet(newName.c_str());
        }
    }
    Synchronize();
    GV->editState->MarkUnsaved();
    GV->editState->MDI->SaveCurrent();
    GV->Console->Printf("Changed the name of tile set %s to %s.", initName.c_str(), newName.c_str());
}

void winTileBrowser::ChangeTileID(cTileImageSet* ts) {
     if (GV->editState->MDI->GetActiveDoc()->bSaved == 0) {
        State::MessageBox(GETL(Lang_Error), GETL2SV("Win_TileBrowser", L"ChangeIDErrorUnsaved"), ST_DIALOG_ICON_ERROR);
        return;
    }
    auto *tile = ts->GetTileByIterator(iSelectedTile);
    int iInitID = tile->GetID();
    char szInitID[16];
    sprintf(szInitID, "%d", tile->GetID());
    int ret = State::InputNumericalDialog(GETL(Lang_WM), GETL2SV("Win_TileBrowser", L"ChangeID"), ST_DIALOG_BUT_OKCANCEL, szInitID, 5);

    if (ret != RETURN_OK || !strcmp(szInitID, GV->szLastInputDialogText.c_str()))
        return;

    std::string root = GV->editState->hDataCtrl->GetFeed(DB_FEED_CUSTOM)->GetAbsoluteLocation() + "/TILES/" + ts->GetName() + "/";
    std::string newID = GV->szLastInputDialogText;

    if (newID.empty()) {
        State::MessageBox(GETL(Lang_Error), GETL2SV("Win_TileBrowser", L"ChangeIDErrorEmpty"), ST_DIALOG_ICON_ERROR);
        return;
    }

    int iNewID = std::stoi(newID);

    if (iNewID >= (1 << 15) - 1 || iNewID < 0) {
        State::MessageBox(GETL(Lang_Error), GETL2SV("Win_TileBrowser", L"ChangeIDErrorInvalid"), ST_DIALOG_ICON_ERROR);
        return;
    }

    int maxTileID = 0;
    for (int i = 0; i < ts->GetTilesCount(); i++) {
        int id = ts->GetTileByIterator(i)->GetID();
        if (id == iNewID) {
            State::MessageBox(GETL(Lang_Error), GETL2SV("Win_TileBrowser", L"ChangeIDErrorExists"), ST_DIALOG_ICON_ERROR);
            return;
        }
        if (id > maxTileID)
            maxTileID = id;
    }

    if (iInitID == maxTileID && iNewID >= maxTileID)
        return;

    maxTileID++;
    if (iNewID > maxTileID) {
        iNewID = maxTileID;
        newID = std::to_string(maxTileID);
        const char *message = GETL2SV("Win_TileBrowser", L"ChangeIDInfoLastTile");
        char *info = new char[strlen(message) + newID.length() + 1];
        sprintf(info, message, iNewID);
        State::MessageBox(GETL(Lang_WM), info, ST_DIALOG_ICON_INFO);
        delete [] info;
    }

    int format = tile->GetImageInfo().iType;
    std::string ext = format == 0 ? ".PID" : format == 1 ? ".BMP" : format == 2 ? ".PCX" : "";
    std::filesystem::path newPath = root + newID + ext;

    if (std::filesystem::exists(newPath)) {
        State::MessageBox(GETL(Lang_Error), GETL2SV("Win_TileBrowser", L"ChangeIDErrorExists"), ST_DIALOG_ICON_ERROR);
        return;
    }

    std::error_code ec;
    std::filesystem::path curPath = root + std::to_string(iInitID) + ext;
    std::filesystem::rename(curPath, newPath, ec);
    if (ec) {
        State::MessageBox(GETL(Lang_Error), GETL2SV("Win_TileBrowser", L"ChangeIDErrorFilesystem"), ST_DIALOG_ICON_ERROR);
        return;
    }
    
    auto *hParser = GV->editState->hParser;
    for (int i = 0; i < hParser->GetPlanesCount(); i++) {
        auto *plane = hParser->GetPlane(i);
        if (strcmp(plane->GetImageSet(0), ts->GetName()))
            continue;
        for (int px = 0; px < plane->GetPlaneWidth(); px++) {
            for (int py = 0; py < plane->GetPlaneHeight(); py++) {
                if (plane->GetTile(px, py)->GetID() == iInitID)
                    plane->GetTile(px, py)->SetID(iNewID);
            }
        }
    }
    Synchronize();
    GV->editState->MarkUnsaved();
    GV->editState->MDI->SaveCurrent();
    GV->Console->Printf("Changed ID of tile %d to %d in tile set %s.", iInitID, iNewID, ts->GetName());
}

void winTileBrowser::RenderTileGroup(std::vector<cTile *> tiles, int x, int y) {
    cTile *selTile = (iSelectedTile == -1 ? 0 : GV->editState->hTileset->GetSet(iSelectedTileSet)->GetTileByIterator(
            iSelectedTile)),
            *hlTile = (iHighlightedT == -1 ? 0 : GV->editState->hTileset->GetSet(iSelectedTileSet)->GetTileByIterator(
            iHighlightedT));

    int tilePickX, tilePickY, tilePickW, tilePickH;
    hge->Gfx_GetClipping(&tilePickX, &tilePickY, &tilePickW, &tilePickH);
    int tilesPerRow = tilePickW / 60;
    for (size_t i = 0; i < tiles.size(); i++) {
        int gridX = (i % tilesPerRow),
            gridY = (i / tilesPerRow);
        int drawX = x + gridX * 60,
            drawY = y + gridY * 80;

        cTile *tile = tiles[i];

        unsigned char alpha = myWin.getAlpha();
        DWORD col = SETA(0xFFFFFFFF, alpha);
        DWORD colDark = SETA(GV->colLineDark, alpha);
        DWORD colBright = SETA(GV->colLineBright, alpha);
        DWORD colBorder = colBright;
        if (tile == selTile) colBorder = SETA(GV->colActive, alpha);
        else if (tile == hlTile) colBorder = SETA(TILE_HIGHLIGHT_COLOR, 0x77 * myWin.getAlphaModifier());
        hge->Gfx_RenderLine(drawX + 5 - 2, drawY + 5 - 1, drawX + 5 + 50, drawY + 5 - 1, colDark);
        hge->Gfx_RenderLine(drawX + 5 - 1, drawY + 5 + 50, drawX + 5 + 50, drawY + 5 + 50, colDark);
        hge->Gfx_RenderLine(drawX + 5 - 1, drawY + 5 - 1, drawX + 5 - 1, drawY + 5 + 50, colDark);
        hge->Gfx_RenderLine(drawX + 5 + 50, drawY + 5 - 1, drawX + 5 + 50, drawY + 5 + 50, colDark);


        hge->Gfx_RenderLine(drawX + 5 - 1, drawY + 5, drawX + 5 + 49, drawY + 5, colBorder);
        hge->Gfx_RenderLine(drawX + 5, drawY + 5 + 49, drawX + 5 + 49, drawY + 5 + 49, colBorder);
        hge->Gfx_RenderLine(drawX + 5, drawY + 5, drawX + 5, drawY + 5 + 49, colBorder);
        hge->Gfx_RenderLine(drawX + 5 + 49, drawY + 5, drawX + 5 + 49, drawY + 5 + 49, colBorder);

        tile->GetImage()->SetColor(col);

        int clipY = std::max(drawY + 5, tilePickY), clipH = std::min(drawY + 5 + 48, tilePickY + tilePickH) - clipY;
        if (clipH > 0) {
            hge->Gfx_SetClipping(drawX + 5, clipY, 48, clipH);
            GV->sprCheckboard->SetColor(col);
            GV->sprCheckboard->Render(drawX + 5, drawY + 5);
            hge->Gfx_SetClipping(tilePickX, tilePickY, tilePickW, tilePickH);
        }
        int pvW = 48*(GV->editState->hTileset->GetMaxTileWidth())/m_iGroupTilesW,
            pvH = 48*(GV->editState->hTileset->GetMaxTileHeight())/m_iGroupTilesH;
        tile->GetImage()->RenderStretch(drawX + 5, drawY + 5, drawX + 5 + pvW, drawY + 5 + pvH);

        GV->fntMyriad16->SetColor(SETA(colBorder != colBright ? 0xFFFFFFFF : 0xFFe1e1e1, alpha));
        GV->fntMyriad16->printf(drawX + 5 + 25, drawY + 50 + 8, HGETEXT_CENTER, "%d", 0, tile->GetID());
    }
}

int winTileBrowser::MouseHandleGroup(std::vector<cTile *> tiles, int x, int y, int tilesPerRow) {
    float mx, my;
    hge->Input_GetMousePos(&mx, &my);

    saTiles->setVerticalScrollAmount(saTiles->getVerticalScrollAmount() + 150 * (-hge->Input_GetMouseWheel()));

    if (mx > x && mx < x + tilesPerRow * 60 - 5 && my > y + 5) {
        int dx = (mx - x) / 60,
                dy = (my - y) / 80;
        int imousetile = (dy * tilesPerRow) + dx;
        if (imousetile < GV->editState->hTileset->GetSet(iSelectedTileSet)->GetTilesCount() && mx > (x + dx * 60 + 5) &&
            mx < (x + dx * 60 + 55) && my > (y + dy * 80 + 5))
            return imousetile;
    }
    return -1;
}
