#include "robot_protocol.h"
#include "string.h"

uint32_t Right_Num = 0;     //pid 次数，用于数据帧判断是否连续
uint32_t Left_Num = 0;

uint32_t Right_Count = 0;   //采集计数，用于行走距离
uint32_t Left_Count = 0;

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

void Protocol_Init(void)
{
    memset((uint8_t *)&Protocol_Status, 0, sizeof(Protocol_Status_Struct));
    memset((uint8_t *)&pTx, 0, sizeof(Protocol_Struct));
    Right_Num = Left_Num = 0;
    Right_Count = Left_Count = 0;
}

void CTRL_Handle(Protocol_Struct *pRx, Protocol_Struct *pTx)
{
    if(sizeof(Ctrl_Struct) == pRx->data_len){
        Protocol_Status.cmd_type = R_CTRL_DOWN_CMD;
        Protocol_Status.v = R_BL_32(pRx->ctrl.v);
        Protocol_Status.w = R_BL_32(pRx->ctrl.w);
        Protocol_Status.Sr = Protocol_Status.Sl = 0;
        Protocol_Status.dst.Vr = R_CAR_Vr(Protocol_Status.v, Protocol_Status.w);
        Protocol_Status.dst.Vl = R_CAR_Vl(Protocol_Status.v, Protocol_Status.w);
        //Protocol_Status.temp.Vr = Protocol_Status.dst.Vr;
        //Protocol_Status.temp.Vl = Protocol_Status.dst.Vl;
        pTx->data = R_SUCCESS;
    }
    else{
        pTx->data = R_FAILD;
    }
}

void VS_Handle(Protocol_Struct *pRx, Protocol_Struct *pTx)
{
    if(sizeof(VS_Struct) == pRx->data_len){
        Protocol_Status.cmd_type = R_VS_DOWN_CMD;
        Protocol_Status.v = R_BL_32(pRx->vs.v);
        Protocol_Status.w = 0;
        Protocol_Status.Sr = Protocol_Status.Sl = R_BL_32(pRx->vs.s)*1000;
        Protocol_Status.dst.Vr = R_CAR_Vr(Protocol_Status.v, Protocol_Status.w);
        Protocol_Status.dst.Vl = R_CAR_Vl(Protocol_Status.v, Protocol_Status.w);
		Protocol_Status.count_r = Protocol_Status.Sr*R_CAR_ENCODER_N/3142/70-80;     //S*N/PI*L
		Protocol_Status.count_l = Protocol_Status.Sl*R_CAR_ENCODER_N/3142/70-80;     //S*N/PI*L
        Protocol_Status.temp.Vr = Protocol_Status.dst.Vr;
        Protocol_Status.temp.Vl = Protocol_Status.dst.Vl;
        Right_Num = Left_Num = 0;		//pid 次数
		Right_Count = Left_Count = 0;	//速度采样次数
        pTx->data = R_SUCCESS;
    }
    else{
        pTx->data = R_FAILD;
    }
}

void WD_Handle(Protocol_Struct *pRx, Protocol_Struct *pTx)
{
    if(sizeof(WD_Struct) == pRx->data_len){
        Protocol_Status.cmd_type = R_WD_DOWN_CMD;
        Protocol_Status.v = 0;
        Protocol_Status.w = R_BL_32(pRx->wd.w);            
        Protocol_Status.Sr = Protocol_Status.Sl = 314*R_CAR_WIDE*R_BL_32(pRx->wd.d)/36;      //s = PI*L*deg/360
        Protocol_Status.dst.Vr = R_CAR_Vr(Protocol_Status.v, Protocol_Status.w);
        Protocol_Status.dst.Vl = R_CAR_Vl(Protocol_Status.v, Protocol_Status.w);
		Protocol_Status.count_r = Protocol_Status.Sr*R_CAR_ENCODER_N/3142/70-80;     //S*N/PI*L
		Protocol_Status.count_l = Protocol_Status.Sl*R_CAR_ENCODER_N/3142/70-80;     //S*N/PI*L
        Protocol_Status.temp.Vr = Protocol_Status.dst.Vr;
        Protocol_Status.temp.Vl = Protocol_Status.dst.Vl;
        Right_Num = Left_Num = 0;		//pid 次数
		Right_Count = Left_Count = 0;	//速度采样次数
        pTx->data = R_SUCCESS;
    }
    else{
        pTx->data = R_FAILD;
    }
}

