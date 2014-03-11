#include <stdio.h>

/*
 * MMD�̃��f���f�[�^(PMD)�`���@�߂��@(�܂Ƃ�) - �ʂ肷����̋L��
 * http://blog.goo.ne.jp/torisu_tetosuki/e/209ad341d3ece2b1b4df24abf619d6e4
 * ���Q�l�ɍ쐬
 */

void dumpString(FILE* fp,int length) {
	char buf[512]={};
	char* b;
	fread(buf,length,1,fp);
	for(b=buf;*b;b++) {
		if(*b=='\\')fputs("\\\\",stdout);
		else if(*b=='\n')fputs("\\n",stdout);
		else if(*b=='\r')fputs("\\r",stdout);
		else putchar(*b);
	}
	putchar('\n');
}

float dumpFloat(FILE* fp) {
	float buf;
	fread(&buf,4,1,fp);
	printf("%f\n",buf);
	return buf;
}

unsigned int dumpDword(FILE* fp) {
	unsigned int buf;
	fread(&buf,4,1,fp);
	printf("%u\n",buf);
	return buf;
}

unsigned int dumpWord(FILE* fp) {
	unsigned short buf;
	fread(&buf,2,1,fp);
	printf("%u\n",buf);
	return buf;
}

unsigned int dumpByte(FILE* fp) {
	unsigned char buf;
	fread(&buf,1,1,fp);
	printf("%u\n",buf);
	return buf;
}

int main(int argc,char* argv[]) {
	FILE* fp;
	unsigned int i,max;
	if(argc!=2) {
		fputs("Usage: pmddump <pmd file path>\n",stderr);
		return 1;
	}
	if((fp=fopen(argv[1],"rb"))==NULL) {
		fputs("file open error\n",stderr);
		return 1;
	}
	/* �w�b�_ */
	dumpString(fp,3);
	dumpFloat(fp);
	dumpString(fp,20);
	dumpString(fp,256);
	/* ���_���X�g */
	max=dumpDword(fp);
	for(i=0;i<max;i++) {
		dumpFloat(fp); /* pos */
		dumpFloat(fp);
		dumpFloat(fp);
		dumpFloat(fp); /* normal_vec */
		dumpFloat(fp);
		dumpFloat(fp);
		dumpFloat(fp); /* uv */
		dumpFloat(fp);
		dumpWord(fp);
		dumpWord(fp);
		dumpByte(fp);
		dumpByte(fp);
	}
	/* �ʒ��_���X�g */
	max=dumpDword(fp);
	for(i=0;i<max;i++)dumpWord(fp);
	/* �ގ����X�g */
	max=dumpDword(fp);
	for(i=0;i<max;i++) {
		dumpFloat(fp); /* diffuse_color */
		dumpFloat(fp);
		dumpFloat(fp);
		dumpFloat(fp); /* alpha */
		dumpFloat(fp); /* specularity */
		dumpFloat(fp); /* specular_color */
		dumpFloat(fp);
		dumpFloat(fp);
		dumpFloat(fp); /* mirror_color */
		dumpFloat(fp);
		dumpFloat(fp);
		dumpByte(fp);
		dumpByte(fp);
		dumpDword(fp);
		dumpString(fp,20);
	}
	/* �{�[�����X�g�ȍ~�͏ȗ� */

	fclose(fp);
	return 0;
}
