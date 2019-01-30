#include "stdio.h"  
#include "Windows.h"  
#include <iostream>  
using namespace std;

unsigned char *readBmp(char *bmpName, int *width, int *height, int *byteCount); //����ͼ��  
bool saveBmp(char *bmpName, unsigned char *imgBuf, int width, int height, int byteCount); //����ͼ��  
void GaussianFilt(int width, int height, int byteCount, int Gaussian[][5], unsigned char *gray_imgbuf, unsigned char *guassian_imgbuf); //��˹�˲�  

void main()
{
	//��ʱ����  
	long start, end;
	long time = 0;
	start = GetTickCount();

	unsigned char *src_imgbuf; //ͼ��ָ��  
	int width, height, byteCount;
	char rootPath1[] = "C:\\Users\\LQW\\Documents\\Visual Studio 2015\\Projects\\HelloWorld\\HelloWorld\\TestInput\\";
	char readPath[1024];
	int frame = 2000;  //����ͼ����  
	for (int i = 1; i <= frame; i++)
	{
		sprintf(readPath, "%s%d.bmp", rootPath1, i);
		src_imgbuf = readBmp(readPath, &width, &height, &byteCount);
		//printf("��=%d����=%d���ֽ�=%d\n",width, height, byteCount);  

		//�����˹ģ��ģ��  
		int Gaussian_mask[5][5] = { { 1,4,7,4,1 },{ 4,16,26,16,4 },{ 7,26,41,26,7 },{ 4,16,26,16,4 },{ 1,4,7,4,1 } };//�ܺ�Ϊ273  

																													 //���ͼ���ڴ����    
		unsigned char *guassian_imgbuf = new unsigned char[width*height*byteCount];

		//��ԭͼ��˹ģ��  
		GaussianFilt(width, height, byteCount, Gaussian_mask, src_imgbuf, guassian_imgbuf);

		char rootPath2[] = "C:\\Users\\LQW\\Documents\\Visual Studio 2015\\Projects\\HelloWorld\\HelloWorld\\TestOutput\\";
		char writePath[1024];
		sprintf(writePath, "%s%d.bmp", rootPath2, i);

		saveBmp(writePath, guassian_imgbuf, width, height, byteCount);

		cout << i << "  " << ((float)i / frame) * 100 << "%" << endl;
		delete[]src_imgbuf;
		delete[]guassian_imgbuf;
	}
	end = GetTickCount();
	InterlockedExchangeAdd(&time, end - start);
	cout << "Total time CPU:";
	cout << time << endl;
	int x;
	cin >> x;
}

void GaussianFilt(int width, int height, int byteCount, int Gaussian[][5], unsigned char *src_imgbuf, unsigned char *guassian_imgbuf)
{
	//��˹ģ������ 5��ѭ������  
	for (int i = 0; i<height; i++)
	{
		for (int j = 0; j<width; j++)
		{
			for (int k = 0; k<byteCount; k++)
			{
				int sum = 0;//��ʱֵ  
				int tempPixelValue = 0;
				for (int m = -2; m <= 2; m++)
				{
					for (int n = -2; n <= 2; n++)
					{
						//�߽紦������Ԫ�ظ�ֵΪ��  
						if (i + m<0 || j + n<0 || i + m >= height || j + n >= width)
							tempPixelValue = 0;
						else
							tempPixelValue = *(src_imgbuf + (i + m)*width*byteCount + (j + n)*byteCount + k);
						//tempPixelValue=*(gray_imgbuf+(i+m)*width+(j+n)+k);      
						sum += tempPixelValue*Gaussian[m + 2][n + 2];
					}
				}
				//tempPixelValue=*(src_imgbuf+(i)*width*byteCount+(j)*byteCount+k);  
				if (sum / 273<0)
					*(guassian_imgbuf + i*width*byteCount + j*byteCount + k) = 0;
				else if (sum / 273>255)
					*(guassian_imgbuf + i*width*byteCount + j*byteCount + k) = 255;
				else
					*(guassian_imgbuf + i*width*byteCount + j*byteCount + k) = sum / 273;
			}
		}
	}
}

