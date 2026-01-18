#ifndef H_C_ANIBANK
#define H_C_ANIBANK

#include "../shared/cANI.h"
#include "../cDataController.h"

class cAniBankAsset : public cAsset {
protected:
    ANI::Animation *m_hAni;

    friend class cBankAni;

public:
    cAniBankAsset(cFile hFile, std::string id);

    ~cAniBankAsset() override;

    void Load() override;

    void Unload() override;

    ANI::Animation *GetAni() { return m_hAni; };

    friend bool cAniBank_SortAssets(cAniBankAsset *a, cAniBankAsset *b);
};

class cBankAni : public cAssetBank<cAniBankAsset> {
public:
    explicit cBankAni(cDataController *hDC) : cAssetBank<cAniBankAsset>(hDC) {}

    cAniBankAsset *GetAssetByID(const char *pszID);

    void SortAssets();

    void BatchProcessStart() override;
    void BatchProcessEnd() override;

    cAniBankAsset *AllocateAssetForMountPoint(cDC_MountEntry mountEntry) override;
    std::string GetMountPointForFile(std::string strFilePath, std::string strPrefix) override;

    const std::string& GetFolderName() override {
        static const std::string name = "ANIS";
        static const std::string namez = "ANIZ";
        return hDC->GetGame() == WWD::Game_Gruntz ? namez : name;
    };

    void DeleteAsset(cAniBankAsset *hAsset) override;
};

#endif
