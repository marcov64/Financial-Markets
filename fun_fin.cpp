#include "fun_head.h"

MODELBEGIN

EQUATION("PreActionS")
/*
Set to 0 the inflow of cash in the funds
*/
CYCLE(cur, "Fund")
 {
  WRITES(cur,"NewCash",0);
 }

RESULT(1 )


EQUATION("ActionS")
/*
Actions of savers
*/
V("PreActionS");

v[12]=VLS(cur->hook,"fInterestPaid",1);
v[13]=V("sTotalAsset");
v[14]=VS(p->hook,"fTotalAsset");
v[15]=v[12]*v[13]/v[14];

v[16]=V("TotalInvested");
if(v[16]!=0)
 	WRITE("ActualInterestRate",v[15]/v[16]);
v[17]=VS(p->hook,"ValueK");
if(v[16]!=0)
 	WRITE("NominalInterestRate",v[15]/(v[17]));


v[0]=V("pSave");
if(v[0]<RND)
 {
  V("AddSavings");
  END_EQUATION(1);
 }

v[1]=V("pCheck");
if(v[1]>RND)
 END_EQUATION(2);
 
v[2]=VLS(p->hook,"fRReturn",1);
v[3]=VL("AvReturn",1);
v[4]=V("SampleReturn");
if(v[2]<v[3] && v[2]<v[4])
 {
  V("ChangeInvestment");
 }
RESULT(2)

EQUATION("AddSavings")
/*
Increase the savings invested by the calling saver
*/
v[0]=V("UnitSavings");
v[1]=VLS(c->hook,"PriceAsset",1);
v[2]=INCRS(c,"TotalInvested",v[0]);
v[3]=v[0]/v[1];
INCRS(c,"sTotalAsset",v[3]);
INCRS(c->hook,"fTotalAsset",v[3]);

INCRS(c->hook,"NewCash",v[0]);

RESULT(1 )

EQUATION("ChangeFund")
/*
Sell investment in one Fund and find a new one
*/

v[0]=V("AvReturn");
v[2]=V("exp");
CYCLE(cur, "Fund")
 {
  v[1]=VLS(cur,"fRReturn",1);
  if(v[1]>v[0])
   WRITES(cur,"app",pow(v[1]/v[0],v[2]));
  else
   WRITES(cur,"app",0); 
 }
cur=RNDDRAW("Fund","app");
v[4]=VS(c,"sTotalAsset");
v[5]=VLS(c->hook,"PriceAsset",1);
v[6]=v[5]*v[4];
v[7]=INCRS(c->hook,"NewCash",-v[6]);
INCRS(c->hook,"fTotalAsset",-v[4]);

if(v[7]<0)
 INTERACTS(c,"Bankrupt",v[6]);
INCRS(cur,"NewCash",v[6]);
v[8]=VLS(cur,"PriceAsset",1);
v[9]=v[6]/v[8];
INCRS(cur,"fTotalAsset",v[9]);
c->hook=cur;
RESULT(1 )

EQUATION("fInterestsGross")
/*
Absolute interests cumulated from investments
*/

v[0]=v[1]=0;
CYCLE(cur, "fI")
 {
  if(cur->hook!=NULL)
   {
    v[1]=VLS(cur->hook,"PriceAsset",1);
    v[2]=VS(cur->hook,"numAsset");
    v[0]+=v[1]*v[2];
    v[8]=VLS(cur->hook,"fInterestsPaid",1);
    v[9]=VS(cur->hook,"fTotalAsset");
    v[1]+=v[8]*v[9]/v[2];
   }  
 }

CYCLE(cur, "rI")
 {
  if(cur->hook!=NULL)
  {
  v[0]+=VS(cur->hook,"Investment");
  v[12]=VLS(cur->hook,"rInterest",1);
  v[13]=VS(cur->hook,"TotInvestment");
  v[1]+=v[12]*v[0]/v[13];
  }
 } 

WRITE("ValueK",v[0]);
RESULT(v[1] )

EQUATION("fInterestPaid")
/*
Net interest paid after fees on interest
*/
v[0]=V("fInterestGross");
v[1]=V("feeInterests");

RESULT(v[0]*(1-v[1]) )

EQUATION("fProfit")
/*
Profit of the fund
*/
v[0]=V("fInterestGross");
v[1]=V("fInterestPaid");
v[2]=V("ValueK");
v[3]=V("feeK");
v[4]=v[0]-v[1]+v[2]*v[3];
RESULT(v[4] )

EQUATION("Cash")
/*
Cash
*/
v[0]=VL("Cash",1);
v[1]=V("NewCash");
v[2]=V("fProfit");
v[3]=v[0]+v[1]-v[2];
RESULT(v[0] )

EQUATION("PriceAsset")
/*
Price of the asset, computed as the total value of the fund divided by the number of assets in circulation
*/

v[0]=V("ValueK");
v[1]=V("sTotalAssets");
v[2]=V("Cash");
RESULT((v[0]+v[2])/v[1] )

