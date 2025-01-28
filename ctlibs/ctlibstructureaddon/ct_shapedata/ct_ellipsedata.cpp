/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forêts (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Supérieure d'Arts et Métiers (ENSAM), Cluny, France.
                     All rights reserved.

 Contact : alexandre.piboule@onf.fr

 Developers : Michaël KREBS (ARTS/ENSAM)

 This file is part of PluginShared library 2.0.

 PluginShared is free library: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 PluginShared is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with PluginShared.  If not, see <http://www.gnu.org/licenses/lgpl.html>.

*****************************************************************************/

#include "ct_ellipsedata.h"

#include "ct_accessor/ct_pointaccessor.h"

#include "ct_pointcloudindex/abstract/ct_abstractpointcloudindex.h"

#include <math.h>

#include <Eigen/Dense>

const double CT_EllipseData::EL_INFINITY = 1.0e10;
const double CT_EllipseData::EL_ZERO = 1.0e-10;

CT_EllipseData::CT_EllipseData() : SuperClass(),
    _error(0)
{
}

CT_EllipseData::CT_EllipseData(const Eigen::Vector3d &center,
                               const CT_LineData& axisA,
                               const CT_LineData& axisB) : SuperClass(center,
                                                                      ((axisA.getP1() - axisA.getP2()).normalized().cross((axisB.getP1() - axisB.getP2()).normalized()).normalized())),
    _axisA(axisA),
    _axisB(axisB),
    _error(0)
{
}

CT_EllipseData::CT_EllipseData(const Eigen::Vector3d &center,
                               const CT_LineData& axisA,
                               const CT_LineData& axisB,
                               double error) : CT_EllipseData(center,
                                                              axisA,
                                                              axisB)
{
    _error = error;
}

const CT_LineData& CT_EllipseData::getAxisA() const
{
    return _axisA;
}

const CT_LineData& CT_EllipseData::getAxisB() const
{
    return _axisB;
}

double CT_EllipseData::getError() const
{
    return _error;
}

void CT_EllipseData::getBoundingBox(Eigen::Vector3d &min, Eigen::Vector3d &max) const
{
    double dist = std::max(_axisA.length(), _axisB.length()) / 2.0;

    min(0) = _center(0) - dist;
    min(1) = _center(1) - dist;
    min(2) = _center(2) - dist;

    max(0) = _center(0) + dist;
    max(1) = _center(1) + dist;
    max(2) = _center(2) + dist;
}


CT_EllipseData* CT_EllipseData::staticCreateZAxisAlignedEllipseDataFromPointCloud(const CT_AbstractPointCloudIndex *pointCloudIndex)
{
    if(pointCloudIndex == nullptr)
        return nullptr;

    int np = int(pointCloudIndex->size())+1;

    if(np < 7)
        return nullptr;

    double **D = new double*[np];
    double **S = new double*[7];
    double **L = new double*[7];
    double **invL = new double*[7];
    double **temp = new double*[7];
    double **C = new double*[7];
    double **V = new double*[7];
    double **sol = new double*[7];
    double **Const = new double*[7];
    double d[7];
    double pvec[7];

    staticInitMatrix(D, np, 7);
    staticInitMatrix(S, 7, 7);
    staticInitMatrix(L, 7, 7);
    staticInitMatrix(invL, 7, 7);
    staticInitMatrix(temp, 7, 7);
    staticInitMatrix(C, 7, 7);
    staticInitMatrix(V, 7, 7);
    staticInitMatrix(sol, 7, 7);
    staticInitMatrix(Const, 7, 7);

    Const[1][3] = -2;
    Const[2][2] = 1;
    Const[3][1] = -2;

    for(int i=0; i<7; ++i)
        d[i] = 0;

    np = np-1;

    double tx = 0;
    double ty = 0;
    double zm = 0;

    CT_PointAccessor pAccess;

    for (int i=1; i <= np; ++i)
    {
        const CT_Point &point = pAccess.constPointAt(pointCloudIndex->constIndexAt(i-1));

        tx = point(0);
        ty = point(1);
        zm += point(2);
        D[i][1] = tx*tx;
        D[i][2] = tx*ty;
        D[i][3] = ty*ty;
        D[i][4] = tx;
        D[i][5] = ty;
        D[i][6] = 1.0;
    }

    zm /= np;

    staticA_TperB(D,D,S,np,6,np,6);

    staticCholdc(S,6,L);

    staticInverse(L,invL,6);

    staticAperB_T(Const,invL,temp,6,6,6,6);
    staticAperB(invL,temp,C,6,6,6,6);

    staticJacobi(C,6,d,V,0);

    staticA_TperB(invL,V,sol,6,6,6,6);

    // Now normalize them
    for (int j=1; j<=6; ++j)  /* Scan columns */
    {
        double mod = 0.0;

        for (int i=1; i<=6; ++i)
        {
            mod += sol[i][j]*sol[i][j];
        }

        for (int i=1; i<=6; ++i)
        {
            sol[i][j] /=  sqrt(mod);
        }
    }

    double EL_ZERO=10e-20;
    int  solind=0;

    for(int i=1; i<=6; ++i)
    {
        if (d[i]<0 && fabs(d[i])>EL_ZERO)
        {
            solind = i;
        }
    }

    // Now fetch the right solution
    for(int j=1; j<=6; ++j)
    {
        pvec[j] = sol[j][solind];
    }

    staticDeleteMatrix(D, np);
    staticDeleteMatrix(S, 7);
    staticDeleteMatrix(L, 7);
    staticDeleteMatrix(invL, 7);
    staticDeleteMatrix(temp, 7);
    staticDeleteMatrix(C, 7);
    staticDeleteMatrix(V, 7);
    staticDeleteMatrix(sol, 7);
    staticDeleteMatrix(Const, 7);

    double cx, cy, x1, y1, x2, y2, x3, y3, x4, y4;

    staticGetCenter(pvec, &cx, &cy);
    staticGetAxisA(pvec, cx, cy, &x1, &y1, &x2, &y2);
    staticGetAxisB(pvec, cx, cy, &x3, &y3, &x4, &y4);

    CT_LineData axisA(Eigen::Vector3d(x1, y1, zm),
                      Eigen::Vector3d(x2, y2, zm));

    CT_LineData axisB(Eigen::Vector3d(x3, y3, zm),
                      Eigen::Vector3d(x4, y4, zm));

    // si l'axe B est plus long que l'axe A
    if(axisB.length() > axisA.length())
    {
        // l'axe A est l'axe mineur
        return new CT_EllipseData(Eigen::Vector3d(cx, cy, zm),
                                  axisA,
                                  axisB, 0);
    }

    // l'axe B est l'axe mineur
    return new CT_EllipseData(Eigen::Vector3d(cx, cy, zm),
                              axisB,
                              axisA, 0);
}

void CT_EllipseData::staticInitMatrix(double **m, int nlig, int ncol)
{
    for(int i=0; i<nlig; ++i)
    {
        m[i] = new double[ncol];

        for(int j=0; j<ncol; ++j)
        {
            m[i][j] = 0;
        }
    }
}

void CT_EllipseData::staticDeleteMatrix(double **m, int nlig)
{
    for(int i=0; i<nlig; ++i)
    {
        delete [] m[i];
    }

    delete [] m;
}

void CT_EllipseData::staticGetCenter(double *pvec, double *cx, double *cy)
{
    double A = pvec[1];
    double B = pvec[2];
    double C = pvec[3];
    double D = pvec[4];
    double E = pvec[5];
    *cx = ((2*C*D-E*B)/(B*B-4*A*C));
    *cy = ((-B*D+2*A*E)/(B*B-4*A*C));
}

void CT_EllipseData::staticGetAxisA(double *pvec, double cx, double cy, double *_x1, double *_y1, double *_x2, double *_y2)
{
    staticGetLine(staticGetQ(pvec), cx, cy, pvec, _x1, _y1, _x2, _y2);
}

void CT_EllipseData::staticGetAxisB(double *pvec, double cx, double cy, double *_x1, double *_y1, double *_x2, double *_y2)
{
    double q = staticGetQ(pvec);
    if (staticIsZero(q)) q = EL_INFINITY;

    staticGetLine(-1.0/q, cx, cy, pvec, _x1, _y1, _x2, _y2);
}

double CT_EllipseData::staticIsZero(double x)
{
    return (fabs(x) < CT_EllipseData::EL_ZERO);
}

double CT_EllipseData::staticGetQ(double *pvec)
{
    if (staticIsZero(pvec[2])) return CT_EllipseData::EL_INFINITY;

    double q = (pvec[3]-pvec[1])/pvec[2];
    q = (double)(sqrt(q*q+1)+q);

    return q;
}

void CT_EllipseData::staticGetLine(double q, double xc, double yc, double *pvec, double *_x1, double *_y1, double *_x2, double *_y2)
{
    // the slope is q

    // avoid division by EL_ZERO
    if ( staticIsZero(q) )
    {
        if ( q >= 0.0 ) q = CT_EllipseData::EL_ZERO;
        else q = -CT_EllipseData::EL_ZERO;
    }

    double A = pvec[1];
    double B = pvec[2];
    double C = pvec[3];
    double D = pvec[4];
    double E = pvec[5];
    double F = pvec[6];

    // these are pretty ugly calculations (and maybe inefficient)
    // but i'm just too lazy to compute the results by hand so
    // this is what Maple gave me.

    double x1 = (0.5*(B*q*xc-2*C*q*yc-B*yc+2*C*q*q*xc-D-E*q+
                         sqrt(-4*B*q*F+8*A*C*q*xc*yc-4*A*C*yc*yc+
                                   2*B*q*q*xc*E-2*B*yc*E*q-2*B*q*xc*D+
                                   4*C*q*yc*D-4*C*q*q*xc*D-4*A*C*q*q*xc*xc+
                                   4*A*E*q*xc-4*A*E*yc-4*C*q*q*F+B*B*yc*yc+
                                   D*D+E*E*q*q-4*A*F+2*D*E*q-2*B*B*q*xc*yc+
                                   B*B*q*q*xc*xc+
                                   2*B*yc*D))/(double)(A+B*q+C*q*q));

    double x2 = (0.5*(B*q*xc-2*C*q*yc-B*yc+2*C*q*q*xc-D-E*q-
                         sqrt(-4*B*q*F+8*A*C*q*xc*yc-4*A*C*yc*yc+
                                   2*B*q*q*xc*E-2*B*yc*E*q-2*B*q*xc*D+
                                   4*C*q*yc*D-4*C*q*q*xc*D-4*A*C*q*q*xc*xc+
                                   4*A*E*q*xc-4*A*E*yc-4*C*q*q*F+B*B*yc*yc+
                                   D*D+E*E*q*q-4*A*F+2*D*E*q-2*B*B*q*xc*yc+
                                   B*B*q*q*xc*xc+
                                   2*B*yc*D))/(double)(A+B*q+C*q*q));

    double y1A = (-0.5*(E+B*x1-
                            sqrt(E*E+2*E*B*x1+B*B*x1*x1-
                                      4*C*A*x1*x1-4*C*F-
                                      4*C*D*x1))/(double)C);

    double y1B = (-0.5*(E+B*x1+
                            sqrt(E*E+2*E*B*x1+B*B*x1*x1-
                                      4*C*A*x1*x1-
                                      4*C*F-4*C*D*x1))/(double)C);

    double y2A = (-0.5*(E+B*x2-
                            sqrt(E*E+2*E*B*x2+B*B*x2*x2-
                                      4*C*A*x2*x2-4*C*F-
                                      4*C*D*x2))/(double)C);

    double y2B = (-0.5*(E+B*x2+
                            sqrt(E*E+2*E*B*x2+B*B*x2*x2-
                                      4*C*A*x2*x2-4*C*F-
                                      4*C*D*x2))/(double)C);

    double s1A = ((double)(y1A-yc))/((double)(x1-xc));
    //double s1B = ((double)(y1B-yc))/((double)(line.x1-xc));
    double s2A = ((double)(y2A-yc))/((double)(x2-xc));
    //double s2B = ((double)(y2B-yc))/((double)(line.x2-xc));

    double y1 = ( fabs(s1A-q) < 1.0e-4 ? y1A : y1B );
    double y2 = ( fabs(s2A-q) < 1.0e-4 ? y2A : y2B );

    *_x1 = x1;
    *_x2 = x2;
    *_y1 = y1;
    *_y2 = y2;
}

void CT_EllipseData::staticMultMatrix(double **m, double **g, double **mg) {
// This function performs the meat of the calculations for the
// curve plotting.  Note that it is not a matrix multiplier in the
// pure sense.  The first matrix is the curve matrix (each curve type
// has its own matrix), and the second matrix is the geometry matrix
// (defined by the control points).  The result is returned in the
// third matrix.

    // First clear the return array
    for(int i=0; i<4; i++)
        for(int j=0; j<2; j++)
            mg[i][j]=0;

    // Perform the matrix math
    for(int i=0; i<4; i++)
        for(int j=0; j<2; j++)
            for(int k=0; k<4; k++)
                mg[i][j]=mg[i][j] + (m[i][k] * g[k][j]);
}


void CT_EllipseData::staticRotate(double **a, int i, int j, int k, int l, double tau, double s)
  {
    double g,h;
    g=a[i][j];h=a[k][l];a[i][j]=g-s*(h+g*tau);
    a[k][l]=h+s*(g-h*tau);
  }

void CT_EllipseData::staticJacobi(double **a, int n, double *d , double **v, int nrot)
  {
    int j,iq,ip,i;
    double tresh,theta,tau,t,sm,s,h,g,c;

    std::vector<double> b(n+1);
    std::vector<double> z(n+1);

    for (ip=1;ip<=n;ip++) {
      for (iq=1;iq<=n;iq++) v[ip][iq]=0.0;
      v[ip][ip]=1.0;
    }
    for (ip=1;ip<=n;ip++) {
      b[ip]=d[ip]=a[ip][ip];
      z[ip]=0.0;
    }
    nrot=0;
    for (i=1;i<=50;i++) {
      sm=0.0;
      for (ip=1;ip<=n-1;ip++) {
        for (iq=ip+1;iq<=n;iq++)
          sm += fabs(a[ip][iq]);
      }
      if (sm == 0.0) {
        /*    free_vector(z,1,n);
              free_vector(b,1,n);  */
        return;
      }
      if (i < 4)
        tresh=0.2*sm/(n*n);
      else
        tresh=0.0;
      for (ip=1;ip<=n-1;ip++) {
        for (iq=ip+1;iq<=n;iq++) {
          g=100.0*fabs(a[ip][iq]);
          if (i > 4 && fabs(d[ip])+g == fabs(d[ip])
              && fabs(d[iq])+g == fabs(d[iq]))
            a[ip][iq]=0.0;
          else if (fabs(a[ip][iq]) > tresh) {
            h=d[iq]-d[ip];
            if (fabs(h)+g == fabs(h))
              t=(a[ip][iq])/h;
            else {
              theta=0.5*h/(a[ip][iq]);
              t=1.0/(fabs(theta)+sqrt(1.0+theta*theta));
              if (theta < 0.0) t = -t;
            }
            c=1.0/sqrt(1+t*t);
            s=t*c;
            tau=s/(1.0+c);
            h=t*a[ip][iq];
            z[ip] -= h;
            z[iq] += h;
            d[ip] -= h;
            d[iq] += h;
            a[ip][iq]=0.0;
            for (j=1;j<=ip-1;j++) {
              staticRotate(a,j,ip,j,iq,tau,s);
              }
            for (j=ip+1;j<=iq-1;j++) {
              staticRotate(a,ip,j,j,iq,tau,s);
              }
            for (j=iq+1;j<=n;j++) {
              staticRotate(a,ip,j,iq,j,tau,s);
              }
            for (j=1;j<=n;j++) {
              staticRotate(v,j,ip,j,iq,tau,s);
              }
            ++nrot;
          }
        }
      }
      for (ip=1;ip<=n;ip++) {
        b[ip] += z[ip];
        d[ip]=b[ip];
        z[ip]=0.0;
      }
    }
    //printf("Too many iterations in routine JACOBI");
  }


//  Perform the Cholesky decomposition
// Return the lower triangular L  such that L*L'=A
void CT_EllipseData::staticCholdc(double **a, int n, double **l)
  {
    int i,j,k;
    double sum;
    std::vector<double> p(n+1);

    for (i=1; i<=n; i++)  {
      for (j=i; j<=n; j++)  {
        for (sum=a[i][j],k=i-1;k>=1;k--) sum -= a[i][k]*a[j][k];
        if (i == j) {
          if (sum<=0.0)
            // printf("\nA is not poitive definite!");
            {}
          else
            p[i]=sqrt(sum); }
        else
          {
            a[j][i]=sum/p[i];
          }
      }
    }
    for (i=1; i<=n; i++)
      for (j=i; j<=n; j++)
        if (i==j)
          l[i][i] = p[i];
        else
          {
            l[j][i]=a[j][i];
            l[i][j]=0.0;
          }
  }


/********************************************************************/
/**    Calcola la inversa della matrice  B mettendo il risultato   **/
/**    in InvB . Il metodo usato per l'inversione e' quello di     **/
/**    Gauss-Jordan.   N e' l'ordine della matrice .               **/
/**    ritorna 0 se l'inversione  corretta altrimenti ritorna     **/
/**    SINGULAR .                                                  **/
/********************************************************************/
int CT_EllipseData::staticInverse(double **TB, double **InvB, int N) {
  int k,i,j,p,q;
  double mult;
  double D,temp;
  double maxpivot;
  int npivot;

  double **B = new double*[N+1];
  double **A = new double*[N+1];

  staticInitMatrix(B, N+1, N+2);
  staticInitMatrix(A, N+1, 2*N+2);

  double eps = 10e-20;


  for(k=1;k<=N;k++)
    for(j=1;j<=N;j++)
      B[k][j]=TB[k][j];

  for (k=1;k<=N;k++)
    {
      for (j=1;j<=N+1;j++)
        A[k][j]=B[k][j];
      for (j=N+2;j<=2*N+1;j++)
        A[k][j]=(double)0;
      A[k][k-1+N+2]=(double)1;
    }
  for (k=1;k<=N;k++)
    {
      maxpivot=fabs((double)A[k][k]);
      npivot=k;
      for (i=k;i<=N;i++)
        if (maxpivot<fabs((double)A[i][k]))
          {
            maxpivot=fabs((double)A[i][k]);
            npivot=i;
          }
      if (maxpivot>=eps)
        {      if (npivot!=k)
                 for (j=k;j<=2*N+1;j++)
                   {
                     temp=A[npivot][j];
                     A[npivot][j]=A[k][j];
                     A[k][j]=temp;
                   } ;
               D=A[k][k];
               for (j=2*N+1;j>=k;j--)
                 A[k][j]=A[k][j]/D;
               for (i=1;i<=N;i++)
                 {
                   if (i!=k)
                     {
                       mult=A[i][k];
                       for (j=2*N+1;j>=k;j--)
                         A[i][j]=A[i][j]-mult*A[k][j] ;
                     }
                 }
             }
      else
        {  // printf("\n The matrix may be singular !!") ;
           return(-1);
         };
    }
  /**   Copia il risultato nella matrice InvB  ***/
  for (k=1,p=1;k<=N;k++,p++)
    for (j=N+2,q=1;j<=2*N+1;j++,q++)
      InvB[p][q]=A[k][j];

  staticDeleteMatrix(A, N+1);
  staticDeleteMatrix(B, N+1);

  return(0);
}            /*  End of INVERSE   */



void CT_EllipseData::staticAperB(double **_A, double **_B, double **_res,
                   int _righA, int _colA, int _righB, int _colB) {

    Q_UNUSED(_righB)

  int p,q,l;
  for (p=1;p<=_righA;p++)
    for (q=1;q<=_colB;q++)
      { _res[p][q]=0.0;
        for (l=1;l<=_colA;l++)
          _res[p][q]=_res[p][q]+_A[p][l]*_B[l][q];
      }
}

void CT_EllipseData::staticA_TperB(double **_A, double  **_B, double **_res,
                     int _righA, int _colA, int _righB, int _colB) {
    Q_UNUSED(_righB)

  int p,q,l;
  for (p=1;p<=_colA;p++)
    for (q=1;q<=_colB;q++)
      { _res[p][q]=0.0;
        for (l=1;l<=_righA;l++)
          _res[p][q]=_res[p][q]+_A[l][p]*_B[l][q];
      }
}

void CT_EllipseData::staticAperB_T(double **_A, double **_B, double **_res,
                     int _righA, int _colA, int _righB, int _colB) {
    Q_UNUSED(_righB)

  int p,q,l;
  for (p=1;p<=_colA;p++)
    for (q=1;q<=_colB;q++)
      { _res[p][q]=0.0;
        for (l=1;l<=_righA;l++)
          _res[p][q]=_res[p][q]+_A[p][l]*_B[q][l];
      }
}

