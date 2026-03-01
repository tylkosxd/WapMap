#include "goToLocation.h"
#include "../globals.h"
#include "../langID.h"
#include "../states/editing_ww.h"
#include "../states/dialog.h"

#define LOCATIONS_WIN_WIDTH 250
#define LOCATIONS_WIN_HEIGHT 401
#define LOCATIONS_BUTTON_OFFSETX 40

extern HGE *hge;

enum LocNameRet {
    LocName_Success = 0,
    LocName_ErrorEmpty,
    LocName_ErrorDuplicate,
    LocName_ErrorTooLong,
    LocName_ErrorGeneric
};

bool validateObjectPtr(WWD::Object* object) {
    auto* mainPlane = GV->editState->hParser->GetMainPlane();
    for (int i = 0; i < mainPlane->GetObjectsCount(); i++) {
        if (mainPlane->GetObjectByIterator(i) == object)
            return 1;
    }
    return 0;
}

int LocationsList::addLocation(std::string& name, int x, int y) {
    if (!v)
        return LocName_ErrorGeneric;
    if (name.empty())
        return LocName_ErrorEmpty;
    for (int i = 0; i < v->size(); ++i) {
        if (name == getLocation(i)->Name)
            return LocName_ErrorDuplicate;
    }
    int len = name.length();
    if (len > 63)
        return LocName_ErrorTooLong;
    stLocation loc;
    strcpy(loc.Name, name.c_str());
    loc.X = x;
    loc.Y = y;
    v->push_back(loc);
    return LocName_Success;
}

bool LocationsList::addLocationLinkedToObject(WWD::Object *object, const char* objType) {
    if (!v || !object)
        return 0;
    stLocation loc;
    std::string name = object->GetName();
    int index = v->size() + 1;

    if (name.empty()) {
        object->SetName((name + objType + " #" + std::to_string(index)).c_str());
    }
    else while (1) {
        int lastH = name.find_last_of('#');
        std::string szIndex = " #" + std::to_string(index);

        if (lastH == std::string::npos)
            name += szIndex;
        else if (name[lastH + 1] < '0' || name[lastH + 1] > '9' || name.back() < '0' || name.back() > '9')
            name += szIndex;

        bool isDuplicate = 0;
        for (int i = 0; i < v->size(); ++i) {
            if (name == getElementAt(i)) {
                isDuplicate = 1;
                break;
            }
        }

        if (!isDuplicate) {
            object->SetName(name.c_str());
            break;
        }
        else {
            lastH = name.find_last_of('#');
            index++;
            name.resize(lastH + 1);
            name += std::to_string(index);
        }
    }

    loc.X = object->GetX();
    loc.Y = object->GetY();
    loc.Object = object;
    v->push_back(loc);
    return 1;
}

void LocationsList::deleteLocation(int i) {
    if (!v) return;
    v->erase(v->begin() + i);
}

stLocation* LocationsList::getLocation(int i) {
    if (!v || i >= (int)(v->size())) return NULL;
    return &(v->at(i));
}

int LocationsList::renameLocation(int index, std::string& newName) {
    if (!v)
        return LocName_ErrorGeneric;
    auto* location = getLocation(index);
    bool hasLinkedObject = location->Object && validateObjectPtr(location->Object);

    if (newName.empty())
        if (hasLinkedObject){
            location->Object->SetName((newName + "#" + std::to_string(index + 1)).c_str());
            return LocName_Success;
        } else
            return LocName_ErrorEmpty;

    if (hasLinkedObject) {
        int indexPos = newName.find_last_of('#');
        if (indexPos == std::string::npos) {
            newName += " #" + std::to_string(index + 1);
        } else {
            newName.resize(indexPos + 1);
            newName += std::to_string(index + 1);
        }
    }

    for (int i = 0; i < v->size(); ++i) {
        if (getElementAt(i) == newName)
            return LocName_ErrorDuplicate;
    }

    if (hasLinkedObject)
        location->Object->SetName(newName.c_str());
    else {
        if (newName.length() > 63)
            return LocName_ErrorTooLong;
        sprintf(location->Name, "%s", newName.c_str());
    }
    
    return LocName_Success;
}

