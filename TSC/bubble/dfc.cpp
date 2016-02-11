// dfc.cpp : Defines the entry point for the console application.
//

#include "bubbleprocess.h"
#include <stdlib.h>    // exit(..), NULL
#include <stdio.h>     // FILE structure
//#include <conio.h>     // clrscr(), getch()
#include <iostream>  // cin, cout objects & methods
#include <fstream>  // cin, cout objects & methods
#include <cmath>
#include <ctime>
#include <QDebug>
using namespace std;

#define PI 3.14
//#define M 30
//#define N 30

#define PLACES 77
#define STARTNO 1



void bubbleProcess::calculateDFCoefficients(const std::vector<bubblePoint>& bubble, QString path, QString fileName, int itemNo, int harmonic1, int harmonic2)
{

    int M = harmonic1;

    int N = harmonic2;

    clock_t start,ends;//Zaman tutmak icin
    float f1=45*PI/180,f2;
    float a[M][N],b[M][N],c[M][N],d[M][N];
  //  cout<<cos(f1)<<endl;
    float angle=5.5, rho;
    f1=angle*M/360;
    float angles[2];
    int angleIndex[2],nRead=0,fixations=0,newFixation=1;
   // int Ns=100,perimeter=5;//Number of saccades and the perimeter angles to be included in the analysis.
  //  int deneme=0;

   // char bubbleDosya[30];

   char coeffDosya[150];
 //   cout<<int(angle)<<endl;
 //   memories[0][0]=0;memories[1][0]=0;

    for (int m=0;m<M;m++)
        for (int n=0;n<N;n++) {
            a[m][n]=0;
            b[m][n]=0;
            c[m][n]=0;
            d[m][n]=0;
        }

    rho=10;

  //  start = clock();
    fixations=0;//Fixation sifirla

    sprintf(coeffDosya,"basis.m");
    fstream file_basisRead(coeffDosya,ios::in);
    if(file_basisRead.is_open()) qDebug()<<"Basis file has been read";
    for (int m=0;m<M;m++)
        for (int n=0;n<N;n++)
            file_basisRead>>m>>n>>a[m][n]>>b[m][n]>>c[m][n]>>d[m][n];
    file_basisRead.close();

    start = clock();
    //sprintf(bubbleDosya,"Tour%i\\c%i\\apes%i\\bubble%i.m",tourNumber,yerSirasi,dizinSirasi,dosyaSirasi);//Her yeni bubble icin basisleri tekrar oku, memories dedigini de sifirlamayi unutma.
   // fstream file_in(bubbleDosya,ios::in);
   // if(file_in.is_open()){
    for(uint i = 0; i < bubble.size(); i++){

        angles[0] = bubble.at(i).panAng;
        angles[1] = bubble.at(i).tiltAng;
        rho = bubble.at(i).val;

           // file_in>>angles[0]>>angles[1]>>rho;

            rho=(rho*10 + 5);

            while (angles[0]<0 || angles[0]>360) {
                if (angles[0]<0)
                    angles[0]=360+angles[0];
                if (angles[0]>360)
                    angles[0]=angles[0]-360;
            }
            while (angles[1]<0 || angles[1]>360) {
                if (angles[1]<0)
                    angles[1]=360+angles[1];
                if (angles[1]>360)
                    angles[1]=angles[1]-360;
            }
            angleIndex[0]=int(angles[0]);
            angleIndex[1]=int(angles[1]);
            /*	for (int i=0;i<fixations;i++) {
                            if (memories[0][i]==angleIndex[0] && memories[1][i]==angleIndex[1])
                            newFixation=0;
                            }*/

            if (newFixation==1) {
                //	memories[0][fixations]=angleIndex[0];
                //	memories[1][fixations]=angleIndex[1];
                fixations++;

                for (int m=0;m<M;m++)
                    for (int n=0;n<N;n++) {
                        f1=float(angleIndex[0]);
                        f2=float(angleIndex[1]);
                        a[m][n]=a[m][n]+rho*cos(m*f1*PI/180)*cos(n*f2*PI/180)*4;
                        b[m][n]=b[m][n]+rho*sin(m*f1*PI/180)*cos(n*f2*PI/180)*4;
                        c[m][n]=c[m][n]+rho*cos(m*f1*PI/180)*sin(n*f2*PI/180)*4;
                        d[m][n]=d[m][n]+rho*sin(m*f1*PI/180)*sin(n*f2*PI/180)*4;
                    }
            }
            newFixation=1;
        }
       // file_in.close();
        ends = clock();

        qDebug() << "One bubble Running Time coefficients (100 fixes): " << (double) (ends - start)*1000 / CLOCKS_PER_SEC << " ms\n";

        qDebug()<<"writing to file coeffs...\n";

        QString fullPath = path;

        fullPath.append(fileName);

        fullPath.append("coeffs_");

        QString ss;

        ss.setNum(itemNo);

        fullPath.append(ss);

        fullPath.append(".m");


        QByteArray ba = fullPath.toLocal8Bit();
        const char *c_str2 = ba.data();

        sprintf(coeffDosya,c_str2);
        fstream file_out(coeffDosya,ios::out);
        file_out<<fixations<<endl;//Kac fixation okunarak katsayilarin elde edildigi
        for (int m=0;m<M;m++)
            for (int n=0;n<N;n++)
                file_out<<m<<" "<<n<<" "<<a[m][n]<<" "<<b[m][n]<<" "<<c[m][n]<<" "<<d[m][n]<<endl;
        file_out.close();

}