//����һ��ͼ���ļ�����·��������ͼ�����ݡ�   
unsigned char *readBmp(char *bmpName, int *width, int *height, int *byteCount)
{
	//���ļ���  
	FILE *fp = fopen(bmpName, "rb");
	if (fp == 0) return 0;
	//�����ļ�ͷ  
	fseek(fp, sizeof(BITMAPFILEHEADER), 0);

	//������Ϣͷ  
	int w, h, b;
	BITMAPINFOHEADER head;
	fread(&head, sizeof(BITMAPINFOHEADER), 1, fp);
	w = head.biWidth;
	h = head.biHeight;
	b = head.biBitCount / 8;
	int lineByte = (w * b + 3) / 4 * 4; //ÿ�е��ֽ���Ϊ4�ı���  

										//������ɫ�� ����ɫ��Ĵ�СΪ1024������ɫͼ��û����ɫ������Ҫ��һ����  
	if (b == 1)
		fseek(fp, 1024, 1);

	//ͼ������  
	unsigned char *imgBuf = new unsigned char[w * h * b];
	for (int i = 0; i<h; i++)
	{
		fread(imgBuf + i*w*b, w*b, 1, fp);
		fseek(fp, lineByte - w*b, 1);
	}
	fclose(fp);

	*width = w, *height = h, *byteCount = b;

	return imgBuf;
}


bool saveBmp(char *bmpName, unsigned char *imgBuf, int width, int height, int byteCount)
{
	if (!imgBuf)
		return 0;

	//�Ҷ�ͼ����ɫ��ռ�1024����ɫͼ��û����ɫ��  
	int palettesize = 0;
	if (byteCount == 1) palettesize = 1024;

	//һ�������ֽ���Ϊ4�ı���  
	int lineByte = (width * byteCount + 3) / 4 * 4;

	FILE *fp = fopen(bmpName, "wb");
	if (fp == 0) return 0;

	//��д�ļ�ͷ  
	BITMAPFILEHEADER fileHead;
	fileHead.bfType = 0x4D42;
	fileHead.bfSize =
		sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + palettesize + lineByte*height;
	fileHead.bfReserved1 = 0;
	fileHead.bfReserved2 = 0;
	fileHead.bfOffBits = 54 + palettesize;
	fwrite(&fileHead, sizeof(BITMAPFILEHEADER), 1, fp);

	// ��д��Ϣͷ  
	BITMAPINFOHEADER head;
	head.biBitCount = byteCount * 8;
	head.biClrImportant = 0;
	head.biClrUsed = 0;
	head.biCompression = 0;
	head.biHeight = height;
	head.biPlanes = 1;
	head.biSize = 40;
	head.biSizeImage = lineByte*height;
	head.biWidth = width;
	head.biXPelsPerMeter = 0;
	head.biYPelsPerMeter = 0;
	fwrite(&head, sizeof(BITMAPINFOHEADER), 1, fp);

	//��ɫ����    
	if (palettesize == 1024)
	{
		unsigned char palette[1024];
		for (int i = 0; i<256; i++)
		{
			*(palette + i * 4 + 0) = i;
			*(palette + i * 4 + 1) = i;
			*(palette + i * 4 + 2) = i;
			*(palette + i * 4 + 3) = 0;
		}
		fwrite(palette, 1024, 1, fp);
	}

	//׼�����ݲ�д�ļ�  
	unsigned char *buf = new unsigned char[height*lineByte];
	for (int i = 0; i<height; i++)
	{
		for (int j = 0; j<width*byteCount; j++)
			*(buf + i*lineByte + j) = *(imgBuf + i*width*byteCount + j);
	}
	fwrite(buf, height*lineByte, 1, fp);

	delete[]buf;

	fclose(fp);

	return 1;
}