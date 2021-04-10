#pragma once


/******INCLUDES*****/
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <time.h>
#include <stdio.h>
#include <string>
#include <thread>

/******NAMESPACES*****/
using namespace std;

/******DEFINES*****/
#define HEADER_SIZE		54
#define HEIGHT_ADDR		18
#define WIDTH_ADDR		22
#define TYPE_ADDR		0
#define TYPE_STD		0x4d42

#define CHAR_MAX_SIZE	0xff


/******STRUCTURES*****/
struct BmpHeader
{
	char header[HEADER_SIZE];
	unsigned int height;
	unsigned int width;
	unsigned short type;
	unsigned long size;
};

struct BmpData
{
	unsigned char* r;
	unsigned char* g;
	unsigned char* b;
};

/******FUNCTION PROTOTYPES*****/
//Main functions
void ConsoleDialog(void);
void Process(string address, bool isUseFstream, bool isSingleThread, int multithreadCount);
//Read functions
bool CheckValidAddress(string address);
bool ReadFile(string address, bool isUseFstream);
bool BmpReadFstream(string address);
bool BmpReadStdio(string address);
void BmpHeaderFill(BmpHeader* bmpHeader);
bool CheckBmpHeader(BmpHeader* bmpHeader);
//Processing functions
void ConvertBitmapToSepia();
void WriteInfo(string address, bool isUseFstream, time_t startTime, time_t stopTime, time_t processStartTime, time_t processStopTime);
void ConvertBitmapToSepiaMultithread(int threadCount);
void threadprocess(long size, long offset);
//Save functions
bool SaveFile(string address, bool isUseFstream);
bool SaveFileFstream(string address);
bool SaveFileStdio(string address);

