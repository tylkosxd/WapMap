#ifndef H_WIN_GO_TO_COORDS
#define H_WIN_GO_TO_COORDS

#include "../../shared/gcnWidgets/wRadioBut.h"
#include "../../shared/gcnWidgets/wLabel.h"
#include "../../shared/gcnWidgets/wTextField.h"
#include "../../shared/gcnWidgets/wButton.h"
#include "window.h"

class winGoToCoords : public cWindow {
private:
    SHR::RadBut* rbCoords[2];
    SHR::Lab *labX[2];
    SHR::Lab *labY[2] ;
    SHR::TextField *tfX[2];
    SHR::TextField *tfY[2];
    SHR::But *butGo;

    void createCoordsInput(const char* label, int i, int offsetY);
    void setCurCamPos();
    void goToCoords();

public:
    winGoToCoords();

    void Open() override;

    void action(const ActionEvent &actionEvent) override;
};

#endif // H_WIN_GO_TO_COORDS