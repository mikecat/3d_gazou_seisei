#include <stdio.h>
#include <math.h>
#include "geo_calc.h"
#include "script_loader.h"

/*
 * 色を混ぜる
 * @param x 混ぜられる色1
 * @param y 混ぜられる色2
 * @param xRatio 混ぜられる色1の強さ。[0,1]で大きいほど結果がxに近くなる。
 * @return 混ぜた色
 */
static color_t mixColor(color_t x,color_t y,double xRatio) {
	color_t ret;
	/* 混色処理 */
	ret.r=x.r*xRatio+y.r*(1.0-xRatio);
	ret.g=x.g*xRatio+y.g*(1.0-xRatio);
	ret.b=x.b*xRatio+y.b*(1.0-xRatio);
	/* -0を0にするために、0は等号を含めた判定 */
	if(ret.r<=0)ret.r=0;
	if(ret.r>1)ret.r=1;
	if(ret.g<=0)ret.g=0;
	if(ret.g>1)ret.g=1;
	if(ret.b<=0)ret.b=0;
	if(ret.b>1)ret.b=1;
	return ret;
}

/*
 * ベクトルを正規化(スカラー倍して長さを1に)する
 * @param out 結果の出力先
 * @param in 入力
 */
static void vectorNormalize(double out[3],const double in[3]) {
	double len=sqrt(in[0]*in[0]+in[1]*in[1]+in[2]*in[2]);
	if(len==0) {
		out[0]=0;
		out[1]=0;
		out[2]=0;
	} else {
		out[0]=in[0]/len;
		out[1]=in[1]/len;
		out[2]=in[2]/len;
	}
}

/*
 * ベクトルの内積を求める
 * @param a 内積の左辺
 * @param b 内積の右辺
 * @return 内積の値
 */
static double vectorInnerProduct(const double a[3],const double b[3]) {
	return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];
}

/*
 * ベクトルの外積を求める
 * @param out 外積を格納する配列
 * @param a 外積の左辺
 * @param b 外積の右辺
 */
static void vectorExteriorProduct(double out[3],const double a[3],const double b[3]) {
	double buf[3];
	/* aまたはbとoutに同じポインタが指定されても大丈夫なように、
	一旦別のバッファに格納する */
	buf[0]=a[1]*b[2]-a[2]*b[1];
	buf[1]=a[2]*b[0]-a[0]*b[2];
	buf[2]=a[0]*b[1]-a[1]*b[0];
	/* 答えをoutに格納する */
	out[0]=buf[0];
	out[1]=buf[1];
	out[2]=buf[2];
}

/*
 * 直線とオブジェクトの当たり判定を行う
 * @param ot 当たっている場所のtを入れる配列
 * @param ou 当たっている場所のuを入れる配列
 * @param ov 当たっている場所のvを入れる配列
 * @param p 直線の基準点(t==0)
 * @param d 直線の方向ベクトル
 * @param obj 当たり判定を行うオブジェクト
 * @return 当たっている場所の数
 */
static int getLineObjectHitPoint(double ot[2],double ou[2],double ov[2],
		const double p[3],const double d[3],const object_t* obj) {
	int tnum=0;
	int i,j;
	double tuv[3];
	double t[2],u[2];
	switch(obj->objType) {
		case OBJTYPE_SPHERE:
			tnum=getLineSphereHitPoint(ot,p,d,obj->pos.s.c,obj->pos.s.r);
			for(i=0;i<tnum;i++)ou[i]=ov[i]=0;
			break;
		case OBJTYPE_PLANE:
			if(getLinePlaneHitPoint(tuv,p,d,obj->pos.p.p,obj->pos.p.d1,obj->pos.p.d2) &&
			(!obj->pos.p.min1e || obj->pos.p.min1<=tuv[1]) &&
			(!obj->pos.p.max1e || tuv[1]<=obj->pos.p.max1) &&
			(!obj->pos.p.min2e || obj->pos.p.min2<=tuv[2]) &&
			(!obj->pos.p.max2e || tuv[2]<=obj->pos.p.max2)) {
				ot[0]=tuv[0];
				ou[0]=tuv[1];
				ov[0]=tuv[2];
				tnum=1;
			} else {
				tnum=0;
			}
			break;
		case OBJTYPE_CYLINDER:
			tnum=getLineCylinderHitPoint(t,u,p,d,obj->pos.c.p,obj->pos.c.d,obj->pos.c.r);
			for(i=j=0;i<tnum;i++) {
				if(
				(!obj->pos.c.mine || obj->pos.c.min<=u[i]) &&
				(!obj->pos.c.maxe || u[i]<=obj->pos.c.max)) {
					ot[j]=t[i];
					ou[j]=u[i];
					ov[j]=0;
					j++;
				}
			}
			tnum=j;
			break;
		case OBJTYPE_DISK:
			if(getLinePlaneHitPoint(tuv,p,d,obj->pos.d.p,obj->pos.d.d1,obj->pos.d.d2) &&
			tuv[1]*tuv[1]+tuv[2]*tuv[2]<=1.0) {
				ot[0]=tuv[0];
				ou[0]=tuv[1];
				ov[0]=tuv[2];
				tnum=1;
			} else {
				tnum=0;
			}
			break;
		case OBJTYPE_TRIANGLE:
			if(getLinePlaneHitPoint(tuv,p,d,obj->pos.t.p,obj->pos.t.d1,obj->pos.t.d2) &&
			tuv[1]>=0.0 && tuv[2]>=0.0 &&
			tuv[1]/obj->pos.t.max1+tuv[2]/obj->pos.t.max2<=1.0) {
				ot[0]=tuv[0];
				ou[0]=tuv[1];
				ov[0]=tuv[2];
				tnum=1;
			} else {
				tnum=0;
			}
			break;
	}
	return tnum;
}

