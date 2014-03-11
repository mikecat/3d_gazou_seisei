#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include "script_loader.h"
#include "get_color.h"

static int barMax;
static int nowBar;

/*
 * �i����������������
 * @param max �i���������l�̍ő�l
 */
static void initProgress(int max) {
	barMax=max;
	nowBar=0;
	puts("--------20--------40--------60--------80-------100 [%]");
}

/* �i�������X�V����
 * @param now ���݂̐i���������l
 */
static void updateProgress(int now) {
#if 0
	/* �I�[�o�[�t���[�΍��� */
	int nowPos=(int)((long long)now*50/barMax);
#else
	/* �ʏ�� */
	int nowPos=now*50/barMax;
#endif
	if(nowPos>50)nowPos=50;
	#ifdef _OPENMP
	#pragma omp flush(nowBar)
	#endif
	if(nowBar<nowPos) {
		#ifdef _OPENMP
		#pragma omp critical
		#endif
		{
			for(;nowBar<nowPos;nowBar++)putchar('#');
			fflush(stdout);
		}
	}
}

int main(int argc,char* argv[]) {
	int width=512;
	int height=512;
	int max;
	int x,y;
	int nowProgress=0;
	FILE* fp=NULL;
	png_structp pngPtr;
	png_infop pngInfoPtr;
	png_bytepp pngImageData;
	/* �R�}���h���C�������̎擾�ƃX�N���v�g�̓ǂݍ��� */
	if(argc<3) {
		puts("Usage: 3d_gazou_seisei <script file> <output file> [width [height [params...]]");
		return 1;
	}
	if(argc>=4)sscanf(argv[3],"%d",&width);
	if(argc>=5)sscanf(argv[4],"%d",&height);
	if(width<=0 || height<=0) {
		puts("invalid size");
		return 1;
	}
	if(!loadScript(argv[1],argc>5?argc-5:0,argv+5))return 1;

	/* PNG�f�[�^���������ޏ��� */
	pngPtr=png_create_write_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
	if(!pngPtr) {
		puts("png_create_write_struct failed");
		return 1;
	}
	pngInfoPtr=png_create_info_struct(pngPtr);
	if(!pngInfoPtr) {
		puts("png_create_info_struct failed");
		png_destroy_write_struct(&pngPtr,NULL);
		return 1;
	}
	fp=fopen(argv[2],"wb");
	if(setjmp(png_jmpbuf(pngPtr))) {
		png_destroy_write_struct(&pngPtr,&pngInfoPtr);
		fclose(fp);
		return 1;
	}
	png_init_io(pngPtr,fp);
	png_set_IHDR(pngPtr,pngInfoPtr,width,height,8,PNG_COLOR_TYPE_RGB,
		PNG_INTERLACE_NONE,PNG_COMPRESSION_TYPE_BASE,PNG_FILTER_TYPE_BASE);

	/* �摜���i�[���郁�����̊m�� */
	puts("generating image data...");
	pngImageData=png_malloc(pngPtr,height*sizeof(png_bytep));
	for(y=0;y<height;y++)pngImageData[y]=png_malloc(pngPtr,width*3);

	/* �摜�����̌v�Z�{�� */
	initProgress(height);
	max=(width>=height?width:height);
	#ifdef _OPENMP
	#pragma omp parallel for schedule(dynamic) private(x)
	#endif
	for(y=0;y<height;y++) {
		for(x=0;x<width;x++) {
			double xoff=(double)(x-width/2)/(max/2);
			double yoff=(double)(height/2-y)/(max/2);
			double vec[3]={
				theWorld.camera.dir[0]+theWorld.camera.dx[0]*xoff+theWorld.camera.dy[0]*yoff,
				theWorld.camera.dir[1]+theWorld.camera.dx[1]*xoff+theWorld.camera.dy[1]*yoff,
				theWorld.camera.dir[2]+theWorld.camera.dx[2]*xoff+theWorld.camera.dy[2]*yoff
			};
			color_t color=getColorOfOnePoint(theWorld.camera.pos,vec,theWorld.ttl);
			pngImageData[y][x*3+0]=(int)(color.r*255.0+EPS);
			pngImageData[y][x*3+1]=(int)(color.g*255.0+EPS);
			pngImageData[y][x*3+2]=(int)(color.b*255.0+EPS);
		}
		#ifdef _OPENMP
		#pragma omp flush(nowProgress)
		#pragma omp atomic
		#endif
		nowProgress++;
		#ifdef _OPENMP
		#pragma omp flush(nowProgress)
		#endif
		updateProgress(nowProgress);
	}
	putchar('\n');
	png_set_rows(pngPtr,pngInfoPtr,pngImageData);

	/* �摜�̃t�@�C���ւ̏������� */
	png_write_png(pngPtr,pngInfoPtr,PNG_TRANSFORM_IDENTITY,NULL);
	for(y=0;y<height;y++)png_free(pngPtr,pngImageData[y]);
	png_free(pngPtr,pngImageData);
	png_destroy_write_struct(&pngPtr,&pngInfoPtr);
	fclose(fp);
	puts("done");

	/* �m�ۂ����e�N�X�`���̊J�� */
	freeTheWorld();
	return 0;
}
