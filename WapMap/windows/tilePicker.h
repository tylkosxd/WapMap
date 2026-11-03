#ifndef H_WIN_TILEPICKER
#define H_WIN_TILEPICKER

#include "../../shared/gcnWidgets/wLabel.h"
#include "../../shared/gcnWidgets/wButton.h"
#include "../../shared/gcnWidgets/wSlider.h"
#include "../../shared/gcnWidgets/wCheckbox.h"

#include "window.h"

class winTilePicker : public cWindow {
private:
    SHR::But *MakeButton(int x, int y, Gfx32Icons icon);
    void RenderSingleTile(int i, int x, int y, int d, DWORD borderColor, bool drawInfo, bool byTileID = 0);
    SHR::But *butSpray, *butReloadBrush, *butPoint, *butLine, *butRect, *butEllipse, *butFlood, *butPipette, *butBrush;
    SHR::Lab *labReloadBrush, *labPointSize, *labSpraySize, *labSprayDensity, *labLineThickness;
    SHR::CBox *cbRectFilled, *cbEllipseFilled;
    SHR::Slider *sliPointSize, *sliSpraySize, *sliSprayDensity, *sliLineThickness;
    SHR::CBox *cbShowTileID, *cbShowProperties;
    SHR::Slider *sliPicker;
    int iTilePickerOffUp, iTilePickerOffDown;
    int iTileDim = 64;
    int iInventorySeparator = 56;
    WIDG::Viewport *vp;

public:
    winTilePicker();

    void Draw(int piCode) override;

    void OnDocumentChange() override;

    void Open() override;

    void Think() override;

    void action(const ActionEvent &actionEvent) override;
    
    void Rebuild(bool forceSliderRefresh = false);

    void RefreshSlider(bool forceSliderRefresh = false);

    void HandleBrushSwitch(int itOld, int itNew);

    void CloseBrush();

    double GetPointSize() { return sliPointSize->getValue(); }

    double GetSpraySize() { return sliSpraySize->getValue(); }

    double GetSprayDensity() { return sliSprayDensity->getValue(); }

    double GetLineThickness() { return sliLineThickness->getValue(); }

    bool GetRectFill() { return cbRectFilled->isSelected(); }

    bool GetEllipseFill() { return cbEllipseFilled->isSelected(); }
};

#endif // H_WIN_TILEPICKER