EQUATION("fInterestRate")
/*
Instantaneous Interest rate
*/
v[0]=V("fInterestPaid");
v[2]=V("ValueK");
v[3]=V("Cash");
v[4]=v[0]/(v[2]+v[3]);
RESULT(v[4] )

EQUATION("fRReturn")
/*
Smoothed interest rate
*/
v[0]=VL("fRReturn",1);
v[1]=V("fInterestRate");
v[2]=V("smoothRR");
v[3]=v[0]*v[2]+(1-v[2])*v[1];
RESULT(v[3] )


EQUATION("fStrategy")
/*
Fund strategy on using its cash
*/
v[0]=V("minShareCash");
v[3]=V("Cash");
v[4]=V("ValueK");
if(v[3]/(v[3]+v[4])>v[0])
 V("Invest");
RESULT(1 )

EQUATION("Invest")
/*
Collect a share of the excess of cash and invest 
*/

v[30]=VS(c,"Cash");
//what if the cash falls?

CYCLE(cur, "Fund")
  WRITES(cur,"app",1);

WRITES(c,"app",0);

v[0]=V("numfSample");
v[2]=0;
for(v[1]=0; v[1]<v[0]; v[1]++)
 {
  cur=RNDDRAW("Fund","app");
  v[3]=VS(cur,"fRReturn");
  if(v[3]>v[2])
   {
    v[2]=v[3];
    cur1=cur;  
   }
 }
v[5]=0; 
CYCLE(cur, "fI")
 {
  v[4]=VS(cur->hook,"fRRturn");
  if(v[4]>v[5])
   {
    v[5]=v[4];
    cur2=cur;
   }
 }

RESULT(1 )













EQUATION("PriceAsset")
/*
Unit Price of the asset
*/

v[0]=V("Cash");
v[1]=0;
v[20]=0;
CYCLE(cur, "fI")
 {
  if(cur->hook!=NULL)
   {
    v[1]=VLS(cur->hook,"PriceAsset",1);
    v[2]=VS(cur->hook,"numAsset");
    v[20]+=v[1]*v[2];
    v[8]=VS(cur->hook,"fLagInterest");
    v[9]=VS(cur->hook,"fTotalAsset");
    v[1]+=v[8]*v[9]/v[2];
   }  
 }

CYCLE(cur, "rI")
 {
  v[20]+=VS(cur->hook,"Investment");
  v[12]=VLS(cur->hook,"rInterest",1);
  v[13]=VS(cur->hook,"TotInvestment");
  v[1]+=v[12]*v[0]/v[13];
 }

v[4]=V("feeK");
v[5]=(v[0]+v[20])*v[4];
v[10]=V("feeInterest");
v[11]=v[1]*v[10];

v[0]-=v[5]+v[11];

INCR("Profit",v[5]+v[11]);
INCR("Cash",-1*(v[5]+v[11]));
v[6]=V("fTotalAsset");
v[7]=(v[0] + v[1]*(1-v[10])  + v[20])/v[6];

RESULT( v[7])

EQUATION("ActionF")
/*
Action of funds
*/

v[0]=V("Cash");
v[1]=0;
CYCLE(cur, "I")
 {
  
 }

RESULT( 1)


EQUATION("ActionS")
/*
Actions of savers
*/

v[0]=V("Saving");
v[1]=V("ProbCheck");
if(v[1]>RND)
 {
  INCRS(p->hook,"NewCash",v[0]);
  END_EQUATION(1);
 }

v[2]=VLS(p->hook,"RReturn",1);
v[3]=V("AvReturn");
v[4]=V("SampleReturn");
if(v[2]>v[3] && v[2]>v[4])
 {
  INCRS(p->hook,"NewCash",v[0]);
  END_EQUATION(2);
 }

V("ChangeInvestment");
INCRS(p->hook,"NewCash",v[0]);
RESULT( 2)


EQUATION("RReturn")
/*
return smoothed
*/
v[0]=V("NetReturn");

v[1]=VL("RReturn",1);
v[2]=V("aReturn");

v[3]=v[2]*v[1]+(1-v[2])*v[0];
RESULT(v[3] )


EQUATION("NetReturn")
/*
Net return 
*/

RESULT( 1)


EQUATION("GrossReturn")
/*
Return on current investments before taxes and fees
*/



RESULT( 1)

EQUATION("InstantReturn")
/*
InstantReturn
*/

v[0]=v[1]=0;

CYCLE(cur, "SecondaryInv")
 {
  v[2]=VS(cur,"KS");
  v[3]=VLS(cur,"NetReturn",1);
  v[0]+=v[2]*v[3];
  v[1]+=v[2];
 }

v[4]=v[5]=0;

CYCLE(cur, "PrimaryInv")
 {
  v[2]=VS(cur,"KP");
  v[3]=VS(cur->hook,"IReturn");
  v[4]+=v[2]*v[3];
  v[5]+=v[2];
 }

v[6]=(v[0]+v[4])/(v[1]+v[5]);
RESULT(v[6])

EQUATION("InstantReturn")
/*
Comment
*/

RESULT( 1)



MODELEND




void close_sim(void)
{

}