void LocationsList::sort() {
    if (!isObjectList)
        return;
    std::sort(v->begin(), v->end(), [](stLocation& a, stLocation& b) {
        std::string str;

        str = a.Object->GetName();
        int index = str.find_last_of("#");
        if (index == std::string::npos)
            return (bool)0;
        str = str.substr(index + 1);
        if (str[0] < '0' || str[0] > '9')
            return (bool)0;
        int numA = std::stoi(str);

        str = b.Object->GetName();
        index = str.find_last_of("#");
        if (index == std::string::npos)
            return (bool)1;
        str = str.substr(index + 1);
        if (str[0] < '0' || str[0] > '9')
            return (bool)1;
        int numB = std::stoi(str);

        return numA < numB;
    });

    std::string name, indexStr;
    int indexPos;
    for (int i = 0; i < v->size(); i++) {
        auto *obj = v->at(i).Object;
        name = obj->GetName();
        indexPos = name.find_last_of("#");
        if (indexPos == std::string::npos) {
            name += " #" + std::to_string(i + 1);
            obj->SetName(name.c_str());
            continue;
        }
        indexStr = name.substr(indexPos + 1);
        if (indexStr.empty()) {
            name = "#" + std::to_string(i + 1);
            obj->SetName(name.c_str());
            continue;
        }
        if (indexStr[0] < '0' || indexStr[0] > '9') {
            name += " #" + std::to_string(i + 1);
            obj->SetName(name.c_str());
            continue;
        }
        if (std::stoi(indexStr.c_str()) != i + 1) {
            name.resize(indexPos + 1);
            name += std::to_string(i + 1);
            obj->SetName(name.c_str());
        }
    }
}

bool LocationsList::moveElement(int index, int move) {
    if (index < 0 || index >= getNumberOfElements())
        return 0;
    if (move == 0)
        return 0;
    int dest = move < 0 ? index - 1 : index + 1;
    if (dest < 0 || dest >= getNumberOfElements())
        return 0;

    if (this->isObjectList) {
        auto *loc = getLocation(index);
        if (validateObjectPtr(loc->Object)) {
            std::string name = loc->Object->GetName();
            int indexPos = name.find_last_of("#") + 1;
            name.resize(indexPos);
            name += std::to_string(dest + 1);
            loc->Object->SetName(name.c_str());
        }
            

        loc = getLocation(dest);
        if (validateObjectPtr(loc->Object)) {
            std::string name = loc->Object->GetName();
            int indexPos = name.find_last_of("#") + 1;
            name.resize(indexPos);
            name += std::to_string(index + 1);
            loc->Object->SetName(name.c_str());
        }
    }

    auto copy = v->at(dest);
    v->at(dest) = v->at(index);
    v->at(index) = copy;

    return 1;
}

std::string LocationsList::getElementAt(int i) {
    if (!v || i >= (int)(v->size())) return "";
    if (isObjectList) {
        auto *object = v->at(i).Object;
        if (!validateObjectPtr(object)) {
            deleteLocation(i);
            return "";
        }
        return v->at(i).Object->GetName();
    }
    return v->at(i).Name;
}

inline const char* getStr(const wchar_t* wstr) {
    return GETL2SV("Win_GoToLocations", wstr);
}

