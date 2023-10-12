
#include <iostream>
#include "canlib.h"
#include <thread>
#include <iostream>

using namespace std;
//функции
void read( canHandle hnd);
void sent( canHandle hnd);
void ListChannels();
void CheckForError(char cmd[50], canStatus stat);
void Check(const char* id, canStatus stat);
//


int main()
{
    setlocale(0, "");
    //открываем канал 
    //отправляем сообзение
    //принимаем сообщение 
    //закрываем канал
    canInitializeLibrary();
    printf("%s\n", "библиотека инициализирована");
    ListChannels();//слушаем каналы

    return 0;
}
void ListChannels()
{
    canHandle hnd;
    canStatus stat;
   
    //

    printf("Writing a message to the channel and waiting for it to be sent \n");
    
    thread tr(sent, hnd);//открываем поток для отправки
    thread tr1(read, hnd);//для чтения
    tr.join();
    tr1.join();
}

void CheckForError(char cmd[50], canStatus stat)
{
    //if stat not ok, print error
    if (stat != canOK)
    {
        char buf[255];
        buf[0] = '\0';
        canGetErrorText(stat, buf, sizeof(buf));
        printf("[%s] %s: failed, stat=%d\n", cmd, buf, (int)stat);
        printf("ABORT (Press any key)");
        getchar();
        abort();
    }
}
void Check(const char* id, canStatus stat)
{
    if (stat != canOK) {
        char buf[255];
        buf[0] = '\0';
        canGetErrorText(stat, buf, sizeof(buf));
        printf("%s: failed, stat=%d (%s)\n", id, (int)stat, buf);
    }
}

void sent( canHandle hnd)
{
    canStatus stat;
    int channel_number = 0;
    printf("открылись для отправки %d\n", channel_number);
    hnd = canOpenChannel(channel_number, canOPEN_ACCEPT_VIRTUAL);
    if (hnd < 0) {
        Check("canOpenChannel", (canStatus)hnd);

        exit(1);
    }
    printf("Setting bitrate and going bus on\n");

    //проверки
    stat = canSetBusParams(hnd, canBITRATE_250K, 0, 0, 0, 0, 0);
    Check("canSetBusParams", stat);
    stat = canBusOn(hnd);
    Check("canBusOn", stat);
    
    char msg[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    for (int i = 0; i < 5; i++)
    {
        stat = canWrite(hnd, 1, msg, 8,0);
        //printf(" %d\n", i);
       Check("canWrite", stat);
    }
}
void read(canHandle hnd)
{
    canStatus statr;
    long idr;
    unsigned int dlcr, flagsr;
    unsigned char msgr[8];
    DWORD timestampr;

    int channel_number1 = 1;
    printf("открылись для чтения %d\n", channel_number1);
    hnd = canOpenChannel(channel_number1, canOPEN_ACCEPT_VIRTUAL);
    statr = canSetBusParams(hnd, canBITRATE_250K, 0, 0, 0, 0, 0);
    Check("canSetBusParams", statr);
    statr = canBusOn(hnd);
    Check("canBusOn", statr);

    for (int i = 0; i < 5; i++)
    {
        statr = canReadWait(hnd, &idr, msgr, &dlcr, &flagsr, &timestampr, 1000);
        if (statr == canOK) {
            if (flagsr & canMSG_ERROR_FRAME) {
                printf("***ERROR FRAME RECEIVED***");
            }
            // If no error flag was found, the program prints the message.
            else {
                printf("Id: %ld, Msg: %u %u %u %u %u %u %u %u %u Flags: %lu\n",
                    idr, dlcr, msgr[0], msgr[1], msgr[2], msgr[3], msgr[4],
                    msgr[5], msgr[6], msgr[7], timestampr);
                
            }
        }
        else if (statr != canERR_NOMSG) {
            Check("canRead", statr);
            //break;
        }
    }
}
