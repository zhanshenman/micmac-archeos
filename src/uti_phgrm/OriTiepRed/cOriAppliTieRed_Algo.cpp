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

#include "OriTiepRed.h"
NS_OriTiePRed_BEGIN

// bool BUGTR = true;



/**********************************************************************/
/*                                                                    */
/*                         cAppliTiepRed                              */
/*                                                                    */
/**********************************************************************/




void cAppliTiepRed::DoLoadTiePoints()
{
   // Load Tie Points in box
   for (int aKI = 0 ; aKI<int(mVecCam.size()) ; aKI++)
   {
       cCameraTiepRed & aCam1 = *(mVecCam[aKI]);
       const std::string & anI1 = aCam1.NameIm();
       // Get list of images sharin tie-P with anI1
       std::list<std::string>  aLI2 = mNM->ListeImOrientedWith(anI1);
       for (std::list<std::string>::const_iterator itL= aLI2.begin(); itL!=aLI2.end() ; itL++)
       {
            const std::string & anI2 = *itL;
            // Test if the file anI2 is in the current pattern
            // As the martini result may containi much more file 
            if (mSetFiles->find(anI2) != mSetFiles->end())
            {
               // The result being symetric, the convention is that some data are stored only for  I1 < I2
               if (anI1 < anI2)
               {
                   cCameraTiepRed & aCam2 = *(mMapCam[anI2]);
                   aCam1.LoadHom(aCam2);
               }
            }
       }
   }
}

void cAppliTiepRed::DoFilterCamAnLinks()
{
   // Select Cam ( and Link ) with enough points, and give a numeration to camera
   {
      // Suppress the links if one of its camera was supressed
      std::list<cLnk2ImTiepRed *> aNewL;
      for (std::list<cLnk2ImTiepRed *>::const_iterator itL=mLnk2Im.begin() ; itL!=mLnk2Im.end() ; itL++)
      {
          // if the two camera are to preserve, then preserve the link
          if ((*itL)->Cam1().SelectOnHom2Im() && (*itL)->Cam2().SelectOnHom2Im())
             aNewL.push_back(*itL);
      }
      std::cout << "   LNK " << mLnk2Im.size() << " " << aNewL.size() << "\n";
      mLnk2Im = aNewL;

      std::vector<cCameraTiepRed *>  aNewV; // Filtered camera
      int aNum=0; // Current number
      for (int aKC=0 ; aKC<int(mVecCam.size()) ; aKC++)
      {
          if (mVecCam[aKC]->SelectOnHom2Im()) // If enouh point  camera is to preserve
          {
             aNewV.push_back(mVecCam[aKC]); // Add to new vec
             mVecCam[aKC]->SetNum(aNum);    // Give current numeration
             aNum++;
          }
          else
          {
             // Forget this camera
             mMapCam[mVecCam[aKC]->NameIm()] = 0;
             // delete mVecCam[aKC];
          }
      }
      std::cout << "   CAMSS " << mVecCam.size() << " " << aNewV.size() << " NUM=" << aNum << "\n";
      mVecCam = aNewV; // Update member vector of cams

      mBufICam = std::vector<int>(mVecCam.size(),0);
   }
}

void Verif(Pt2df aPf)
{
   Pt2dr aPd = ToPt2dr(aPf);
   if (std_isnan(aPd.x) || std_isnan(aPd.y))
   {
       std::cout << "PB PTS " << aPf << " => " << aPd << "\n";
       ELISE_ASSERT(false,"PB PTS in Verif");
   }
}


