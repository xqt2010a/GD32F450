#ifndef __MENU_H__
#define __MENU_H__


typedef struct{
    const uint8_t type:4;  /* ��ʾ��ʽ */
    const uint8_t index:4; /* ��� */
    const uint8_t *pText;  /* ������ */
    const Page_S *pSon;    /* ����ҳ */
}Item_S;

typedef struct{
    const Page_S *pParent;  /* ��ҳ */
    void (*Function)(uint8_t KeyIndex);  /* ��ǰҳ�Ļص� */
    const Item_S  *pItem;   /* ��ǰ�� */
    const uint8_t ItemNum;  /* ��ĸ��� */
}Page_S;

#endif  /* __MENU_H__ */