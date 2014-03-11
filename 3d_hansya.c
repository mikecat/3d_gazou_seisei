#include <stdio.h>
#include <math.h>

/*
 * (s[0],s[1])��(d[0],d[1])�Ɍ��_�𒆐S�Ƃ����]�^���ňڂ��s������߂�
 * @param outmat �o�͂������o���s��
 * @param s �ڂ���錳�̓_
 * @param d �ڂ���̓_
 */
static void getRotateMatrix(double outmat[2][2],const double s[2],const double d[2]) {
	double a,b,delta;
	delta=s[0]*s[0]+s[1]*s[1];
	if(delta==0) {
		/* error */
		a=1;
		b=0;
	} else {
		a=(s[0]*d[0]+s[1]*d[1])/delta;
		b=(-s[1]*d[0]+s[0]*d[1])/delta;
	}
	outmat[0][0]=a;outmat[0][1]=-b;
	outmat[1][0]=b;outmat[1][1]=a;
}

/*
 * mat*in�����߁Aout�Ɋi�[����
 * @param out ��Z���ʂ̓_�̏o�͐�
 * @param mat �s��
 * @param in ��Z����_
 */
static void mulMatrix(double out[2],double mat[2][2],const double in[2]) {
	double outbuf[2];
	outbuf[0]=mat[0][0]*in[0]+mat[0][1]*in[1];
	outbuf[1]=mat[1][0]*in[0]+mat[1][1]*in[1];
	out[0]=outbuf[0];
	out[1]=outbuf[1];
}

/*
 * in���@��n�̕��ʂŋ��ʔ��˂����x�N�g�������߂�
 * @param out �o��
 * @param in ���͂̔��˂���x�N�g��
 * @param n ���̖͂@���x�N�g��
 */
void getVectorMillor(double out[3],const double in[3],const double n[3]) {
	double mat1[2][2]; /* (x,y,z) -> (x,sqrt(y*y+z*z),0) */
	double mat2[2][2]; /* (x,sqrt(y*y+z*z),0) -> (sqrt(x*x+y*y+z*z),0,0) */
	double imat1[2][2]; /* (x,sqrt(y*y+z*z),0) -> (x,y,z) */
	double imat2[2][2]; /* (sqrt(x*x+y*y+z*z),0,0) -> (x,sqrt(y*y+z*z),0) */
	double n_tvec1_s[2]={n[1],n[2]};
	double n_tvec1_d[2]={sqrt(n[1]*n[1]+n[2]*n[2]),0};
	double n_tvec2_s[2]={n[0],n_tvec1_d[0]};
	double n_tvec2_d[2]={sqrt(n[0]*n[0]+n[1]*n[1]+n[2]*n[2]),0};
	double in_tvec1_s[2]={in[1],in[2]};
	double in_tvec1_d[2];
	double in_tvec2_s[2];
	double in_tvec2_d[2];
	double out_tvec1_s[2];
	double out_tvec1_d[2];
	double out_tvec2_s[2];
	double out_tvec2_d[2];
	/* n��x���ɕ��s�ɂȂ�悤�ɉ�] */
	getRotateMatrix(mat1,n_tvec1_s,n_tvec1_d);
	getRotateMatrix(imat1,n_tvec1_d,n_tvec1_s);
	getRotateMatrix(mat2,n_tvec2_s,n_tvec2_d);
	getRotateMatrix(imat2,n_tvec2_d,n_tvec2_s);
	mulMatrix(in_tvec1_d,mat1,in_tvec1_s);
	in_tvec2_s[0]=in[0];in_tvec2_s[1]=in_tvec1_d[0];
	mulMatrix(in_tvec2_d,mat2,in_tvec2_s);
	/* �Ώۂ̃x�N�g����x�����𔽓](-1�{) */
	out_tvec2_s[0]=-in_tvec2_d[0];
	out_tvec2_s[1]=in_tvec2_d[1];
	/* �t��] */
	mulMatrix(out_tvec2_d,imat2,out_tvec2_s);
	out_tvec1_s[0]=out_tvec2_d[1];
	out_tvec1_s[1]=in_tvec1_d[1];
	mulMatrix(out_tvec1_d,imat1,out_tvec1_s);
	/* ���ʂ������o�� */
	out[0]=out_tvec2_d[0];
	out[1]=out_tvec1_d[0];
	out[2]=out_tvec1_d[1];
}

#ifndef BUILD_AS_COMPONENT
int main(void) {
	double in[3];
	double n[3];
	double nn[3];
	double out[3];
	fputs("input format is \"in_x in_y in_z n_x n_y n_z\"\n",stderr);
	while(scanf("%lg%lg%lg%lg%lg%lg",&in[0],&in[1],&in[2],&n[0],&n[1],&n[2])==6) {
		vectorNormalize(nn,n);
		getVectorMillor(out,in,nn);
		printf("%g %g %g\n",out[0],out[1],out[2]);
	}
	return 0;
}
#endif
