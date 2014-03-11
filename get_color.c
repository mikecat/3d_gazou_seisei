#include <stdio.h>
#include <math.h>
#include "geo_calc.h"
#include "script_loader.h"

/*
 * �F��������
 * @param x ��������F1
 * @param y ��������F2
 * @param xRatio ��������F1�̋����B[0,1]�ő傫���قǌ��ʂ�x�ɋ߂��Ȃ�B
 * @return �������F
 */
static color_t mixColor(color_t x,color_t y,double xRatio) {
	color_t ret;
	/* ���F���� */
	ret.r=x.r*xRatio+y.r*(1.0-xRatio);
	ret.g=x.g*xRatio+y.g*(1.0-xRatio);
	ret.b=x.b*xRatio+y.b*(1.0-xRatio);
	/* -0��0�ɂ��邽�߂ɁA0�͓������܂߂����� */
	if(ret.r<=0)ret.r=0;
	if(ret.r>1)ret.r=1;
	if(ret.g<=0)ret.g=0;
	if(ret.g>1)ret.g=1;
	if(ret.b<=0)ret.b=0;
	if(ret.b>1)ret.b=1;
	return ret;
}

/*
 * �x�N�g���𐳋K��(�X�J���[�{���Ē�����1��)����
 * @param out ���ʂ̏o�͐�
 * @param in ����
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
 * �x�N�g���̓��ς����߂�
 * @param a ���ς̍���
 * @param b ���ς̉E��
 * @return ���ς̒l
 */
static double vectorInnerProduct(const double a[3],const double b[3]) {
	return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];
}

/*
 * �x�N�g���̊O�ς����߂�
 * @param out �O�ς��i�[����z��
 * @param a �O�ς̍���
 * @param b �O�ς̉E��
 */
static void vectorExteriorProduct(double out[3],const double a[3],const double b[3]) {
	double buf[3];
	/* a�܂���b��out�ɓ����|�C���^���w�肳��Ă����v�Ȃ悤�ɁA
	��U�ʂ̃o�b�t�@�Ɋi�[���� */
	buf[0]=a[1]*b[2]-a[2]*b[1];
	buf[1]=a[2]*b[0]-a[0]*b[2];
	buf[2]=a[0]*b[1]-a[1]*b[0];
	/* ������out�Ɋi�[���� */
	out[0]=buf[0];
	out[1]=buf[1];
	out[2]=buf[2];
}

/*
 * �����ƃI�u�W�F�N�g�̓����蔻����s��
 * @param ot �������Ă���ꏊ��t������z��
 * @param ou �������Ă���ꏊ��u������z��
 * @param ov �������Ă���ꏊ��v������z��
 * @param p �����̊�_(t==0)
 * @param d �����̕����x�N�g��
 * @param obj �����蔻����s���I�u�W�F�N�g
 * @return �������Ă���ꏊ�̐�
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
 * �I�u�W�F�N�g�̌����ڂɊւ�����(������_�A�@���A�F)���擾����
 * @param alpha alpha���i�[����|�C���^
 * @param np ������_���i�[����z��
 * @param n �@�����i�[����z��
 * @param sp �����̎n�_
 * @param d �����̕����x�N�g��
 * @param t ���O�Ɍv�Z����t
 * @param u ���O�Ɍv�Z����u
 * @param v ���O�Ɍv�Z����v
 * @param obj �Ώۂ̃I�u�W�F�N�g
 * @return ���̓_�̃I�u�W�F�N�g�̐F
 */
