// sepia-test-win.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "sepia-test.h"
#pragma warning(disable:4996)

int main()
{
	ConsoleDialog();
}

void ConsoleDialog(void)
{
	bool exitFlag = false;
	bool isUseFstream = true;
	bool isSingleThread = true;
	int multithreadCount = 0;
	string command;
	string address;

	cout << "Hello! This is the test sepia filter programm." << endl <<
		"I can use Fstream or Stdio open/save functions." << endl <<
		"Also I can use multithread processing. (wow)" << endl <<
		"List of Commands:" << endl <<
		"FSTREAM ---------------- fstream file open method         (default)" << endl <<
		"STDIO ------------------ stdio file open method" << endl <<
		"SINGLE ----------------- single thread simple processing  (default)" << endl <<
		"MULTI ------------------ multithread processing" << endl <<
		"OPEN ------------------- open file" << endl <<
		"EXIT ------------------- exit programm" << endl;


	while (!exitFlag)
	{
		cin >> command;
		if (command == "EXIT")
			exitFlag = true;
		else if (command == "OPEN")
		{
			cout << "Write the full adress of bmp file" << endl;
			cin >> address;

			Process(address, isUseFstream, isSingleThread, multithreadCount);
		}
		else if (command == "FSTREAM")
			isUseFstream = true;
		else if (command == "STDIO")
			isUseFstream = false;
		else if (command == "SINGLE")
			isSingleThread = true;
		else if (command == "MULTI")
		{
			string temp;
			cout << "Please write the count of threads 1<N<8" << endl;
			cin >> temp;
			multithreadCount = stoi(temp);
			if (multithreadCount > 1 && multithreadCount <= 8)
				isSingleThread = false;
			else
				cout << "Not valid number" << endl;
		}
		else // undefined command
			cout << "Sorry, just don't understand your command" << endl;
	}
}

clock_t startTime, stopTime;
clock_t processStartTime, processStopTime;
void Process(string address, bool isUseFstream , bool isSingleThread, int multithreadCount)
{
	if (CheckValidAddress(address))
	{
		cout << "Let's try to open your file" << endl;
		if (ReadFile(address, isUseFstream))
		{
			startTime = clock();
			cout << "Open and Read Ok!" << endl
				<< "Now process" << endl;

			processStartTime = clock();
			if (isSingleThread)
				ConvertBitmapToSepia();
			else
				ConvertBitmapToSepiaMultithread(multithreadCount);
			processStopTime = clock();
			cout << "Process Ok!" << endl
				<< "Now Save" << endl;

			if (!SaveFile(address, isUseFstream))
				cout << "Can't save file" << endl;
			stopTime = clock();
			WriteInfo(address, isUseFstream, startTime, stopTime, processStartTime, processStopTime);
			cout << "Write OPEN for one more file, or EXIT" << endl;
		}
		else
			cout << "Sorry can't read your file :(" << endl;
	}
	else
		cout << "Not valid file address" << endl;
}

#pragma region READ_REGION

/*Проверить подходит ли адрес файла*/
bool CheckValidAddress(string address)
{
	return (address.find(".bmp") != string::npos);
}


/*Прочитать файл по адресу*/
bool ReadFile(string address, bool isUseFstream)
{
	if (isUseFstream)
		return BmpReadFstream(address);
	else
		return BmpReadStdio(address);
}

BmpHeader bmpHeader;
BmpData dataOriginal;
/*Прочитать файл по адресу используя fstream*/
bool BmpReadFstream(string address)
{
	fstream bmpFile;
	bmpFile.open(address, ios::in | ios::binary);
	if (bmpFile)
	{
		//read header
		bmpFile.read(bmpHeader.header, HEADER_SIZE);
		BmpHeaderFill(&bmpHeader);
		if (CheckBmpHeader(&bmpHeader))
		{
			//dynamic array
			dataOriginal.r = new unsigned char[bmpHeader.size];
			dataOriginal.g = new unsigned char[bmpHeader.size];
			dataOriginal.b = new unsigned char[bmpHeader.size];
			char* data = new char[bmpHeader.size*3];
			//read data
			bmpFile.read(data,bmpHeader.size*3);
			bmpFile.close();
			//parsing data
			for (size_t i = 0; i < bmpHeader.size; i++)
			{
				dataOriginal.b[i] = data[i*3];
				dataOriginal.g[i] = data[i*3+1];
				dataOriginal.r[i] = data[i*3+2];
			}
			free(data);
			return true;
		}
		else
		{
			bmpFile.close();
			cout << "Not valid BMP header!" << endl;
			return false;
		}
	}
	else
		return false;
}