void cAppliTiepRed::DoReduceBox()
{


    DoLoadTiePoints();
    DoFilterCamAnLinks();
    // == OK

   // merge topological tie point

    // Create an empty merging struct
    mMergeStruct  = new  tMergeStr(mVecCam.size(),true);
    // for each link do the mergin
    for (std::list<cLnk2ImTiepRed *>::const_iterator itL=mLnk2Im.begin() ; itL!=mLnk2Im.end() ; itL++)
    {
        (*itL)->Add2Merge(mMergeStruct);
    }
    mMergeStruct->DoExport();                  // "Compile" to make the point usable
    mLMerge =  & mMergeStruct->ListMerged();    // Get the merged multiple points
    std::vector<int> aVHist(mVecCam.size()+1,0);

    // Compute the average 
    double aSzTileAver = sqrt(mBoxLoc.surf()/mLMerge->size()); 

    
    // Quod tree for spatial indexation
    mQT = new tTiePRed_QT ( mPMul2Gr, mBoxLoc, 5  /* 5 obj max  box */, 2*aSzTileAver);
    // Heap for priority management


    // OK
   // give ground coord to multiple point and put them in quod-tree  and  heap 
    {
       std::vector<double> aVPrec;
       mVPM.reserve(mLMerge->size());

       for (std::list<tMerge *>::const_iterator itM=mLMerge->begin() ; itM!=mLMerge->end() ; itM++)
       {
           cPMulTiepRed * aPM = new cPMulTiepRed(*itM,*this);
           if (mBoxLoc.inside(aPM->Pt()))
           {
              mVPM.push_back(aPM);
              
              aVPrec.push_back(aPM->Prec());

              ELISE_ASSERT((*itM)->NbSom()<int(aVHist.size()),"JJJJJJJJJJJJJJJJJJJJ");
              aVHist[(*itM)->NbSom()] ++;
              mQT->insert(aPM);
           }
           else
           {
              delete aPM;
           }
       }
       if (aVPrec.size() ==0)
       {   
          return;
       }
       mStdPrec = MedianeSup(aVPrec);  

       mHeap = new tTiePRed_Heap(mPMulCmp);
       // The gain can be computed once we know the standard precision
       for (int aKP=0 ; aKP<int(mVPM.size()) ; aKP++)
       {
           mVPM[aKP]->InitGain(*this);
           mHeap->push(mVPM[aKP]);
       }
    }
    int aNbInit = mHeap->nb();
// PAs OK

    tPMulTiepRedPtr aPMPtr;
    while (mHeap->pop(aPMPtr))
    {
          mListSel.push_back(aPMPtr);
          aPMPtr->Remove();
          aPMPtr->SetSelected();
          std::set<tPMulTiepRedPtr>  aSetNeigh; // = *(new std::set<tPMulTiepRedPtr>);
          double aDist= mDistPMul * mResol;
          mQT->RVoisins(aSetNeigh,aPMPtr->Pt(),aDist);

          for (std::set<tPMulTiepRedPtr>::const_iterator itS=aSetNeigh.begin() ; itS!=aSetNeigh.end() ; itS++)
          {

              // tPMulTiepRedPtr aNeigh = aSetNeigh[aK];
              tPMulTiepRedPtr aNeigh = *itS;
              if (! aNeigh->Removed())
              {
                  aNeigh->UpdateNewSel(aPMPtr,*this);
                  if (aNeigh->Removable())
                  {
                      aNeigh->Remove();
                      mQT->remove(aNeigh);
                      mHeap->Sortir(aNeigh);
                  }
              }
          }
          /* std::cout << "         GAIN " << aPMPtr->Gain() << " " 
                       <<  aPMPtr->Merge()->NbArc()  << " " <<  aPMPtr->Merge()->NbSom() 
                       << " " << aSetNeigh.size() << " " <<  mHeap->nb() << "\n";
          */
          

    }

    VonGruber();

    // PAOK
    std::cout << "NBPTS " <<  aNbInit << " => " <<  mListSel.size() << "\n";
    DoExport();
}

