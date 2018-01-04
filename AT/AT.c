/******************************
auth:wxc
data:2017-12-22
com:�½����̣��������ATָ��Ľ�����д�뷵��
todo:

auth:wxc
data:2017-12-25
com:��ӽṹ�岢����ɷ������ݣ����ս������ݣ���ȡʱ����Ⱥ���
todo:

auth:wxc
data:2017-12-27
com:��Ӳ������ú��������Ҹ��߲������󣬷��Ͳ�ͬ��ָ��,���sim800�����ݶ�ȡ����
todo:


*******************************/
#include "AT.h"

//typedef struct AtData
//{
//    int count;
//    char data[COUNT_REVDATA];
//}ATDat;

static ATCmd *head;
static char *gArrCmdFlag;
static int ARRAY_COUNT = 0;
static uint g_timecount = 0;
static int ATNowMode = 0;
static callFunc *backcallfunc;
void initAtCmd(ATCmd *dat,callFunc *func)
{
    if(dat != null)
    {
        ARRAY_COUNT = ARRAY(dat);
        head = dat;
        gArrCmdFlag = (char *)malloc(ARRAY_COUNT);

        backcallfunc = func;
    }
}

int getAtNowMode()
{
    return ATNowMode;
}
void setAtNowMode(int mode)
{
    ATNowMode = mode;
}
void closeAtCmd()
{
    head = 0;
    if(gArrCmdFlag != null)
    {
        free(gArrCmdFlag);
    }
}

void UsartSendCmd(char* dat)
{
    if(dat != null)
    {
        uartSendstring(dat);
    }
}
char *getATrevdata(int index)
{
    if(&head[index] != null)
    {
        return head[index].replay;
    }
}
/** \brief  setAtParm
 *
 * \param index:�±� ,
 * \param parm ����
 * \return void
 *
 */

void setAtParm(int index,char *parm)
{
    if(parm == null && &head[index] == null)
    {
        return;
    }
    head[index].parm = parm;
}
/** \brief sendAtCmd
 *
 * \param index �±֪꣬�����Ǹ�ָ����ִ��
 * \param
 * \return void
 *
 */

void sendAtCmd(int index,int time)
{
    if(&head[index] != null)
    {
        char buff[100] = {0};
        g_timecount = time;
        setAtNowMode(ATMODE); //����Ϊָ���ģʽ
        cleanATrevFlag(index); // ��ս���״̬�͵�ǰ����״̬
        if(head[index].parm != 0 && head[index].parm != "" ) //������û�в��������и�ʽ�����
        {
            sprintf(buff,"%s=%s\r\n",head[index].common,head[index].parm);
        }
        else
        {
            sprintf(buff,"%s\r\n",head[index].common);
        }
        gArrCmdFlag[index] = SEND;
        head[index].revflag = ERROR;
        UsartSendCmd(buff);
    }
}
static char revDatabuff[COUNT_REVDATA];
static int readCount = 0;
void getRevData(char *data)
{
    if(data!= null)
    {
        //�ѽ��յ�������һ��ȫ��������. �������������
        char temp_buff[1024];
        memcpy(temp_buff,revDatabuff,readCount);

        memcpy(data,temp_buff,readCount);

        memset(revDatabuff,0,readCount);
        readCount = 0;


    }
}

void input(char ch)
{
    //  dadsadadasdasds\r\n
    // \r\n OK  \r\n
    static int index = 0;
    static uchar stcRevbuff[COUNT_REVDATA/2] = {0};

    stcRevbuff[index++] = ch;

    for(int i = 0 ; i < ARRAY_COUNT ; i ++)
    {
        //�յ�����ʲô���ݣ�������������ж� ����ֻ�ж��Ƿ񷵻�
        if(gArrCmdFlag[i] == SEND && strstr(stcRevbuff,head[i].rpybuff)!= null)
        {
            //���ڷ���״̬�����ҷ���ֵ������ָ���Ľ�����
            strcpy(head[i].replay,(char *)stcRevbuff);
            memset(stcRevbuff,0,index);
            index = 0;
            gArrCmdFlag[i] = PENDING;
        }//if(gCmdFlag[i] == SEND)
    }//for
}
//���ָ��ִ��֮���״̬��Ϣ
void cleanATrevFlag(int index)
{
    if(&head[index] != null)
    {
        gArrCmdFlag[index] = WAIT;
        head[index].revflag = WAIT;
    }
}
//����ָ��ִ�н����OK �� ERROR
char getATrevFlag(int index)
{
    if(&head[index] != null)
    {
        return head[index].revflag;
    }
    return -1;
}

void callBackfunc(char ch,int time)
{

    if(getAtNowMode() == REVDATA)
    {
        //�ѽ��ܵ��������ݴ洢��ָ���ĵط�
        revDatabuff[readCount++] = ch;
        if(readCount > COUNT_REVDATA)
        {
            readCount = 0;
        }
    }
    else
    {
        input(ch);
    }
    //�ж�ָ��ִ�е�״̬
    for(int i = 0 ; i < ARRAY_COUNT ; i++)
    {
        if(time - g_timecount > head[i].timeout && gArrCmdFlag[i] == SEND)
        {
            head[i].revflag = ERROR;
            gArrCmdFlag[i] = WAIT;
            (*backcallfunc)(head[i].revflag,i);
            return;
        }
        if(gArrCmdFlag[i] == PENDING)
        {
            //����ǵȴ������ָ��
            head[i].revflag = OK;
            gArrCmdFlag[i] = WAIT;
            (*backcallfunc)(head[i].revflag,i);
            break;
        }
    }
}






