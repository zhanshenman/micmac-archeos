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

#include "StdAfx.h"
#include <algorithm>

class cAppliLiquor;
class cIntervLiquor;

// ffmpeg -i MVI_0001.MOV  -ss 30 -t 20 Im%5d_Ok.png

// Im*_Ok => OK
// Im*_Nl => Image Nulle (eliminee)


void BanniereLiquor()
{
    std::cout <<  "\n";
    std::cout <<  " *********************************************\n";
    std::cout <<  " *     LI-near                               *\n";
    std::cout <<  " *     QU-ick                                *\n";
    std::cout <<  " *     OR-ientation                          *\n";
    std::cout <<  " *********************************************\n\n";

}

//=================================================

class cIntervLiquor
{
     public :

         cIntervLiquor(cAppliLiquor * anAppli,int aBegin,int aEnd,int aProf);
         int Num()   const  {return mNum;}
         int Begin() const  {return mBegin;}
         int End()   const  {return mEnd;}
         std::string  NameOri() const {return "Liquor_" +  ((mProf==0) ? "Final" : ToString(mNum));}
         std::string  NameMerge() const {return "MergeLiq_" + ToString(mNum);}
         void SetF1(cIntervLiquor * aIL) {mF1=aIL;}
         void SetF2(cIntervLiquor * aIL) {mF2=aIL;}
         cIntervLiquor * F1() {return mF1;}
         cIntervLiquor * F2() {return mF2;}

     private :
         static int      TheCpt;

         cAppliLiquor *  mAppli;
         int             mBegin;
         int             mEnd;
         int             mProf;
         int             mNum;
         cIntervLiquor * mF1;
         cIntervLiquor * mF2;
};


class cAppliLiquor
{
    public :
        cAppliLiquor(int argc,char ** argv);
        const std::string & Dir() {return mEASF.mDir;}


    private :
        cIntervLiquor * SplitRecInterv(int aDeb,int aEnd,int aProf);
        std::string ComTerm(const  cIntervLiquor&) const;
        void DoComTerm();
        void DoComRec(int aLevel);
        std::string  StrImMinMax(const  cIntervLiquor& anIL) const;


        std::string mFullName;
        std::string mCalib;
        cElemAppliSetFile mEASF;
        const std::vector<std::string> * mVNames;
        std::vector<std::list<cIntervLiquor*> > mInterv;

        int                              mNbIm;
        int                              mSzLim;
        int                              mOverlapMin;  // Il faut un peu de redondance
        int                              mOverlapMax;  // Si redondance trop grande, risque de divergence au raccord
        double                           mOverlapProp; // entre les 2, il peut sembler logique d'avoir  une raccord prop
};

// =============  cIntervLiquor ===================================

int cIntervLiquor::TheCpt=0;

cIntervLiquor::cIntervLiquor(cAppliLiquor * anAppli,int aBegin,int aEnd,int aProf) :
   mAppli (anAppli),
   mBegin (aBegin),
   mEnd   (aEnd),
   mProf  (aProf),
   mNum   (TheCpt++),
   mF1    (0),
   mF2    (0)
{
}




// =============  cAppliLiquor ===================================

cAppliLiquor::cAppliLiquor(int argc,char ** argv)  :
    mSzLim       (40),
    mOverlapMin  (3),
    mOverlapMax  (40),
    mOverlapProp (0.1)
{


    ElInitArgMain
     (
           argc,argv,
           LArgMain() << EAMC(mFullName,"Full name (Dir+Pat)", eSAM_IsPatFile)
                      << EAMC(mCalib,"Calibration Dir",eSAM_IsExistDirOri),
           LArgMain() << EAM(mSzLim,"SzInit",true,"Sz of initial interval (Def=50)")
                      << EAM(mOverlapProp,"OverLap",true,"Prop overlap (Def=0.1) ")
    );

    if (MMVisualMode) return;

    mEASF.Init(mFullName);
    mVNames = mEASF.SetIm();
    mNbIm = (int)mVNames->size();
    StdCorrecNameOrient(mCalib,Dir());


    SplitRecInterv(0,mNbIm,0);
    DoComTerm();

    for (int aLevel = (int)(mInterv.size() - 2);  aLevel>=0 ;  aLevel--)
    {
         DoComRec(aLevel);
    }
}


