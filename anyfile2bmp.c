/*
2019.11.02	JongHeon Baek
*/
#include <Windows.h>
#include <wingdi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
typedef struct tagbmpfile {
	BITMAPFILEHEADER fhead;
	BITMAPINFOHEADER ihead;
	RGBTRIPLE* data;
}BMPFILE;

void killProc(const char* err) { puts(err); exit(1); }
int _IsFileExist(const char*);

/*
argv[0]: name of program
argv[1]: option
argv[2]: name of srcfile
argv[3]: name of desfile
argv[4]: keyword(for encryption)
*/

int _init_BUFFER(const char*, const int);
void _encrypt_data(long, const unsigned char*, int);
void _decipher_data(long, const unsigned char*, int);
int _IsFileExt(const char*, const char*, int);
void Save_bmp(BMPFILE*, long, const char*);
BMPFILE* Create_bmp(int, int);

unsigned char* buffer;
unsigned char* processed;

int main(int argc, char* argv[]) {
	int _type=0;
	{
		// Understand the argv
		if (argc <= 1) {
			puts("Usage: anyfile2bmp [OPTION] [SRCFNAME] [DESFNAME] [KEYWORD]");
			return 0;
		}
		//show help message
		else if (!strncmp(argv[1], "--help", 6) || !strncmp(argv[1], "-h", 2)) {
			puts("CONVERT FILE TO bmp OR bmp TO ANOTHER FORMAT");
			puts("\nUsage: anyfile2bmp [OPTION] [SRCFNAME] [DESFNAME] [KEYWORD]\n");
			puts("Options:");
			puts("	-h, --help		Print this message and exit");
			puts("	--version		Print program version and exit");
			puts("	-e, --encrypt		Convert file to .bmp format");
			puts("	-d, --decipher		Convert bmp to other format\n");
			return 0;
		}
		//show version
		else if (!strncmp(argv[1], "--version", 10)) {
			puts("created by JongHeon Baek, 2019.11.02");
			return 0;
		}
		//show usage(missing fname or keyword)
		else if (argc < 5) { puts("Usage: anyfile2bmp [OPTION] [SRCFNAME] [DESFNAME] [KEYWORD]"); }
		//encryption: _type=1
		else if (!strncmp(argv[1], "-e", 2) || !strncmp(argv[1], "--encrypt", 9)) {
			_type = 1;
			if (!_IsFileExt(argv[3], "bmp", 3)) {
				fputs("\nError: ", stdout); fputs(argv[3], stdout); puts(" is not .bmp type");
				return 1;
			}
		}
		//decipher: _type=2
		else if (!strncmp(argv[1], "-d", 2) || !strncmp(argv[1], "--decipher", 10)) {
			_type = 2;
			if (!_IsFileExt(argv[2], "bmp", 3)) {
				fputs("\nError: ", stdout); fputs(argv[2], stdout); puts(" is not .bmp type");
				return 1;
			}
		}
		//if user used another option
		else {
			fputs("Option error: the option ", stdout); fputs(argv[1], stdout); puts(" is not valid");
			puts("\nUsage: anyfile2bmp [OPTION] [SRCFNAME] [DESFNAME] [KEYWORD]\n");
			puts("Use \"anyfile2bmp --help\" to see possible options");
			return 0;
		}

		//Program Starts
		//argv[2] to argv[3] with argv[4] 
		if (_type == 1) { fputs("Excrypt ", stdout); }
		else { fputs("Decipher ", stdout); }
		printf("%s to %s with key %s\n", argv[2], argv[3], argv[4]); Sleep(1000);
		//Check the filename valid
		fputs("[File] Searching file ", stdout); fputs(argv[2], stdout); putchar(':'); Sleep(1000);
		if (!_IsFileExist(argv[2])) {
			fputs(" FAIL", stdout);	Sleep(500);
			fputs("\n\nError: Cannot find file", stdout); puts(argv[2]);
			return 1;
		}
		fputs(" OK\n", stdout);
	}

	//encryption
	if (_type == 1) 
	{	
		//Read argv[2] and save in the buffer
		int size = _init_BUFFER(argv[2], _type);

		//Processing the data with keyword argv[4]
		fputs("[PROCESSING] Encrypting datas with ", stdout); fputs(argv[4], stdout); putchar(':');
		_encrypt_data(size, (unsigned char*)argv[4], strlen(argv[4])); Sleep(1000);
		fputs(" Success\n", stdout);
		
		//Create default bmp file
		int height, width;
		int flag=0;
		
		fputs("[SETUP] Determining size of image...", stdout);
		printf("	Program returned height: ");
		if (size/3+1 <= 400) { width = 20, height = 20; }
		else if (size/3+1 <= 600) { width = 30, height = 30; }
		else if (size/3+1 <= 784) { width = 28, height = 28; }
		else if (size/3+1 <= 76800) { width = 320, height = 240; }
		else if (size/3+1 <= 307200) { width = 640, height = 480; }
		else if (size/3+1 <= 480000) { width = 800, height = 600; }
		else if (size/3+1 <= 786432) { width = 1024, height = 768; }
		else if (size/3+1 <= 1228800) { width = 1280, height = 960; }
		else if (size/3+1 <= 2073600) { width = 1920, height = 1080; }
		else {
			height = 0, width = 0;
			puts("system can convert under 6.1MB."); flag = 1;
		}
		printf("%d, width: %d. \n", height, width);
		if (flag == 0) {
			printf("Size of new file: %dB. \n", 54 + height * width * sizeof(RGBTRIPLE));
			fputs("Do you want to custom the size? (Y/N)", stdout);
			do {
				switch (getchar()) {
				case 'Y':
				case 'y':
					flag = 1;
					break;
				case 'N':
				case 'n':
					flag = 0;
					break;
				default:
					flag = 2;
				}
			} while (flag == 2);
		}
		if (flag == 1) {
			fputs("[INPUT] intput height and width\n", stdout);
			while(flag--){
				fputs("	height:	", stdout); scanf_s("%d", &height);
				fputs("	width: ", stdout); scanf_s("%d", &width);
				if (height*width*3 < size) { flag++; puts("Alert: Image is to small to crypt whole data"); }
			}
		}
		
		fputs("[SETUP] Creating default bmp file: ", stdout);
		BMPFILE* bmp = Create_bmp(width, height); Sleep(500);
		fputs("Success\n", stdout);

		//Copy the data
		fputs("[SETUP] Copying datas to bmp: ", stdout);
		memset(bmp->data, 0, width*height * sizeof(RGBTRIPLE));
		memcpy_s(bmp->data, sizeof(RGBTRIPLE)*width*height, processed, sizeof(unsigned char)*size);
		Sleep(1000); fputs("Success\n", stdout);

		//save image as name argv[3]
		fputs("[SAVE] Saving bmp file... ", stdout);
		FILE* fp; if (fopen_s(&fp, argv[3], "rb") == 0) {
			int flag = 1;
			printf("\nSeems %s is already exist. Do You want to continue? (Y/N)", argv[3]);
			while (flag) {
				switch (getchar()) {
				case 'Y':
				case 'y':
					flag = 0;
					fclose(fp);
					break;
				case 'N':
				case 'n':
					puts("Process stopped");
					fclose(fp);
					return 0;
					break;
				default:
					break;
				}
			}
		}
		Save_bmp(bmp, height*width, argv[3]); Sleep(1000);
		fputs("[SAVE] Save completed\n", stdout);
		//Free all variables
		free(bmp->data);
		free(bmp);
	}
	//decipher
	else if (_type = 2) 
	{
		//Read argv[2] and save only datas in the buffer(without header)
		int size = _init_BUFFER(argv[2], _type);
		
		//Processing the data with keyword argv[4]
		fputs("[PROCESSING] Deciphering the raw data: ", stdout);
		_decipher_data(size, (unsigned char*)argv[4], strlen(argv[4])); Sleep(1000);
		fputs("Success\n", stdout);
		
		//Create argv[3] file
		FILE* fp; 
		if (fopen_s(&fp, argv[3], "rb") == 0) {
			int flag = 1;
			printf("Seems %s is already exist. Do You want to continue? (Y/N)", argv[3]);
			while(flag){
				switch (getchar()) {
				case 'Y':
				case 'y':
					flag = 0;
					fclose(fp);
					break;
				case 'N':
				case 'n':
					puts("Process stopped");
					fclose(fp);
					return 0;
					break;
				default:
					break;
				}
			}
		}
		fopen_s(&fp, argv[3], "wb");
		fputs("[SAVE] Saving data to ", stdout); fputs(argv[3], stdout); putchar(':');
		
		fwrite(processed, sizeof(unsigned char), size, fp);
		
		fputs(" Success\n", stdout);
		
		//Free all variables
		fclose(fp);
	}
	printf("\nThe result file has saved as %s\n", argv[3]);
	free(buffer);
	free(processed);
	Sleep(2000);
	return 0;
}

