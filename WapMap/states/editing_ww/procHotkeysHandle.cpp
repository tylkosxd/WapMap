#include "../editing_ww.h"
#include "../../langID.h"
#include "../../cObjectUserData.h"
#include "../../../shared/gcnWidgets/wComboButton.h"
#include "../../databanks/tiles.h"
#include "../../windows/tilePicker.h"

extern HGE *hge;

void State::EditingWW::HandleHotkeys() {
    if (!GV->StateMgr->IsAppFocused()) return;
    float mx, my;
    hge->Input_GetMousePos(&mx, &my);
    bool bFocus = vPort->GetWidget()->isFocused();
    bool bCtrl = hge->Input_GetKeyState(HGEK_CTRL),
        bShift = hge->Input_GetKeyState(HGEK_SHIFT),
        bAlt = hge->Input_GetKeyState(HGEK_ALT);
    if (bFocus && bCtrl && hge->Input_KeyDown(HGEK_A)) {
        if (iMode == EWW_MODE_TILE) {
            iTileSelectX1 = iTileSelectY1 = 0;
            iTileSelectX2 = GetActivePlane()->GetPlaneWidth() - 1;
            iTileSelectY2 = GetActivePlane()->GetPlaneHeight() - 1;
            vPort->MarkToRedraw();
        } else if (iMode == EWW_MODE_OBJECT) {
            vObjectsPicked = hPlaneData[GetActivePlaneID()]->ObjectData.hQuadTree->GetObjectsByArea(0, 0,
                                                                                                    GetActivePlane()->GetPlaneWidthPx(),
                                                                                                    GetActivePlane()->GetPlaneHeightPx());
        }
    } else if (iMode == EWW_MODE_OBJECT && vObjectsPicked.size() > 0 && iActiveTool == EWW_TOOL_NONE &&
               (hge->Input_KeyDown(HGEK_PGUP) || hge->Input_KeyDown(HGEK_PGDN))) {
        int id = (hge->Input_KeyDown(HGEK_PGUP) ? OBJMENU_ZC_INC : OBJMENU_ZC_DEC);
        if (bShift)
            id += 2;
        else if (bCtrl)
            id += 1;
        objZCoordContext->EmulateClickID(id);
    } else if (iMode == EWW_MODE_OBJECT && bCtrl && hge->Input_KeyDown(HGEK_F) &&
               hmbObject->butIconSearchObject->isEnabled()) {
        hmbObject->butIconSearchObject->simulatePress();
    } else if (bFocus && bCtrl && hge->Input_KeyDown(HGEK_X) &&
               vObjectsPicked.size() != 0 && iMode == EWW_MODE_OBJECT) {
        objContext->EmulateClickID(OBJMENU_CUT);
    } else if (bFocus && bCtrl && hge->Input_KeyDown(HGEK_C) &&
               vObjectsPicked.size() != 0 && iMode == EWW_MODE_OBJECT) {
        objContext->EmulateClickID(OBJMENU_COPY);
    } else if (bFocus && bCtrl && hge->Input_KeyDown(HGEK_C) && iMode == EWW_MODE_TILE &&
               iTileSelectX1 != -1 && iTileSelectX2 != -1 && iTileSelectY1 != -1 && iTileSelectY2 != -1) {
        tilContext->EmulateClickID(TILMENU_COPY);
    } else if (bFocus && bCtrl && hge->Input_KeyDown(HGEK_X) && iMode == EWW_MODE_TILE &&
               iTileSelectX1 != -1 && iTileSelectX2 != -1 && iTileSelectY1 != -1 && iTileSelectY2 != -1) {
        tilContext->EmulateClickID(TILMENU_CUT);
    } else if (bFocus && iMode == EWW_MODE_TILE && iCurTileCbE != CLIPBOARD_IS_EMPTY &&
               ((bCtrl && hge->Input_KeyDown(HGEK_V)) ||
                (bFocus && bAlt && hge->Input_KeyDown(HGEK_LBUTTON)))) {
        float mx, my;
        hge->Input_GetMousePos(&mx, &my);
        contextX = mx;
        contextY = my;
        tilContext->EmulateClickID(TILMENU_PASTE);
    } else if (bFocus && iMode == EWW_MODE_TILE && hge->Input_KeyDown(HGEK_DELETE) &&
               iTileSelectX1 != -1 && iTileSelectX2 != -1 && iTileSelectY1 != -1 && iTileSelectY2 != -1) {
        tilContext->EmulateClickID(TILMENU_DELETE);
    } else if (bFocus && hge->Input_KeyDown(HGEK_DELETE) && vObjectsPicked.size() != 0) {
        objContext->EmulateClickID(OBJMENU_DELETE);
    } else if (bCtrl && bAlt && hge->Input_KeyDown(HGEK_P)) {

    } else if (iMode == EWW_MODE_TILE && (iActiveTool == EWW_TOOL_BRUSH || iActiveTool == EWW_TOOL_PENCIL || iActiveTool == EWW_TOOL_FILL || iActiveTool == EWW_TOOL_PIPETTE)) {
        auto *dd = MDI->GetActiveDoc();
        if (hge->Input_KeyDown(HGEK_E)) {
            SetTool(EWW_TOOL_PENCIL);
            dd->iTilePicked[dd->iTileSlot] = EWW_TILE_ERASE;
            vPort->MarkToRedraw();
        }
        else if (hge->Input_KeyDown(HGEK_L)) {
            if (iActiveTool != EWW_TOOL_PENCIL)
                SetTool(EWW_TOOL_PENCIL);
            iTileDrawMode = EWW_DRAW_LINE;
            hwinTilePicker->Rebuild();
            vPort->MarkToRedraw();
        }
        else if (hge->Input_KeyDown(HGEK_R)) {
            if (iActiveTool != EWW_TOOL_PENCIL)
                SetTool(EWW_TOOL_PENCIL);
            iTileDrawMode = EWW_DRAW_RECT;
            hwinTilePicker->Rebuild();
            vPort->MarkToRedraw();
        }
        else if (hge->Input_KeyDown(HGEK_C)) {
            if (iActiveTool != EWW_TOOL_PENCIL)
                SetTool(EWW_TOOL_PENCIL);
            iTileDrawMode = EWW_DRAW_ELLIPSE;
            hwinTilePicker->Rebuild();
            vPort->MarkToRedraw();
        }
        else if (hge->Input_KeyDown(HGEK_S)) {
            if (iActiveTool != EWW_TOOL_PENCIL)
                SetTool(EWW_TOOL_PENCIL);
            iTileDrawMode = EWW_DRAW_SPRAY;
            hwinTilePicker->Rebuild();
            vPort->MarkToRedraw();
        }
        else if (hge->Input_KeyDown(HGEK_P)) {
            int tool = iActiveTool != EWW_TOOL_PIPETTE ? EWW_TOOL_PIPETTE : EWW_TOOL_PENCIL;
            SetTool(tool);
            vPort->MarkToRedraw();
        }
        else if (hge->Input_KeyDown(HGEK_F)) {
            SetTool(EWW_TOOL_FILL);
            vPort->MarkToRedraw();
        }
        else if (hge->Input_KeyDown(HGEK_B)) {
            SetTool(EWW_TOOL_BRUSH);
            vPort->MarkToRedraw();
        }
        
        int bNewTileSlot = 0;
        if (hge->Input_KeyDown(HGEK_1)) {
            dd->iTileSlot = 0;
            bNewTileSlot = 1;
        } else if (hge->Input_KeyDown(HGEK_2)) {
            dd->iTileSlot = 1;
            bNewTileSlot = 1;
        } else if (hge->Input_KeyDown(HGEK_3)) {
            dd->iTileSlot = 2;
            bNewTileSlot = 1;
        } else if (hge->Input_KeyDown(HGEK_4)) {
            dd->iTileSlot = 3;
            bNewTileSlot = 1;
        } else if (hge->Input_KeyDown(HGEK_5)) {
            dd->iTileSlot = 4;
            bNewTileSlot = 1;
        } else if (hge->Input_KeyDown(HGEK_6)) {
            dd->iTileSlot = 5;
            bNewTileSlot = 1;
        } else if (hge->Input_KeyDown(HGEK_7)) {
            dd->iTileSlot = 6;
            bNewTileSlot = 1;
        } else if (hge->Input_KeyDown(HGEK_8)) {
            dd->iTileSlot = 7;
            bNewTileSlot = 1;
        }
        if (bNewTileSlot && iActiveTool == EWW_TOOL_BRUSH)
            SetTool(EWW_TOOL_PENCIL);

        int iOldP = iActiveTool == EWW_TOOL_BRUSH ? dd->iBrushPicked : dd->iTilePicked[dd->iTileSlot];
        if (hge->Input_KeyDown(HGEK_ADD)) {
            auto *set = hTileset->GetSet(GetActivePlane()->GetImageSet(0));
            if (!set) return;
            if ((iActiveTool == EWW_TOOL_PENCIL || iActiveTool == EWW_TOOL_FILL) && iOldP + 1 < set->GetTilesCount()) {
                dd->iTilePicked[dd->iTileSlot]++;
                vPort->MarkToRedraw();
            } else if (iActiveTool == EWW_TOOL_BRUSH && iOldP + 1 < set->GetBrushesCount()) {
                dd->iBrushPicked++;
                vPort->MarkToRedraw();
            }
        } else if (hge->Input_KeyDown(HGEK_SUBTRACT) && iOldP > 0) {
            if (iActiveTool == EWW_TOOL_PENCIL || iActiveTool == EWW_TOOL_FILL) {
                dd->iTilePicked[dd->iTileSlot]--;
                vPort->MarkToRedraw();
            } else if (iActiveTool == EWW_TOOL_BRUSH) {
                dd->iBrushPicked--;
                vPort->MarkToRedraw();
            }
        }
    } else if (iMode == EWW_MODE_OBJECT && !vObjectsPicked.empty() &&
               vPort->GetWidget()->isFocused() && iActiveTool == EWW_TOOL_NONE &&
               (bCtrl || bShift || bAlt) &&
               (hge->Input_KeyDown(HGEK_LEFT) || hge->Input_KeyDown(HGEK_RIGHT) || hge->Input_KeyDown(HGEK_UP) ||
                hge->Input_KeyDown(HGEK_DOWN))) {
        int moveResX = 0, moveResY = 0;
        int moveRes = bCtrl ? 1 : bShift? 10 : bAlt ? 64 : 0;

        if (hge->Input_KeyDown(HGEK_LEFT)) moveResX = -moveRes;
        else if (hge->Input_KeyDown(HGEK_RIGHT)) moveResX = moveRes;
        else if (hge->Input_KeyDown(HGEK_UP)) moveResY = -moveRes;
        else if (hge->Input_KeyDown(HGEK_DOWN)) moveResY = moveRes;

        for (auto & object : vObjectsPicked) {
            int posX = object->GetParam(WWD::Param_LocationX),
                posY = object->GetParam(WWD::Param_LocationY);
            posX += moveResX;
            posY += moveResY;
            if (posX < 0 || posY < 0 || posX >= GetActivePlane()->GetPlaneWidthPx() ||
                posY >= GetActivePlane()->GetPlaneHeightPx())
                continue;
            object->SetParam(WWD::Param_LocationX, posX);
            object->SetParam(WWD::Param_LocationY, posY);
            GetUserDataFromObj(object)->SyncToObj();
        }
        vPort->MarkToRedraw();
        MarkUnsaved();
    } else if (bFocus && hge->Input_KeyDown(HGEK_HOME)) {
        NavigateToStartLocation();
    } else if (bFocus && hParser && hge->Input_KeyDown(HGEK_END)) {
        NavigateToEndLocation();
    } else if (bCtrl && hge->Input_KeyDown(HGEK_M)) {
        cbutActiveMode->setSelectedEntryID(!cbutActiveMode->getSelectedEntryID(), 1);
    } else if (bFocus && bCtrl && hge->Input_KeyDown(HGEK_G)) {
        hPlaneData[GetActivePlaneID()]->bDrawGrid = !hPlaneData[GetActivePlaneID()]->bDrawGrid;
        vPort->MarkToRedraw();
    } else if (bFocus && bCtrl && hge->Input_KeyDown(HGEK_B)) {
        hPlaneData[GetActivePlaneID()]->bDrawBoundary = !hPlaneData[GetActivePlaneID()]->bDrawBoundary;
        vPort->MarkToRedraw();
    } else if (bFocus && iMode == EWW_MODE_OBJECT && vObjectsPicked.size() == 1) {
        if (hge->Input_KeyDown(HGEK_C)) {
            auto* ud = GetUserDataFromObj(vObjectsPicked[0]);
            NavigateToPoint(ud->GetX(), ud->GetY());
        } else if (hge->Input_KeyDown(HGEK_P)) {
            OpenObjectWindow(vObjectsPicked[0]);
        } else if (hge->Input_KeyDown(HGEK_E)) {
            if (IsEditableObject(vObjectsPicked[0], NULL)) {
                OpenObjectEdit(vObjectsPicked[0]);
            }
        }
    } else if (hge->Input_KeyDown(HGEK_ESCAPE)) {
        if (iActiveTool == EWW_TOOL_MEASURE || iActiveTool == EWW_TOOL_BRUSHOBJECT) {
            SetTool(EWW_TOOL_NONE);
        }
    }
}