/*Прочитать файл по адресу используя стандартную библиотеку*/
bool BmpReadStdio(string address)
{
	FILE* bmpFile = fopen(address.c_str(), "r");

	if (bmpFile != NULL)
	{
		//read header
		for (size_t i = 0; i < HEADER_SIZE; i++)
			bmpHeader.header[i] = getc(bmpFile);
		BmpHeaderFill(&bmpHeader);
		if (CheckBmpHeader(&bmpHeader))
		{
			//dynamic array
			dataOriginal.r = new unsigned char[bmpHeader.size];
			dataOriginal.g = new unsigned char[bmpHeader.size];
			dataOriginal.b = new unsigned char[bmpHeader.size];
			//read data
			char* data = new char[bmpHeader.size * 3];
			fread(data, 1, bmpHeader.size * 3, bmpFile);
			fclose(bmpFile);
			//parsing data
			for (size_t i = 0; i < bmpHeader.size; i++)
			{
				dataOriginal.b[i] = data[i * 3];
				dataOriginal.g[i] = data[i * 3 + 1];
				dataOriginal.r[i] = data[i * 3 + 2];
			}
			free(data);
			return true;
		}
		else
		{
			fclose(bmpFile);
			cout << "Not valid BMP header!" << endl;
			return false;
		}
	}
	else
		return false;
}

/*Заполнить значения в структуре заголовка*/
void BmpHeaderFill(BmpHeader* bmpHeader)
{
	bmpHeader->height = *(int*)&bmpHeader->header[HEIGHT_ADDR];
	bmpHeader->width = *(int*)&bmpHeader->header[WIDTH_ADDR];
	bmpHeader->type = *(short*)&bmpHeader->header[TYPE_ADDR];
	bmpHeader->size = bmpHeader->height * bmpHeader->width;
}

/*Проверить, что это точно заголовок битмап файла*/
bool CheckBmpHeader(BmpHeader *bmpHeader)
{
	return (bmpHeader->type == TYPE_STD);
}
#pragma endregion

#pragma region PROCESSING_REGION

BmpData dataProcessed;
const double coefficient[3][3] = { {0.393, 0.769, 0.189},
								 {0.349, 0.686, 0.168},
								 {0.272, 0.534, 0.131} };
/*Преобразовать в сепию с помощью тонирования*/
void ConvertBitmapToSepia()
{
	dataProcessed.r = new unsigned char[bmpHeader.size];
	dataProcessed.g = new unsigned char[bmpHeader.size];
	dataProcessed.b = new unsigned char[bmpHeader.size];

	int temp;
	for (size_t i = 0; i < bmpHeader.size; i++)
	{
		temp = ((dataOriginal.r[i] * coefficient[0][0]) + (dataOriginal.g[i] * coefficient[0][1]) + (dataOriginal.b[i] * coefficient[0][2]));
		dataProcessed.r[i] = (temp > 255) ? 255 : temp;

		temp = ((dataOriginal.r[i] * coefficient[1][0]) + (dataOriginal.g[i] * coefficient[1][1]) + (dataOriginal.b[i] * coefficient[1][2]));
		dataProcessed.g[i] = (temp > 255) ? 255 : temp;

		temp = ((dataOriginal.r[i] * coefficient[2][0]) + (dataOriginal.g[i] * coefficient[2][1]) + (dataOriginal.b[i] * coefficient[2][2]));
		dataProcessed.b[i] = (temp > 255) ? 255 : temp;
	}
}

void ConvertBitmapToSepiaMultithread(int threadCount)
{
	if (threadCount != 0 && threadCount < 10)
	{
		dataProcessed.r = new unsigned char[bmpHeader.size];
		dataProcessed.g = new unsigned char[bmpHeader.size];
		dataProcessed.b = new unsigned char[bmpHeader.size];

		thread* threads = new thread[threadCount];
		long size = bmpHeader.size / threadCount;
		//разбить массив на некоторое количество подмассивов
		
		for (size_t i = 0; i < threadCount; i++)
		{
			if (i != threadCount - 1)
				threads[i] = thread(threadprocess, size, size* i);
			else
				threads[i] = thread(threadprocess, bmpHeader.size-size*i, size * i);
		}
		for (size_t i = 0; i < threadCount; i++)
			threads[i].join();
	}
}

