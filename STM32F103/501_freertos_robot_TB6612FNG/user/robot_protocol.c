#include "robot_protocol.h"

Protocol_Status_Struct Protocol_Status;
Protocol_Struct pTx;

char EncodeProcess(const uint8_t *buf)
{
    Protocol_Struct *pRx = (Protocol_Struct *)buf;
    
    char rvalue = R_FAILD;
    pTx.header = R_CMD_HEADER;
    if(R_CMD_HEADER == pRx->header){
        if(R_CTRL_DOWN_CMD == pRx->cmd_type){   //行走轨迹控制
            if(R_CTRL_DATA_LEN == pRx->data_len){
                //BCC Decode
                Protocol_Status.cmd_type = R_CTRL_DOWN_CMD;
                Protocol_Status.ctrl.v = pRx->ctrl.v;
                Protocol_Status.ctrl.w = pRx->ctrl.w;
            }
            else{
                pTx.cmd_type = R_CTRL_UP_CMD;
                pTx.data_len = R_OTHER_DATA_LEN;
                pTx.data = R_FAILD;
                //BCC Code
                R_PUTS((uint8_t *)&pTx, (R_FIX_LEN+R_OTHER_DATA_LEN));
            }
            rvalue = R_SUCCESS;
        }
        else if(R_MODE_DOWN_CMD == pRx->cmd_type){  //模式切换
            if(R_OTHER_DATA_LEN == pRx->data_len){
                //BCC
                Protocol_Status.cmd_type = R_MODE_DOWN_CMD;
                Protocol_Status.mode = pRx->mode;
                return R_SUCCESS;
            }
            else{
                pTx.cmd_type = R_MODE_UP_CMD;
                pTx.data_len = R_OTHER_DATA_LEN;
                pTx.data = R_FAILD;
                //BCC Code
                R_PUTS((uint8_t *)&pTx, (R_FIX_LEN+R_OTHER_DATA_LEN));
            }
            rvalue = R_SUCCESS;
        }
    }
    return rvalue;
}

void PathReport(void)
{
    pTx.header = R_CMD_HEADER;
    pTx.cmd_type = R_PATH_REPORT_CMD;
    pTx.data_len = R_PATH_DATA_LEN;
//    pTx.path.v = Protocol_Status.path.v;
//    pTx.path.w = Protocol_Status.path.w;
//    pTx.path.deg_w = Protocol_Status.path.deg_w;
//    pTx.path.deg = Protocol_Status.path.deg;
    //BCC Code
    pTx.path.v = 0x31313131;
    pTx.path.w = 0x32323232;  
    pTx.path.deg_w = 0x3333;  
    pTx.path.deg = 0x3434;
        
    //R_PUTS((uint8_t *)&pTx, (R_FIX_LEN+R_PATH_DATA_LEN));
    R_PUTS("helloabcdefghijklmn\r\n",21);
}