winLocationsBrowser::winLocationsBrowser() : cWindow(getStr(L"WinCaption"), LOCATIONS_WIN_WIDTH, LOCATIONS_WIN_HEIGHT) {
    int y = 8;

    tabarLocs = new SHR::TabbedArea();
    tabarLocs->addActionListener(this);
    tabarLocs->setDimension(gcn::Rectangle(0, 0, LOCATIONS_WIN_WIDTH, 25));
    myWin.add(tabarLocs, 0, y);

    for (int i = 0; i < Locs::COUNT; i++) {
        lmLocs[i] = new LocationsList();
        wchar_t temp[32];
        wsprintfW(temp, L"Tab_%d", i);
        tabarLocs->addTab(getStr(temp));
    }
    
    lmLocs[Locs::Checkpoints]->v = &vCheckpoints;
    vCheckpoints.reserve(100);
    lmLocs[Locs::Checkpoints]->isObjectList = 1;

    lmLocs[Locs::Warps]->v = &vWarps;
    vWarps.reserve(100);
    lmLocs[Locs::Warps]->isObjectList = 1;

    y += 25;

    lbLocs = new SHR::ListBox(lmLocs[Locs::Favourites]);
    lbLocs->setDimension(gcn::Rectangle(0, 0, LOCATIONS_WIN_WIDTH - 4, 310));
    lbLocs->addActionListener(this);
    saLocs = new SHR::ScrollArea(lbLocs, SHR::ScrollArea::SHOW_NEVER, SHR::ScrollArea::SHOW_AUTO);
    saLocs->setDimension(gcn::Rectangle(0, 0, LOCATIONS_WIN_WIDTH - 4, 310));
    myWin.add(saLocs, 1, y);

    y += 316;

    int x = 8;

    butAddFav = new SHR::But(GV->hGfxInterface, GV->sprIcons16[Icon16_Add]);
    butAddFav->setDimension(gcn::Rectangle(0, 0, 32, 30));
    butAddFav->addActionListener(this);
    butAddFav->SetTooltip(GETL(Lang_AddFavLocation));
    myWin.add(butAddFav, x, y);
    x += LOCATIONS_BUTTON_OFFSETX;

    butDelete = new SHR::But(GV->hGfxInterface, GV->sprIcons16[Icon16_Trash]);
    butDelete->setDimension(gcn::Rectangle(0, 0, 32, 30));
    butDelete->addActionListener(this);
    butDelete->SetTooltip(GETL(Lang_Delete));
    butDelete->setEnabled(0);
    myWin.add(butDelete, x, y);
    x += LOCATIONS_BUTTON_OFFSETX;

    butEdit = new SHR::But(GV->hGfxInterface, GV->sprIcons16[Icon16_Pencil]);
    butEdit->setDimension(gcn::Rectangle(0, 0, 32, 30));
    butEdit->addActionListener(this);
    butEdit->SetTooltip(getStr(L"Edit"));
    butEdit->setEnabled(0);
    myWin.add(butEdit, x, y);
    x += LOCATIONS_BUTTON_OFFSETX;

    butMoveDown = new SHR::But(GV->hGfxInterface, GV->sprIcons16[Icon16_Down]);
    butMoveDown->setDimension(gcn::Rectangle(0, 0, 32, 30));
    butMoveDown->addActionListener(this);
    butMoveDown->SetTooltip(GETL(Lang_MoveDown));
    butMoveDown->setEnabled(0);
    myWin.add(butMoveDown, x, y);
    x += LOCATIONS_BUTTON_OFFSETX;

    butMoveUp = new SHR::But(GV->hGfxInterface, GV->sprIcons16[Icon16_Up]);
    butMoveUp->setDimension(gcn::Rectangle(0, 0, 32, 30));
    butMoveUp->addActionListener(this);
    butMoveUp->SetTooltip(GETL(Lang_MoveUp));
    butMoveUp->setEnabled(0);
    myWin.add(butMoveUp, x, y);
    x += LOCATIONS_BUTTON_OFFSETX;

    butFollowWarp = new SHR::But(GV->hGfxInterface, GV->sprIcons16[Icon16_Warp]);
    butFollowWarp->setDimension(gcn::Rectangle(0, 0, 32, 30));
    butFollowWarp->addActionListener(this);
    butFollowWarp->SetTooltip(getStr(L"FollowWarp"));
    butFollowWarp->setEnabled(0);
    myWin.add(butFollowWarp, x, y);
}

void winLocationsBrowser::AddFavLocation(int x, int y) {
    int ret = State::InputDialog(GETL(Lang_WM), getStr(L"DialogInputName"), ST_DIALOG_BUT_OKCANCEL, NULL);
    if (ret != RETURN_OK)
        return;
    int rr = lmLocs[Locs::Favourites]->addLocation(GV->szLastInputDialogText, x, y);
    if (rr == LocName_Success) {
        GV->editState->MarkUnsaved();
    } else {
        wchar_t temp[32];
        wsprintfW(temp, L"DialogInputNameError_%d", rr);
        State::MessageBox(GETL(Lang_Error), getStr(temp), ST_DIALOG_ICON_ERROR);
    }
}

void winLocationsBrowser::RefreshLists() {
    const char* strSCp1     = getStr(L"SuperCheckpoint1");
    const char* strSCp2     = getStr(L"SuperCheckpoint2");
    const char* strCp       = getStr(L"Checkpoint");
    const char* strWarp     = getStr(L"Warp");
    const char* strBossWarp = getStr(L"BossWarp");

    auto* mainPlane = GV->editState->hParser->GetMainPlane();

    lmLocs[Locs::Favourites]->v = &(GV->editState->MDI->GetActiveDoc()->vFavLocations);
    lmLocs[Locs::Checkpoints]->v->clear();
    lmLocs[Locs::Warps]->v->clear();

    for (int i = 0; i < mainPlane->GetObjectsCount(); i++) {
        auto *obj =  mainPlane->GetObjectByIterator(i);
        const char* logic = obj->GetLogic();

        if (strstr(logic, "Checkpoint")) {
            lmLocs[Locs::Checkpoints]->addLocationLinkedToObject(obj,
                    (logic[0] == 'F' ? strSCp1 : logic[0] == 'S' ? strSCp2 : strCp)); // check the first character only (FirstSuperCheckpoint, SecondSuperCheckpoint or Checkpoint):
            continue;
        }
        // mapping warps
        if (obj->GetParam(WWD::Param_SpeedX) <= 0 && obj->GetParam(WWD::Param_SpeedY) <= 0)
            continue;

        if (!strcmp(logic, "SpecialPowerup") &&
                (!strcmp(obj->GetImageSet(), "GAME_WARP") || !strcmp(obj->GetImageSet(), "GAME_VERTWARP"))) {
            lmLocs[Locs::Warps]->addLocationLinkedToObject(obj, strWarp);
            continue;
        }
        if (!strcmp(logic, "BossWarp")) {
            lmLocs[Locs::Warps]->addLocationLinkedToObject(obj, strBossWarp);
        }
    }
    lmLocs[Locs::Checkpoints]->sort();
    lmLocs[Locs::Warps]->sort();
}