void  cAppliLiquor::DoComRec(int aLevel)
{
   std::list<std::string> aLComMerge;
   for
   (
        std::list<cIntervLiquor*>::iterator II=mInterv[aLevel].begin();
        II!=mInterv[aLevel].end();
        II++
   )
   {
        cIntervLiquor & anIL = **II;
        std::string aComMerge =    MM3dBinFile("Morito")
                                + "Ori-"+ anIL.F1()->NameOri() + std::string("/Orientation.*xml ")
                                + "Ori-"+ anIL.F2()->NameOri() + std::string("/Orientation.*xml ")
                                +  anIL.NameMerge();

         aLComMerge.push_back(aComMerge);
   }
   cEl_GPAO::DoComInParal(aLComMerge);

   std::list<std::string> aLComComp;
   for
   (
        std::list<cIntervLiquor*>::iterator II=mInterv[aLevel].begin();
        II!=mInterv[aLevel].end();
        II++
   )
   {
        cIntervLiquor & anIL = **II;
        std::string aComComp =     MM3dBinFile("Campari")
                                +  mFullName  + " "
                                +  anIL.NameMerge() + " "
                                +  anIL.NameOri()  + " "
                                +  StrImMinMax(anIL)
                                +  " SigmaTieP=2.0 ";

        if (aLevel==0)
        {
              // aComComp = aComComp + " AllFree=true ";
        }
        // std::cout << aComComp << "\n";
        aLComComp.push_back(aComComp);
   }
   cEl_GPAO::DoComInParal(aLComComp);
}


void cAppliLiquor::DoComTerm()
{
   std::list<std::string> aLComInit;
   for
   (
        std::list<cIntervLiquor*>::iterator II=mInterv.back().begin();
        II!=mInterv.back().end();
        II++
   )
   {
        std::string aCom = ComTerm(**II) ;
        aLComInit.push_back(aCom);
        std::cout << aCom << "\n";
   }
   cEl_GPAO::DoComInParal(aLComInit);
}


cIntervLiquor * cAppliLiquor::SplitRecInterv(int aDeb,int aEnd,int aProf)
{
   cIntervLiquor * aRes =  new cIntervLiquor(this,aDeb,aEnd,aProf);
   int aLarg = aEnd-aDeb;
   if (aLarg < mSzLim)
   {
       // std::cout << "INTERV " << aDeb << " " << aEnd << "\n";

   }
   else
   {
         int anOverlap = ElMax(mOverlapMin,ElMin(mOverlapMax,round_ni(aLarg*mOverlapProp)));
         int aNewLarg = round_up((aLarg + anOverlap)/2.0);

         aRes->SetF1(SplitRecInterv(aDeb,aDeb+aNewLarg,aProf+1));
         aRes->SetF2(SplitRecInterv(aEnd-aNewLarg,aEnd,aProf+1));
   }

   for (int aP = (int)mInterv.size() ; aP<=aProf ; aP++)
   {
      std::list<cIntervLiquor*> aL;
      mInterv.push_back(aL);
   }
   mInterv[aProf].push_back(aRes);


   return aRes;
}

std::string  cAppliLiquor::StrImMinMax(const  cIntervLiquor& anIL) const
{
   std::string aN1  = (*mVNames)[anIL.Begin()];
   std::string aN2  = (*mVNames)[anIL.End()-1];
   return  std::string(" ImMinMax=[" +aN1+ "," + aN2 + "] ");
}

std::string cAppliLiquor::ComTerm(const  cIntervLiquor& anIL) const
{

   // std::string aN1  = (*mVNames)[anIL.Begin()];
   // std::string aN2  = (*mVNames)[anIL.End()-1];
   std::string aNMil  = (*mVNames)[(anIL.End()+anIL.Begin())/2];
   std::string aOut = anIL.NameOri();


   std::string aCom = MM3dBinFile("Tapas")
                      + " Figee "
                      + mFullName
                      + std::string(" InCal=" + mCalib)
                      // + std::string(" ImMinMax=[" +aN1+ "," + aN2 + "] ")
                      + StrImMinMax(anIL)
                      + std::string(" ImInit=" +aNMil + " ")
                      + std::string(" Out=" + aOut + " ")
                      + std::string(" RefineAll=false ")
                      + std::string(" SauvAutom=NONE ")
                      ;

   return aCom;
}

//========================== :: ===========================

int Liquor_main(int argc,char ** argv)
{
    cAppliLiquor anAppli(argc,argv);

    BanniereLiquor();
    return EXIT_SUCCESS;
}


/*Footer-MicMac-eLiSe-25/06/2007

Ce logiciel est un programme informatique servant �  la mise en
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
associés au chargement,  �  l'utilisation,  �  la modification et/ou au
développement et �  la reproduction du logiciel par l'utilisateur étant
donné sa spécificité de logiciel libre, qui peut le rendre complexe �
manipuler et qui le réserve donc �  des développeurs et des professionnels
avertis possédant  des  connaissances  informatiques approfondies.  Les
utilisateurs sont donc invités �  charger  et  tester  l'adéquation  du
logiciel �  leurs besoins dans des conditions permettant d'assurer la
sécurité de leurs systèmes et ou de leurs données et, plus généralement,
�  l'utiliser et l'exploiter dans les mêmes conditions de sécurité.

Le fait que vous puissiez accéder �  cet en-tête signifie que vous avez
pris connaissance de la licence CeCILL-B, et que vous en avez accepté les
termes.
Footer-MicMac-eLiSe-25/06/2007*/
