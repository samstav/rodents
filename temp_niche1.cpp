#include "stdafx.h"


const int maxmonths=12;             //12 months in the year
const int j=5;
const int maxageclass=60;         // max age classes
float R=500;                     //resource
int month;                     // between 1 and 12
float t;
float m;

class Species
{
private:
//define variable for each object
float N[maxageclass];//pop size as a function of month
float nextN[maxageclass];       //next pop size as a function of month
float Nsum;//all adults, no babies counted
float nextNsum;
float s[maxageclass];//this is a potential monthly survivorship dep. on resource
float f[maxageclass];//this is a potential fecundity dependent on seasonality, age and resource available

float s_scalar1,s_scalar2;      // scale for survival?
float f_scalar;                 // scale for fecundity?
float a_scalar1, a_scalar2;     // scale for storage
float reqR;// required R for survival.. or survival then fecundity??
float reqRs;
float reqRf;
float remainR[maxageclass];
float usedR;
float storage1;    //average storage of pre-reproductives
float storage2;    // average storage of post-reproductiveas
float popstorage;
//species-defined parameters
int id;
float maxs;            //max survivorship
int maxage;            //max age
int afr;            //age of forst reproduction
float maxf;            // max fecundity
float Rthresh;            //threshhold above which survivorship is max
float pupcost;            //resource cost of making one pup;
float Survcost;                //resource cost of surviving one month
int beginrepro;            //beginning month of reproduction
int endrepro;            //end month of reproduction
float maxpup;            // max brood size
// float R;
float metcost;
float maxstorage;



public:
//****definition of species and population attributes****************************


 Species(int id,float maxs,int maxage,float maxf,int afr,float pupcost,float metcost,int beginrepro,int endrepro,float maxpup)     //afr age of first reproduction
{
//FILE *input;
//input = fopen("Species.par","r");
//fscanf(input,"%d %f %d %f %d %f %f %f %f %d %d %d" &id, &maxs, &maxage, &maxf, &afr, &Rthresh, &pupcost, &Survcost, &metcost, &beginrepro, &endrepro, &maxpup);


//setting age specific survivorship,1st month 0.9, 2nd-first year half the maxs, 1-5 yr maxs
 Nsum =0;
for (int i=0;i<maxage;i++)
{
 N[i]=10;
 Nsum += N[i];
if(i==0) s[i]=0.9;
else
{
if(i>0 && i<=4) s[i] = 0.6*maxs;
else
{
if(i>4 && i<maxage) s[i]=maxs;
else s[i] = 0;
}
}
//printf("\\n %d %3.2f %3.2f ", i, N[i], s[i]);//getch();
}

//setting age-specific fecundity,1st month 0.0, 1-5yr maxf
for (int i=0;i<maxage;i++)
{
if(i< afr) f[i] = 0.0;
else f[i] = maxf;
//printf("\\n %d %3.2f %3.2f ", i, N[i], f[i]);//getch();
}

//0-1 scalar limiting storage (a), reproduction (f) and survival (s) due to resource availability
 s_scalar1 = s_scalar2 = 1;
 f_scalar =1;
 a_scalar1 = a_scalar2 = 1;
 storage1 = storage2 = 0.5*maxstorage;
}

//****end definition species attributes*******************************
//****defining resource allocation*******************************************

void calc_useResource()
{
float Ravail;//per cap
float reqR=0;
float reqRf=0;
float reqRs=0;

//first priority: survivorship
//case 1: pre-reporoductives

 Ravail = R/Nsum + storage1;
for(int i=1;i<=maxage;i++)
{
 reqR += metcost*s[i-1]*N[i-1];
}
 reqR = reqR/Nsum;

if(Ravail <= reqR)
{
 a_scalar1 = 0;
 s_scalar1 = Ravail/reqR;
}
else
{
 Ravail = R/Nsum;
if(Ravail <= reqR)
{
 a_scalar1 = (Ravail/reqR)*storage1;
 s_scalar1 = 1;
}
else
{
 a_scalar1 = 1;
 s_scalar1 =1;
}
}

//case 2: reproductives

Ravail = R/Nsum + storage2;
for(int i=1;i<=maxage;i++)
{
 reqRs += metcost*s[i-1]*N[i-1];
}
for(int i = 1;i <= maxage;i++)
{
 reqRf += pupcost*f[i]*N[i];
}
 reqR = (reqRs+reqRf)/Nsum;

if(Ravail <= reqR)
{
 a_scalar2 = 0;
 s_scalar2 = Ravail/reqR;
 f_scalar =0;
}
else 
{
 Ravail = R/Nsum;
if(Ravail <= reqR)
{
 a_scalar2 = (Ravail/reqR)*storage2;
 s_scalar2 = 1;
 f_scalar =1;
}
else
{
 a_scalar2 = 1;
 s_scalar2 =1;
 f_scalar =1;
}
}
}

//*****end of resource allocation*******************

float pop_storage()
{
if(remainR>0);
{
for(int i=0;i<t;i++)   //trying to add remainR across each time step into popstorage
{
 popstorage+=remainR[i];
}
}
return popstorage;
}
//********population growth***************************
float calc_nextN()
{
float drain =0;
 nextN[0]=0.0;

//1st: aging process***************

//pre-reproductives
if(s_scalar1<1)    //all fat reserves used and some starvation
{
for(int i=1;i<afr;i++)
{
 nextN[i]=s_scalar1*s[i-1]*N[i-1];
 drain += metcost*nextN[i];
}
 storage1 =0;
}
else
{
if (a_scalar1 <1)// no starvation and some fat reserves survive
{
for(int i=1;i<afr;i++)
{
 nextN[i]=s_scalar1*s[i-1]*N[i-1];
 drain += metcost*nextN[i];
}
 storage1 = (1-a_scalar1)*storage1;
}
else// no starvation not loss of fat reserves
{
for(int i=1;i<=afr;i++)
{
 nextN[i]=s_scalar1*s[i-1]*N[i-1];
 drain += metcost*nextN[i];
}
}
}

//reproductives
if(s_scalar2<1)//all fat reserves used and some starvation and no reproduction
{
for(int i=1;i<=maxage;i++)
{
 nextN[i]=s_scalar2*s[i-1]*N[i-1];
 drain += metcost*nextN[i];
}
 storage2 =0;
}
else 
{
if (a_scalar2 <1)// no starvation and some fat reserves survive and reproduction can occur
{
for(int i=afr;i<=maxage;i++)
{
 nextN[i]=s_scalar2*s[i-1]*N[i-1];
 drain += metcost*nextN[i];
}
 storage2 = (1-a_scalar2)*storage2;
}
else// no starvation not loss of fat reserves and reproduction can occur
{
for(int i=1;i<=maxage;i++)
{
 nextN[i]=s_scalar2*s[i-1]*N[i-1];
 drain += metcost*nextN[i];
}
}
}

// ****2nd: reproduction**********
if(month >=beginrepro && month <=endrepro)//reproduction if enough resource and appropriate season
{

for(int i=1;i<maxage;i++)
{
 nextN[0] += f_scalar*f[i]*N[i];
}
 drain = pupcost*nextN[0];
}
return drain;
}
//*********end of propulation growth******************


void grow()//setting nextN as N for the next time step
{
for(int i=0;i<m;i++)N[i]=nextN[i];
}

float get_nextN(int i){return nextN[i];}

float get_nextN()//summing all age classes for total N
{
 nextNsum=0;
for(int i=0;i<m;i++)nextNsum += nextN[i];
return nextNsum;
}

float get_N(int i){return N[i];}

float get_N()
{
 Nsum=0;
for(int i=0;i<m;i++)Nsum += N[i];
return Nsum;
}

};


