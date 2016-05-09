/*Header-MicMac-eLiSe-25/06/2007

    MicMac : Multi Image Correspondances par Methodes Automatiques de Correlation
    eLiSe  : ELements of an Image Software Environnement

    www.micmac.ign.fr

   
    Copyright : Institut Geographique National
    Author : Marc Pierrot Deseilligny
    Contributors : Gregoire Maillet, Didier Boldo.

[1] M. Pierrot-Deseilligny, N. Paparoditis.
    "A multiresolution and optimization-based image matching approach:
    An application to surface reconstruction from SPOT5-HRS stereo imagery."
    In IAPRS vol XXXVI-1/W41 in ISPRS Workshop On Topographic Mapping From Space
    (With Special Emphasis on Small Satellites), Ankara, Turquie, 02-2006.

[2] M. Pierrot-Deseilligny, "MicMac, un lociel de mise en correspondance
    d'images, adapte au contexte geograhique" to appears in 
    Bulletin d'information de l'Institut Geographique National, 2007.

Francais :

   MicMac est un logiciel de mise en correspondance d'image adapte 
   au contexte de recherche en information geographique. Il s'appuie sur
   la bibliotheque de manipulation d'image eLiSe. Il est distibue sous la
   licences Cecill-B.  Voir en bas de fichier et  http://www.cecill.info.


English :

    MicMac is an open source software specialized in image matching
    for research in geographic information. MicMac is built on the
    eLiSe image library. MicMac is governed by the  "Cecill-B licence".
    See below and http://www.cecill.info.

Header-MicMac-eLiSe-25/06/2007*/
#include "Apero.h"


extern bool AllowUnsortedVarIn_SetMappingCur;

#define HandleGL 1

/*     ==== MODELISATION MATHEMATIQUE-1 ====================
  
   Pour les notations on reprend la terminologie LR (Left-Right) et Time (date de prise de vue)

      Block Inc :
          LinkL  LinkR ...

      Pour un bloc a N Camera, il y a N rotation inconnue (donc une de plus que necessaire) qui lie la camera a
   un systeme de reference; en pratique le systeme de reference  se trouve etre de la "premiere" camera (c'est ce qui
   est fait lors de l'estimation. c'est ce qui est maintenu pour lever l'arbitraire des equations, mais a part cela
   la "premiere" camera ne joue aucun role particulier);

      Les valeurs stockees dans LinkL (t.q. stockees dans LiaisonsSHC, voir cImplemBlockCam.cpp  CONV-ORI)

          LinkL =   L  -> Ref
          LinkR =   R  -> Ref

      Time i : CamLi  CamRi   ...  ;  CamLi : Li  -> Monde
      Time j : CamLj  CamRj   ...
      Time k : CamLk  CamRk   ...


       Les equation sont du type :

          L to R =      CamRi-1  CamLi   =   CamRk -1 CamLk , qqs k,i (et L,R)

     On ecrit cela 
 
             CamRi-1  CamLi = LinkR-1 LinkL  ,  qqs i, L , R  (Eq1) 

*/

/*
     Soit L = (Tl,Wl)  avec WL matrice rotation et Tl centre de projection   L(P) = Wl* P + Tl  (Cam->Monde)
     Et R-1 (P) = Wr-1*P - Wr-1 * Tr = tWr * P - Tr  ;   R-1 = (Wr-1,- Wr-1* P)
    

      CamRi-1  CamLi  = (Wr-1,- Wr-1* Pr) X (Wl,Pl) = (Wr-1*Wl,- Wr-1* Pr +  Wr-1* Pl)
*/

class cIBC_ImsOneTime;
class cIBC_OneCam;
class cImplemBlockCam;
class cEqObsBlockCam ;

// IBC : suffix for Implement Block Cam
// cIBC_ImsOneTime  : regroupe les pose acquise au meme temps T
// cIBC_OneCam      : contient les info partagee par la meme tete de camera


// Compute the parameter of the transformation of Point in L coordinate
// to point in R coordinates
void CalcParamEqRel
     (
          Pt3d<Fonc_Num> & aTr, 
          ElMatrix<Fonc_Num> & aMat, 
          cRotationFormelle & aRotR,
          cRotationFormelle & aRotL,
          int                 aRNumGl,
          int                 aLNumGl
      )
{
#if (HandleGL != 1)
    ELISE_ASSERT(! aRotR.IsGL(),"Guimbal lock in Eq Rig still unsupported");
    ELISE_ASSERT(! aRotL.IsGL(),"Guimbal lock in Eq Rig still unsupported");
    aRNumGl=-1;
    aLNumGl=-1;
#endif

    ElMatrix<Fonc_Num> aRMat = aRotR.MatFGLComplete(aRNumGl);
    ElMatrix<Fonc_Num> aLMat = aRotL.MatFGLComplete(aLNumGl);
    ElMatrix<Fonc_Num> aRMatInv = aRMat.transpose();

    aMat = aRMatInv * aLMat;
    aTr = aRMatInv * (aRotL.COpt() - aRotR.COpt());

}

