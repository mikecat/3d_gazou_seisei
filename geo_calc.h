#ifndef GEO_CALC_H_GUARD
#define GEO_CALC_H_GUARD

#ifndef EPS
#define EPS (1e-7)
#endif

/* 3d_hansya.c */
void getVectorMillor(double out[3],const double in[3],const double n[3]);
/* kyu_koten.c */
int getLineSphereHitPoint(double ot[2],
	const double ins[3],const double ind[3],const double incc[3],double incr);
/* heimen_koten.c */
int getLinePlaneHitPoint(double ot[3],const double ils[3],const double ild[3],
	const double ips[3],const double ipd1[3],const double ipd2[3]);
/* enchu_koten.c */
int getLineCylinderHitPoint(
	double ot[2],double os[2],const double pl[3],const double dl[3],
	const double pc[3],const double dc[3],double r);

#endif
