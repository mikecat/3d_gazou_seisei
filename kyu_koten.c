#include <stdio.h>
#include <math.h>

#ifndef EPS
#define EPS (1e-7)
#endif

/*
 * 直線P=ins+t*indと球面の交点を求める
 * @param ot 結果のtを格納する配列
 * @param ins 入力直線の基準となる点
 * @param ind 入力直線の方向ベクトル
 * @param incc 入力球の中心
 * @param incr 入力球の半径
 * @return 交点の個数
 */
int getLineSphereHitPoint(double ot[2],
		const double ins[3],const double ind[3],const double incc[3],double incr) {
	double xt,yt,zt;
	double a,b,c;
	double d;
	double s;
	xt=ins[0]-incc[0];
	yt=ins[1]-incc[1];
	zt=ins[2]-incc[2];
	a=ind[0]*ind[0]+ind[1]*ind[1]+ind[2]*ind[2];
	b=2.0*(ind[0]*xt+ind[1]*yt+ind[2]*zt);
	c=xt*xt+yt*yt+zt*zt-incr*incr;
	d=b*b-4.0*a*c;
	if(d<0)return 0;
	else if(-EPS<d && d<EPS) {
		ot[0]=-b/(2.0*a);
		return 1;
	}
	s=sqrt(d);
	/* a>0なので、ot[0]<ot[1] */
	ot[0]=(-b-s)/(2.0*a);
	ot[1]=(-b+s)/(2.0*a);
	return 2;
}

#ifndef BUILD_AS_COMPONENT
int main(void) {
	double sp[3],d[3],cc[3],cr;
	double ot[2];
	int otnum;
	while(scanf("%lg%lg%lg%lg%lg%lg%lg%lg%lg%lg",
			&sp[0],&sp[1],&sp[2],&d[0],&d[1],&d[2],&cc[0],&cc[1],&cc[2],
			&cr)==10) {
		otnum=getLineSphereHitPoint(ot,sp,d,cc,cr);
		if(otnum==0) {
			puts("not found");
		} else {
			printf("t=%lg (x,y,z)=(%lg,%lg,%lg)\n",ot[0],
				sp[0]+ot[0]*d[0],sp[1]+ot[0]*d[1],sp[2]+ot[0]*d[2]);
			if(otnum==2) {
				printf("t=%lg (x,y,z)=(%lg,%lg,%lg)\n",ot[1],
					sp[0]+ot[1]*d[0],sp[1]+ot[1]*d[1],sp[2]+ot[1]*d[2]);
			}
		}
	}
	return 0;
}
#endif