class cEqObsBlockCam  : public cNameSpaceEqF,
                        public cObjFormel2Destroy
{
     public :
         friend class   cSetEqFormelles;
        const std::vector<double> &  AddObs(const double & aPdsTr,const double & aPdsMatr);
        void  DoAMD(cAMD_Interf * anAMD);

     private  :

         cEqObsBlockCam
         (
             cRotationFormelle & aRotRT0,
             cRotationFormelle & aRotLT0,
             cRotationFormelle & aRotRT1,
             cRotationFormelle & aRotLT1,
             bool                doGenerateCode
         );

          void GenerateCode();
          cEqObsBlockCam(const cEqObsBlockCam &); // Non Implemanted

          cSetEqFormelles *   mSet;
          cRotationFormelle * mRotRT0;
          cRotationFormelle * mRotLT0;
          cRotationFormelle * mRotRT1;
          cRotationFormelle * mRotLT1;
          cIncListInterv      mLInterv;
          std::string         mNameType;
          cElCompiledFonc*    mFoncEqResidu;
#if  (HandleGL)
           cMatr_Etat_PhgrF   mMatR0;
           cMatr_Etat_PhgrF   mMatL0;
           cMatr_Etat_PhgrF   mMatR1;
           cMatr_Etat_PhgrF   mMatL1;
#endif
};

cEqObsBlockCam::cEqObsBlockCam
(
    cRotationFormelle & aRotRT0,
    cRotationFormelle & aRotLT0,
    cRotationFormelle & aRotRT1,
    cRotationFormelle & aRotLT1,
    bool                doGenerateCode
) :
    mSet       (aRotRT0.Set()),
    mRotRT0    (&aRotRT0),
    mRotLT0    (&aRotLT0),
    mRotRT1    (&aRotRT1),
    mRotLT1    (&aRotLT1),
    mNameType  ("cCodeBlockCam"),
    mFoncEqResidu  (0),
#if  (HandleGL)
    mMatR0("GL_MK0",3,3),
    mMatL0("GL_MK1",3,3),
    mMatR1("GL_MK2",3,3),
    mMatL1("GL_MK3",3,3)
#endif

{

   AllowUnsortedVarIn_SetMappingCur = true;

   ELISE_ASSERT(mSet==mRotRT0->Set(),"Different sets incEqObsBlockCam");
   ELISE_ASSERT(mSet==mRotLT0->Set(),"Different sets incEqObsBlockCam");
   ELISE_ASSERT(mSet==mRotRT1->Set(),"Different sets incEqObsBlockCam");
   ELISE_ASSERT(mSet==mRotLT1->Set(),"Different sets incEqObsBlockCam");


   mRotRT0->IncInterv().SetName("OriR0");
   mRotLT0->IncInterv().SetName("OriL0");
   mRotRT1->IncInterv().SetName("OriR1");
   mRotLT1->IncInterv().SetName("OriL1");

   mLInterv.AddInterv(mRotRT0->IncInterv());
   mLInterv.AddInterv(mRotLT0->IncInterv());
   mLInterv.AddInterv(mRotRT1->IncInterv());
   mLInterv.AddInterv(mRotLT1->IncInterv());
   
   if (doGenerateCode)
   {
      GenerateCode();
      return;
   }

   mFoncEqResidu = cElCompiledFonc::AllocFromName(mNameType);
   ELISE_ASSERT(mFoncEqResidu!=0,"Cannot allocate cEqObsBlockCam");
   mFoncEqResidu->SetMappingCur(mLInterv,mSet);

#if  (HandleGL)
   mMatR0.InitAdr(*mFoncEqResidu);
   mMatL0.InitAdr(*mFoncEqResidu);
   mMatR1.InitAdr(*mFoncEqResidu);
   mMatL1.InitAdr(*mFoncEqResidu);

#endif

   mSet->AddFonct(mFoncEqResidu);


}


void  cEqObsBlockCam::DoAMD(cAMD_Interf * anAMD)
{
    std::vector<int> aNums;

    aNums.push_back(mRotRT0->IncInterv().NumBlocAlloc());
    aNums.push_back(mRotLT0->IncInterv().NumBlocAlloc());
    aNums.push_back(mRotRT1->IncInterv().NumBlocAlloc());
    aNums.push_back(mRotLT1->IncInterv().NumBlocAlloc());

    for (int aK1=0 ; aK1 <int(aNums.size()) ; aK1++)
        for (int aK2=aK1 ; aK2 <int(aNums.size()) ; aK2++)
            anAMD->AddArc(aNums[aK1],aNums[aK2],true);

}