void threadprocess(long size, long offset)
{
	int temp;
	for (size_t i = 0; i < size; i++)
	{
		long j = i + offset;
		temp = ((dataOriginal.r[j] * coefficient[0][0]) + (dataOriginal.g[j] * coefficient[0][1]) + (dataOriginal.b[j] * coefficient[0][2]));
		dataProcessed.r[j] = (temp > 255) ? 255 : temp;

		temp = ((dataOriginal.r[j] * coefficient[1][0]) + (dataOriginal.g[j] * coefficient[1][1]) + (dataOriginal.b[j] * coefficient[1][2]));
		dataProcessed.g[j] = (temp > 255) ? 255 : temp;

		temp = ((dataOriginal.r[j] * coefficient[2][0]) + (dataOriginal.g[j] * coefficient[2][1]) + (dataOriginal.b[j] * coefficient[2][2]));
		dataProcessed.b[j] = (temp > 255) ? 255 : temp;
	}
}

/*Вывести информацию о процессе*/
void WriteInfo(string address, bool isUseFstream, time_t startTime, time_t stopTime, time_t processStartTime, time_t processStopTime)
{
	cout << endl << "INFO" << endl;
	cout << "File name: " << address.substr(address.find_last_of("\\/") + 1, string::npos) << endl;
	cout << "Bitmap height: " << to_string(bmpHeader.height) << endl;
	cout << "Bitmap width: " << to_string(bmpHeader.width) << endl;
	cout << "Size : " << to_string(bmpHeader.size) << " pixels" << endl;
	cout << "Used " << (isUseFstream ? "fstream" : "stdio") << " file processing system" << endl;
	cout << "Processing time: " << to_string((processStopTime - processStartTime) / (double) CLOCKS_PER_SEC) << " sec" << endl;
	cout << "All time: " << to_string((stopTime - startTime) / (double) CLOCKS_PER_SEC) << " sec" << endl;
	cout << endl;
}
#pragma endregion

#pragma region SAVE_REGION

/*Сохранить обработанный файл*/
bool SaveFile(string address, bool isUseFstream)
{
	if (isUseFstream)
		return SaveFileFstream(address);
	else
		return SaveFileStdio(address);
}

/*Сохранить, используя поток*/
bool SaveFileFstream(string address)
{
	fstream bmpFileSave;
	address = address.insert(address.size() - 4, "_sepia");
	bmpFileSave.open(address, ios::out | ios::binary);
	if (bmpFileSave)
	{
		//make data
		char* data = new char[bmpHeader.size * 3];
		for (size_t i = 0; i < bmpHeader.size; i++)
		{
			data[i * 3] = dataProcessed.b[i];
			data[i * 3 + 1] = dataProcessed.g[i];
			data[i * 3 + 2] = dataProcessed.r[i];
		}
		//write header
		bmpFileSave.write(bmpHeader.header, HEADER_SIZE);
		//write data
		bmpFileSave.write(data, bmpHeader.size * 3);
		//close file
		bmpFileSave.close();
		free(data);
		return true;
	}
	else
		return false;
}

/*Сохранить, используя стандартную библиотеку*/
bool SaveFileStdio(string address)
{
	FILE* bmpFileSave = fopen(address.insert(address.size() - 4, "_sepia").c_str(), "r");

	if (bmpFileSave != NULL)
	{
		//make data
		char* data = new char[bmpHeader.size * 3];
		for (size_t i = 0; i < bmpHeader.size; i++)
		{
			data[i * 3] = dataProcessed.b[i];
			data[i * 3 + 1] = dataProcessed.g[i];
			data[i * 3 + 2] = dataProcessed.r[i];
		}
		//write header
		fwrite(bmpHeader.header, 1, HEADER_SIZE, bmpFileSave);
		//write data
		fwrite(data, 1, bmpHeader.size * 3, bmpFileSave);
		//close file
		fclose(bmpFileSave);
		free(data);
		return true;
	}
	else
		return false;
}
#pragma endregion