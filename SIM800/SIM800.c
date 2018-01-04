
/******************************
auth:wxc
data:2017-12-25
com:����sim800�ļ����������sim800��һϵ�к���
todo: at+caq   at+creg   ATE  tcp   udp ͸������͸��,CCID  ........

auth:wxc
data:2017-12-27
com:���ָ��ṹ�壬ʵ�ֹ��ú����������tcp��udp���������ͣ���ȡ������
todo: һ������ʵ�ַ��ͺͽ��գ�Ŀǰָ����յ�ģʽ���е����⣬͸����������ʹ��
*******************************/

#include "../AT/AT.h"
#include "SIM800.h"


//typedef struct AtCommon
//{
//    int index;
//    char *common;
//    char replay[COUNT_REVDATA];
//    char *data;
//    int timeout;
//    func backcallfunc;
//    char revflag;
//    char rpybuff[10];
//}ATCmd;
ATCmd gArrCmd[] =
{
    {0,"ATE","",0,20,0,0,"OK"},
    {1,"AT+CSQ","+CSQ:24,99\r\nOK\r\n",0,20,0,0,"OK"},
    {2,"AT+creg","",0,20,0,0,"OK"},
    {3,"AT+ccid","",0,20,0,0,"OK"},
    {4,"AT","",0,20,0,0,"OK"},
    {5,"AT+CPIN","",0,20,0,0,"READY"},
    {6,"AT+CGATT","",0,20,0,0,"OK"},
    {7,"AT+CSTT","",0,20,0,0,"OK"},
    {8,"AT+CIFSR","",0,20,0,0,"\r\n"},
    {9,"AT+CIPSTART","",0,20,0,0,"OK"},
    {10,"AT+CIPMODE","",0,20,0,0,"OK"}, //1��͸�� 0�Ƿ�͸��
    {11,"AT+CIPCLOSE","",0,20,0,0,"OK"},
    {12,"AT+CIPSEND","",0,20,0,0,">"},
    {13,"AT+CIPSRIP","",0,20,0,0,"OK"},
    {14,"AT+CIP","",0,20,0,0,">"},
};



/** \brief backcallfunc  �ص�������ָ�����֮�󣬻������������������ݷ���
 *
 * \param mode ���ص�ģʽ
 * \param index ��ǰִ�е�ָ��
 * \return
 *
 */
void backcallfunc(int mode,int index)
{
    switch(mode)
    {
    case OK:
        break;
    case ERROR:
        break;

    default : break;

    }
}

//��ʼ��
void initSim800()
{
    initAtCmd(gArrCmd,backcallfunc);
}
char isExecOK(char *buff)
{
    if(buff != null)
    {
        if(strstr(buff,"OK") != null)
        {
            return OK;
        }
    }
    return ERROR;
}
/** \brief ��ȡָ�����ƺ��������
 *
 * \param
 * \param
 * \return
 *
 */

char *getAtValue(char *data)
{
    char *dataIndex;
    if(data != null && (dataIndex = strchr(data,':')) != null)
    {
        return dataIndex + 1;  //�� ':'������һλ
    }
    return null;
}
/** \brief ��ȡ��������   getAtValueforString
 *
 * \param data �����ַ���
 * \param len �м�������
 * \return �����ַ���
 *
 */
 static char * getAtValueforString(char *data,int len)
 {
     static char revbuff[10][50] = {{0}};
     static int buffIndex = 0;
     int i = 0;
     char *tempData;
     char buff[50] = {0};
     if(data == null)
     {
        buffIndex ++;
        return revbuff[buffIndex];
     }
     else
     {
        for(int j = 0 ; j < 10 ; j ++)
        {
            memset(revbuff[j],0,50);
        }
        buffIndex = 0;
     }
     for(i = 0 ; i < len -1 ; i ++)
     {
        tempData = strchr(data,',');
        memset(buff,0,sizeof(buff));
        strncpy(buff,data,tempData-data);
        memcpy(revbuff[i],buff,sizeof(buff));
        data = tempData+1;   //tempDataĿǰָ�� ',' �ַ�����
     }
     memset(buff,0,sizeof(buff));
     tempData = strchr(data,'\r');
     strncpy(buff,data,tempData-data);
     memcpy(revbuff[i],buff,sizeof(buff));
     return revbuff[buffIndex];
 }