cEqObsBlockCam * cSetEqFormelles::NewEqBlockCal
                 ( 
                         cRotationFormelle & aRotRT0,
                         cRotationFormelle & aRotLT0,
                         cRotationFormelle & aRotRT1,
                         cRotationFormelle & aRotLT1,
                         bool                doGenerateCode
                 )
{
     ELISE_ASSERT(aRotRT0.Set() == this,"cSetEqFormelles::NewEqBlockCal");

     cEqObsBlockCam * aRes = new cEqObsBlockCam(aRotRT0,aRotLT0,aRotRT1,aRotLT1,doGenerateCode);

     AddObj2Kill(aRes);
     return aRes;
}

void cEqObsBlockCam::GenerateCode()
{
    Pt3d<Fonc_Num>     aTrT0;
    ElMatrix<Fonc_Num> aMatT0(3,3);
    CalcParamEqRel(aTrT0,aMatT0,*mRotRT0,*mRotLT0,0,1);

    Pt3d<Fonc_Num>     aTrT1;
    ElMatrix<Fonc_Num> aMatT1(3,3);
    CalcParamEqRel(aTrT1,aMatT1,*mRotRT1,*mRotLT1,2,3);


    Pt3d<Fonc_Num> aResTr = aTrT1-aTrT0;
    ElMatrix<Fonc_Num> aResMat = aMatT1-aMatT0;

    std::vector<Fonc_Num> aVF;
    aVF.push_back(aResTr.x);
    aVF.push_back(aResTr.y);
    aVF.push_back(aResTr.z);
    for (int aKi=0 ; aKi<3 ; aKi++)
    {
        for (int aKj=0 ; aKj<3 ; aKj++)
        {
           aVF.push_back(aResMat(aKi,aKj));
 
           // aVF.back().show(std::cout); std::cout<<"\n"; getchar();
        }
    }

    cElCompileFN::DoEverything
    (
        DIRECTORY_GENCODE_FORMEL,  // Directory ou est localise le code genere
        mNameType,  // donne les noms de fichier .cpp et .h ainsi que les nom de classe
        aVF,  // expressions formelles 
        mLInterv  // intervalle de reference
    );

}

const std::vector<double> &  cEqObsBlockCam::AddObs(const double & aPdsTr,const double & aPdsMatr)
{
#if  (HandleGL)
   mMatR0.SetEtat(mRotRT0->MGL());
   mMatL0.SetEtat(mRotLT0->MGL());
   mMatR1.SetEtat(mRotRT1->MGL());
   mMatL1.SetEtat(mRotLT1->MGL());
#endif
   //    mGPS.SetEtat(aGPS);
   std::vector<double> aVPds;
   for (int aK=0 ; aK<3; aK++) 
       aVPds.push_back(aPdsTr);

   for (int aK=0 ; aK<9; aK++) 
       aVPds.push_back(aPdsMatr);

  // Compute the values and its derivative (in class cCodeBlockCam here)
  // Link it to the real index of the variable
  // fill the covariance stuff taking into account these values
  return mSet->VAddEqFonctToSys(mFoncEqResidu,aVPds,false);
}

void GenerateCodeBlockCam()
{
   cSetEqFormelles aSet;

   ElRotation3D aRot(Pt3dr(0,0,0),0,0,0);
   cRotationFormelle * aRotRT0 = aSet.NewRotation (cNameSpaceEqF::eRotLibre,aRot);
   cRotationFormelle * aRotLT0 = aSet.NewRotation (cNameSpaceEqF::eRotLibre,aRot);
   cRotationFormelle * aRotRT1 = aSet.NewRotation (cNameSpaceEqF::eRotLibre,aRot);
   cRotationFormelle * aRotLT1 = aSet.NewRotation (cNameSpaceEqF::eRotLibre,aRot);


  cEqObsBlockCam * aEOBC = aSet.NewEqBlockCal (*aRotRT0,*aRotLT0,*aRotRT1,*aRotLT1,true);
  DoNothingButRemoveWarningUnused(aEOBC);
}


class cAperoEqBlockTimeRel
{
    public :
         cAperoEqBlockTimeRel(cEqObsBlockCam * anEQ) :
             mEQ (anEQ)
          {
          }
         cEqObsBlockCam & EQ() {return *mEQ;}
    private :
          cEqObsBlockCam * mEQ;
};

/***********************************************************/
/*                                                         */
/*                                                         */
/*                                                         */
/***********************************************************/

