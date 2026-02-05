#include "goToLocation.h"
#include "../globals.h"
#include "../langID.h"
#include "../states/editing_ww.h"
#include "../states/dialog.h"

extern HGE *hge;

enum LocNameRet {
    LocName_Success = 0,
    LocName_ErrorEmpty,
    LocName_ErrorDuplicate,
    LocName_ErrorTooLong,
    LocName_ErrorNoListModel
};

int LocationsList::addLocation(const std::string& name, int x, int y) {
    if (!v)
        return LocName_ErrorNoListModel;
    if (name.empty())
        return LocName_ErrorEmpty;
    for (unsigned int i = 0; i < v->size(); ++i) {
        if (name == getLocation(i).Name)
            return LocName_ErrorDuplicate;
    }
    int len = name.length();
    if (len > 63)
        return LocName_ErrorTooLong;
    stLocation loc;
    strncpy(loc.Name, name.c_str(), len + 1);
    loc.X = x;
    loc.Y = y;
    v->push_back(loc);
    return LocName_Success;
}

void LocationsList::deleteLocation(int i) {
    if (!v) return;
    v->erase(v->begin() + i);
}

stLocation LocationsList::getLocation(int i) {
    if (!v) {
        stLocation empty;
        return empty;
    }
    return (*v)[i];
}

int LocationsList::renameLocation(int index, const std::string& newName) {
    if (!v)
        return LocName_ErrorNoListModel;
    if (newName.empty())
        return LocName_ErrorEmpty;
    for (unsigned int i = 0; i < v->size(); ++i) {
        if (getLocation(i).Name == newName)
            return LocName_ErrorDuplicate;
    }
    int len = newName.length();
    if (len > 63)
        return LocName_ErrorTooLong;
    memset((*v)[index].Name, 0, 64);
    strncpy((*v)[index].Name, newName.c_str(), len + 1);
    return LocName_Success;
}

std::string LocationsList::getElementAt(int i) {
    if (!v) return 0;
    return (*v)[i].Name;
}

const char* getStr(const wchar_t* wstr) {
    return GETL2SV("Win_GoToLocations", wstr);
}

winLocationsBrowser::winLocationsBrowser() : cWindow(getStr(L"WinCaption"), 250, 400) {
    int y = 15;

    tabarLocs = new SHR::TabbedArea();
    tabarLocs->addActionListener(this);
    tabarLocs->setDimension(gcn::Rectangle(0, 0, 230, 25));
    myWin.add(tabarLocs, 8, 15);

    for (int i = 0; i < Locs::COUNT; i++) {
        lmLocs[i] = new LocationsList();
        wchar_t temp[32];
        wsprintfW(temp, L"Tab_%d", i);
        tabarLocs->addTab(getStr(temp));
    }
    
    lmLocs[Locs::Checkpoints]->v = &vCheckpoints;
    vCheckpoints.reserve(100);

    lmLocs[Locs::Warps]->v = &vWarps;
    vWarps.reserve(100);

    y += 25;

    lbLocs = new SHR::ListBox(lmLocs[Locs::Favourites]);
    lbLocs->setDimension(gcn::Rectangle(0, 0, 230, 290));
    lbLocs->addActionListener(this);
    saLocs = new SHR::ScrollArea(lbLocs, SHR::ScrollArea::SHOW_NEVER, SHR::ScrollArea::SHOW_AUTO);
    saLocs->setDimension(gcn::Rectangle(0, 0, 230, 290));
    myWin.add(saLocs, 8, y);
    y += 305;

    butDelete = new SHR::But(GV->hGfxInterface, getStr(L"Delete"));
    butDelete->setDimension(gcn::Rectangle(0, 0, 70, 28));
    butDelete->addActionListener(this);
    butDelete->setIcon(GV->sprIcons16[Icon16_Trash]);
    butDelete->setEnabled(0);
    myWin.add(butDelete, 8, y);

    butEdit = new SHR::But(GV->hGfxInterface, getStr(L"Edit"));
    butEdit->setDimension(gcn::Rectangle(0, 0, 70, 28));
    butEdit->addActionListener(this);
    butEdit->setIcon(GV->sprIcons16[Icon16_Pencil]);
    butEdit->setEnabled(0);
    myWin.add(butEdit, 89, y);

    butRefresh = new SHR::But(GV->hGfxInterface, getStr(L"Refresh"));
    butRefresh->setDimension(gcn::Rectangle(0, 0, 70, 28));
    butRefresh->addActionListener(this);
    butRefresh->setIcon(GV->sprIcons16[Icon16_Refresh]);
    butRefresh->setEnabled(0);
    myWin.add(butRefresh, 170, y);
}

void winLocationsBrowser::AddFavLocation(int x, int y) {
    const auto& ret = State::InputDialog(GETL(Lang_WM), getStr(L"DialogInputName"), ST_DIALOG_BUT_OKCANCEL, NULL);
    if (ret.value != RETURN_OK)
        return;
    int rr = lmLocs[Locs::Favourites]->addLocation(ret.data, x, y);
    if (rr == LocName_Success) {
        GV->editState->MarkUnsaved();
    } else {
        wchar_t temp[32];
        wsprintfW(temp, L"DialogInputNameError_%d", rr);
        State::MessageBox(GETL(Lang_Error), getStr(temp), ST_DIALOG_ICON_ERROR);
    }
}

