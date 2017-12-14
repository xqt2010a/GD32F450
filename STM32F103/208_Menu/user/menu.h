#ifndef __MENU_H__
#define __MENU_H__

//typedef struct{
//    const uint8_t type:4;  /* ��ʾ��ʽ */
//    const uint8_t index:4; /* ��� */
//    const uint8_t ItemNum;  /* ��ĸ��� */
//    const uint8_t *pText;  /* ������ */
//    void (*Function)(uint8_t KeyIndex);  /* ��ǰҳ�Ļص� */
//    const Page_S *pParent;  /* ��ҳ */
//    const Page_S *pSon;    /* ����ҳ */   
//}Page_S;

typedef struct{
    const uint8_t count; /* ��� */
    const uint8_t *pText;  /* ������ */
    void (*Function)(uint8_t KeyIndex);  /* ��ǰҳ�Ļص� */
    const Page_S *pParent;  /* ��ҳ */
    const Page_S *pSon;    /* ����ҳ */   
}Page_S;

#endif  /* __MENU_H__ */