cTypeCodageMatr ExportMatr(const ElMatrix<double> & aMat)
{
    cTypeCodageMatr  aCM;

    aMat.GetLig(0, aCM.L1() );
    aMat.GetLig(1, aCM.L2() );
    aMat.GetLig(2, aCM.L3() );
    aCM.TrueRot().SetVal(true);

    return aCM;
}

ElMatrix<double> ImportMat(const cTypeCodageMatr & aCM)
{
   ElMatrix<double> aMat(3,3);

   SetLig(aMat,0,aCM.L1());
   SetLig(aMat,1,aCM.L2());
   SetLig(aMat,2,aCM.L3());

   return aMat;
}



/***********************************************************/
/*                                                         */
/*                                                         */
/*                                                         */
/***********************************************************/


class cIBC_ImsOneTime
{
    public :
        cIBC_ImsOneTime(int aNbCam,const std::string& aNameTime) ;
        void  AddPose(cPoseCam *, int aNum);
        cPoseCam * Pose(int aKP);
        const std::string & NameTime() const {return mNameTime;}

    private :

        std::vector<cPoseCam *> mCams;
        std::string             mNameTime;
};


class cIBC_OneCam
{
      public :
          cIBC_OneCam(const std::string & ,int aNum);
          const int & Num() const;
          const std::string & NameCam() const;
          const bool & V0Init() const;
          void Init0(const cParamOrientSHC & aPSH);
      private :
          std::string mNameCam;
          int         mNum;
          Pt3dr             mC0;
          ElMatrix<double>  mMat0;
          bool              mV0Init;
};



class cImplemBlockCam
{
    public :
         // static cImplemBlockCam * AllocNew(cAppliApero &,const cStructBlockCam,const std::string & anId);
         cImplemBlockCam(cAppliApero & anAppli,const cStructBlockCam,const cBlockCamera & aBl,const std::string & anId );

         void EstimCurOri(const cEstimateOrientationInitBlockCamera &);
         void Export(const cExportBlockCamera &);

         void DoCompensation(const cObsBlockCamRig &);
         void DoAMD(cAMD_Interf * anAMD);

    private :

         cAppliApero &               mAppli;
         cStructBlockCam             mSBC;
         cLiaisonsSHC *              mLSHC;
         cStructBlockCam             mEstimSBC;
         std::string                 mId;

         std::map<std::string,cIBC_OneCam *>   mName2Cam;
         std::vector<cIBC_OneCam *>            mNum2Cam;
         int                                   mNbCam;
         int                                   mNbTime;

         std::map<std::string,cIBC_ImsOneTime *> mName2ITime;
         std::vector<cIBC_ImsOneTime *>          mNum2ITime;
         bool                                    mDoneIFC;
         bool                                    mForCompens;
         bool                                    mForRelCompens;
         bool                                    mForGlobCompens;
         int                                     mKPivot; // Number of Pivot 

         std::vector<cAperoEqBlockTimeRel >      mVectEqRel;
};

    // =================================
    //              cIBC_ImsOneTime
    // =================================

class  cCmp_IOT_Ptr
{
   public :
       bool operator () (cIBC_ImsOneTime * aT1,cIBC_ImsOneTime * aT2)
       {
          return  aT1->NameTime() < aT2->NameTime();
       }
};
static cCmp_IOT_Ptr TheIOTCmp;


cIBC_ImsOneTime::cIBC_ImsOneTime(int aNb,const std::string & aNameTime) :
       mCams     (aNb),
       mNameTime (aNameTime)
{
}

void  cIBC_ImsOneTime::AddPose(cPoseCam * aPC, int aNum) 
{
    cPoseCam * aPC0 =  mCams.at(aNum);
    if (aPC0 != 0)
    {
         std::cout <<  "For cameras " << aPC->Name() <<  "  and  " << aPC0->Name() << "\n";
         ELISE_ASSERT(false,"Conflicting name from KeyIm2TimeCam ");
    }
    
    mCams[aNum] = aPC;
}

cPoseCam * cIBC_ImsOneTime::Pose(int aKP)
{
   return mCams.at(aKP);
}
    // =================================
    //              cIBC_OneCam 
    // =================================

cIBC_OneCam::cIBC_OneCam(const std::string & aNameCam ,int aNum) :
    mNameCam (aNameCam ),
    mNum     (aNum),
    mMat0    (1,1),
    mV0Init  (false)
{
}

const int & cIBC_OneCam::Num() const {return mNum;}
const std::string & cIBC_OneCam::NameCam() const { return mNameCam; }
const bool & cIBC_OneCam::V0Init() const {return mV0Init;}

void cIBC_OneCam::Init0(const cParamOrientSHC & aPOS)
{
    mV0Init = true;
    mC0   = aPOS.Vecteur();
    mMat0 = ImportMat(aPOS.Rot());
    
}


    // =================================
    //       cImplemBlockCam
    // =================================

