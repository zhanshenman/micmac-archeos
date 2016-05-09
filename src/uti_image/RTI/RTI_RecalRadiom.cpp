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

#include "RTI.h"


class cAppli_RecalRadio : public  cAppliWithSetImage
{
     public :
          cAppli_RecalRadio(int argc,char ** argv);

          std::string mNameI1;
          std::string mNameMaster;
          std::string mNameParam;
          Pt2di       mSz;
};

cAppli_RecalRadio::cAppli_RecalRadio(int argc,char ** argv) :
    cAppliWithSetImage(argc-1,argv+1,TheFlagNoOri)
{
    const cInterfChantierNameManipulateur::tSet * aSetIm = mEASF.SetIm();

    if (aSetIm->size() ==0)
    {
          ELISE_ASSERT(false,"No image in RTI_RR");
    }
    else if (aSetIm->size() > 1)
    {
         ExpandCommand(3,"",true);
         return;
    }

    ElInitArgMain
    (
         argc,argv,
         LArgMain()  << EAMC(mNameI1, "Name Image 1",  eSAM_IsPatFile)
                     << EAMC(mNameParam, "Name XML", eSAM_IsExistFile),
         LArgMain()  << EAM(mNameMaster, "Master",true,"Name of master image if != def master", eSAM_IsExistFile)
    );

   cAppli_RTI aRTIA(mNameParam,mNameI1);


   cOneIm_RTI_Slave * aSl = aRTIA.UniqSlave();
   // cOneIm_RTI_Master * aMas = aRTIA.Master();

/*
   cOneIm_RTI * aMas = aRTIA.Master();
   if (EAMIsInit(&mNameMaster))
   {
       cAppli_RTI * aRTIA2 = new cAppli_RTI (mNameParam,mNameMaster);
       aMas =  aRTIA2->UniqSlave();
   }
*/

   //   Tiff_Im aTif2 = aSl->DoImReduite();
   //   Tiff_Im aTifMasq (aSl->NameMasqR().c_str());
   //   Tiff_Im aTif1 = aMas->DoImReduite();
   Tiff_Im aTif1 = aSl->ImFull();
   Tiff_Im aTifMasq = aSl->MasqFull();
   Tiff_Im aTif2("Mediane.tif");

   mSz = aTif2.sz();
   ELISE_ASSERT((aTifMasq.sz()==mSz) && (aTif1.sz()==mSz),"Sz incohe in RTIRecalRadiom");
 
   Im2D_REAL4 aI1(mSz.x,mSz.y);
   ELISE_COPY(aTif1.all_pts(),aTif1.in(),aI1.out());
   TIm2D<REAL4,REAL8> aTI1(aI1);

   Im2D_REAL4 aI2(mSz.x,mSz.y);
   ELISE_COPY(aTif2.all_pts(),aTif2.in(),aI2.out());
   TIm2D<REAL4,REAL8> aTI2(aI2);

   Im2D_Bits<1> aIM(mSz.x,mSz.y);
   ELISE_COPY(aTifMasq.all_pts(),aTifMasq.in(),aIM.out());
   TIm2DBits<1> aTIM(aIM);

   
   Im2D_Bits<1> aIMDil (mSz.x,mSz.y);

   ELISE_COPY
   (
        aIM.all_pts(),
        dilat_d4(aIM.in(0) && (aI2.in(1e9)>aRTIA.Param().SeuilSat().Val()) ,1),  
        aIMDil.out()
   );
   

   
   int aStep = 5;
   Pt2di aP;
   std::vector<double> aV1s2;
   for (aP.x =0 ; aP.x<mSz.x ; aP.x+= aStep)
   {
       for (aP.y =0 ; aP.y<mSz.y ; aP.y+= aStep)
       {
           if (aTIM.get(aP))
           {
                double aRatio = aTI1.get(aP) / ElMax(0.1,aTI2.get(aP));
                aV1s2.push_back(aRatio);
           }
       }
   }
   double aMed = MedianeSup(aV1s2);
   std::cout << "MEDIAN= " << aMed << "\n";
   Tiff_Im::CreateFromFonc 
   (
          "RATIO"+aSl->Name()+".tif",
          mSz, 
          (aI1.in() /(Max(0.1,aI2.in())*aMed)),
          GenIm::real4
    );
   
}


int RTIRecalRadiom_main(int argc,char ** argv)
{
     cAppli_RecalRadio anAppli(argc,argv);

     return EXIT_SUCCESS;
}


void cAppli_RTI::MakeImageMed(const Box2di & aBox)
{
   std::cout << " cAppli_RTI::MakeImageMed " << aBox._p0 << "\n";
   Pt2di aSz = aBox.sz();

  std::vector<Im2D_REAL4 *> aVIm;
  std::vector<TIm2D<REAL4,REAL8> *> aVTIm;
  Im2D_REAL4 aMed(aSz.x,aSz.y);
  TIm2D<REAL4,REAL8>  aTMed(aMed);

   for (int aK=0 ; aK<int(mVIms.size()) ; aK++)
   {
       aVIm.push_back(new Im2D_REAL4(aSz.x,aSz.y));
       aVTIm.push_back(new TIm2D<REAL4,REAL8>(*(aVIm.back())));

       ELISE_COPY
       (
           aVIm.back()->all_pts(),
           trans(mVIms[aK]->ImFull().in(),aBox._p0),
           aVIm.back()->out()
       );
   }

   Pt2di aP;
   for (aP.x=0 ; aP.x<aSz.x ; aP.x++)
   {
       for (aP.y=0 ; aP.y<aSz.y ; aP.y++)
       {
           std::vector<double> aVVals;
           for (int aK=0 ; aK<int(mVIms.size()) ; aK++)
           {
               aVVals.push_back(aVTIm[aK]->get(aP));
           }
           aTMed.oset(aP,MedianeSup(aVVals));
       }
   }

   Tiff_Im aTifMed(mNameImMed.c_str());
   ELISE_COPY
   (
       rectangle(aBox._p0,aBox._p1),
       trans(aMed.in(),-aBox._p0),
       aTifMed.out()
   );
}


void cAppli_RTI::MakeImageMed()
{
   mNameImMed = "Mediane.tif";
   Tiff_Im aTif1 = mMasterIm->ImFull();

   Tiff_Im
   (
       mNameImMed.c_str(),
       aTif1.sz(),
       aTif1.type_el(),
       Tiff_Im::No_Compr,
       aTif1.phot_interp()
   );


   Pt2di aSz = aTif1.sz();


   int aBloc = 500;

   Pt2di aP0;

   for (aP0.x=0 ; aP0.x<aSz.x ; aP0.x+=aBloc)
   {
       for (aP0.y=0 ; aP0.y<aSz.y ; aP0.y+=aBloc)
       {
            Pt2di aP1 = Inf(aSz,aP0+Pt2di(aBloc,aBloc));
            MakeImageMed(Box2di(aP0,aP1));
       }
   }
   
}

int RTIMed_main(int argc,char **argv)
{
    std::string aNameParam;
    ElInitArgMain
    (
         argc,argv,
         LArgMain()  << EAMC(aNameParam, "Name XML", eSAM_IsExistFile),
         LArgMain()  
    );

   cAppli_RTI aRTIA(aNameParam);
   aRTIA.MakeImageMed();
  
   return EXIT_SUCCESS;
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