void MODE_Handle(Protocol_Struct *pRx, Protocol_Struct *pTx)
{
    if(sizeof(Mode_Struct) == pRx->data_len){
        Protocol_Status.cmd_type = R_MODE_DOWN_CMD;
        Protocol_Status.mode.mode = pRx->mode.mode;
        pTx->data = R_SUCCESS;
    }
    else{
        pTx->data = R_FAILD;
    }
}

void FLAG_Handle(Protocol_Struct *pRx, Protocol_Struct *pTx)
{
    if(sizeof(Flag_Struct) == pRx->data_len){
        Protocol_Status.bcc_flag = pRx->flag.bcc;
        pTx->data = R_SUCCESS;
    }
    else{
        pTx->data = R_FAILD;
    }
}


char EncodeProcess(const uint8_t *buf)
{
    uint8_t bcc = 0;
    Protocol_Struct *pRx = (Protocol_Struct *)buf;
    
    if(R_CMD_HEADER == pRx->header){    //校验头OK
        pTx.header = R_CMD_HEADER;
        pTx.data_len = R_ONE_DATA_LEN;
        bcc = *((uint8_t *)(&pRx->cmd_type)+pRx->data_len+2);
        
        if((bcc == xor_sum((uint8_t *)(&pRx->cmd_type), pRx->data_len+2))||
            (Protocol_Status.bcc_flag == 1)){
            if(R_CTRL_DOWN_CMD == pRx->cmd_type){       //行走轨迹控制
                pTx.cmd_type = R_CTRL_UP_CMD;
                CTRL_Handle(pRx, &pTx);
            }
            else if(R_VS_DOWN_CMD == pRx->cmd_type){    //线速度，距离
                pTx.cmd_type = R_VS_UP_CMD;
                VS_Handle(pRx, &pTx);
            }
            else if(R_WD_DOWN_CMD == pRx->cmd_type){    //角速度，度
                pTx.cmd_type = R_WD_UP_CMD;
                WD_Handle(pRx, &pTx);
            }
            else if(R_MODE_DOWN_CMD == pRx->cmd_type){  //模式切换
                pTx.cmd_type = R_MODE_UP_CMD;
                MODE_Handle(pRx, &pTx);
            }
            else if(R_FLAG_DOWN_CMD == pRx->cmd_type){  //自定义
                pTx.cmd_type = R_FLAG_DOWN_CMD;
                FLAG_Handle(pRx, &pTx);
            }
            else{
                pTx.cmd_type = 0;
                pTx.data = R_FAILD;
            }
        }
        else{   //bcc error
            if(R_CTRL_DOWN_CMD == pRx->cmd_type){
                pTx.cmd_type = R_CTRL_UP_CMD;
            }
            else if(R_VS_DOWN_CMD == pRx->cmd_type){
                pTx.cmd_type = R_VS_UP_CMD;
            }
            else if(R_WD_DOWN_CMD == pRx->cmd_type){
                pTx.cmd_type = R_WD_UP_CMD;
            }
            else if(R_MODE_DOWN_CMD == pRx->cmd_type){
                pTx.cmd_type = R_MODE_UP_CMD;
            }
            else{
                pTx.cmd_type = 0;
            }
            pTx.data = R_FAILD;
        }
        
        *(((uint8_t *)&pTx.cmd_type)+2+pTx.data_len) = xor_sum((uint8_t *)&pTx.cmd_type, 2+pTx.data_len);    //bcc
        R_PUTS((uint8_t *)&pTx, R_FIX_LEN+pTx.data_len);
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