/*
 * オブジェクトの見た目に関する情報(見える点、法線、色)を取得する
 * @param alpha alphaを格納するポインタ
 * @param np 見える点を格納する配列
 * @param n 法線を格納する配列
 * @param sp 視線の始点
 * @param d 視線の方向ベクトル
 * @param t 事前に計算したt
 * @param u 事前に計算したu
 * @param v 事前に計算したv
 * @param obj 対象のオブジェクト
 * @return この点のオブジェクトの色
 */
static color_t getObjectViewInfo(double *alpha,double np[3],double n[3],const double sp[3],
		const double d[3],double t,double u,double v,const object_t* obj) {
	color_t col;
	/* 視線とオブジェクトが交わる場所 */
	np[0]=sp[0]+t*d[0];
	np[1]=sp[1]+t*d[1];
	np[2]=sp[2]+t*d[2];
	/* その点での法線 */
	switch(obj->objType) {
		case OBJTYPE_SPHERE:
			/* その場所での球の法線 */
			n[0]=np[0]-obj->pos.s.c[0];
			n[1]=np[1]-obj->pos.s.c[1];
			n[2]=np[2]-obj->pos.s.c[2];
			break;
		case OBJTYPE_PLANE:
			/* その場所での平面の法線、すなわち平面をつくる2本のベクトルの外積 */
			vectorExteriorProduct(n,obj->pos.p.d1,obj->pos.p.d2);
			break;
		case OBJTYPE_CYLINDER:
			/* その場所での円筒の法線 */
			n[0]=np[0]-(obj->pos.c.p[0]+u*obj->pos.c.d[0]);
			n[1]=np[1]-(obj->pos.c.p[1]+u*obj->pos.c.d[1]);
			n[2]=np[2]-(obj->pos.c.p[2]+u*obj->pos.c.d[2]);
			break;
		case OBJTYPE_DISK:
			/* その場所での円板の法線、すなわち平面をつくる2本のベクトルの外積 */
			vectorExteriorProduct(n,obj->pos.d.d1,obj->pos.d.d2);
			break;
		case OBJTYPE_TRIANGLE:
			/* その場所での三角形の板の法線、すなわち平面をつくる2本のベクトルの外積 */
			vectorExteriorProduct(n,obj->pos.t.d1,obj->pos.t.d2);
			break;
		default:
			/* 未知のオブジェクト(通常はここに来るはずはない) */
			n[0]=0;
			n[1]=0;
			n[2]=1;
			break;
	}
	
	/* その点でのオブジェクトの色 */
	*alpha=obj->alpha;
	if(obj->useTexture) {
		double dummy;
		double mu,mv;
		double tu,tv;
		int tui,tvi;
		/* テクスチャマッピングの計算を行う */
		mu=(obj->toffset[0])+(obj->tdx[0])*u+(obj->tdy[0])*v;
		mv=(obj->toffset[1])+(obj->tdx[1])*u+(obj->tdy[1])*v;
		/* テクスチャ上の位置(割合)を求める */
		tu=modf(mu,&dummy);
		tv=modf(mv,&dummy);
		if(tu<0)tu+=1.0;
		if(tv<0)tv+=1.0;
		if(tu>1)tu=1;
		if(tv>1)tv=1;
		/* テクスチャ上の座標を求める */
		tui=(int)floor(obj->tWidth*tu);
		tvi=(int)floor(obj->tHeight*tv);
		if(tui<0)tui=0;
		if(tui>=obj->tWidth)tui=obj->tWidth-1;
		if(tvi<0)tvi=0;
		if(tvi>=obj->tHeight)tvi=obj->tHeight-1;
		/* テクスチャの色のデータを取得する */
		if(obj->texture==NULL) {
			col=colBlack;
		} else {
			int index=tvi*obj->tWidth+tui;
			col.r=(double)obj->texture[index].r/255.0;
			col.g=(double)obj->texture[index].g/255.0;
			col.b=(double)obj->texture[index].b/255.0;
			if(obj->useTextureAlpha)*alpha=(double)obj->texture[index].a/255.0;
		}
	} else {
		col=obj->color;
	}
	return col;
}

