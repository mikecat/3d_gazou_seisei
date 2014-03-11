#include <stdio.h>

#ifndef EPS
#define EPS (1e-7)
#endif

/*
 * 三元連立方程式を解く
 * @param out 結果を出力する配列
 * @param keisu 入力の係数行列
 * @param kitiVector 入力の既知ベクトル
 * @return 解けるなら真、解けないなら偽
 */
static int solve3genRenrituHotesiki(double out[3],double keisu[3][3],const double kitiVector[3]) {
	const int p[6][3]={
		{0,1,2},{0,2,1},{1,0,2},{1,2,0},{2,0,1},{2,1,0}
	};
	double a,b,c,d,e,f,g,h,i,j,k,l;
	double temp;
	int gotit=0;
	int s;
	for(s=0;s<6;s++) {
		a=keisu[p[s][0]][0];b=keisu[p[s][0]][1];c=keisu[p[s][0]][2];d=kitiVector[p[s][0]];
		e=keisu[p[s][1]][0];f=keisu[p[s][1]][1];g=keisu[p[s][1]][2];h=kitiVector[p[s][1]];
		i=keisu[p[s][2]][0];j=keisu[p[s][2]][1];k=keisu[p[s][2]][2];l=kitiVector[p[s][2]];
		/* 答えを代入する */
		temp=(a*f-b*e)*(g*j-f*k)-(c*f-b*g)*(e*j-f*i);
		if(temp<-EPS || EPS<temp) {
			gotit|=1;
			out[0]=((d*f-b*h)*(g*j-f*k)-(c*f-b*g)*(h*j-f*l))/
				((a*f-b*e)*(g*j-f*k)-(c*f-b*g)*(e*j-f*i));
		}
		temp=(b*e-a*f)*(g*i-e*k)-(c*e-a*g)*(f*i-e*j);
		if(temp<-EPS || EPS<temp) {
			gotit|=2;
			out[1]=((d*e-a*h)*(g*i-e*k)-(c*e-a*g)*(h*i-e*l))/
				((b*e-a*f)*(g*i-e*k)-(c*e-a*g)*(f*i-e*j));
			out[2]=((d*e-a*h)*(f*i-e*j)-(b*e-a*f)*(h*i-e*l))/
				((c*e-a*g)*(f*i-e*j)-(b*e-a*f)*(g*i-e*k));
		}
		if(gotit==3)return 1;
	}
	return 0;
}

/*
 * 直線と平面の交点を求め、ips+u*ipd1+v*ipd2==ils+t*ildの形で表現する
 * @param ot 交点の{t,u,v}を出力する配列
 * @param ils 入力の直線の基準点(t==0)
 * @param ild 入力の直線の方向ベクトル
 * @param ips 入力の平面の基準点(u==0 && v==0)
 * @param ipd1 入力の平面の方向ベクトル1(u)
 * @param ipd2 入力の平面の方向ベクトル2(v)
 * @return 交点が存在するなら真、存在しないなら偽
 */
int getLinePlaneHitPoint(double ot[3],const double ils[3],const double ild[3],
		const double ips[3],const double ipd1[3],const double ipd2[3]) {
	double keisu[3][3]={
		{-ild[0],ipd1[0],ipd2[0]},
		{-ild[1],ipd1[1],ipd2[1]},
		{-ild[2],ipd1[2],ipd2[2]}
	};
	double vec[3]={ils[0]-ips[0],ils[1]-ips[1],ils[2]-ips[2]};
	return solve3genRenrituHotesiki(ot,keisu,vec);
}

#ifndef BUILD_AS_COMPONENT
int main(void) {
#if 0
#define TEST_3GEN_RENRITU_HOUTEISIKI
#endif
#ifdef TEST_3GEN_RENRITU_HOUTEISIKI
	double k[3][3];
	double v[3];
	while(scanf("%lg%lg%lg%lg",&k[0][0],&k[0][1],&k[0][2],&v[0])==4) {
		double a[3];
		if(scanf("%lg%lg%lg%lg",&k[1][0],&k[1][1],&k[1][2],&v[1])!=4)break;
		if(scanf("%lg%lg%lg%lg",&k[2][0],&k[2][1],&k[2][2],&v[2])!=4)break;
		if(solve3genRenrituHotesiki(a,k,v)) {
			printf("%g\n%g\n%g\n",a[0],a[1],a[2]);
			printf("errors: %g %g %g\n",
				k[0][0]*a[0]+k[0][1]*a[1]+k[0][2]*a[2]-v[0],
				k[1][0]*a[0]+k[1][1]*a[1]+k[1][2]*a[2]-v[1],
				k[2][0]*a[0]+k[2][1]*a[1]+k[2][2]*a[2]-v[2]
			);
		} else {
			puts("no/imcomplete(ziyuudo>0) answer");
		}
	}
#else
	double ls[3],ld[3];
	double ps[3],pd1[3],pd2[3];
	while(scanf("%lg%lg%lg%lg%lg%lg",&ls[0],&ls[1],&ls[2],&ld[0],&ld[1],&ld[2])==6) {
		double tuv[3];
		if(scanf("%lg%lg%lg%lg%lg%lg%lg%lg%lg",
			&ps[0],&ps[1],&ps[2],&pd1[0],&pd1[1],&pd1[2],&pd2[0],&pd2[1],&pd2[2])!=9)break;
		if(getLinePlaneHitPoint(tuv,ls,ld,ps,pd1,pd2)) {
			printf("%t=%g, u=%g, v=%g\n",tuv[0],tuv[1],tuv[2]);
			printf("point = (%g,%g,%g)\n",
				ls[0]+tuv[0]*ld[0],ls[1]+tuv[0]*ld[1],ls[2]+tuv[0]*ld[2]);
		} else {
			puts("no answer ( maziwaranai or heimenzyou )");
		}
	}
#endif
	return 0;
}
#endif
