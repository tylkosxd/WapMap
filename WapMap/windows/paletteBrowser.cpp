#include "paletteBrowser.h"

#include "../shared/commonFunc.h"
#include "../langID.h"
#include "../states/editing_ww.h"
#include "../databanks/palettes.h"

winPaletteBrowser::winPaletteBrowser() : cWindow(GETL2S("Win_PaletteBrowser", "WinCaption"), 201, 300) {
    lbPalettes = new SHR::ListBox();
    lbPalettes->setWidth(myWin.getWidth() - 9);
    lbPalettes->addActionListener(this);
    myWin.add(lbPalettes, 3, 0);
    vp = new WIDG::Viewport(this, 0);
    myWin.add(vp);
}

winPaletteBrowser::~winPaletteBrowser() {
    delete lbPalettes;
    delete vp;
}

void winPaletteBrowser::UpdatePalettes() {
    lbPalettes->setListModel(GV->editState->hPalettesBank);
    lbPalettes->setSelected(GV->editState->hPalettesBank->getSelectedPaletteIndex());
    myWin.setHeight(225 + lbPalettes->getHeight());
    lbPalettes->setY(205);
}

void winPaletteBrowser::OnDocumentChange() {
    UpdatePalettes();
}

void winPaletteBrowser::Open() {
    UpdatePalettes();
    cWindow::Open();
}

void winPaletteBrowser::Draw(int piCode) {
    PID::Palette* palette = GV->editState->hDataCtrl->GetPalette();
    int startX, startY;
    myWin.getAbsolutePosition(startX, startY);
    startX += 5;
    startY += 27;
    int i = 0;

    #define TILE_SIZE 12
    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 16; x++, i++) {
            SHR::SetQuad(&q, SETA(palette->GetColor(i), myWin.getAlpha()), x*TILE_SIZE+startX, y*TILE_SIZE+startY,
                 x*TILE_SIZE+startX+TILE_SIZE-1, y*TILE_SIZE+startY+TILE_SIZE-1);
            hge->Gfx_RenderQuad(&q);
        }
    }
}

void winPaletteBrowser::action(const ActionEvent &actionEvent) {
    if (actionEvent.getSource() == lbPalettes) {
        GV->editState->hPalettesBank->setSelectedPaletteIndex(lbPalettes->getSelected());
    }
}
