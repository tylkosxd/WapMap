#include "../editing_ww.h"

void State::EditingWW::NavigateToStartLocation()
{
    fCamX = hParser->GetStartX() - (vPort->GetWidth() / 2 / fZoom);
    fCamY = hParser->GetStartY() - (vPort->GetHeight() / 2 / fZoom);
}

void State::EditingWW::NavigateToEndLocation()
{
    for (int i = 0; i < hParser->GetMainPlane()->GetObjectsCount(); ++i) {
        auto object = hParser->GetMainPlane()->GetObjectByIterator(i);

        if (!strcmp(object->GetLogic(), "BossStager") || !strcmp(object->GetLogic(), "EndOfLevelPowerup")
            || (!strcmp(object->GetLogic(), "SpecialPowerup")
                && (!strcmp(object->GetImageSet(), "GAME_MAPPIECE")
                    || !strcmp(object->GetImageSet(), "LEVEL_GEM")))) {
            fCamX = object->GetX() - (vPort->GetWidth() / 2 / fZoom);
            fCamY = object->GetY() - (vPort->GetHeight() / 2 / fZoom);

            return;
        }
    }
}

void State::EditingWW::NavigateToPoint(int x, int y)
{
    auto* mainPlane = hParser->GetMainPlane();
    if (x > mainPlane->GetPlaneWidthPx())
        x = mainPlane->GetPlaneWidthPx();
    if (y > mainPlane->GetPlaneHeightPx())
        y = mainPlane->GetPlaneHeightPx();
    fCamX = x - (vPort->GetWidth() / 2 / fZoom);
    fCamY = y - (vPort->GetHeight() / 2 / fZoom);
}