/*
 * ある場所からある方向を見たときの色を計算する
 * @param sp 始点
 * @param d 方向ベクトル
 * @param ttl 再帰計算の回数の制限値(0以下になったら打ち切る)
 * @return 色
 */
color_t getColorOfOnePoint(double sp[3],double d[3],int ttl) {
	/* オブジェクトと視線の交点およびそこでの法線、Diffuseもどきの情報 */
	double np[3];
	double n[3],nn[3];
	double du;

	/* Specularもどきの情報 */
	double toLight[3];
	double hwv[3];
	double lightValue;
	int lightFlag;

	color_t col={0,0,0};
	color_t diffuseColor={0,0,0};
	int i,j;
	double mint=-1;
	int mini=-1;
	double minu=0,minv=0;
	double alpha=0;

	/* 無限再帰およびそれによるスタックオーバーフローを防ぐ */
	if(ttl<=0)return col;

	/* オブジェクトを1個ずつ見ていき、視線と交わるか判定する */
	for(i=0;i<theWorld.objectNum;i++) {
		double t[2],u[2],v[2];
		int tnum;
		tnum=getLineObjectHitPoint(t,u,v,sp,d,&theWorld.objects[i]);
		for(j=0;j<tnum;j++) {
			if(t[j]>EPS && (mint<0 || t[j]<mint)) {
				mint=t[j];
				minu=u[j];
				minv=v[j];
				mini=i;
				break;
			}
		}
	}

	/* 視線の先にオブジェクトが無いので、背景色を返す */
	if(mini<0)return theWorld.backGroundColor;

	/* 視線とオブジェクトが交わる場所、法線、色を求める */
	col=getObjectViewInfo(&alpha,np,n,sp,d,mint,minu,minv,&theWorld.objects[mini]);
	vectorNormalize(nn,n);

	/* 視線とオブジェクトの交点から光源へのベクトル */
	toLight[0]=theWorld.light.pos[0]-np[0];
	toLight[1]=theWorld.light.pos[1]-np[1];
	toLight[2]=theWorld.light.pos[2]-np[2];
	/* 視線とオブジェクトの交点から光源までの間に遮るものが無いか判定する */
	lightFlag=1;
	for(i=0;lightFlag && i<theWorld.objectNum;i++) {
		double t[2],u[2],v[2];
		int tnum;
		tnum=getLineObjectHitPoint(t,u,v,np,toLight,&theWorld.objects[i]);
		for(j=0;j<tnum;j++) {
			if(EPS<t[j] && t[j]+EPS<1.0) {
				lightFlag=0;
				break;
			}
		}
	}

	if(lightFlag) {
		/* Diffuseもどきを計算 */
		du=vectorInnerProduct(n,toLight);
		if(du<0)du=-du;
		du/=sqrt(vectorInnerProduct(n,n));
		du/=sqrt(vectorInnerProduct(toLight,toLight));
		if(du<0)du=0;
		if(du>1)du=1;

		/* Specularもどきを計算 */
		vectorNormalize(hwv,d);
		hwv[0]=-hwv[0]+toLight[0];
		hwv[1]=-hwv[1]+toLight[1];
		hwv[2]=-hwv[2]+toLight[2];
		vectorNormalize(hwv,hwv);
		lightValue=vectorInnerProduct(hwv,nn);
		if(lightValue<0)lightValue=-lightValue;
		lightValue/=sqrt(vectorInnerProduct(hwv,hwv));
		lightValue/=sqrt(vectorInnerProduct(nn,nn));
		lightValue=pow(lightValue,theWorld.objects[mini].lightPower);
		if(lightValue<0)lightValue=0;
		if(lightValue>1)lightValue=1;
	} else {
		/* 光は当たっていない */
		du=0;
		lightValue=0;
	}
	diffuseColor.r=col.r*theWorld.light.color.r;
	diffuseColor.g=col.g*theWorld.light.color.g;
	diffuseColor.b=col.b*theWorld.light.color.b;

	col=mixColor(
		mixColor(
			mixColor(
				diffuseColor,
				colBlack,
				du),
			mixColor(
				theWorld.light.color,
				colBlack,
				lightValue),
			0.5),
		col,
		theWorld.objects[mini].lightAlpha);

	/* 反射した光を計算する */
	if(theWorld.objects[mini].alpha<1) {
		double nd[3];
		getVectorMillor(nd,d,nn);
		col=mixColor(col,getColorOfOnePoint(np,nd,ttl-1),
			alpha);
	}

	return col;
}