void winLocationsBrowser::RefreshLists() {
    lmLocs[Locs::Favourites]->v = &(GV->editState->MDI->GetActiveDoc()->vFavLocations);
    lmLocs[Locs::Checkpoints]->v->clear();
    lmLocs[Locs::Warps]->v->clear();

    const char* strSCp1     = GETL2S("Win_GoToLocations", "SuperCheckpoint1");
    const char* strSCp2     = GETL2S("Win_GoToLocations", "SuperCheckpoint2");
    const char* strCp       = GETL2S("Win_GoToLocations", "Checkpoint");
    const char* strWarp     = GETL2S("Win_GoToLocations", "Warp");
    const char* strBossWarp = GETL2S("Win_GoToLocations", "BossWarp");

    auto* mainPlane = GV->editState->hParser->GetMainPlane();

    for (int i = 0; i < mainPlane->GetObjectsCount(); i++) {
        auto *obj =  mainPlane->GetObjectByIterator(i);
        const char* logic = obj->GetLogic();
        // mapping checkpoints
        if (strstr(logic, "Checkpoint")) {
            char tmp[64];
            int count = lmLocs[Locs::Checkpoints]->getNumberOfElements() + 1;
            // check the first character only (FirstSuperCheckpoint, SecondSuperCheckpoint or Checkpoint):
            const char* str = logic[0] == 'F' ? strSCp1 : logic[0] == 'S' ? strSCp2 : strCp;
            sprintf(tmp, str, count);
            lmLocs[Locs::Checkpoints]->addLocation(tmp, obj->GetX(), obj->GetY());
            continue;
        }
        // mapping warps
        if (obj->GetParam(WWD::Param_SpeedX) <= 0 && obj->GetParam(WWD::Param_SpeedY) <= 0)
            continue;
        auto* imageset = obj->GetImageSet();
        if (!strcmp(logic, "SpecialPowerup") &&
                (!strcmp(imageset, "GAME_WARP") || !strcmp(imageset, "GAME_VERTWARP"))) {
            char tmp[64];
            int count = (lmLocs[Locs::Warps]->getNumberOfElements() + 1);
            sprintf(tmp, strWarp, count);
            lmLocs[Locs::Warps]->addLocation(tmp, obj->GetX(), obj->GetY());
            continue;
        }
        if (!strcmp(logic, "BossWarp")) {
            char tmp[64];
            int count = (lmLocs[Locs::Warps]->getNumberOfElements() + 1);
            sprintf(tmp, strBossWarp, count);
            lmLocs[Locs::Warps]->addLocation(tmp, obj->GetX(), obj->GetY());
        }
    }
}

void winLocationsBrowser::Open() {
    RefreshLists();
    myWin.setPosition(240, hge->System_GetState(HGE_SCREENHEIGHT)/8);
    myWin.setVisible(true);
    myWin.getParent()->moveToTop(&myWin);
}

void winLocationsBrowser::OnDocumentChange() {
    RefreshLists();
}

void winLocationsBrowser::action(const ActionEvent &actionEvent) {
    if (actionEvent.getSource() == tabarLocs)
        return; // changing tabs in Think()

    if (actionEvent.getSource() == butRefresh) {
        RefreshLists();
        return;
    }

    int i = lbLocs->getSelected();
    if (i < 0 || i >= lmLocs[m_selectedTab]->getNumberOfElements())
        return;
    stLocation location = lmLocs[m_selectedTab]->getLocation(i);

    if (actionEvent.getSource() == lbLocs) {
        auto* ES = GV->editState;
        ES->fCamX = location.X - ES->vPort->GetWidth() / 2 / ES->fZoom;
        ES->fCamY = location.Y - ES->vPort->GetHeight() / 2 / ES->fZoom;
        return;
    }

    if (m_selectedTab != Locs::Favourites)
        return;
    
    if (actionEvent.getSource() == butDelete) {
        lmLocs[Locs::Favourites]->deleteLocation(i);
        GV->editState->MarkUnsaved();
        return;
    }

    if (actionEvent.getSource() == butEdit) {
        const auto& ret = State::InputDialog(GETL(Lang_WM), getStr(L"DialogInputName"), ST_DIALOG_BUT_OKCANCEL, location.Name);
        if (ret.value == RETURN_OK && location.Name != ret.data) {
            int rr = lmLocs[Locs::Favourites]->renameLocation(i, ret.data);
            if (rr == LocName_Success) {
                GV->editState->MarkUnsaved();
            } else {
                wchar_t temp[32];
                wsprintfW(temp, L"DialogInputNameError_%d", rr);
                State::MessageBox(GETL(Lang_Error), getStr(temp), ST_DIALOG_ICON_ERROR);
            }
        }
        return;
    }
};

void winLocationsBrowser::Think() {
    int i = tabarLocs->getSelectedTabIndex();
    if (m_selectedTab != i) {
        lbLocs->setListModel(lmLocs[i]);
        m_selectedTab = i;
    }
    bool isEditable = m_selectedTab == Locs::Favourites && lbLocs->getSelected() >= 0;
    butDelete->setEnabled(isEditable);
    butEdit->setEnabled(isEditable);
    butRefresh->setEnabled(m_selectedTab != Locs::Favourites);
}