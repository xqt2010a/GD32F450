#include "robot_protocol.h"

Protocol_Status_Struct Protocol_Status;
Protocol_Struct pTx;

uint8_t xor_sum(const uint8_t* pDataIn, int iLenIn) //BCC
{
    uint8_t ret = 0;
 
    for (int i = 0; i < iLenIn; i++) {
        ret ^= pDataIn[i];
    }
    return ret;
}

char EncodeProcess(const uint8_t *buf)
{
    uint8_t bcc = 0;
    Protocol_Struct *pRx = (Protocol_Struct *)buf;
    
    if(R_CMD_HEADER == pRx->header){    //校验头OK
        pTx.header = R_CMD_HEADER;
        pTx.data_len = R_ONE_DATA_LEN;
        bcc = *((uint8_t *)(&pRx->cmd_type)+pRx->data_len+2);
        
        if(R_CTRL_DOWN_CMD == pRx->cmd_type){       //行走轨迹控制
            pTx.cmd_type = R_CTRL_UP_CMD;
            if(bcc == xor_sum((uint8_t *)(&pRx->cmd_type), pRx->data_len+2)){
                if(sizeof(Ctrl_Struct) == pRx->data_len){
                    Protocol_Status.cmd_type = R_CTRL_DOWN_CMD;
                    Protocol_Status.ctrl.v = R_BL_32(pRx->ctrl.v);
                    Protocol_Status.ctrl.w = R_BL_32(pRx->ctrl.w);
                    Protocol_Status.dst.Vr = R_CAR_Vr(Protocol_Status.ctrl.v, Protocol_Status.ctrl.w);
                    Protocol_Status.dst.Vl = R_CAR_Vl(Protocol_Status.ctrl.v, Protocol_Status.ctrl.w);
                    pTx.data = R_SUCCESS;
                }
                else{
                    pTx.data = R_FAILD;
                }
            }
            else{
                pTx.data = R_FAILD;
            }
            *(((uint8_t *)&pTx.cmd_type)+2+pTx.data_len) = xor_sum((uint8_t *)&pTx.cmd_type, 2+pTx.data_len);   //bcc
            R_PUTS((uint8_t *)&pTx, R_FIX_LEN+pTx.data_len);
        }
        else if(R_MODE_DOWN_CMD == pRx->cmd_type){  //模式切换
            pTx.cmd_type = R_MODE_UP_CMD;
            if(bcc == xor_sum((uint8_t *)(&pRx->cmd_type), pRx->data_len+2)){
                if(sizeof(Mode_Struct) == pRx->data_len){
                    Protocol_Status.cmd_type = R_MODE_DOWN_CMD;
                    Protocol_Status.mode.mode = pRx->mode.mode;
                    pTx.data = R_SUCCESS;
                }
                else{
                    pTx.data = R_FAILD;
                }
            }
            else{
                pTx.data = R_FAILD;
            }
            *(((uint8_t *)&pTx.cmd_type)+2+pTx.data_len) = xor_sum((uint8_t *)&pTx.cmd_type, 2+pTx.data_len);    //bcc
            R_PUTS((uint8_t *)&pTx, R_FIX_LEN+pTx.data_len);
        }
        return R_SUCCESS;
    }
    else{
        return R_FAILD;
    }
}

void PathReport(void)
{
    pTx.header = R_CMD_HEADER;
    pTx.cmd_type = R_PATH_REPORT_CMD;
    pTx.data_len = R_PATH_DATA_LEN;
    pTx.path.v = R_BL_32(Protocol_Status.path.v);
    pTx.path.w = R_BL_32(Protocol_Status.path.w);
    pTx.path.deg_w = R_BL_16(Protocol_Status.path.deg_w);
    pTx.path.deg = R_BL_16(Protocol_Status.path.deg);
    //BCC Code
//    pTx.path.v = 0x31313131;
//    pTx.path.w = 0x32323232;  
//    pTx.path.deg_w = 0x3333;  
//    pTx.path.deg = 0x3434;
    *(((uint8_t *)&pTx.cmd_type)+2+pTx.data_len) = xor_sum((uint8_t *)&pTx.cmd_type, 2+pTx.data_len);    //bcc    
    R_PUTS((uint8_t *)&pTx, (R_FIX_LEN+R_PATH_DATA_LEN));
    //R_PUTS("helloabcdefghijklmn\r\n",21);
}