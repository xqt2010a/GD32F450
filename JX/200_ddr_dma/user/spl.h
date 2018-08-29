#ifndef SPL_H
#define SPL_H
#define IMAGE_MAGIC 0x5AA55AA5 
#define SPL_LOADIMAGE_BUF_LEN	0x2000
#define SPL_LOADIMAGE_BUF_ADDR	0x94000

int ddr_load_train_code(int type);
#endif