#include "robot_protocol.h"
#include "string.h"

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

//void Speed_Limit(uint8_t type)  //0:VS  1:WD
//{
//    int32_t a,b,s;
//    s = ABS_FUC(Protocol_Status.dst_s.r);
//    if(s<(R_CAR_PI*R_CAR_WIDE/100)){
//        if(s > STOP_LEN){
//            a = (int64_t)(MAX_SPEED-MIN_SPEED)*ZOOM_RATE/(R_CAR_PI*R_CAR_WIDE/100-STOP_LEN);   //放大10000倍
//            b = MAX_SPEED - (int64_t)a*(R_CAR_PI*R_CAR_WIDE/100)/ZOOM_RATE;
//            if(type){   //WD
//                if(Protocol_Status.dst_v.Vr >= 0){
//                    Protocol_Status.dst_v.Vr = (int64_t)a*s/ZOOM_RATE+b; //y = ax + b
//                    Protocol_Status.dst_v.Vl = -Protocol_Status.dst_v.Vr;
//                }
//                else{
//                    Protocol_Status.dst_v.Vl = (int64_t)a*s/ZOOM_RATE+b; //y = ax + b
//                    Protocol_Status.dst_v.Vr = Protocol_Status.dst_v.Vl;
//                }
//            }
//            else{       //VS
//                Protocol_Status.dst_v.Vl = Protocol_Status.dst_v.Vr = (int64_t)a*s/ZOOM_RATE+b; //y = ax + b
//            }
//        }
//        else if(s > 0){
//            Protocol_Status.dst_v.Vl = Protocol_Status.dst_v.Vr = MIN_SPEED;
//        }
//    }
//}

void Protocol_Init(void)
{
    memset((uint8_t *)&Protocol_Status, 0, sizeof(Protocol_Status_Struct));
    memset((uint8_t *)&pTx, 0, sizeof(Protocol_Struct));
}

void CTRL_Handle(Protocol_Struct *pRx, Protocol_Struct *pTx)
{
    if(sizeof(Ctrl_Struct) == pRx->data_len){
        Protocol_Status.cmd_type = R_CTRL_DOWN_CMD;
        Protocol_Status.v = R_BL_32(pRx->ctrl.v);
        Protocol_Status.w = R_BL_32(pRx->ctrl.w);
        Protocol_Status.dst_s.r = Protocol_Status.dst_s.l = 0;
        Protocol_Status.dst_v.Vr = R_CAR_Vr(Protocol_Status.v, Protocol_Status.w);
        Protocol_Status.dst_v.Vl = R_CAR_Vl(Protocol_Status.v, Protocol_Status.w);
        pTx->data = R_SUCCESS;
    }
    else{
        pTx->data = R_FAILD;
    }
}

/* 线速度 距离 */
void VS_Handle(Protocol_Struct *pRx, Protocol_Struct *pTx)
{
    if(sizeof(VS_Struct) == pRx->data_len){
        Protocol_Status.cmd_type = R_VS_DOWN_CMD;
        Protocol_Status.v = R_BL_32(pRx->vs.v);
        Protocol_Status.w = 0;
        Protocol_Status.dst_s.r = Protocol_Status.dst_s.l = R_BL_32(pRx->vs.s)*1000;
        Protocol_Status.dst_v.Vr = R_CAR_Vr(Protocol_Status.v, Protocol_Status.w);
        Protocol_Status.dst_v.Vl = R_CAR_Vl(Protocol_Status.v, Protocol_Status.w);
        //Speed_Limit(0);
        Protocol_Status.dst_temp_v.Vr = Protocol_Status.dst_v.Vr;
        Protocol_Status.dst_temp_v.Vl = Protocol_Status.dst_v.Vl;
        pTx->data = R_SUCCESS;
    }
    else{
        pTx->data = R_FAILD;
    }
}

/* 角速度 角度 */
void WD_Handle(Protocol_Struct *pRx, Protocol_Struct *pTx)
{
    uint64_t run_s;
    if(sizeof(WD_Struct) == pRx->data_len){
        
        Protocol_Status.cmd_type = R_WD_DOWN_CMD;
        Protocol_Status.v = 0;
        Protocol_Status.w = R_BL_32(pRx->wd.w); 
        run_s = (uint64_t)R_CAR_PI*R_CAR_WIDE*R_BL_32(ABS_FUC(pRx->wd.d));
        Protocol_Status.dst_s.r = Protocol_Status.dst_s.l = run_s/36000;      //s = PI*L*deg/360
        Protocol_Status.dst_v.Vr = R_CAR_Vr(Protocol_Status.v, Protocol_Status.w);
        Protocol_Status.dst_v.Vl = R_CAR_Vl(Protocol_Status.v, Protocol_Status.w);
        //Speed_Limit(1);
        Protocol_Status.dst_temp_v.Vr = Protocol_Status.dst_v.Vr;
        Protocol_Status.dst_temp_v.Vl = Protocol_Status.dst_v.Vl;
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
                
            memset((uint8_t *)&Protocol_Status, 0, sizeof(Protocol_Status_Struct));
            
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
    *(((uint8_t *)&pTx.cmd_type)+2+pTx.data_len) = xor_sum((uint8_t *)&pTx.cmd_type, 2+pTx.data_len);    //bcc    
    R_PUTS((uint8_t *)&pTx, (R_FIX_LEN+R_PATH_DATA_LEN));
}