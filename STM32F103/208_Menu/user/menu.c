#include "menu.h"

uint8_t ItemSum = 0;

uint8_t GetItemNum(const Page_S *pPage)
{
    
}

void ShowPage(const Page_S *pPage)
{
    uint8_t i;
    if(NULL == pPage){
        return;
    }
    for(i=0; i<pPage->count; i++){
        printf("%s\r\n",pPage->pText);
    }
    if()
}

void ShowParentPage(const Page_S *pPage)
{
    pPage = pPage->pParent;
    ShowPage(pPage);
}

void ShowSonPage(const Page_S *pPage)
{
    pPage = pPage->pSon;
    ShowPage(pPage);
}