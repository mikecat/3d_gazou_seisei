#include <stdio.h>
#include <math.h>

#ifndef EPS
#define EPS (1e-7)
#endif

/*
(pl[0]+t*dl[0]-pc[0]-s*dc[0])*(pl[0]+t*dl[0]-pc[0]-s*dc[0])+
	(pl[1]+t*dl[1]-pc[1]-s*dc[1])*(pl[1]+t*dl[1]-pc[1]-s*dc[1])+
	(pl[2]+t*dl[2]-pc[2]-s*dc[2])*(pl[2]+t*dl[2]-pc[2]-s*dc[2])=r*r

(pl[0]+t*dl[0]-pc[0]-s*dc[0])*dc[0]+
	(pl[1]+t*dl[1]-pc[1]-s*dc[1])*dc[1]+
	(pl[2]+t*dl[2]-pc[2]-s*dc[2])*dc[2]=0

������ł��ˁB�ق���wxMaxima�ɓ������܂��傤�B
*/

/*
 * �����Ɖ~���̌�_�����߂�B������pl+t*dl�A�~���̎���pc+s*dc�ŕ\�����B
 * @param ot ��_��t���o�͂���z��
 * @param os ��_��s���o�͂���z��
 * @param pl ���͂̒����̊�_(t==0)
 * @param dl ���͂̒����̕����x�N�g��
 * @param pc ���͂̉~���̎��̊�_(s==0)
 * @param dc ���͂̉~���̕����x�N�g��
 * @param r �~���̔��a
 * @return ��_�̐�
 */
int getLineCylinderHitPoint(
		double ot[2],double os[2],const double pl[3],const double dl[3],
		const double pc[3],const double dc[3],double r) {
	/* �ŏ��̎��̌W�� */
	double a_tt,a_ss,a_ts,a_t,a_s,a_const;
	/* ��Ԗڂ̎��̌W�� */
	double b_t,b_s,b_const;
	/* s=p*t+q */
	double p,q;
	/* t�����߂�񎟕������̌W�� */
	double e_tt,e_t,e_const;
	double e_D; /* ���ʎ� */

	/* a_tt*t*t+a_ts*t*s+a_ss*s*s+a_t*t+a_s*s+a_const=0 */
	a_tt=dl[0]*dl[0]+dl[1]*dl[1]+dl[2]*dl[2];
	a_ts=-2.0*(dc[0]*dl[0]+dc[1]*dl[1]+dc[2]*dl[2]);
	a_t=2.0*(dl[0]*pl[0]+dl[1]*pl[1]+dl[2]*pl[2]-dl[0]*pc[0]-dl[1]*pc[1]-dl[2]*pc[2]);
	a_ss=dc[0]*dc[0]+dc[1]*dc[1]+dc[2]*dc[2];
	a_s=2.0*(-dc[0]*pl[0]-dc[1]*pl[1]-dc[2]*pl[2]+dc[0]*pc[0]+dc[1]*pc[1]+dc[2]*pc[2]);
	a_const=pl[0]*pl[0]+pl[1]*pl[1]+pl[2]*pl[2]+pc[0]*pc[0]+pc[1]*pc[1]+pc[2]*pc[2]
		-2.0*(pc[0]*pl[0]+pc[1]*pl[1]+pc[2]*pl[2])-r*r;

	/* b_s*s=b_t*t+b_const */
	b_t=dc[0]*dl[0]+dc[1]*dl[1]+dc[2]*dl[2];
	b_s=dc[0]*dc[0]+dc[1]*dc[1]+dc[2]*dc[2];
	b_const=dc[0]*pl[0]+dc[1]*pl[1]+dc[2]*pl[2]-dc[0]*pc[0]-dc[1]*pc[1]-dc[2]*pc[2];

	if(-EPS<b_s && b_s<EPS)return 0;
	p=b_t/b_s;
	q=b_const/b_s;

	/* e_tt*t*t+e_t*t+e_const=0 */
	e_tt=a_ss*p*p+a_ts*p+a_tt;
	e_t=(2*a_ss*p+a_ts)*q+a_s*p+a_t;
	e_const=a_ss*q*q+a_s*q+a_const;

	/* �ꎟ������������ */
	if(-EPS<e_tt && e_tt<EPS) {
		if(-EPS<e_t && e_t<EPS) {
			/* ���������܂��͖������� */
			return 0;
		} else {
			ot[0]=-e_const/e_t;
			os[0]=p*ot[0]+q;
			return 1;
		}
	}

	/* �񎟕����������� */
	e_D=e_t*e_t-4.0*e_tt*e_const;
	if(e_D<=-EPS) {
		return 0;
	} else if(e_D<EPS) {
		ot[0]=-e_t/(2.0*e_tt);
		os[0]=p*ot[0]+q;
		return 1;
	} else {
		double D_sqrt=sqrt(e_D);
		ot[0]=(-e_t-D_sqrt)/(2.0*e_tt);
		os[0]=p*ot[0]+q;
		ot[1]=(-e_t+D_sqrt)/(2.0*e_tt);
		os[1]=p*ot[1]+q;
		return 2;
	}
}

#ifndef BUILD_AS_COMPONENT
int main(void) {
	double pl[3],dl[3],pc[3],dc[3],r;
	while(scanf("%lg%lg%lg%lg%lg%lg",&pl[0],&pl[1],&pl[2],&dl[0],&dl[1],&dl[2])==6) {
		double s[2],t[2];
		int ansnum;
		if(scanf("%lg%lg%lg%lg%lg%lg%lg",&pc[0],&pc[1],&pc[2],&dc[0],&dc[1],&dc[2],&r)!=7)break;
		ansnum=getLineCylinderHitPoint(t,s,pl,dl,pc,dc,r);
		if(ansnum==0) {
			puts("no answer");
		} else {
			int i;
			for(i=0;i<ansnum;i++) {
				printf("t=%lg s=%lg P=(%lg,%lg,%lg)\n",t[i],s[i],
					pl[0]+t[i]*dl[0],pl[1]+t[i]*dl[1],pl[2]+t[i]*dl[2]);
			}
		}
	}
	return 0;
}
#endif