cImplemBlockCam::cImplemBlockCam
(
     cAppliApero & anAppli,
     const cStructBlockCam aSBC,
     const cBlockCamera &  aParamCreateBC,
     const std::string & anId
) :
      mAppli      (anAppli),
      mSBC        (aSBC),
      mEstimSBC   (aSBC),
      mId         (anId),
      mDoneIFC    (false),
      mForCompens     (false),
      mForRelCompens  (false),
      mForGlobCompens (false),
      mKPivot          (0)
{
    const std::vector<cPoseCam*> & aVP = mAppli.VecAllPose();
   

    // On initialise les camera
    for (int aKP=0 ; aKP<int(aVP.size()) ; aKP++)
    {
          cPoseCam * aPC = aVP[aKP];
          std::string aNamePose = aPC->Name();
          std::pair<std::string,std::string> aPair =   mAppli.ICNM()->Assoc2To1(mSBC.KeyIm2TimeCam(),aNamePose,true);
          std::string aNameCam = aPair.second;
          if (! DicBoolFind(mName2Cam,aNameCam))
          {

               cIBC_OneCam *  aCam = new cIBC_OneCam(aNameCam, (int)mNum2Cam.size());
               mName2Cam[aNameCam] = aCam;
               mNum2Cam.push_back(aCam); 
          }
    }
    mNbCam  = (int)mNum2Cam.size();

    
    // On regroupe les images prises au meme temps
    for (int aKP=0 ; aKP<int(aVP.size()) ; aKP++)
    {
          cPoseCam * aPC = aVP[aKP];
          std::string aNamePose = aPC->Name();
          std::pair<std::string,std::string> aPair =   mAppli.ICNM()->Assoc2To1(mSBC.KeyIm2TimeCam(),aNamePose,true);
          std::string aNameTime = aPair.first;
          std::string aNameCam  = aPair.second;
          
          cIBC_ImsOneTime * aIms =  mName2ITime[aNameTime];
          if (aIms==0)
          {
               aIms = new cIBC_ImsOneTime(mNbCam,aNameTime);
               mName2ITime[aNameTime] = aIms;
               mNum2ITime.push_back(aIms);
          }
          cIBC_OneCam * aCam = mName2Cam[aNameCam];
          aIms->AddPose(aPC,aCam->Num());
    }
    mNbTime = (int)mNum2ITime.size();
    std::sort(mNum2ITime.begin(),mNum2ITime.end(),TheIOTCmp);


    mLSHC = mSBC.LiaisonsSHC().PtrVal();
    mForCompens = aParamCreateBC.UseForBundle().IsInit();
// ## mForCompens => Mettre dans StructBlockCam
//    On peut avoir equation / a calib et  I/I+1 (pour model derive)
    if (mForCompens)
    {
       const cUseForBundle & aUFB = aParamCreateBC.UseForBundle().Val();

       if (aUFB.GlobalBundle())
       {
          mForGlobCompens = true;
          ELISE_ASSERT(false,"GlobalBundle in Rigid Block, still unsupported");
          for 
          (
               std::list<cParamOrientSHC>::const_iterator itPOS=mLSHC->ParamOrientSHC().begin();
               itPOS !=mLSHC->ParamOrientSHC().end();
               itPOS++
          )
          {
             cIBC_OneCam * aCam = mName2Cam[itPOS->IdGrp()];
             ELISE_ASSERT(aCam!=0,"Cannot get cam from IdGrp");
             ELISE_ASSERT(! aCam->V0Init(),"Multiple Init For IdGrp");

//  ## Creer la camera formelle initialisee en fonction de ParamOrientSHC dans aCam
// LA
             std::cout << "xxxxxxxxxxxxxxxx CCCaaaammm " << aCam << "\n";
          }
       }
       if (aUFB.RelTimeBundle())
       {
           mForRelCompens = true;
           for (int aKTime=1 ; aKTime<mNbTime ; aKTime++)
           {
               cIBC_ImsOneTime * aT0 =  mNum2ITime[aKTime-1];
               cIBC_ImsOneTime * aT1 =  mNum2ITime[aKTime];
               for (int aKCam=0 ; aKCam<mNbCam ; aKCam++)
               {
                   if (aKCam != mKPivot)
                   {
                       cPoseCam * aPcR0 = aT0->Pose(mKPivot);
                       cPoseCam * aPcL0 = aT0->Pose(aKCam);
                       cPoseCam * aPcR1 = aT1->Pose(mKPivot);
                       cPoseCam * aPcL1 = aT1->Pose(aKCam);
                       if (aPcR0 && aPcL0 && aPcR1 &&aPcL1)
                       {
                          cEqObsBlockCam * anEq = mAppli.SetEq().NewEqBlockCal
                                                      (
                                                         aPcR0->RF(),
                                                         aPcL0->RF(),
                                                         aPcR1->RF(),
                                                         aPcL1->RF(),
                                                         false
                                                      );

                           mVectEqRel.push_back(cAperoEqBlockTimeRel(anEq));
                       }
                   }
               }
           }
       }
// Creer les equation dans  cIBC_ImsOneTime 
    }

}

