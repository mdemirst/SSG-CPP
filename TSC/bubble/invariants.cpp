// invariants.cpp : Defines the entry point for the console application.
//
#include "bubbleprocess.h"
//#include "stdafx.h"
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

#define PLACES 63
#define STARTNO 0



using namespace std;

std::vector< std::vector< float > > bubbleProcess::calculateInvariants(const vector<bubblePoint>& bubble, QString path, QString fileName , int itemNo, int harmonic1, int harmonic2)
{

    std::vector< std::vector<float> > result;

    result.resize(harmonic1);

    for(int i = 0; i < harmonic1;i++)
    {
        result[i].resize(harmonic2);
    }

    int M = harmonic1;

    int N = harmonic2;

    //int  tourNumber = 3;
    //int main(void) {
    char invariantDosya[150],coeffDosya[150];
    //int dizinSirasi,dosyaSirasi,yerSirasi,incelenenNokta=0;
    float a[M][N],b[M][N],c[M][N],d[M][N],I[M][N];

    int incelenenNokta=0;

    QString fullPath = path;

    fullPath.append("coeffs_");

    QString ss;

    ss.setNum(itemNo);

    fullPath.append(ss);

    fullPath.append(".m");


    QByteArray ba = fullPath.toLocal8Bit();
    const char *c_str2 = ba.data();

    //if (yerSirasi!=4 || dizinSirasi!=14) {
    //sprintf(coeffDosya,"c%i\\apes%i\\coeffs%i.m",yerSirasi,dizinSirasi,dosyaSirasi);

    sprintf(coeffDosya, c_str2);
    qDebug()<<coeffDosya<<"\n";

    fullPath = path;

    fullPath.append(fileName);
    fullPath.append(ss);
    fullPath.append(".m");

    ba = fullPath.toLocal8Bit();
    const char *c_str3 = ba.data();

    sprintf(invariantDosya,c_str3);
    qDebug()<<invariantDosya<<"\n";

    fstream file_in(coeffDosya,ios::in);

    if(file_in.is_open()){

        file_in>>incelenenNokta;

        for (int m=0;m<M;m++)
            for (int n=0;n<N;n++)
                file_in>>m>>n>>a[m][n]>>b[m][n]>>c[m][n]>>d[m][n];
        file_in.close();

      //  fstream file_Invariant(invariantDosya,ios::out);

        for (int m=0;m<M;m++)

            for (int n=0;n<N;n++)
            {

                if (m==0)
                    I[m][n]=a[m][n]*a[m][n]+c[m][n]*c[m][n];
                else
                    I[m][n]=a[m][n]*a[m][n]+b[m][n]*b[m][n]+c[m][n]*c[m][n]+d[m][n]*d[m][n];
             //   file_Invariant<<m<<" "<<n<<" "<<I[m][n]<<endl;

                result[m][n] = I[m][n];
            }
        //file_Invariant.close();
    }

   return result;

}
std::vector< std::vector< float > > bubbleProcess::calculateInvariants(DFCoefficients coeff, int harmonic1, int harmonic2)
{

    std::vector< std::vector<float> > result;

    result.resize(harmonic1);

    for(int i = 0; i < harmonic1;i++)
    {
        result[i].resize(harmonic2);
    }

    int M = harmonic1;

    int N = harmonic2;

    //int  tourNumber = 3;
    //int main(void) {
    //char invariantDosya[150],coeffDosya[150];
    //int dizinSirasi,dosyaSirasi,yerSirasi,incelenenNokta=0;
   // float a[M][N],b[M][N],c[M][N],d[M][N],I[M][N];

    float I[M][N];
    //int incelenenNokta=0;

  /*  QString fullPath = path;

    fullPath.append("coeffs_");

    QString ss;

    ss.setNum(itemNo);

    fullPath.append(ss);

    fullPath.append(".m");


    QByteArray ba = fullPath.toLocal8Bit();
    const char *c_str2 = ba.data();

    //if (yerSirasi!=4 || dizinSirasi!=14) {
    //sprintf(coeffDosya,"c%i\\apes%i\\coeffs%i.m",yerSirasi,dizinSirasi,dosyaSirasi);

    sprintf(coeffDosya, c_str2);
    qDebug()<<coeffDosya<<"\n";

    fullPath = path;

    fullPath.append(fileName);
    fullPath.append(ss);
    fullPath.append(".m");

    ba = fullPath.toLocal8Bit();
    const char *c_str3 = ba.data();

    sprintf(invariantDosya,c_str3);
    qDebug()<<invariantDosya<<"\n";

    fstream file_in(coeffDosya,ios::in);

    if(file_in.is_open()){

        file_in>>incelenenNokta;

        for (int m=0;m<M;m++)
            for (int n=0;n<N;n++)
                file_in>>m>>n>>a[m][n]>>b[m][n]>>c[m][n]>>d[m][n];
        file_in.close();

      //  fstream file_Invariant(invariantDosya,ios::out);

        for (int m=0;m<M;m++)

            for (int n=0;n<N;n++)
            {

                if (m==0)
                    I[m][n]=a[m][n]*a[m][n]+c[m][n]*c[m][n];
                else
                    I[m][n]=a[m][n]*a[m][n]+b[m][n]*b[m][n]+c[m][n]*c[m][n]+d[m][n]*d[m][n];
             //   file_Invariant<<m<<" "<<n<<" "<<I[m][n]<<endl;

                result[m][n] = I[m][n];
            }
        //file_Invariant.close();
    }
*/


    for (int m=0;m<M;m++)

        for (int n=0;n<N;n++)
        {

            if (m==0)
                I[m][n]=coeff.a[m][n]*coeff.a[m][n]+coeff.c[m][n]*coeff.c[m][n];
            else
                I[m][n]=coeff.a[m][n]*coeff.a[m][n]+coeff.b[m][n]*coeff.b[m][n]+coeff.c[m][n]*coeff.c[m][n]+coeff.d[m][n]*coeff.d[m][n];
         //   file_Invariant<<m<<" "<<n<<" "<<I[m][n]<<endl;

            result[m][n] = I[m][n];
        }


    return result;

}
cv::Mat bubbleProcess::calculateInvariantsMat(DFCoefficients coeff, int harmonic1, int harmonic2)
{

    // Create 1x100 floating point matrix for result
    cv::Mat result(1,harmonic2*harmonic1,CV_32FC1);

    //result.resize(harmonic1);



    int M = harmonic1;

    int N = harmonic2;


  //  float I[M][N];
    //int incelenenNokta=0;


    for (int m=0;m<M;m++)

        for (int n=0;n<N;n++)
        {

            float val = 0;
            if (m==0)
                val =coeff.a[m][n]*coeff.a[m][n]+coeff.c[m][n]*coeff.c[m][n];
            else
                val =coeff.a[m][n]*coeff.a[m][n]+coeff.b[m][n]*coeff.b[m][n]+coeff.c[m][n]*coeff.c[m][n]+coeff.d[m][n]*coeff.d[m][n];
         //   file_Invariant<<m<<" "<<n<<" "<<I[m][n]<<endl;

            result.at<float>(0,10*m+n) = val;

        }


    return result;

}


