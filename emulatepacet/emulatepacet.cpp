
#include <iostream>
#include "canlib.h"
#include <thread>
#include <iostream>
#include <sstream>
#include <vector>
#include <conio.h>
using namespace std;
//функции
void read( canHandle hnd);
void sent( canHandle hnd);
void ListChannels();
void CheckForError(char cmd[50], canStatus stat);
void Check(const char* id, canStatus stat);
int length;
int nomber;
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
    //парсим
    
    FILE* fl;
    fopen_s(&fl, "E:\\proga\\emulatepacet\\emulatepacet\\res\\BUS-log.txt", "rt");
    if (!fl) //что-то не так с файлом
    {
        cout << "Проблема с файлом";
        exit(1); //выходим
    }
    char bufer[100]; //буфер под текстовые строк
    
    int x0, x1, x2, x3, x4, x5, x6, x7;
    int counter = 0;
    
    while (!feof(fl)) //цикл пока не кончится файл
    {
        unsigned char message[8] ;
        fgets(bufer, 100, fl); //берем строку в текстовый буфер 
        Check("canWrite", stat);
        std::string input = bufer;

        // Используем std::stringstream для разделения строки на токены
        std::stringstream ss(input);
        std::string token;
        while (ss >> token) 
        {
            if (token == "s") 
            {
                            // Найден токен "s", начинаем извлекать значения double
                std::vector<double> values;

                while (ss >> token) 
                {
                    // Попытаемся преобразовать строку в double и добавить ее в массив
                    char* endptr;
                    double value = std::strtol(token.c_str(), &endptr, 16);
                    if (*endptr == '\0') 
                    { // Проверяем, что преобразование прошло успешно
                        values.push_back(value);
                    }
                }
                nomber = values[0];
                unsigned char* msg=new unsigned char[9];
                for (int i = 1; i < nomber+1; i++)
                {
                    msg[i-1] = values[i];
                    
            
                }
                if (nomber < 8)
                {
                    for (int i = nomber + 1; i < 9; i++)
                    {
                        msg[i] = 0;
                    }
                }
                stat = canWrite(hnd, counter, msg, nomber, 0);
                Sleep(100);
                counter++;
                if (counter == 2047)
                {

                }
                break; // Мы нашли и обработали токен "s", завершаем поиск
                       
            }
        }
        
    }
    stat = canBusOff(hnd);
    Check("canBusOff", stat);
    //закрываем канал
    stat = canClose(hnd);
    Check("canClose", stat);

    //
    
}
void read(canHandle hnd)
{
    canHandle hnd1;
    canStatus statr;
    canStatus statbuf;
    long idr;
    unsigned int dlcr, flagsr;
    
    //unsigned char msgr[8];
    DWORD timestampr;

    int channel_number1 = 1;
    printf("открылись для чтения %d\n", channel_number1);
    hnd1 = canOpenChannel(channel_number1, canOPEN_ACCEPT_VIRTUAL);
    statr = canSetBusParams(hnd1, canBITRATE_250K, 0, 0, 0, 0, 0);
    Check("canSetBusParams", statr);
    statr = canBusOn(hnd1);
    Check("canBusOn", statr);
    int id=0;
    while(id!=2047)
    {
        unsigned char* mess = new unsigned char[nomber];
        statr = canReadWait(hnd1, &idr, mess, &dlcr, &flagsr, &timestampr, 120);
        if (statr == canOK) {
            if (flagsr & canMSG_ERROR_FRAME) {
                printf("***ERROR FRAME RECEIVED***");
            }
            // If no error flag was found, the program prints the message.
            else {
               
                printf("Id: %ld, Msg: %u %u %u %u %u %u %u %u %u Flags: %lu\n",
                    idr, dlcr, mess[0], mess[1], mess[2], mess[3], mess[4],
                    mess[5], mess[6], mess[7], mess[8], timestampr);
                id = idr;
                
            }
        }
        else if (statr != canOK) {
            Check("canRead", statr);
            break;
        }
        
        
    }
    statr = canBusOff(hnd);
    Check("canBusOff", statr);
    //закрываем канал
    statr = canClose(hnd);
    Check("canClose", statr);
}