// Rajouter structure compens dans SectionObservation


void cImplemBlockCam::DoCompensation(const cObsBlockCamRig & anObs)
{
    if (anObs.GlobalPond().IsInit()) 
    {
       ELISE_ASSERT(mForGlobCompens,"Require GlobCompen, not specify at creation");
    }
    if (anObs.RelTimePond().IsInit()) 
    {
       ELISE_ASSERT(mForRelCompens,"Require RelCompen, not specify at creation");
       cRigidBlockWeighting aRBW = anObs.RelTimePond().Val();
       for (int aKE=0 ; aKE<int(mVectEqRel.size()) ; aKE++)
       {
          cEqObsBlockCam &  anEQ = mVectEqRel[aKE].EQ() ;
          const std::vector<double> & aResidu = anEQ.AddObs(aRBW.PondOnTr(),aRBW.PondOnRot());
          double aSomEcartMat = 0;
          for (int aK=3 ; aK<12 ; aK++)
              aSomEcartMat += ElSquare(aResidu[aK]);
       
          std::cout << aKE << " EcMat  " <<  sqrt(aSomEcartMat)   
                   << " XYZ " <<  aResidu[0] << " " << aResidu[1] << " " << aResidu[2] <<" \n";
       }
    }
}

void cImplemBlockCam::Export(const cExportBlockCamera & aEBC)
{
    MakeFileXML(mEstimSBC,mAppli.ICNM()->Dir()+aEBC.NameFile());
}


void  cImplemBlockCam::DoAMD(cAMD_Interf * anAMD)
{
      for (int aKE=0 ; aKE<int(mVectEqRel.size()) ; aKE++)
      {
          cEqObsBlockCam &  anEQ = mVectEqRel[aKE].EQ() ;
          anEQ.DoAMD(anAMD);
      }
}



void cImplemBlockCam::EstimCurOri(const cEstimateOrientationInitBlockCamera & anEOIB)
{
   cLiaisonsSHC aLSHC;
   for (int aKC=0 ; aKC<mNbCam ; aKC++)
   {
       if (anEOIB.Show().Val())
          std::cout << "=================================================\n";
       Pt3dr aSomTr(0,0,0);
       double aSomP=0;
       ElMatrix<double> aSomM(3,3,0.0);
       for (int aKT=0 ; aKT<mNbTime ; aKT++)
       {
            cIBC_ImsOneTime *  aTime =  mNum2ITime[aKT];
            cPoseCam * aP0 = aTime->Pose(0);
            cPoseCam * aP1 = aTime->Pose(aKC);
            if (aP0 && aP1)
            {
                ElRotation3D  aR0toM = aP0->CurCam()->Orient().inv(); // CONV-ORI
                ElRotation3D  aR1toM = aP1->CurCam()->Orient().inv();

                ElRotation3D aR1to0 = aR0toM.inv() * aR1toM;  //  CONV-ORI

                if (anEOIB.Show().Val())
                {
                    std::cout << "  EstimCurOri " << aP0->Name() <<  " " << aP1->Name() << "\n";
                    std::cout << "    " <<  aR1to0.ImAff(Pt3dr(0,0,0)) 
                                      << " " << aR1to0.teta01() 
                                      << " " << aR1to0.teta02() 
                                      << " " << aR1to0.teta12() 
                                      << "\n";
                }
                aSomTr = aSomTr+ aR1to0.tr();
                aSomM += aR1to0.Mat();
                aSomP++;
            }
       }
       if (aSomP)
       {
           aSomTr = aSomTr / aSomP;
           aSomM *=  1.0/aSomP;
           aSomM = NearestRotation(aSomM);
           ElRotation3D aRMoy(aSomTr,aSomM,true);
           std::cout << "  ==========  AVERAGE =========== \n";
           std::cout << "    " <<  aRMoy.ImAff(Pt3dr(0,0,0))
                               << " tetas " << aRMoy.teta01() 
                               << "  " << aRMoy.teta02() 
                               << "  " << aRMoy.teta12() 
                               << "\n";

           cParamOrientSHC aP;
           aP.IdGrp() = mNum2Cam[aKC]->NameCam();
           aP.Vecteur() = aRMoy.ImAff(Pt3dr(0,0,0));
           aP.Rot() = ExportMatr(aSomM);
           aLSHC.ParamOrientSHC().push_back(aP);
       }
   }
   
   mEstimSBC.LiaisonsSHC().SetVal(aLSHC);
}



    // =================================
    //       cAppliApero
    // =================================