/*

void cAppliTiepRed::VonGruber()
{
     for (int aK1=0 ; aK1<int(mVecCam.size()) ; aK1++)
     {
         std::vector<tPMulTiepRedPtr> aVK1;
         for (int aKP=0 ; aKP<int(mVPM.size()) ; aKP++)
         {
             if (mVPM[aKP]->Merge()->IsInit(aK1))
                aVK1.push_back(mVPM[aKP]);
         }
         std::cout << "VonGrube " << aK1 << " " << aVK1.size() << "\n";



         for (int aK2=aK1+1 ; aK2<int(mVecCam.size()) ; aK2++)
         {
              VonGruber(aVK1,mVecCam[aK1],mVecCam[aK2]);
         }
     }
}

void cAppliTiepRed::VonGruber(const std::vector<tPMulTiepRedPtr> & aVK1,cCameraTiepRed * aCam1,cCameraTiepRed * aCam2)
{
    std::vector<tPMulTiepRedPtr> aVK1K2;

    for (int aKP=0 ; aKP<int(aVK1.size()) ; aKP++)
    {
        if (aVK1[aKP]->Merge()->IsInit(aCam2->Num())  && (aVK1[aKP]->Prec() < (0.5+2*StdPrec())))
           aVK1K2.push_back(aVK1[aKP]);
    }
    if (aVK1K2.size() ==0) 
       return;

    double aDistMax = euclid(mBoxLoc.sz());
    int aNbInside=0;
    for (int aKP=0 ; aKP<int(aVK1K2.size()) ; aKP++)
    {
        tPMulTiepRedPtr aSom = aVK1K2[aKP];
        aSom->SetDistVonGruber(aDistMax,*this);
        if (aSom->Selected())
        {
           mQT->insert(aSom);
           aNbInside++;
        }
    }


    double aDistMoy = sqrt(mBoxLoc.surf()/(1+aNbInside));

    for (int aKP=0 ; aKP<int(aVK1K2.size()) ; aKP++)
    {
        tPMulTiepRedPtr aSom = aVK1K2[aKP];
        double aDist = aDistMax;
        if ((!aSom->Selected()) && (aNbInside > 0))
        {
           tPMulTiepRedPtr aNearObj = mQT->NearestObj(aSom->Pt(),aDistMoy,aDistMax);
           aDist = euclid(aNearObj->Pt(),aSom->Pt());
           aSom->ModifDistVonGruber(aDist,*this);
        }
    }

    bool Cont=true;
    while (Cont)
    {
        double aMaxDist  = 0;
        double aGainMax  = 0;
        tPMulTiepRedPtr aMaxSom = 0;
        for (int aKP=0 ; aKP<int(aVK1K2.size()) ; aKP++)
        {
             tPMulTiepRedPtr aSom = aVK1K2[aKP];
             if (!aSom->Selected()) 
             {
                 if (aSom->Gain() > aGainMax)
                 {
                      aGainMax = aSom->Gain();
                      aMaxSom = aSom;
                 }
                 aMaxDist = ElMax(aMaxDist,aSom->DMin());
             }
        }
    }


    std::cout << "HHHHhh " << aVK1K2.size() << "\n";


    mQT->clear();
}

*/




void cAppliTiepRed::DoExport()
{


    int aNbCam = mVecCam.size();
    std::vector<std::vector<ElPackHomologue> > aVVH (aNbCam,std::vector<ElPackHomologue>(aNbCam));
    for (std::list<tPMulTiepRedPtr>::const_iterator itP=mListSel.begin(); itP!=mListSel.end();  itP++)
    {
         tMerge * aMerge = (*itP)->Merge();
         const std::vector<Pt2dUi2> &  aVE = aMerge->Edges();
         for (int aKCple=0 ; aKCple<int(aVE.size()) ; aKCple++)
         {
              int aKCam1 = aVE[aKCple].x;
              int aKCam2 = aVE[aKCple].y;
              cCameraTiepRed * aCam1 = mVecCam[aKCam1];
              cCameraTiepRed * aCam2 = mVecCam[aKCam2];

              /*
                  if ((aCam1->NameIm() == "Abbey-IMG_0207.jpg") && (aCam2->NameIm() == "Abbey-IMG_0206.jpg"))
                  {
                     std::cout << "cAppliTiepRed::DoExportcAppliTiepRed::DoExport\n";
                  }
              */


              Pt2df aP1 = aMerge->GetVal(aKCam1);
              Pt2df aP2 = aMerge->GetVal(aKCam2);

              Pt2dr aQ1 = aCam1->Hom2Cam(aP1);
              Pt2dr aQ2 = aCam2->Hom2Cam(aP2);

              aVVH[aKCam1][aKCam2].Cple_Add(ElCplePtsHomologues(aQ1,aQ2));
              // Symetrisation
              aVVH[aKCam2][aKCam1].Cple_Add(ElCplePtsHomologues(aQ2,aQ1));

              if (VerifNM())
              {
                   // Pt2dr aW1 = mNM->CalibrationCamera(aCam1->NameIm());
                   // std::cout << "FFFFffGG  :" << mNM->CalibrationCamera(aCam1->NameIm())->Radian2Pixel(Pt2dr(aP1.x,aP1.y)) - aQ1 << "\n";
              }
               

              Verif(aP1);
              Verif(aP2);
         }
    }

    int aSomH=0;
    for (int aKCam1=0 ; aKCam1<aNbCam ; aKCam1++)
    {
        for (int aKCam2=0 ; aKCam2<aNbCam ; aKCam2++)
        {
             const ElPackHomologue & aPack = aVVH[aKCam1][aKCam2];
             aSomH += aPack.size();
             if (aPack.size())
             {
                  aPack.StdPutInFile(NameHomol(mVecCam[aKCam1]->NameIm(),mVecCam[aKCam2]->NameIm(),mKBox));
             }
        }
    }
}

NS_OriTiePRed_END


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