void winLocationsBrowser::Open() {
    RefreshLists();
    myWin.setPosition(160, LAY_VIEWPORT_Y + 40);
    myWin.setVisible(true);
    myWin.getParent()->moveToTop(&myWin);
}

void winLocationsBrowser::OnDocumentChange() {
    RefreshLists();
}

void winLocationsBrowser::action(const ActionEvent &actionEvent) {
    if (actionEvent.getSource() == tabarLocs) {
        RefreshLists();
        return; // changing tabs in Think()
    }

    int i = lbLocs->getSelected();
    if (i < 0 || i >= lmLocs[m_selectedTab]->getNumberOfElements())
        return;
    auto* location = lmLocs[m_selectedTab]->getLocation(i);

    if (actionEvent.getSource() == butAddFav) {
        std::string name = location->Object->GetName();
        if (name.length() > 63)
            name.resize(63);
        lmLocs[Locs::Favourites]->addLocation(name, location->X, location->Y);
        return;
    }

    if (actionEvent.getSource() == lbLocs) {
        GV->editState->NavigateToPoint(location->X, location->Y);
    }
    else if (actionEvent.getSource() == butDelete) {
        if (m_selectedTab == Locs::Checkpoints || m_selectedTab == Locs::Warps) {
            if (validateObjectPtr(location->Object)) {
                GV->editState->hParser->GetMainPlane()->DeleteObject(location->Object);
                GV->editState->vPort->MarkToRedraw();
            }
        }
        lmLocs[m_selectedTab]->deleteLocation(i);
        GV->editState->MarkUnsaved();
    }
    else if (actionEvent.getSource() == butEdit) {
        std::string initStr = lmLocs[m_selectedTab]->getElementAt(i);
        if (lmLocs[m_selectedTab]->isObjectList)
            initStr = initStr.substr(0, initStr.find_last_of("#") - 1);
        int ret = State::InputDialog(GETL(Lang_WM), getStr(L"DialogInputName"), ST_DIALOG_BUT_OKCANCEL, initStr.c_str());
        if (ret == RETURN_OK && initStr != GV->szLastInputDialogText) {
            int rr = lmLocs[m_selectedTab]->renameLocation(i, GV->szLastInputDialogText);
            if (rr == LocName_Success) {
                GV->editState->MarkUnsaved();
            } else {
                wchar_t temp[32];
                wsprintfW(temp, L"DialogInputNameError_%d", rr);
                State::MessageBox(GETL(Lang_Error), getStr(temp), ST_DIALOG_ICON_ERROR);
            }
            RefreshLists();
        }
    }
    else if (actionEvent.getSource() == butMoveDown) {
        int selected = lbLocs->getSelected();
        if (lmLocs[m_selectedTab]->moveElement(selected, 1))
            lbLocs->setSelected(selected + 1);
    }
    else if (actionEvent.getSource() == butMoveUp) {
        int selected = lbLocs->getSelected();
        if (lmLocs[m_selectedTab]->moveElement(selected, -1))
            lbLocs->setSelected(selected - 1);
    }
    else if (actionEvent.getSource() == butFollowWarp) {
        GV->editState->NavigateToWarpDestination(location->Object);
    }
};

void winLocationsBrowser::Think() {
    int tab = tabarLocs->getSelectedTabIndex();
    if (m_selectedTab != tab) {
        lbLocs->setListModel(lmLocs[tab]);
        m_selectedTab = tab;
        saLocs->setVerticalScrollAmount(0);
    }
    int selected = lbLocs->getSelected();
    butAddFav->setEnabled(selected >= 0 && m_selectedTab != Locs::Favourites);
    butDelete->setEnabled(selected >= 0);
    butEdit->setEnabled(selected >= 0);
    butMoveDown->setEnabled(selected >= 0 && selected < lmLocs[m_selectedTab]->getNumberOfElements() - 1);
    butMoveUp->setEnabled(selected >= 1 && selected < lmLocs[m_selectedTab]->getNumberOfElements());
    butFollowWarp->setEnabled(selected >= 0 && m_selectedTab == Locs::Warps);
}