void  cAppliApero::AMD_AddBlockCam()
{
   for 
   (
        std::map<std::string,cImplemBlockCam *>::iterator itB = mBlockCams.begin();
        itB != mBlockCams.end();
        itB++
   )
   {
       itB->second->DoAMD(mAMD);
   }

}

void cAppliApero::InitBlockCameras()
{
  for 
  (
        std::list<cBlockCamera>::const_iterator itB= mParam.BlockCamera().begin();
        itB!=mParam.BlockCamera().end();
        itB++
  )
  {
       std::string anId = itB->Id().ValWithDef(itB->NameFile());
       cStructBlockCam aSB = StdGetObjFromFile<cStructBlockCam>
                             (
                                 mICNM->Dir() + itB->NameFile(),
                                 StdGetFileXMLSpec("ParamChantierPhotogram.xml"),
                                 "StructBlockCam",
                                 "StructBlockCam"
                             );
       cImplemBlockCam * aIBC = new cImplemBlockCam(*this,aSB,*itB,anId);
       mBlockCams[anId] = aIBC;
  }
}


cImplemBlockCam * cAppliApero::GetBlockCam(const std::string & anId)
{
   cImplemBlockCam* aRes = mBlockCams[anId];
   ELISE_ASSERT(aRes!=0,"cAppliApero::GetBlockCam");

   return aRes;
}

void  cAppliApero::EstimateOIBC(const cEstimateOrientationInitBlockCamera & anEOIB)
{ 
    cImplemBlockCam * aBlock = GetBlockCam(anEOIB.Id());
    aBlock->EstimCurOri(anEOIB);
}


void cAppliApero:: ExportBlockCam(const cExportBlockCamera & aEBC)
{
    cImplemBlockCam * aBlock = GetBlockCam(aEBC.Id());
    aBlock->Export(aEBC);
}

void cAppliApero::AddObservationsRigidBlockCam
     (
         const cObsBlockCamRig & anOBCR,
         bool IsLastIter,
         cStatObs & aSO
     )
{
    cImplemBlockCam * aBlock = GetBlockCam(anOBCR.Id());
    aBlock->DoCompensation(anOBCR);
}


/***********************************************************/
/*                                                         */
/*   BRAS DE LEVIER                                        */
/*                                                         */
/***********************************************************/

cAperoOffsetGPS::cAperoOffsetGPS(const cGpsOffset & aParam,cAppliApero & anAppli) :
    mAppli (anAppli),
    mParam (aParam),
    mBaseUnk (mAppli.SetEq().NewBaseGPS(mParam.ValInit()))
{
}


const cGpsOffset & cAperoOffsetGPS::ParamCreate() const {return mParam;}
cBaseGPS *         cAperoOffsetGPS::BaseUnk()           {return mBaseUnk;}


/***********************************************************/
/*                                                         */
/*               cCompiledObsRelGPS                        */
/*                                                         */
/***********************************************************/

class cCmpPtrPoseTime
{
    public :
      bool operator () (cPoseCam * aPC1,cPoseCam * aPC2)
      {
          return aPC1->Time() < aPC2->Time();
      }
};

cCompiledObsRelGPS::cCompiledObsRelGPS
(
    cAppliApero &     anAppli,
    cDeclareObsRelGPS aXML
)   :
    mXML  (aXML),
    mAppli (&anAppli)
{
   cElRegex  anAutom(mXML.PatternSel(),10);

   const std::vector<cPoseCam*> & aVP = mAppli->VecAllPose();

   for (int aKP=0 ; aKP<int(aVP.size()) ; aKP++)
   {
        cPoseCam * aPose = aVP[aKP];
        if (anAutom.Match(aPose->Name()))
        {
           mVOrderedPose.push_back(aPose);
        }
   }

   cCmpPtrPoseTime aCmp;
   std::sort(mVOrderedPose.begin(),mVOrderedPose.end(),aCmp);

   for (int aKP=1 ; aKP<int(mVOrderedPose.size()) ; aKP++)
   {
      cPoseCam * aPC1 = mVOrderedPose[aKP-1];
      cPoseCam * aPC2 = mVOrderedPose[aKP];

      mVObs.push_back(mAppli->SetEq().NewEqRelativeGPS(aPC1->RF(),aPC2->RF()));
        // std::cout << "TTTT " << mVOrderedPose[aKP]->Name() << " " << mVOrderedPose[aKP]->Time() -  mVOrderedPose[aKP-1]->Time() << "\n";
   }
}