void main()
{

const int species=1;  //we only have 2 species even though we designated the functions with 10
double t;
float scale;
//double h=0.2;
//int icount=0;
//int imax=1.0/h+1;
//FILE *outfile;
//outfile = fopen("8fecun.out","w");

 Species A(1, 0.9, 45, 0.5, 4, 0.25, 0.25, 3, 9, 5.0);
//printf("\\n %f %d %f %d ", A.maxs A.maxage A.maxf A.afr);


int month;
for(int t=0; t<600; t++)  //monthly loop
{

float Rdrain;
float N;

//keep traCK OF MONTH OF THE YEAR
if(t==0){month = 1,R=500;}
else(month++);
if(month==13)month=1;

 R+=107;   //monthly rate of resource renewal;
 scale = 1.0;
 Rdrain =0;
int flag=0;
while(flag==0)
{
 A.calc_useResource();//where scaling of storage and reproduction occurs
 Rdrain += A.calc_nextN();      //***This is where the drain on resource is calculated

if(R-Rdrain>0){ flag=1;}
else {scale-=0.1; if (scale<0.00001) flag=1;}
}

 printf("\\n %2d   %2d    %3.2f    %3.2f     %5.2f     %5.2f", t, month, R, Rdrain, A.get_N(), A.get_nextN()); getch();
//fprintf(outfile,"\\n %2d     %2d     %3.2f     %3.2f     %5.2f", t, month, R, Rdrain, A.get_N());
 A.grow();
//B.grow();
//C.grow();
//float Rd=0;
 R-=Rdrain;

//float Rr=0;
//Rr+=Rrenewal;

//R=Rdrain+Rrenewal;


}
 getch();
}



}
