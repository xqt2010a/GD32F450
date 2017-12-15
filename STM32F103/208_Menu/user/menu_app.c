#include "menu_app.h"

#define HomePage_NUM    3
#define SettingPage_NUM 3

const Item_S MainItem[] = {
    0x00, 0x00, "信息", &HomePage,
    0x00, 0x01, "设置", &SettingPage,
    0x00, 0x02, "版本", &VersionPage,
    0x00, 0x03, "时间", &TimePage,
    0x00, 0x04, "状态", NULL,
    0x00, 0x05, "报警", NULL,
    0x00, 0x06, "飞信", NULL,
    0x00, 0x07, "状态", NULL,
};

const Page_S HomePage[] = {
    {HomePage_NUM, "设置", Setting_Page, NULL, &SettingPage},
    {HomePage_NUM, "版本", Version_Page, NULL, NULL},
    {HomePage_NUM, "时间", Time_Page,    NULL, NULL}};

const Page_S SettingPage[] = {
    {SettingPage_NUM, "set0", Set_Page0, HomePage, &NULL},
    {SettingPage_NUM, "set1", Set_Page1, HomePage, &NULL},
    {SettingPage_NUM, "set2", Set_Page2, HomePage, &NULL}};


const Page_S MainPage[] =       { NULL,     MainPage_CallBack, MainItem, sizeof(Main_Item)/sizeof(Item_S) };
const Page_S HomePage[] =       {&MainPage, HomePage_CallBack, HomeItem, sizeof(Home_Item)/sizeof(Item_S) };
const Page_S SettingPage[] =    {&MainPage, SettingPage_CallBack, SettingItem, sizeof(Setting_Item)/sizeof(Item_S) };
const Page_S VersionPage[] =    {&MainPage, VersionPage_CallBack, NULL, 0 };
const Page_S TimePage[] =       {&MainPage, TimePage_CallBack, NULL, 0 };
const Page_S HomeTextPage[] =    {&SettingPage, HomeTextPage_CallBack, NULL, 0 };



void MainPage_CallBack(uint8_t KeyItem)
{
    switch(KeyItem)
    {
        case KEY_UP:	
		case KEY_Down:	
		case KEY_Left:	
		case KEY_Right:
			KeySelItem(key);
			break;
			
		case KEY_Return:  //主菜单对返回没有处理
			ShowPage(&mainPage);
			break;
		case KEY_Ok:
			ShowItemPage();
			break;
    }
}

void HomePage_CallBack(uint8_t KeyItem)
{

}

void SettingPage_CallBack(uint8_t KeyItem)
{
    switch(KeyItem)
    {
        case KEY_UP:	
		case KEY_Down:	
		case KEY_Left:	
		case KEY_Right:
			KeySelItem(key);
			break;
			
		case KEY_Return:  
			ShowParentPage();
			break;
		case KEY_Ok:
			ShowItemPage();
			break;
    }
}

void VersionPage_CallBack(uint8_t KeyItem)
{
    printf("Version Information\r\n");
    printf("MF:   abwba\r\n");
    printf("ADDR: shanghai\r\n");
    printf("Ver:  V0.1\r\n");
    
    if(KEY_RETURN == KeyItem){
        ShowParentPage();
    }
}

void TimePage_CallBack(uint8_t KeyItem)
{
    printf("Date: %s\r\n", __DATE__);
    printf("Time: %s\r\n", __TIME__);
    if(KEY_RETURN == KeyItem){
        ShowParentPage();
    }
}

void HomeTextPage_CallBack(uint8_t KeyItem)
{
    printf("None.\r\n");
    if(KEY_RETURN == KeyItem){
        ShowParentPage();
    }
}



