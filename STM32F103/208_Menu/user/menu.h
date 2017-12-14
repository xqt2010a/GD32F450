#ifndef __MENU_H__
#define __MENU_H__

//typedef struct{
//    const uint8_t type:4;  /* 显示方式 */
//    const uint8_t index:4; /* 序号 */
//    const uint8_t ItemNum;  /* 项的个数 */
//    const uint8_t *pText;  /* 项名字 */
//    void (*Function)(uint8_t KeyIndex);  /* 当前页的回调 */
//    const Page_S *pParent;  /* 父页 */
//    const Page_S *pSon;    /* 项子页 */   
//}Page_S;

typedef struct{
    const uint8_t count; /* 序号 */
    const uint8_t *pText;  /* 项名字 */
    void (*Function)(uint8_t KeyIndex);  /* 当前页的回调 */
    const Page_S *pParent;  /* 父页 */
    const Page_S *pSon;    /* 项子页 */   
}Page_S;

#endif  /* __MENU_H__ */