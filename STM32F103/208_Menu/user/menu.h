#ifndef __MENU_H__
#define __MENU_H__


typedef struct{
    const uint8_t type:4;
    const uint8_t index:4;
    const uint8_t *pText;
    const Page_S *pSon;
}Item_S;

typedef struct{
    const Page_S *pParent;
    void (*Function)(uint8_t KeyIndex);
    const Item_S  *pItem;
    const uint8_t ItemNum;
}Page_S;

#endif  /* __MENU_H__ */