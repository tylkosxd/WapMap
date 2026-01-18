#ifndef PALETTES_H
#define PALETTES_H

#include "imageSets.h"
#include "tiles.h"
#include "../cDataController.h"

class cPalette : public cAsset {
    std::string mountPoint;
    PID::Palette* palette;
public:
    cPalette(PID::Palette* palette, std::string mountPoint) : palette(palette) { _strName = mountPoint.c_str() + 10; }
    ~cPalette() override { delete palette; }

    PID::Palette* GetPalette() { return palette; }

    void Load() override {}
    void Unload() override {}
};

class cBankPalettes : public cAssetBank<cPalette> {
    cBankImageSet *imagesBank;
    cBankTile* tilesBank;
public:
    explicit cBankPalettes(cDataController *hDC, cBankImageSet *imagesBank, cBankTile* tilesBank) : cAssetBank<cPalette>(hDC), imagesBank(imagesBank), tilesBank(tilesBank) {}

    const std::string& GetFolderName() override {
        static const std::string name = "PALETTES";
        // static const std::string namez = "LOGICZ";
        return /*hDC->GetGame() == WWD::Game_Gruntz ? namez :*/ name;
    };

    void BatchProcessEnd() override;

    std::string GetMountPointForFile(std::string strFilePath, std::string strPrefix) override;

    cPalette *AllocateAssetForMountPoint(cDC_MountEntry mountEntry) override;

    void DeleteAsset(cPalette *hPalette) override;

    int getSelectedPaletteIndex();
    void setSelectedPaletteIndex(int i);
};

#endif //PALETTES_H
