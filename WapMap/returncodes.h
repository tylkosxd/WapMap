#ifndef H_RETURNCODES
#define H_RETURNCODES

enum class ReturnCodeType : unsigned char {
    Unset = 0,
    LoadMap,
    DialogState,
    MapShot,
    ObjectProp,
    ObjPropSelectedValues
};

struct ReturnCode {
    ReturnCodeType type = ReturnCodeType::Unset;
    int value = 0;

    operator int() const { return value; }
};

#endif