int _IsFileExist(const char* fname) {
	FILE* fp;
	fopen_s(&fp, fname, "r");
	if (fp == NULL) { return 0; }
	else { fclose(fp); return 1; }
}

int _IsFileExt(const char* fname, const char* ext, int len) {
	char* pt = strrchr(fname, '.');
	if (!strncmp(pt + 1, ext, len)) {	//if the extension right
		return 1;
	}
	else { return 0; }
}
int _init_BUFFER(const char* fname, const int type) {
	if (type == 1) {	//save all data
		FILE* fp; ;
		if (fopen_s(&fp, fname, "rb")) { killProc("File not found"); }
		fseek(fp, 0, SEEK_END);
		int sz = ftell(fp);

		buffer = (unsigned char*)malloc(sizeof(unsigned char)*(sz));

		printf("[READING] %s\n", fname);
		printf("	FILE SIZE: %dB	\n", sz);
		fseek(fp, 0, SEEK_SET);
		fread_s(buffer, sizeof(unsigned char)*sz, sizeof(unsigned char), sz, fp);
		fclose(fp);
		return sz;
	}
	else if (type == 2) {	//skip bmp header and save all
		FILE* fp; fopen_s(&fp, fname, "rb");
		fseek(fp, 0, SEEK_END);
		printf("[READING] %s\n", fname);
		printf("	FILE SIZE: %dB	\n", ftell(fp));
		int sz = ftell(fp)-54;
		buffer = (unsigned char*)malloc(sizeof(unsigned char)*(sz+1));

		fseek(fp, 54, SEEK_SET);
		fread_s(buffer, sizeof(unsigned char)*sz, sizeof(unsigned char), sz, fp);
		fclose(fp);

		return sz;
	}
	return 0;
}
void _encrypt_data(long size, const unsigned char* keyword, int len) {
	processed = (unsigned char*)malloc(sizeof(unsigned char)*size);
	for (int i = 0; i < size; i++) {
		processed[i] = (buffer[i] + keyword[i%len]) % 256;
	}
}
void _decipher_data(long size, const unsigned char* keyword, int len) {
	processed = (unsigned char*)malloc(sizeof(unsigned char)*size);
	for (int i = 0; i < size; i++) {
		int temp = buffer[i] - keyword[i%len];
		if (temp < 0) { temp += 256; }
		processed[i] = temp;
	}
}
void Save_bmp(BMPFILE* image, long imgsize, const char* fname) {
	FILE* fp; fopen_s(&fp, fname, "wb");
	fwrite(&image->fhead, sizeof(BITMAPFILEHEADER), 1, fp);
	fwrite(&image->ihead, sizeof(BITMAPINFOHEADER), 1, fp);
	fwrite(image->data, sizeof(RGBTRIPLE), imgsize, fp);
	fclose(fp);
	return;
}
BMPFILE* Create_bmp(int width, int height) 
{
	BMPFILE* res = (BMPFILE*)malloc(sizeof(BMPFILE));
	res->fhead.bfType = 0x4D42;
	res->fhead.bfSize = 0x36;
	res->fhead.bfReserved1 = 0;
	res->fhead.bfReserved2 = 0;
	res->fhead.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	
	res->ihead.biSize = sizeof(BITMAPINFOHEADER);
	res->ihead.biWidth = width;
	res->ihead.biHeight = height;
	res->ihead.biPlanes = 0;
	res->ihead.biBitCount = 24;
	res->ihead.biCompression = 0;
	res->ihead.biSizeImage = height*width*3;
	res->ihead.biXPelsPerMeter = 0;
	res->ihead.biYPelsPerMeter = 0;
	res->ihead.biClrUsed = 0;
	res->ihead.biClrImportant = 0;

	res->data = (RGBTRIPLE*)malloc(sizeof(RGBTRIPLE)*width*height);
	
	return res;
}