const cDeclareObsRelGPS &             cCompiledObsRelGPS::XML() const {return mXML;}
const std::vector<cPoseCam *> &       cCompiledObsRelGPS::VOrderedPose() const {return mVOrderedPose;}
const std::vector<cEqRelativeGPS *> & cCompiledObsRelGPS::VObs() const {return mVObs;}




void cAppliApero::InitObsRelGPS()
{
   for 
   (
       std::list<cDeclareObsRelGPS>::const_iterator itD=mParam.DeclareObsRelGPS().begin();
       itD != mParam.DeclareObsRelGPS().end() ;
       itD++
   )
   {
       cCompiledObsRelGPS * anObs = new cCompiledObsRelGPS(*this,*itD);
       const std::string  & anId = itD->Id();
       ELISE_ASSERT(mMCORelGps[anId] ==0,"Multiple Id in DeclareObsRelGPS");
       mMCORelGps[anId] = anObs;
   }
}


void  cAppliApero::AddOneObservationsRelGPS(const cObsRelGPS & aXMLObs)
{
     cCompiledObsRelGPS * aCObs = mMCORelGps[aXMLObs.Id()];
     const cGpsRelativeWeighting & aPond = aXMLObs.Pond();
     ELISE_ASSERT(aCObs!=0,"cAppliApero::AddObservationsRelGPS cannot find id");
     const std::vector<cPoseCam *> &  aVP = aCObs->VOrderedPose();
     const std::vector<cEqRelativeGPS *> & aVR = aCObs->VObs();

     for (int aKR = 0 ; aKR < int(aVR.size()) ; aKR++)
     {
         cPoseCam * aPC1 = aVP[aKR];
         cPoseCam * aPC2 = aVP[aKR+1];
         cEqRelativeGPS * anObs =  aVR[aKR];

         Pt3dr aC1 = aPC1->ObsCentre();
         Pt3dr aC2 = aPC2->ObsCentre();
         Pt3dr aDif21 = aC2-aC1;

         Pt3dr aResidu = anObs->Residu(aDif21);
         if ((! aPond.MaxResidu().IsInit()) || (euclid(aResidu) < aPond.MaxResidu().Val()))
         {
             double aT1 = aPC1->Time();
             double aT2 = aPC2->Time();
             double aSigma = aPond.SigmaMin() + aPond.SigmaPerSec() * ElAbs(aT1-aT2);
             Pt3dr aPds(1/ElSquare(aSigma),1/ElSquare(aSigma),1/ElSquare(aSigma));
             anObs->AddObs(aDif21,aPds);

             std::cout << "RELGPS " << aPC1->Name() << " " << aResidu 
                      << " D=" << euclid(aResidu) 
                      << " Sig0 " << aSigma<< "\n";
         }
     }
}

void  cAppliApero::AddObservationsRelGPS(const std::list<cObsRelGPS> & aLO)
{
    for (std::list<cObsRelGPS>::const_iterator itO=aLO.begin() ; itO!=aLO.end() ; itO++)
       AddOneObservationsRelGPS(*itO);
}

/*Footer-MicMac-eLiSe-25/06/2007

Ce logiciel est un programme informatique servant à la mise en
correspondances d'images pour la reconstruction du relief.

Ce logiciel est régi par la licence CeCILL-B soumise au droit français et
respectant les principes de diffusion des logiciels libres. Vous pouvez
utiliser, modifier et/ou redistribuer ce programme sous les conditions
de la licence CeCILL-B telle que diffusée par le CEA, le CNRS et l'INRIA 
sur le site "http://www.cecill.info".

En contrepartie de l'accessibilité au code source et des droits de copie,
de modification et de redistribution accordés par cette licence, il n'est
offert aux utilisateurs qu'une garantie limitée.  Pour les mêmes raisons,
seule une responsabilité restreinte pèse sur l'auteur du programme,  le
titulaire des droits patrimoniaux et les concédants successifs.

A cet égard  l'attention de l'utilisateur est attirée sur les risques
associés au chargement,  à l'utilisation,  à la modification et/ou au
développement et à la reproduction du logiciel par l'utilisateur étant 
donné sa spécificité de logiciel libre, qui peut le rendre complexe à 
manipuler et qui le réserve donc à des développeurs et des professionnels
avertis possédant  des  connaissances  informatiques approfondies.  Les
utilisateurs sont donc invités à charger  et  tester  l'adéquation  du
logiciel à leurs besoins dans des conditions permettant d'assurer la
sécurité de leurs systèmes et ou de leurs données et, plus généralement, 
à l'utiliser et l'exploiter dans les mêmes conditions de sécurité. 

Le fait que vous puissiez accéder à cet en-tête signifie que vous avez 
pris connaissance de la licence CeCILL-B, et que vous en avez accepté les
termes.
Footer-MicMac-eLiSe-25/06/2007*/
