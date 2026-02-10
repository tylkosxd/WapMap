#ifndef H_WIN_GO_TO_LOCATION
#define H_WIN_GO_TO_LOCATION

#include "../cMDI.h"
#include "../../shared/gcnWidgets/wListBox.h"
#include "../../shared/gcnWidgets/wScrollArea.h"
#include "../../shared/gcnWidgets/wButton.h"
#include "../../shared/gcnWidgets/wTabbedArea.h"
#include "../../shared/gcnWidgets/wTab.h"

#include "window.h"

class LocationsList : public gcn::ListModel {
    private:
        std::vector<stLocation> *v = NULL;

        LocationsList() {};

        int addLocation(const std::string& name, int x, int y, WWD::Object *linkedObject = NULL);

        void deleteLocation(int i);

        stLocation* getLocation(int i);

        int renameLocation(int index, const std::string& newName);
    
    public:

        std::string getElementAt(int i);

        int getNumberOfElements() { return v ? v->size() : 0; };

    friend class winLocationsBrowser;
};

class winLocationsBrowser : public cWindow {
    enum Locs {
        Favourites = 0,
        Checkpoints,
        Warps,
        COUNT
    };

    private:
        SHR::ListBox *lbLocs;
        SHR::ScrollArea *saLocs;
        SHR::TabbedArea *tabarLocs;

        std::vector<stLocation> vWarps;
        std::vector<stLocation> vCheckpoints;

        LocationsList *lmLocs[Locs::COUNT];

        SHR::But *butDelete;
        SHR::But *butEdit;
        SHR::But *butRefresh;

        int m_selectedTab = Locs::Favourites;

        void RefreshLists();

    public:
        winLocationsBrowser();

        void AddFavLocation(int x, int y);

        void Open();

        void OnDocumentChange() override;

        void Think() override;

        void action(const ActionEvent &actionEvent) override;
};

#endif // H_WIN_GO_TO_LOCATION