static color_t getObjectViewInfo(double *alpha,double np[3],double n[3],const double sp[3],
		const double d[3],double t,double u,double v,const object_t* obj) {
	color_t col;
	/* �����ƃI�u�W�F�N�g�������ꏊ */
	np[0]=sp[0]+t*d[0];
	np[1]=sp[1]+t*d[1];
	np[2]=sp[2]+t*d[2];
	/* ���̓_�ł̖@�� */
	switch(obj->objType) {
		case OBJTYPE_SPHERE:
			/* ���̏ꏊ�ł̋��̖@�� */
			n[0]=np[0]-obj->pos.s.c[0];
			n[1]=np[1]-obj->pos.s.c[1];
			n[2]=np[2]-obj->pos.s.c[2];
			break;
		case OBJTYPE_PLANE:
			/* ���̏ꏊ�ł̕��ʂ̖@���A���Ȃ킿���ʂ�����2�{�̃x�N�g���̊O�� */
			vectorExteriorProduct(n,obj->pos.p.d1,obj->pos.p.d2);
			break;
		case OBJTYPE_CYLINDER:
			/* ���̏ꏊ�ł̉~���̖@�� */
			n[0]=np[0]-(obj->pos.c.p[0]+u*obj->pos.c.d[0]);
			n[1]=np[1]-(obj->pos.c.p[1]+u*obj->pos.c.d[1]);
			n[2]=np[2]-(obj->pos.c.p[2]+u*obj->pos.c.d[2]);
			break;
		case OBJTYPE_DISK:
			/* ���̏ꏊ�ł̉~�̖@���A���Ȃ킿���ʂ�����2�{�̃x�N�g���̊O�� */
			vectorExteriorProduct(n,obj->pos.d.d1,obj->pos.d.d2);
			break;
		case OBJTYPE_TRIANGLE:
			/* ���̏ꏊ�ł̎O�p�`�̔̖@���A���Ȃ킿���ʂ�����2�{�̃x�N�g���̊O�� */
			vectorExteriorProduct(n,obj->pos.t.d1,obj->pos.t.d2);
			break;
		default:
			/* ���m�̃I�u�W�F�N�g(�ʏ�͂����ɗ���͂��͂Ȃ�) */
			n[0]=0;
			n[1]=0;
			n[2]=1;
			break;
	}
	
	/* ���̓_�ł̃I�u�W�F�N�g�̐F */
	*alpha=obj->alpha;
	if(obj->useTexture) {
		double dummy;
		double mu,mv;
		double tu,tv;
		int tui,tvi;
		/* �e�N�X�`���}�b�s���O�̌v�Z���s�� */
		mu=(obj->toffset[0])+(obj->tdx[0])*u+(obj->tdy[0])*v;
		mv=(obj->toffset[1])+(obj->tdx[1])*u+(obj->tdy[1])*v;
		/* �e�N�X�`����̈ʒu(����)�����߂� */
		tu=modf(mu,&dummy);
		tv=modf(mv,&dummy);
		if(tu<0)tu+=1.0;
		if(tv<0)tv+=1.0;
		if(tu>1)tu=1;
		if(tv>1)tv=1;
		/* �e�N�X�`����̍��W�����߂� */
		tui=(int)floor(obj->tWidth*tu);
		tvi=(int)floor(obj->tHeight*tv);
		if(tui<0)tui=0;
		if(tui>=obj->tWidth)tui=obj->tWidth-1;
		if(tvi<0)tvi=0;
		if(tvi>=obj->tHeight)tvi=obj->tHeight-1;
		/* �e�N�X�`���̐F�̃f�[�^���擾���� */
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
 * ����ꏊ���炠������������Ƃ��̐F���v�Z����
 * @param sp �n�_
 * @param d �����x�N�g��
 * @param ttl �ċA�v�Z�̉񐔂̐����l(0�ȉ��ɂȂ�����ł��؂�)
 * @return �F
 */
color_t getColorOfOnePoint(double sp[3],double d[3],int ttl) {
	/* �I�u�W�F�N�g�Ǝ����̌�_����т����ł̖@���ADiffuse���ǂ��̏�� */
	double np[3];
	double n[3],nn[3];
	double du;

	/* Specular���ǂ��̏�� */
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

	/* �����ċA����т���ɂ��X�^�b�N�I�[�o�[�t���[��h�� */
	if(ttl<=0)return col;

	/* �I�u�W�F�N�g��1�����Ă����A�����ƌ���邩���肷�� */
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

	/* �����̐�ɃI�u�W�F�N�g�������̂ŁA�w�i�F��Ԃ� */
	if(mini<0)return theWorld.backGroundColor;

	/* �����ƃI�u�W�F�N�g�������ꏊ�A�@���A�F�����߂� */
	col=getObjectViewInfo(&alpha,np,n,sp,d,mint,minu,minv,&theWorld.objects[mini]);
	vectorNormalize(nn,n);

	/* �����ƃI�u�W�F�N�g�̌�_��������ւ̃x�N�g�� */
	toLight[0]=theWorld.light.pos[0]-np[0];
	toLight[1]=theWorld.light.pos[1]-np[1];
	toLight[2]=theWorld.light.pos[2]-np[2];
	/* �����ƃI�u�W�F�N�g�̌�_��������܂ł̊ԂɎՂ���̂����������肷�� */
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
		/* Diffuse���ǂ����v�Z */
		du=vectorInnerProduct(n,toLight);
		if(du<0)du=-du;
		du/=sqrt(vectorInnerProduct(n,n));
		du/=sqrt(vectorInnerProduct(toLight,toLight));
		if(du<0)du=0;
		if(du>1)du=1;

		/* Specular���ǂ����v�Z */
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
		/* ���͓������Ă��Ȃ� */
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

	/* ���˂��������v�Z���� */
	if(theWorld.objects[mini].alpha<1) {
		double nd[3];
		getVectorMillor(nd,d,nn);
		col=mixColor(col,getColorOfOnePoint(np,nd,ttl-1),
			alpha);
	}

	return col;
}