DFCoefficients bubbleProcess::calculateDFCoefficients(const std::vector<bubblePoint>& bubble, int harmonic1, int harmonic2)
{

    int M = harmonic1;

    int N = harmonic2;

    DFCoefficients dfcoeff;

    dfcoeff.a.resize(M);
    dfcoeff.b.resize(M);
    dfcoeff.c.resize(M);
    dfcoeff.d.resize(M);

    for(int i = 0 ; i < M; i++)
    {

        dfcoeff.a[i].resize(N);
        dfcoeff.b[i].resize(N);
        dfcoeff.c[i].resize(N);
        dfcoeff.d[i].resize(N);
    }

    for(int i = 0 ; i < 0; i++)
    {
        for(int j = 0 ; j < 0; j++)
        {

            dfcoeff.a[i][j] = 0;
            dfcoeff.b[i][j] = 0;
            dfcoeff.c[i][j] = 0;
            dfcoeff.d[i][j] = 0;


        }

    }

    clock_t start,ends;//Zaman tutmak icin
    float f1=45*PI/180,f2;
   // float a[M][N],b[M][N],c[M][N],d[M][N];
   // cout<<cos(f1)<<endl;
    float angle=5.5, rho;
    f1=angle*M/360;
    float angles[2];
    int angleIndex[2],nRead=0,fixations=0,newFixation=1;
   // int Ns=100,perimeter=5;//Number of saccades and the perimeter angles to be included in the analysis.
  //  int deneme=0;

   // char bubbleDosya[30];

   char coeffDosya[150];
   // cout<<int(angle)<<endl;
 //   memories[0][0]=0;memories[1][0]=0;

   /* for (int m=0;m<M;m++)
        for (int n=0;n<N;n++) {
            a[m][n]=0;
            b[m][n]=0;
            c[m][n]=0;
            d[m][n]=0;
        }
*/
    rho=10;

  //  start = clock();
    fixations=0;//Fixation sifirla

    sprintf(coeffDosya,"basis.m");
    fstream file_basisRead(coeffDosya,ios::in);

    if(file_basisRead.is_open())
    {

        qDebug()<<"Basis file has been read";

        for (int m=0;m<M;m++)
            for (int n=0;n<N;n++)
                file_basisRead>>m>>n>>dfcoeff.a[m][n]>>dfcoeff.b[m][n]>>dfcoeff.c[m][n]>>dfcoeff.d[m][n];

        file_basisRead.close();
    }
    start = clock();
    //sprintf(bubbleDosya,"Tour%i\\c%i\\apes%i\\bubble%i.m",tourNumber,yerSirasi,dizinSirasi,dosyaSirasi);//Her yeni bubble icin basisleri tekrar oku, memories dedigini de sifirlamayi unutma.
   // fstream file_in(bubbleDosya,ios::in);
   // if(file_in.is_open()){
    for(uint i = 0; i < bubble.size(); i++)
    {

        angles[0] = bubble.at(i).panAng;
        angles[1] = bubble.at(i).tiltAng;
        rho = bubble.at(i).val;

           // file_in>>angles[0]>>angles[1]>>rho;

            rho=(rho*10 + 5);

            while (angles[0]<0 || angles[0]>360) {
                if (angles[0]<0)
                    angles[0]=360+angles[0];
                if (angles[0]>360)
                    angles[0]=angles[0]-360;
            }
            while (angles[1]<0 || angles[1]>360) {
                if (angles[1]<0)
                    angles[1]=360+angles[1];
                if (angles[1]>360)
                    angles[1]=angles[1]-360;
            }
            angleIndex[0]=int(angles[0]);
            angleIndex[1]=int(angles[1]);


            if (newFixation==1)
            {

                fixations++;

                for (int m=0;m<M;m++)
                    for (int n=0;n<N;n++) {
                        f1=float(angleIndex[0]);
                        f2=float(angleIndex[1]);
                        dfcoeff.a[m][n]=dfcoeff.a[m][n]+rho*cos(m*f1*PI/180)*cos(n*f2*PI/180)*4;
                        dfcoeff.b[m][n]=dfcoeff.b[m][n]+rho*sin(m*f1*PI/180)*cos(n*f2*PI/180)*4;
                        dfcoeff.c[m][n]=dfcoeff.c[m][n]+rho*cos(m*f1*PI/180)*sin(n*f2*PI/180)*4;
                        dfcoeff.d[m][n]=dfcoeff.d[m][n]+rho*sin(m*f1*PI/180)*sin(n*f2*PI/180)*4;
                    }
            }
            newFixation=1;
        }
       // file_in.close();
        ends = clock();

      //  qDebug() << "One bubble Running Time coefficients (100 fixes): " << (double) (ends - start)*1000 / CLOCKS_PER_SEC << " ms\n";

       // qDebug()<<"writing to file coeffs...\n";

      /*  QString fullPath = path;

        fullPath.append(fileName);

        fullPath.append("coeffs_");

        QString ss;

        ss.setNum(itemNo);

        fullPath.append(ss);

        fullPath.append(".m");*/


    /*    QByteArray ba = fullPath.toLocal8Bit();
        const char *c_str2 = ba.data();

        sprintf(coeffDosya,c_str2);
        fstream file_out(coeffDosya,ios::out);
        file_out<<fixations<<endl;//Kac fixation okunarak katsayilarin elde edildigi
        for (int m=0;m<M;m++)
            for (int n=0;n<N;n++)
                file_out<<m<<" "<<n<<" "<<a[m][n]<<" "<<b[m][n]<<" "<<c[m][n]<<" "<<d[m][n]<<endl;
        file_out.close();*/

        return dfcoeff;
}