/** \brief ��ȡ��������
 *
 * \param  ������ַ���
 * \param  �м�������
 * \return ������������
 *
 */
 static int * getAtValueforint(char *data,int len)
 {
     static int revbuff[50] = {0};
     int i = 0;
     char *tempData;
     char buff[50] = {0};
     if(data == null)
     {
        return null;
     }
     for(i = 0 ; i < len -1 ; i ++)
     {
        tempData = strchr(data,',');
        memset(buff,0,sizeof(buff));
        strncpy(buff,data,tempData-data);
        revbuff[i] = atoi(buff);

        data = tempData+1;   //tempDataĿǰָ�� ',' �ַ�����
     }
    memset(buff,0,sizeof(buff));
    tempData = strchr(data,'\r');
    strncpy(buff,data,tempData-data);
    revbuff[i] = atoi(buff);
    return revbuff;
 }

/** \brief getAtCommomInt
 *
 * \param  ��Ҫ���͵�ָ�����Ҫ���صĲ����ĸ���
 * \param
 * \return
 *
 */

int *getAtCommomInt(int CommomId,int parmLen)
{
    static int *arrData;
    //CLEAN_ARR(arrCsqData,2);
    sendAtCmd(CommomId,0);
    print("had send cmd \r\n");
    while(getATrevFlag(CommomId) != WAIT)
    {
        //�������
        print("rev success \r\n");
        char *data = getATrevdata(CommomId);
        if(isExecOK(data) == OK)
        {
            if((data = getAtValue(data)) == null)
            {
                break;
            }
            //��������ȡcsq������
            arrData = getAtValueforint(data,parmLen);
        }
        //cleanATrevFlag(CommomId);
    }
    return arrData;
}
/** \brief getAtCommomString
 *
 * \param  ��Ҫ���͵�ָ�����Ҫ���صĲ����ĸ���
 * \param
 * \return
 *
 */
static char **atRevData;
char **getAtCommomString(int CommomId,int parmLen)
{
    //CLEAN_ARR(arrCsqData,2);
    sendAtCmd(CommomId,0);
    print("had send cmd \r\n");
    while(getATrevFlag(CommomId) != WAIT)
    {
        //�������
        print("rev success \r\n");
        char *data = getATrevdata(CommomId);
        if(isExecOK(data) == OK)
        {
            if((data = getAtValue(data)) == null)
            {
                break;
            }
            //��������ȡcsq������
            atRevData[0] = getAtValueforString(data,parmLen);
            for(int i = 1 ; i < parmLen ; i ++)
            {
                atRevData[i] = getAtValueforString(null,parmLen);
            }
        }
        //cleanATrevFlag(CommomId);
    }
    return atRevData;
}
/**\ getAtCommom
 *
 * \param ����ָ��Ͳ�����ר������û�з���ֵ��ָ����д���
 * \param
 * \return
 *
 */
int getAtCommom(int CommomId,char *parm)
{
    setAtParm(CommomId,parm);
    sendAtCmd(CommomId,0);

    print("had send cmd \r\n");
    while(getATrevFlag(CommomId) != WAIT)
    {
        //�������
        print("rev success \r\n");
        char *data = getATrevdata(CommomId);
        return getATrevFlag(CommomId);
    }
}
/**\ cifsr
 *
 * \param ���ƻ�CIFSR��ָ��
 * \param
 * \return
 *
 */
 char *getatCifsrforIPAddress(int commomdId)
 {
    static char ipAdress[50];
    sendAtCmd(commomdId,0);
    print("had send cmd \r\n");
    while(getATrevFlag(commomdId) != WAIT)
    {
        //�������
        print("rev success \r\n");
        char *data = getATrevdata(commomdId);
        if(strstr(data,"ERROR") != null)
        {
            return null;
        }
        //��������ȡcsq������
        memcpy(ipAdress,data,sizeof(ipAdress));
        //cleanATrevFlag(CommomId);
    }
    return ipAdress;
 }

