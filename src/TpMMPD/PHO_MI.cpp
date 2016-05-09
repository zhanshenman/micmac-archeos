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
#include "kugelhupf.h"
#include "../uti_phgrm/NewOri/NewOri.h"
#include <algorithm>
#include "PHO_MI.h"
#if ELISE_windows
    #define uint unsigned
#endif



PS_Window PS(char * name, bool auth_lzw = false)
{
      // sz of images we will use

         Pt2di SZ(256,256);

     //  palette allocation

         Disc_Pal  Pdisc = Disc_Pal::P8COL();
         Gray_Pal  Pgr (80);
         Circ_Pal  Pcirc = Circ_Pal::PCIRC6(30);
         BiCol_Pal Prb  (
                           Elise_colour::black,
                           Elise_colour::red,
                           Elise_colour::blue,
                           10,10
                       );
         RGB_Pal   Prgb(10,10,10);



         Elise_Set_Of_Palette SOP
                              (
                                      NewLElPal(Pdisc)
                                   +  Elise_Palette(Prb)
                                   +  Elise_Palette(Pgr)
                                   +  Elise_Palette(Prgb)
                                   +  Elise_Palette(Pcirc)
                              );

     // Creation of postscript windows

           char  buf[200];
           sprintf(buf,"DOC/PS/%s.eps",name);

           PS_Display disp(buf,"Mon beau fichier ps",SOP,auth_lzw);

           return  disp.w_centered_max(SZ,Pt2dr(4.0,4.0));
}



/**
 * TestPointHomo: read point homologue entre 2 image après Tapioca
 * Inputs:
 *  - Nom de 2 images
 *  - Fichier de calibration du caméra
 * Output:
 *  - List de coordonné de point homologue
 *  - serie1_Line$ mm3d PHO_MI "soussol_161015_001_0000[6-8].tif" Ori-Serie1/
 * */


void StdCorrecNameHomol_G(std::string & aNameH,const std::string & aDir)
{
    int aL = strlen(aNameH.c_str());
    if (aL && (aNameH[aL-1]==ELISE_CAR_DIR))
        {aNameH = aNameH.substr(0,aL-1);}
    if ((strlen(aNameH.c_str())>=5) && (aNameH.substr(0,5)==std::string("Homol")))
       aNameH = aNameH.substr(5,std::string::npos);
    std::string aTest =  ( isUsingSeparateDirectories()?MMOutputDirectory():aDir ) + "Homol"+aNameH+ ELISE_CAR_DIR;
}


vector<AbreHomol> creatAbreFromPattern(vector<string> aSetImages, string aNameHomol, string aFullPatternImages, string aOriInput)
{
    //=============Manip File Name=====================
      ELISE_fp::AssertIsDirectory(aNameHomol);

      // Initialize name manipulator & files
      std::string aDirImages, aPatImages;
      SplitDirAndFile(aDirImages,aPatImages,aFullPatternImages);

      StdCorrecNameOrient(aOriInput,aDirImages);//remove "Ori-" if needed

      cInterfChantierNameManipulateur * aICNM=cInterfChantierNameManipulateur::BasicAlloc(aDirImages);

      ELISE_ASSERT(aSetImages.size()>1,"Number of image must be > 1");
      vector<string> aSetImg = aSetImages;
    //======================================================
    vector<string> tempArbeRacine;
    vector<AbreHomol> Abre;
    for (uint i=0; i<aSetImg.size(); i++)
    {
        AbreHomol aAbre;
        for (uint j=0; j<aSetImg.size(); j++)
        {
            string nameImgB = aSetImg[j];
            string nameImgA = aSetImg[i];

            std::string aKey =   std::string("NKS-Assoc-CplIm2Hom@")
                    +  std::string(aNameHomol)
                    +  std::string("@")
                    +  std::string("dat");
            std::string aHomol = aICNM->Assoc1To2(aKey,nameImgA,nameImgB,true);
            StdCorrecNameHomol_G(aHomol,aDirImages);

            bool Exist = ELISE_fp::exist_file(aHomol);
            if (Exist)
            {
                //creer abre de Homol
                aAbre.ImgRacine = nameImgA;
                aAbre.ImgBranch.push_back(nameImgB);
                ElPackHomologue aPackIn =  ElPackHomologue::FromFile(aHomol);
                aAbre.NbPointHomo.push_back(aPackIn.size());
            }
        }
        Abre.push_back(aAbre);
        tempArbeRacine.push_back(aSetImg[i]);
    }
    for (uint k1=0; k1<Abre.size(); k1++)
    {
        for (uint k2=0; k2<Abre[k1].ImgBranch.size(); k2++)
        {
            vector <string> ColImg3eme;
            std::vector<string>::iterator it;
            it = std::find(tempArbeRacine.begin(), tempArbeRacine.end(), Abre[k1].ImgBranch[k2]);
            double p = std::distance( tempArbeRacine.begin(), it );
            for (uint k3=0; k3<Abre[p].ImgBranch.size(); k3++)
            {
                //search for common img b/w Abre[k1] and Abre[p]
                std::vector<string>::iterator it1;
                it1 = std::find(Abre[k1].ImgBranch.begin(), Abre[k1].ImgBranch.end(), Abre[p].ImgBranch[k3]);
                double p1 = std::distance( Abre[k1].ImgBranch.begin(), it1 );
                bool isPresent = (it1 != Abre[k1].ImgBranch.end());
                if (isPresent)
                {
                    ColImg3eme.push_back(Abre[k1].ImgBranch[p1]);//common img entre Abre[k1] and Abre[p]
                }
            }
            Abre[k1].Img3eme.push_back(ColImg3eme);
        }

    }
return Abre;
}


vector<string> displayAbreHomol(vector<AbreHomol> aAbre, bool disp)
{
    vector<string> result;
    for (uint i=0;i<aAbre.size();i++)
    {result.push_back(aAbre[i].ImgRacine);}
    if (disp)
    {
        for (uint i=0;i<aAbre.size();i++)
        {
            cout<<aAbre[i].ImgRacine<<endl;
            for(uint k=0; k<aAbre[i].ImgBranch.size(); k++)
            {
                cout<<" ++ "<<aAbre[i].ImgBranch[k]<<endl;
                for(uint l=0; l<aAbre[i].Img3eme[k].size(); l++)
                {
                    cout<<"   ++ "<<aAbre[i].Img3eme[k][l]<<endl;
                }
            }
        }
    }
    return result;
}


vector<bool> FiltreDe3img(string aNameImg1, string aNameImg2, string aNameImg3, string aNameHomol, string aDirImages, string aPatImages, string aOriInput, bool ExpTxt, Pt2dr centre_img, double diag, double aDistRepr, double aDistHom)
{
  //=============Manip File Name=====================
    ELISE_fp::AssertIsDirectory(aNameHomol);

    // Initialize name manipulator & files
    cInterfChantierNameManipulateur * aICNM=cInterfChantierNameManipulateur::BasicAlloc(aDirImages);
    std::string anExt = ExpTxt ? "txt" : "dat";
    std::string aKHIn =   std::string("NKS-Assoc-CplIm2Hom@")
            +  std::string(aNameHomol)
            +  std::string("@")
            +  std::string(anExt);

    //===========================================================
    vector<bool> result;
    //==========import img1 img2 img3===========
    std::string aOri1 = aICNM->Assoc1To1("NKS-Assoc-Im2Orient@-"+aOriInput,aNameImg1,true);
    CamStenope * aCam1 = CamOrientGenFromFile(aOri1 , aICNM);
    std::string aOri2 = aICNM->Assoc1To1("NKS-Assoc-Im2Orient@-"+aOriInput,aNameImg2,true);
    CamStenope * aCam2 = CamOrientGenFromFile(aOri2 , aICNM);
    std::string aOri3 = aICNM->Assoc1To1("NKS-Assoc-Im2Orient@-"+aOriInput,aNameImg3,true);
    CamStenope * aCam3 = CamOrientGenFromFile(aOri3 , aICNM);

    std::string aHomoIn1_2 = aICNM->Assoc1To2(aKHIn, aNameImg1, aNameImg2,true);
    StdCorrecNameHomol_G(aHomoIn1_2,aDirImages);
    ElPackHomologue aPackIn1_2, aPackIn1_3, aPackIn2_3;
    bool Exist1_2 = ELISE_fp::exist_file(aHomoIn1_2);
    if (Exist1_2)
    {
     aPackIn1_2 =  ElPackHomologue::FromFile(aHomoIn1_2);
    }

    std::string aHomoIn1_3 = aICNM->Assoc1To2(aKHIn, aNameImg1, aNameImg3,true);
    StdCorrecNameHomol_G(aHomoIn1_3,aDirImages);
    bool Exist1_3 = ELISE_fp::exist_file(aHomoIn1_3);
    if (Exist1_3)
    {
     aPackIn1_3 =  ElPackHomologue::FromFile(aHomoIn1_3);
    }

    std::string aHomoIn2_3 = aICNM->Assoc1To2(aKHIn, aNameImg2, aNameImg3, true);
    StdCorrecNameHomol_G(aHomoIn2_3,aDirImages);
    bool Exist2_3 = ELISE_fp::exist_file(aHomoIn2_3);
    if (Exist2_3)
    {
     aPackIn2_3 =  ElPackHomologue::FromFile(aHomoIn2_3);
    }

    //================traite======================//
    //double w=3;
    double count_pass_reproj=0;
    double countGoodTrip = 0;
    for (ElPackHomologue::const_iterator itP=aPackIn1_2.begin(); itP!=aPackIn1_2.end() ; itP++)
    {

        Pt2dr aP1 = itP->P1();  //Point img1
        Pt2dr aP2 = itP->P2();  //Point img2
        double d;
        bool pass_reproj;
        //=================verifier par reprojeter============
        Pt3dr PInter1_2= aCam1->ElCamera::PseudoInter(aP1, *aCam2, aP2, &d);	//use Point img1 & 2 to search point 3d
        Pt2dr PReproj3 = aCam3->ElCamera::R3toF2(PInter1_2);					//use point 3d to search Point img3
        //double dist_centre = sqrt(pow((PReproj3.x - centre_img.x),2) + pow((PReproj3.y - centre_img.y),2));
        //bool inside = (dist_centre < 0.67*diag/2) ? true : false;
        //chercher triplet
        const ElCplePtsHomologues  * aTriplet2_3 = aPackIn2_3.Cple_Nearest(aP2,true);
        const ElCplePtsHomologues  * aTriplet1_3 = aPackIn1_3.Cple_Nearest(aP1,true);
        double distP2 = sqrt(pow((aTriplet2_3->P1().x - aP2.x),2) + pow((aTriplet2_3->P1().y - aP2.y),2));
        double distP3 = sqrt(pow((aTriplet1_3->P2().x - aTriplet2_3->P2().x),2) + pow((aTriplet1_3->P2().y - aTriplet2_3->P2().y),2));
        if ( (distP2 < aDistHom) && (distP3 < aDistHom) )
        {
            Pt2dr aP3 = aTriplet2_3->P2();
            countGoodTrip ++;
            //check condition reproject
            double distRepr = sqrt(pow((aP3.x - PReproj3.x),2) + pow((aP3.y - PReproj3.y),2));

            if (distRepr < aDistRepr)
            {
                pass_reproj=1;
                count_pass_reproj++;
            }
            else
            {
                pass_reproj=0;
            }

        }
        else
        {
            pass_reproj=0;
        }
        result.push_back(pass_reproj);
    }
    //cout <<"   ++ => "<<count_pass_reproj<<" / "<<countGoodTrip<<" / "<<aPackIn1_2.size()<<endl;
    count_pass_reproj = 0;
    for (uint i=0;i<result.size(); i++)
    {
        if (result[i])
        {
            count_pass_reproj++;
        }
    }
    //cout <<"   ++ Verif => "<<count_pass_reproj<<" / "<<countGoodTrip<<" / "<<result.size()<<endl;
    return result;
}


void creatHomolFromPair(string aNameImg1, string aNameImg2, string aNameHomol, string aDirImages, string aPatImages, string aHomolOutput, bool ExpTxt, vector<bool> decision)
{
    cout<<"ecrit...";
    //=============Manip File Name=====================
    ELISE_fp::AssertIsDirectory(aNameHomol);

    // Initialize name manipulator & files
    cInterfChantierNameManipulateur * aICNM=cInterfChantierNameManipulateur::BasicAlloc(aDirImages);
    std::string anExt = ExpTxt ? "txt" : "dat";
    std::string aKHIn =   std::string("NKS-Assoc-CplIm2Hom@")
            +  std::string(aNameHomol)
            +  std::string("@")
            +  std::string(anExt);

    //===========================================================
    std::string aKHOut =   std::string("NKS-Assoc-CplIm2Hom@")
                        +  std::string(aHomolOutput)
                        +  std::string("@")
                        +  std::string("txt");

    std::string aKHOutDat =   std::string("NKS-Assoc-CplIm2Hom@")
                        +  std::string(aHomolOutput)
                        +  std::string("@")
                        +  std::string("dat");

    std::string aHomoIn1_2 = aICNM->Assoc1To2(aKHIn, aNameImg1, aNameImg2,true);
    StdCorrecNameHomol_G(aHomoIn1_2,aDirImages);
    ElPackHomologue aPackIn1_2, aPackIn1_3, aPackIn2_3;
    bool Exist1_2 = ELISE_fp::exist_file(aHomoIn1_2);
    if (Exist1_2)
    {
     aPackIn1_2 =  ElPackHomologue::FromFile(aHomoIn1_2);
    }

    //creat name of homomologue file dans 2 sens
    ElPackHomologue Pair1_2, Pair1_2i;
    std::string NameHomolPair1 = aICNM->Assoc1To2(aKHOut, aNameImg1, aNameImg2, true);
    std::string NameHomolDatPair1 = aICNM->Assoc1To2(aKHOutDat, aNameImg1, aNameImg2, true);
    std::string NameHomolDatPair1i = aICNM->Assoc1To2(aKHOutDat, aNameImg2, aNameImg1 , true);
    double ind=0;
    cout<<NameHomolDatPair1;
    for (ElPackHomologue::const_iterator itP=aPackIn1_2.begin(); itP!=aPackIn1_2.end() ; itP++)
    {
        if(decision[ind])
        {
            Pair1_2.Cple_Add(ElCplePtsHomologues( itP->P1(), itP->P2() ));
            Pair1_2i.Cple_Add(ElCplePtsHomologues( itP->P2() , itP->P1() ));
        }
        ind++;
    }
    if (ExpTxt)
        {Pair1_2.StdPutInFile(NameHomolPair1);}
    Pair1_2.StdPutInFile(NameHomolDatPair1);
    Pair1_2i.StdPutInFile(NameHomolDatPair1i);
    cout<<"..done !"<<endl;
}



VerifParRepr::VerifParRepr(vector<string> mListImg, string aDirImages, string aPatImages, string aNameHomol, string aOri, string aHomolOutput, bool ExpTxt, double aDistHom, double aDistRepr)
{    
        this->mListImg = mListImg;
        this->mDirImages = aDirImages;
        this->mPatImages = aPatImages;
        this->mNameHomol = aNameHomol;
        this->mOri = aOri;
        this->mHomolOutput = aHomolOutput;
        this->mExpTxt = ExpTxt;
        this->mDistHom = aDistHom;
        this->mDistRepr = aDistRepr;
}


vector<AbreHomol> VerifParRepr::creatAbre()
{

    //=============Manip File Name=====================
      cInterfChantierNameManipulateur * aICNM=cInterfChantierNameManipulateur::BasicAlloc(this->mDirImages);
      ELISE_ASSERT(this->mListImg.size()>1,"Number of image must be > 1");
    //======================================================
    vector<string> tempArbeRacine;
    vector<AbreHomol> Abre;
    for (uint i=0; i<this->mListImg.size(); i++)
    {
        AbreHomol aAbre;
        for (uint j=0; j<this->mListImg.size(); j++)
        {
            string nameImgB = this->mListImg[j];
            string nameImgA = this->mListImg[i];

            std::string aKey =   std::string("NKS-Assoc-CplIm2Hom@")
                    +  std::string(this->mNameHomol)
                    +  std::string("@")
                    +  std::string("dat");
            std::string aHomol = aICNM->Assoc1To2(aKey,nameImgA,nameImgB,true);
            StdCorrecNameHomol_G(aHomol,this->mDirImages);

            bool Exist = ELISE_fp::exist_file(aHomol);
            if (Exist)
            {
                //creer abre de Homol
                aAbre.ImgRacine = nameImgA;
                aAbre.ImgBranch.push_back(nameImgB);
                ElPackHomologue aPackIn =  ElPackHomologue::FromFile(aHomol);
                aAbre.NbPointHomo.push_back(aPackIn.size());
            }
        }
        Abre.push_back(aAbre);
        tempArbeRacine.push_back(this->mListImg[i]);
    }
    for (uint k1=0; k1<Abre.size(); k1++)
    {
        for (uint k2=0; k2<Abre[k1].ImgBranch.size(); k2++)
        {
            vector <string> ColImg3eme;
            std::vector<string>::iterator it;
            it = std::find(tempArbeRacine.begin(), tempArbeRacine.end(), Abre[k1].ImgBranch[k2]);
            double p = std::distance( tempArbeRacine.begin(), it );
            for (uint k3=0; k3<Abre[p].ImgBranch.size(); k3++)
            {
                //search for common img b/w Abre[k1] and Abre[p]
                std::vector<string>::iterator it1;
                it1 = std::find(Abre[k1].ImgBranch.begin(), Abre[k1].ImgBranch.end(), Abre[p].ImgBranch[k3]);
                double p1 = std::distance( Abre[k1].ImgBranch.begin(), it1 );
                bool isPresent = (it1 != Abre[k1].ImgBranch.end());
                if (isPresent)
                {
                    ColImg3eme.push_back(Abre[k1].ImgBranch[p1]);//common img entre Abre[k1] and Abre[p]
                }
            }
            Abre[k1].Img3eme.push_back(ColImg3eme);
        }

    }
    //filtre abre for traitement
    vector<string> tempArbeRacine_N;
    vector<AbreHomol> Abre_N;
    for (uint i=0; i<Abre.size(); i++)
    {
        AbreHomol aAbre;
        for (uint j=0; j<Abre[i].ImgBranch.size(); j++)
        {
            std::vector<string>::iterator it1;
            it1 = std::find(tempArbeRacine.begin(), tempArbeRacine.end(), Abre[i].ImgBranch[j]);
            double p1 = std::distance( tempArbeRacine.begin(), it1 );
            if (p1 > i)
            {
                aAbre.ImgBranch.push_back(Abre[i].ImgBranch[j]);
                aAbre.ImgRacine = Abre[i].ImgRacine;
                aAbre.Img3eme.push_back(Abre[i].Img3eme[j]);
            }
        }
        if(aAbre.ImgBranch.size() > 0)
        {
            Abre_N.push_back(aAbre);
            tempArbeRacine_N.push_back(Abre[i].ImgRacine);
        }
    }
    this->mAbre = Abre_N;
    this->mtempArbeRacine = tempArbeRacine_N;
    Tiff_Im mTiffImg3(tempArbeRacine[0].c_str());

    Pt2dr centre_img(mTiffImg3.sz().x/2, mTiffImg3.sz().y/2);
    this->mcentre_img = centre_img;
    this->mdiag = sqrt(pow((double)mTiffImg3.sz().x,2.) + pow((double)mTiffImg3.sz().y,2.));
return Abre_N;
}

vector<string> VerifParRepr::displayAbreHomol(vector<AbreHomol> aAbre, bool disp)
{
    vector<string> result;
    for (uint i=0;i<aAbre.size();i++)
    {result.push_back(aAbre[i].ImgRacine);}
    if (disp)
    {
        for (uint i=0;i<aAbre.size();i++)
        {
            cout<<aAbre[i].ImgRacine<<endl;
            for(uint k=0; k<aAbre[i].ImgBranch.size(); k++)
            {
                cout<<" ++ "<<aAbre[i].ImgBranch[k]<<endl;
                for(uint l=0; l<aAbre[i].Img3eme[k].size(); l++)
                {
                    cout<<"   ++ "<<aAbre[i].Img3eme[k][l]<<endl;
                }
            }
        }
    }
    return result;
}


vector<bool> VerifParRepr::FiltreDe3img(string aNameImg1, string aNameImg2, string aNameImg3)
{
  //=============Manip File Name=====================
    ELISE_fp::AssertIsDirectory(this->mNameHomol);

    // Initialize name manipulator & files
    cInterfChantierNameManipulateur * aICNM=cInterfChantierNameManipulateur::BasicAlloc(this->mDirImages);
    std::string anExt = this->mExpTxt ? "txt" : "dat";
    std::string aKHIn =   std::string("NKS-Assoc-CplIm2Hom@")
            +  std::string(this->mNameHomol)
            +  std::string("@")
            +  std::string(anExt);
    //===========================================================
    vector<bool> result;
    //==========import img1 img2 img3===========
    std::string aOri1 = aICNM->Assoc1To1("NKS-Assoc-Im2Orient@-"+this->mOri,aNameImg1,true);
    CamStenope * aCam1 = CamOrientGenFromFile(aOri1 , aICNM);
    std::string aOri2 = aICNM->Assoc1To1("NKS-Assoc-Im2Orient@-"+this->mOri,aNameImg2,true);
    CamStenope * aCam2 = CamOrientGenFromFile(aOri2 , aICNM);
    std::string aOri3 = aICNM->Assoc1To1("NKS-Assoc-Im2Orient@-"+this->mOri,aNameImg3,true);
    CamStenope * aCam3 = CamOrientGenFromFile(aOri3 , aICNM);


    std::string aHomoIn1_2 = aICNM->Assoc1To2(aKHIn, aNameImg1, aNameImg2,true);
    StdCorrecNameHomol_G(aHomoIn1_2,this->mDirImages);
    ElPackHomologue aPackIn1_2, aPackIn1_3, aPackIn2_3;
    bool Exist1_2 = ELISE_fp::exist_file(aHomoIn1_2);
    if (Exist1_2)
    {
     aPackIn1_2 =  ElPackHomologue::FromFile(aHomoIn1_2);
    }

    std::string aHomoIn1_3 = aICNM->Assoc1To2(aKHIn, aNameImg1, aNameImg3,true);
    StdCorrecNameHomol_G(aHomoIn1_3,this->mDirImages);
    bool Exist1_3 = ELISE_fp::exist_file(aHomoIn1_3);
    if (Exist1_3)
    {
     aPackIn1_3 =  ElPackHomologue::FromFile(aHomoIn1_3);
    }

    std::string aHomoIn2_3 = aICNM->Assoc1To2(aKHIn, aNameImg2, aNameImg3, true);
    StdCorrecNameHomol_G(aHomoIn2_3,this->mDirImages);
    bool Exist2_3 = ELISE_fp::exist_file(aHomoIn2_3);
    if (Exist2_3)
    {
     aPackIn2_3 =  ElPackHomologue::FromFile(aHomoIn2_3);
    }

    //================traite======================//
    //double w=3;
    double count_pass_reproj=0;
    double countGoodTrip = 0;
    for (ElPackHomologue::const_iterator itP=aPackIn1_2.begin(); itP!=aPackIn1_2.end() ; itP++)
    {

        Pt2dr aP1 = itP->P1();  //Point img1
        Pt2dr aP2 = itP->P2();  //Point img2
        double d;
        bool pass_reproj;
        //=================verifier par reprojeter============
        Pt3dr PInter1_2= aCam1->ElCamera::PseudoInter(aP1, *aCam2, aP2, &d);	//use Point img1 & 2 to search point 3d
        Pt2dr PReproj3 = aCam3->ElCamera::R3toF2(PInter1_2);					//use point 3d to search Point img3
        //double dist_centre = sqrt(pow((PReproj3.x - this->mcentre_img.x),2) + pow((PReproj3.y - this->mcentre_img.y),2));
        //bool inside = (dist_centre < 0.67*this->mdiag/2) ? true : false;
        //chercher triplet
        const ElCplePtsHomologues  * aTriplet2_3 = aPackIn2_3.Cple_Nearest(aP2,true);
        const ElCplePtsHomologues  * aTriplet1_3 = aPackIn1_3.Cple_Nearest(aP1,true);
        double distP2 = sqrt(pow((aTriplet2_3->P1().x - aP2.x),2) + pow((aTriplet2_3->P1().y - aP2.y),2));
        double distP3 = sqrt(pow((aTriplet1_3->P2().x - aTriplet2_3->P2().x),2) + pow((aTriplet1_3->P2().y - aTriplet2_3->P2().y),2));
        if ( (distP2 < this->mDistHom)  && (distP3 < this->mDistHom) )
        {
            Pt2dr aP3 = aTriplet2_3->P2();
            countGoodTrip ++;
            //check condition reproject
            double distRepr = sqrt(pow((aP3.x - PReproj3.x),2) + pow((aP3.y - PReproj3.y),2));

            if (distRepr < this->mDistRepr)
            {
                pass_reproj=1;
                count_pass_reproj++;
            }
            else
            {
                pass_reproj=0;
            }
        }
        else
        {
            pass_reproj=0;
        }
        result.push_back(pass_reproj);
    }
    //cout <<"   ++ => "<<count_pass_reproj<<" / "<<countGoodTrip<<" / "<<aPackIn1_2.size()<<endl;
    cout<<"Verif FiltreDe3img "<<result.size()<<" "<<aPackIn1_2.size()<<endl;
    cout <<"Verif FiltreDe3img NbPts Filtré => "<<count_pass_reproj<<" "<<countGoodTrip<<endl;
    count_pass_reproj = 0;
    countGoodTrip = 0;
    return result;
}


void VerifParRepr::creatHomolFromPair(string aNameImg1, string aNameImg2, vector<bool> decision)
{
    cout<<"ecrit...";
    //=============Manip File Name=====================
    ELISE_fp::AssertIsDirectory(this->mNameHomol);

    // Initialize name manipulator & files
    cInterfChantierNameManipulateur * aICNM=cInterfChantierNameManipulateur::BasicAlloc(this->mDirImages);
    std::string anExt = this->mExpTxt ? "txt" : "dat";
    std::string aKHIn =   std::string("NKS-Assoc-CplIm2Hom@")
            +  std::string(this->mNameHomol)
            +  std::string("@")
            +  std::string(anExt);

    //===========================================================
    std::string aKHOut =   std::string("NKS-Assoc-CplIm2Hom@")
                        +  std::string(this->mHomolOutput)
                        +  std::string("@")
                        +  std::string("txt");

    std::string aKHOutDat =   std::string("NKS-Assoc-CplIm2Hom@")
                        +  std::string(this->mHomolOutput)
                        +  std::string("@")
                        +  std::string("dat");

    std::string aHomoIn1_2 = aICNM->Assoc1To2(aKHIn, aNameImg1, aNameImg2,true);
    StdCorrecNameHomol_G(aHomoIn1_2,this->mDirImages);
    ElPackHomologue aPackIn1_2, aPackIn1_3, aPackIn2_3;
    bool Exist1_2 = ELISE_fp::exist_file(aHomoIn1_2);
    if (Exist1_2)
    {
     aPackIn1_2 =  ElPackHomologue::FromFile(aHomoIn1_2);
     cout<<"Verif creatHomolFromPair "<<decision.size()<<" "<<aPackIn1_2.size();
    }

    //creat name of homomogue file dans 2 sens
    ElPackHomologue Pair1_2, Pair1_2i;
    std::string NameHomolPair1 = aICNM->Assoc1To2(aKHOut, aNameImg1, aNameImg2, true);
    std::string NameHomolDatPair1 = aICNM->Assoc1To2(aKHOutDat, aNameImg1, aNameImg2, true);
    std::string NameHomolDatPair1i = aICNM->Assoc1To2(aKHOutDat, aNameImg2, aNameImg1 , true);
    double ind=0;
    for (ElPackHomologue::const_iterator itP=aPackIn1_2.begin(); itP!=aPackIn1_2.end() ; itP++)
    {
        if(decision[ind])
        {
            Pair1_2.Cple_Add(ElCplePtsHomologues( itP->P1(), itP->P2() ));
            Pair1_2i.Cple_Add(ElCplePtsHomologues( itP->P2() , itP->P1() ));
        }
        ind++;
    }
    Pair1_2.StdPutInFile(NameHomolPair1);
    Pair1_2.StdPutInFile(NameHomolDatPair1);
    Pair1_2i.StdPutInFile(NameHomolDatPair1i);
    cout<<"..done !"<<endl;
}


void VerifParRepr::FiltragePtsHomo()
{
    vector<string> tempArbeRacine = this->mtempArbeRacine;

    vector<AbreHomol> aAbre = this->mAbre;

    //Pt2dr centre_img=this->mcentre_img;
    //double diag = this->mdiag;


    double stat = 0;
    double all = 0;
    for (uint i=0;i<aAbre.size();i++)
    {
        cout<<aAbre[i].ImgRacine<<endl;
        string aNameImg1 = aAbre[i].ImgRacine;
        for(uint k=0; k<aAbre[i].ImgBranch.size(); k++)
        {
            vector< vector<bool> > ColDec;
            cout<<" ++ "<<aAbre[i].ImgBranch[k]<<endl;
            string aNameImg2 = aAbre[i].ImgBranch[k];
            for(uint l=0; l<aAbre[i].Img3eme[k].size(); l++)
            {
                cout<<"   + Com + "<<aAbre[i].Img3eme[k][l]<<endl;
                //====Triplet Image==========//
                aNameImg1 = aAbre[i].ImgRacine;
                aNameImg2 = aAbre[i].ImgBranch[k];
                string aNameImg3 = aAbre[i].Img3eme[k][l];
                cout<<"   + Tri + "<<aNameImg1<<" "<<aNameImg2<<" "<<aNameImg3<<endl;
                vector<bool> result = FiltreDe3img( aNameImg1,  aNameImg2,  aNameImg3);
                ColDec.push_back(result);
            }
            cout<<"Verif FiltragePtsHomo "<<ColDec.size()<<" "<<ColDec[0].size();
            //=====decision=====
            vector<bool> decision; vector<double> decPoint;

            for(uint m=0; m<ColDec[0].size(); m++)
            {
                bool dec = 0; double point=0;
                for (uint n=0; n<ColDec.size(); n++)
                {
                    dec = dec || ColDec[n][m];
                    if (ColDec[n][m])
                    {point++;}
                }
                decision.push_back(dec);
                decPoint.push_back(point);
            }
            cout<<" "<<decision.size()<<" "<<decPoint.size()<<endl;
            //creat homol file with decision and pack homo b/w aNameImg1 aNameImg2
            //.....
            double totalImgCom = aAbre[i].Img3eme[k].size();
            double countVerif=0;
            for(uint o=0; o<decision.size(); o++)
            {
                if (decision[o] && ((decPoint[o]/totalImgCom) > 0.5) )
                {countVerif++; decision[o] = 1;}
                else
                {decision[o] = 0;}
            }
            cout<<"     *-*-*-*-*-*   "<<" NbPt Filtré = "<<countVerif<< " / "<<decision.size()<<endl<<endl;
            stat = stat + countVerif;
            all = all + decision.size();
            creatHomolFromPair(aNameImg1, aNameImg2, decision);
        }
    }
    cout<<endl<<endl<<"+- %Pt good + "<<stat/all*100<<"%"<<endl;

}


VectorSurface::VectorSurface(Pt2dr dirX, Pt2dr dirY)
{
    this->dirX = dirX;
    this->dirY = dirY;
}


VectorSurface::VectorSurface()
{
    this->dirX = Pt2dr(0,0);
    this->dirY = Pt2dr(0,0);
}

RepereImagette::RepereImagette(Pt2dr centre, Pt2dr dirX, Pt2dr dirY)
{
    this->centre = centre;
    this->dirX = dirX;
    this->dirY = dirY;
}

RepereImagette::RepereImagette()
{
    this->centre = Pt2dr(0,0);
    this->dirX = Pt2dr(0,0);
    this->dirY = Pt2dr(0,0);
}

Pt2dr RepereImagette::uv2img(Pt2dr coorOrg)
{
    Pt2dr coorInImgB(0,0);
    coorInImgB.x = (this->centre.x) - coorOrg.x*this->dirX.x - coorOrg.y*this->dirY.x;
    coorInImgB.y = (this->centre.y) - coorOrg.y*this->dirX.y - coorOrg.y*this->dirY.y;
    return coorInImgB;
}
//   R3 : "reel" coordonnee initiale
//   L3 : "Locale", apres rotation
//   C2 :  camera, avant distortion
//   F2 : finale apres Distortion
//
//       Orientation      Projection      Distortion
//   R3 -------------> L3------------>C2------------->F2

CplImg::CplImg(string aNameImg1, string aNameImg2, string aNameHomol, string aOri, string aHomolOutput,
               string aFullPatternImages, bool ExpTxt, double aPropDiag, double aCorel, double aSizeVignette,
               bool aDisplayVignette, bool aFiltreBy1Img, double aTauxGood, double aSizeSearchAutour):
    mNameImg1(aNameImg1), mFiltreBy1Img(aFiltreBy1Img), mTauxGood(aTauxGood)
{
    this->mNameImg1 = aNameImg1;
    this->mNameImg2 = aNameImg2;
    this->mNameHomol = aNameHomol;
    this->mHomolOutput = aHomolOutput;
    this->mExpTxt = ExpTxt;
    this->mPropDiag = aPropDiag;
    this->mCorel = aCorel;
    this->mdisplayVignette = aDisplayVignette;
    this->msizeVignette = aSizeVignette;
    this->mSizeSearchAutour = aSizeSearchAutour;
   //====== Initialize name manipulator & files=====//
    ELISE_fp::AssertIsDirectory(aNameHomol);
    std::string aDirImages, aPatImages;
    SplitDirAndFile(aDirImages,aPatImages,aFullPatternImages);
    StdCorrecNameOrient(aOri,aDirImages);//remove "Ori-" if needed
    cInterfChantierNameManipulateur * aICNM=cInterfChantierNameManipulateur::BasicAlloc(aDirImages);
    const std::vector<std::string> aSetImages = *(aICNM->Get(aPatImages));
    ELISE_ASSERT(aSetImages.size()>1,"Number of image must be > 1");

    this->mOri = aOri;
    this->mSetImages = aSetImages;
    this->mDirImages = aDirImages;
    this->mPatImages = aPatImages;
    this->mICNM = aICNM;

    std::string anExt = ExpTxt ? "txt" : "dat";

    this->mKHOut =   std::string("NKS-Assoc-CplIm2Hom@")
                        +  std::string(aHomolOutput)
                        +  std::string("@")
                        +  std::string("txt");

    this->mKHOutDat =   std::string("NKS-Assoc-CplIm2Hom@")
                        +  std::string(aHomolOutput)
                        +  std::string("@")
                        +  std::string("dat");

    this->mKHIn =   std::string("NKS-Assoc-CplIm2Hom@")
                       +  std::string(aNameHomol)
                       +  std::string("@")
                       +  std::string(anExt);


 //===========================================================
    //====lire Img1 et Img2=====//
    Tiff_Im aTiffImg1(aNameImg1.c_str());
    Tiff_Im aTiffImg2(aNameImg2.c_str());
    //====lire Cam1 et Cam2====//
    std::string aOri1 = aICNM->Assoc1To1("NKS-Assoc-Im2Orient@-"+ aOri, aNameImg1, true);
    CamStenope * aCam1 = CamOrientGenFromFile(aOri1 , aICNM);
    std::string aOri2 = aICNM->Assoc1To1("NKS-Assoc-Im2Orient@-"+ aOri, aNameImg2,true);
    CamStenope * aCam2 = CamOrientGenFromFile(aOri2 , aICNM);
    this->mCam1 = aCam1;
    this->mCam2 = aCam2;
}

void CplImg::SupposeVecSruf1er(Pt2dr dirX, Pt2dr dirY)
{
    VectorSurface a(dirX, dirY);
    this->mSurfImg1 = a;
}

bool IsInside(Pt2dr checkPoint, Tiff_Im mTiffImg1, double percent = 1)
{
    bool in=FALSE;
    Pt2dr centre_img(mTiffImg1.sz().x/2, mTiffImg1.sz().y/2);
    double diag = sqrt(pow((double)(mTiffImg1.sz().x/2),2.) + pow((double)(mTiffImg1.sz().y/2),2.));
    double dist = sqrt(pow(checkPoint.x - centre_img.x,2) + pow(checkPoint.y - centre_img.y , 2));
    if( (fabs(checkPoint.x-mTiffImg1.sz().x/2) < mTiffImg1.sz().x/2) && (fabs(checkPoint.y-mTiffImg1.sz().y/2) < mTiffImg1.sz().y/2) )
    {
        if (dist/diag <= percent)
            {in=true; }
        else
            {in=false;}
    }
    return in;
}
vector<bool> CplImg::CalVectorSurface(string m3emeImg, string ModeSurf)
{
    vector<bool> result;
    cInterfChantierNameManipulateur * aICNM = this->mICNM;
    string aKHIn = this->mKHIn;
    string aDirImages = this->mDirImages;
    string aNameImg1 = this->mNameImg1;
    string aNameImg2 = this->mNameImg2;
    CamStenope * aCam1 = this->mCam1;
    CamStenope * aCam2 = this->mCam2;
    bool displayVignette = this->mdisplayVignette;
    //====Import collection 3eme Image====//
    string aNameImg3 = m3emeImg;
    std::string aOri3 = aICNM->Assoc1To1("NKS-Assoc-Im2Orient@-"+ this->mOri, aNameImg3, true);
    CamStenope * aCam3 = CamOrientGenFromFile(aOri3 , aICNM);
    //====Import Pack Homologue======//
    std::string aHomoIn1_2 = aICNM->Assoc1To2(aKHIn, aNameImg1, aNameImg2,true);
    StdCorrecNameHomol_G(aHomoIn1_2,aDirImages);
    ElPackHomologue aPackIn1_2, aPackIn1_3, aPackIn2_3;
    bool Exist1_2 = ELISE_fp::exist_file(aHomoIn1_2);
    if (Exist1_2)
         {aPackIn1_2 =  ElPackHomologue::FromFile(aHomoIn1_2);}

    std::string aHomoIn1_3 = aICNM->Assoc1To2(aKHIn, aNameImg1, aNameImg3,true);
    StdCorrecNameHomol_G(aHomoIn1_3,aDirImages);
    bool Exist1_3 = ELISE_fp::exist_file(aHomoIn1_3);
    if (Exist1_3)
        {aPackIn1_3 =  ElPackHomologue::FromFile(aHomoIn1_3);}

    std::string aHomoIn2_3 = aICNM->Assoc1To2(aKHIn, aNameImg2, aNameImg3, true);
    StdCorrecNameHomol_G(aHomoIn2_3,aDirImages);
    bool Exist2_3 = ELISE_fp::exist_file(aHomoIn2_3);
    if (Exist2_3)
        {aPackIn2_3 =  ElPackHomologue::FromFile(aHomoIn2_3);}

    // ====Import images Tiff and IM2D =====//
    Tiff_Im mTiffImg1(aNameImg1.c_str());
    Tiff_Im mTiffImg2(aNameImg2.c_str());
    Tiff_Im mTiffImg3(aNameImg3.c_str());

    Im2D<U_INT1,INT4> mIm2DImg1(mTiffImg1.sz().x,mTiffImg1.sz().y);
    Im2D<U_INT1,INT4> mIm2DImg2(mTiffImg2.sz().x,mTiffImg2.sz().y);
    Im2D<U_INT1,INT4> mIm2DImg3(mTiffImg3.sz().x,mTiffImg3.sz().y);
    this->mImg1 = mIm2DImg1;
    this->mImg2 = mIm2DImg2;
    TIm2D<U_INT1,INT4> mTIm2DImg3(mTiffImg3.sz());
    ELISE_COPY(mTIm2DImg3.all_pts(),mTiffImg3.in(),mTIm2DImg3.out());

    TIm2D<U_INT1,INT4> mTIm2DImg2(mTiffImg2.sz());
    ELISE_COPY(mTIm2DImg2.all_pts(),mTiffImg2.in(),mTIm2DImg2.out());

    ELISE_COPY(
                 mTiffImg1.all_pts(),
                 mTiffImg1.in(),
                 mIm2DImg1.out()
              );
    ELISE_COPY(
                 mTiffImg2.all_pts(),
                 mTiffImg2.in(),
                 mIm2DImg2.out()
              );
    ELISE_COPY(
                 mTiffImg3.all_pts(),
                 mTiffImg3.in(),
                 mIm2DImg3.out()
              );
/*
//convert img3 to image dame
    ELISE_COPY(
                mTiffImg3.all_pts(),
                ((FX/10+FY/10)%2)*255,
                mIm2DImg3.out()
                );
//on veut image dame format Tiff
    ELISE_COPY(
                mIm2DImg3.all_pts(),
                ((FX/10+FY/10)%2)*255,
                mTiffImg3.out()
                );
*/
    Pt2dr centre_img(mTiffImg1.sz().x/2, mTiffImg1.sz().y/2);
    //=======================================================//
    double count =0;
    for (ElPackHomologue::const_iterator itP=aPackIn1_2.begin(); itP!=aPackIn1_2.end() ; itP++)
    {
        Pt2dr aP1 = itP->P1();
        //(fabs(aP1.x-2736)<20)&&(fabs(aP1.y-1824)<20)
        bool decide = false;

            Pt2dr aP2 = itP->P2();
            //======Profondeur a partir de cam 1 et cam 2======
            double d;
            Pt3dr Pt_pseudointer= aCam1->ElCamera::PseudoInter(aP1, *aCam2, aP2, &d);	//use Point img1 & 2 to search point 3d
                //====== calcul profondeur correspondant avec direction viseur de cam 1 =====
            double prof_d = aCam1->ProfInDir(Pt_pseudointer,aCam1->DirK());
            Pt3dr Pt_H = aCam1->ImEtProf2Terrain(aP1, prof_d);  //pt3d intersection entre point img 1 et 2 mais se situe dans la direction viseur de cam 1

            Pt2dr aP3 = aCam3->R3toF2(Pt_H); Pt2dr aP3_o=aP3;

            //Pt3dr OptCenterImg1 = aCam1->VraiOpticalCenter();
            //cout<<"OptCenterImg1 "<<OptCenterImg1<<endl;
            //double prof_d = sqrt(pow((OptCenterImg1.x - Pt_H.x),2) + pow((OptCenterImg1.y - Pt_H.y),2) + pow((OptCenterImg1.z - Pt_H.z),2));

            //double dist_centre = sqrt(pow((aP3 - centre_img).x, 2) + pow((aP3 - centre_img).y, 2));
           vector<bool> Coldecide;
           for (int ii=-this->mSizeSearchAutour; ii<=this->mSizeSearchAutour; ii++)
           {
               for (int jj=-this->mSizeSearchAutour; jj<=this->mSizeSearchAutour; jj++)
               {
                   int sizeVignette = this->msizeVignette;
                   aP3 = aP3_o + Pt2dr(ii,jj);
                   if( IsInside(aP3, mTiffImg3, 1) )
                   {
                       //=== 3) Calcul vector direction de surface Hu et Hv dans l'espace ===
                       Pt2dr SupDirX = aP1+Pt2dr(10,0);
                       Pt2dr SupDirY = aP1+Pt2dr(0,10);
                       Pt3dr Pt_Hu, Pt_Hv;
                       if (ModeSurf == "plan")
                       {
                        Pt_Hu = aCam1->ImEtProf2Terrain(SupDirX, prof_d); //hyphothese surface est une plan perpendiculaire
                        Pt_Hv = aCam1->ImEtProf2Terrain(SupDirY, prof_d);
                       }
                       if (ModeSurf == "sphere")
                       {

                            Pt3dr vecaP1 = aCam1->C2toDirRayonL3(aP1);
                            Pt3dr vecSupDirX = aCam1->C2toDirRayonL3(SupDirX);
                            //Pt3dr vecSupDirY = aCam1->C2toDirRayonL3(SupDirY);

                            //angle b/w (aP1, PP)
                            Pt3dr vecPP = aCam1->C2toDirRayonL3(aCam1->PP());
                            double length_vecaP1 = sqrt(vecaP1.x*vecaP1.x + vecaP1.y*vecaP1.y + vecaP1.z*vecaP1.z);
                            double length_vecPP = sqrt(vecPP.x*vecPP.x + vecPP.y*vecPP.y + vecPP.z*vecPP.z);
                            double cosPhi = (vecaP1.x * vecPP.x + vecaP1.y * vecPP.y + vecaP1.z * vecPP.z) / (length_vecaP1*length_vecPP);
                            //Rayon
                            double R=prof_d / cosPhi;
                            //angle b/w (SupDirX, PP)
                            double length_vecSupDirX = sqrt(vecSupDirX.x*vecSupDirX.x + vecSupDirX.y*vecSupDirX.y + vecSupDirX.z*vecSupDirX.z);
                            double cosPhi2 = (vecSupDirX.x * vecPP.x + vecSupDirX.y * vecPP.y + vecSupDirX.z * vecPP.z) / (length_vecSupDirX*length_vecPP);
                            //profondeur SupDirX
                            double prof_SupDirX = R*cosPhi2;
                            Pt_Hu = aCam1->ImEtProf2Terrain(SupDirX, prof_SupDirX);
                            cout<<prof_SupDirX<<" "<<Pt_Hu;
                            Pt_Hu = aCam1->ImEtProf2Terrain(SupDirX, prof_d);
                            cout<<" "<<prof_d<<" "<<Pt_Hu<<endl;

                       }
                       //=== 4) ReProjecte Hu et Hv de l'espace à img 3 =====
                       Pt2dr Pt_Hu_dansImg3 = aCam3->R3toF2(Pt_Hu);
                       Pt2dr Pt_Hv_dansImg3 = aCam3->R3toF2(Pt_Hv);
                       //=== 5) Vector direction de surface d'img 3 ===
                       Pt2dr DirX = aP3 - Pt_Hu_dansImg3;
                       Pt2dr DirY = aP3 - Pt_Hv_dansImg3;
                       VectorSurface aDirSurfImg3(DirX,DirY);
                       //=== 6) Calcul coordonne des autres point dans le mire d'img 1 correspondant avec img 2 ===
                       //Vignette d'img 1
                       cCorrelImage::setSzW(sizeVignette);
                       cCorrelImage Imgette1; cCorrelImage Imgette3_o;
                       Imgette1.getFromIm(&mIm2DImg1, aP1.x, aP1.y);
                       Imgette3_o.getFromIm(&mIm2DImg3, aP3.x, aP3.y);
                       RepereImagette RepImgette3(aP3, DirX, DirY);
                       Pt2di aP3access;
                       TIm2D<U_INT1,INT4> mTIm2DImgette3(Pt2di(sizeVignette*2+1, sizeVignette*2+1));
                       Im2D<U_INT1,INT4> mIm2DImgette3(sizeVignette*2+1, sizeVignette*2+1);
                       bool out=false;
                       for (int i=-sizeVignette; i<=sizeVignette; i++)
                       {
                           for (int k=-sizeVignette; k<=sizeVignette; k++)
                           {
                               Pt2di aVois(i,k);
            if( IsInside(aP3, mTiffImg3, 1) )
            {
                //=== 3) Calcul vector direction de surface Hu et Hv dans l'espace ===
                Pt2dr SupDirX = aP1+Pt2dr(1,0);
                Pt2dr SupDirY = aP1+Pt2dr(0,1);             
                Pt3dr Pt_Hu = aCam1->ImEtProf2Terrain(SupDirX, prof_d); //hyphothese surface est une sphere                
                Pt3dr Pt_Hv = aCam1->ImEtProf2Terrain(SupDirY, prof_d);
                //=== 4) ReProjecte Hu et Hv de l'espace à img 3 =====
                Pt2dr Pt_Hu_dansImg3 = aCam3->R3toF2(Pt_Hu);
                Pt2dr Pt_Hv_dansImg3 = aCam3->R3toF2(Pt_Hv);
                //=== 5) Vector direction de surface d'img 3 ===
                Pt2dr DirX = aP3 - Pt_Hu_dansImg3;
                Pt2dr DirY = aP3 - Pt_Hv_dansImg3;
                VectorSurface aDirSurfImg3(DirX,DirY);
                //=== 6) Calcul coordonne des autres point dans le mire d'img 1 correspondant avec img 2 ===
                //Vignette d'img 1
                cCorrelImage::setSzW(sizeVignette);
                cCorrelImage Imgette1; cCorrelImage Imgette3_o;
                Imgette1.getFromIm(&mIm2DImg1, aP1.x, aP1.y);
                Imgette3_o.getFromIm(&mIm2DImg3, aP3.x, aP3.y);
                RepereImagette RepImgette3(aP3, DirX, DirY);
                Pt2di aP3access;
                TIm2D<U_INT1,INT4> mTIm2DImgette3(Pt2di(sizeVignette*2+1, sizeVignette*2+1));
                Im2D<U_INT1,INT4> mIm2DImgette3(sizeVignette*2+1, sizeVignette*2+1);
                bool out=false;
                for (int i=-sizeVignette; i<=sizeVignette; i++)
                {
                    for (int k=-sizeVignette; k<=sizeVignette; k++)
                    {
                        Pt2di aVois(i,k);
                        Pt2dr pixelCorrImg3 =  RepImgette3.uv2img(Pt2dr(i,k));
                        aP3access.x = int(round(pixelCorrImg3.x)); aP3access.y = int(round(pixelCorrImg3.y));
                        Pt2dr aP3Test; aP3Test.x = ceil(aP3access.x); aP3Test.y = ceil(aP3access.y);
                        if ( IsInside( aP3Test , mTiffImg3, this->mPropDiag) )
                            {
                                INT4 val = mTIm2DImg3.getr(pixelCorrImg3, -1);
                                /*== ecrire dans un pixel d'image ====*/
                                mTIm2DImgette3.oset_svp(aVois+Pt2di(sizeVignette,sizeVignette),val);
                                out=false;
                            }
                        else
                            {out = true; break;}
                    }
                    if (out)
                    {result.push_back(false); break;}
                }
                // ==== comparer par corellation ==== //
                if (!out)
                {   
                    ELISE_COPY(mTIm2DImgette3.all_pts(),mTIm2DImgette3.in(),mIm2DImgette3.out());
                    cCorrelImage Imgette3;
                    Imgette3.getWholeIm(&mIm2DImgette3);
                    double corl = Imgette3.CrossCorrelation(Imgette1);
                    double corl_o = Imgette3_o.CrossCorrelation(Imgette1);
                    if (displayVignette)
                    {
                        cout<<endl<<"Order = Deforme Img3 - Img1 - Origin Img3"<<endl;
                        cout<<"Corell Deforme = "<<corl<<" - Corell Origin = "<<corl_o<<endl;
                        if (mW==0)
                            mW = Video_Win::PtrWStd(mIm2DImgette3.sz()*4);  //vignette deforme img3
                        ELISE_COPY(mW->all_pts(), mIm2DImgette3.in()[Virgule(FX/4,FY/4)] ,mW->ogray());
                        if (mW1==0)
                        {
                            mW1 = new Video_Win(*mW,Video_Win::eDroiteH,Imgette1.getIm()->sz()*4);  //vignette img 1
                            mW2 = new Video_Win(*mW1,Video_Win::eDroiteH,Imgette3_o.getIm()->sz()*4);   //vignette origin img 3
                        }
                        ELISE_COPY(mW1->all_pts(), Imgette1.getIm()->in()[Virgule(FX/4,FY/4)] ,mW1->ogray());
                        ELISE_COPY(mW2->all_pts(), Imgette3_o.getIm()->in()[Virgule(FX/4,FY/4)] ,mW2->ogray());
                        mW2->clik_in();
                    }
                    if (corl > this->mCorel)
                        {count++;result.push_back(true);}
                    else
                        {result.push_back(false);}
                }
            }
                               Pt2dr pixelCorrImg3 =RepImgette3.uv2img(Pt2dr(i,k));
                               aP3access.x = int(round(pixelCorrImg3.x)); aP3access.y = int(round(pixelCorrImg3.y));
                               Pt2dr aP3Test; aP3Test.x = ceil((double)aP3access.x); aP3Test.y = ceil((double)aP3access.y);
                               if ( IsInside( aP3Test , mTiffImg3, this->mPropDiag) )
                               {
                                   INT4 val = mTIm2DImg3.getr(pixelCorrImg3, -1);
                                   /*== ecrire dans un pixel d'image ====*/
                                   mTIm2DImgette3.oset_svp(aVois+Pt2di(sizeVignette,sizeVignette),val);
                                   out=false;
                               }
                               else
                               {out = true; decide = false; break;}
                           }
                           if (out)
                           {break;}
                       }
                       // ==== comparer par corellation ==== //
                       if (!out) //out=false, donc dans image
                       {
                           ELISE_COPY(mTIm2DImgette3.all_pts(),mTIm2DImgette3.in(),mIm2DImgette3.out());
                           cCorrelImage Imgette3;
                           Imgette3.getWholeIm(&mIm2DImgette3);
                           double corl = Imgette3.CrossCorrelation(Imgette1);
                           double corl_o = Imgette3_o.CrossCorrelation(Imgette1);
                           if (displayVignette)
                           {
                               cout<<endl<<"Order = Deforme Img3 - Img1 - Origin Img3"<<endl;
                               cout<<"Corell Deforme = "<<corl<<" - Corell Origin = "<<corl_o<<endl;
                               if (mW==0)
                                   mW = Video_Win::PtrWStd(mIm2DImgette3.sz()*4);  //vignette deforme img3
                               ELISE_COPY(mW->all_pts(), mIm2DImgette3.in()[Virgule(FX/4,FY/4)] ,mW->ogray());
                               if (mW1==0)
                               {
                                   mW1 = new Video_Win(*mW,Video_Win::eDroiteH,Imgette1.getIm()->sz()*4);  //vignette img 1
                                   mW2 = new Video_Win(*mW1,Video_Win::eDroiteH,Imgette3_o.getIm()->sz()*4);   //vignette origin img 3
                               }
                               ELISE_COPY(mW1->all_pts(), Imgette1.getIm()->in()[Virgule(FX/4,FY/4)] ,mW1->ogray());
                               ELISE_COPY(mW2->all_pts(), Imgette3_o.getIm()->in()[Virgule(FX/4,FY/4)] ,mW2->ogray());
                               mW2->clik_in();
                           }
                           if (corl > this->mCorel)
                                {decide=true;}
                           else
                                {decide=false;}
                           Coldecide.push_back(decide);
                       }
                   }
               }
           }
           bool decideF=false;
           for (uint hh=0; hh<Coldecide.size(); hh++)
            {decideF = decideF || Coldecide[hh];}
            if (decideF)
                {count++;}
            result.push_back(decideF);
        }
    cout<<"------------------------"<<endl<<"Trip: "<<aNameImg1<<" + "<<aNameImg2<<" + "<<aNameImg3<<endl<<count/aPackIn1_2.size()*100<<" % conserve of "<<aPackIn1_2.size()<<" "<<result.size()<<endl;
    return result;
}


int PHO_MI_main(int argc,char ** argv)
{
    cout<<"*********************"<<endl;
    cout<<"* P : Points        *"<<endl;
    cout<<"* H : Homologues    *"<<endl;
    cout<<"* O : Observés sur  *"<<endl;
    cout<<"* M : Modèle        *"<<endl;
    cout<<"* I : Initial       *"<<endl;
    cout<<"*********************"<<endl;

    std::string aFullPatternImages = ".*.tif", aOriInput, aNameHomol="Homol/", aHomolOutput="_Filtered/", bStrategie = "6";
    double aDistRepr=10, aDistHom=20, aPropDiag =1 ,aCorel = 0.7, aSizeVignette=5, aTauxGood = 0.5, aSizeSearchAutour=0;
    bool ExpTxt = false, aDisplayVignette = false, aFiltreBy1Img=true;
    ElInitArgMain			//initialize Elise, set which is mandantory arg and which is optional arg
    (
    argc,                   //nb d’arguments
    argv,                   //chaines de caracteres contenants tous les arguments
    //mandatory arguments - arg obligatoires²
    LArgMain()  << EAMC(aFullPatternImages, "Pattern of images to compute",  eSAM_IsPatFile)
                << EAMC(aOriInput, "Input Initial Orientation",  eSAM_IsExistDirOri),
    //optional arguments - arg facultatifs
    LArgMain()  << EAM(aNameHomol, "HomolIn", true, "Name of input Homol foler, Homol par default")
                << EAM(ExpTxt,"ExpTxt",true,"Ascii format for in and out, def=false")
                << EAM(aHomolOutput, "HomolOut" , true, "Output corrected Homologues folder")
                << EAM(bStrategie, "Strategie" , true, "Strategie de filtre les points homols")
                << EAM(aDistRepr, "Dist" , true, "Distant to verify reprojection point")
                << EAM(aDistHom, "DistHom" , true, "Distant to verify triplet")
                << EAM(aPropDiag, "PropDiag" , true, "For fisheye lens")
                << EAM(aCorel, "CorelThres" , true, "Threshold for corellation value  [-1 1]")
                << EAM(aSizeVignette, "SizeVignette" , true, "Size of Corellation Vignette [default = 5]")
                << EAM(aDisplayVignette, "DispVignette" , true, "Display imagette before do corellation [defalut=false]")
                << EAM(aFiltreBy1Img, "By1Img" , true, "Decide result is good if it's good in 1 of collection 3eme image [defalut=true]")
                << EAM(aTauxGood, "TauxGood" , true, "Decide result is good if it's good in TauxGood% of collection 3eme image [defalut=0.5]")
                << EAM(aSizeSearchAutour, "Autour" , true, "Search autour for better decide correlation [default= 0 pixels]")
    );
    if (MMVisualMode) return EXIT_SUCCESS;

    ELISE_fp::AssertIsDirectory(aNameHomol);

    // Initialize name manipulator & files
    std::string aDirImages, aPatImages;
    SplitDirAndFile(aDirImages,aPatImages,aFullPatternImages);

    StdCorrecNameOrient(aOriInput,aDirImages);//remove "Ori-" if needed

    cInterfChantierNameManipulateur * aICNM=cInterfChantierNameManipulateur::BasicAlloc(aDirImages);
    const std::vector<std::string> aSetImages = *(aICNM->Get(aPatImages));

    ELISE_ASSERT(aSetImages.size()>1,"Number of image must be > 1");
 //============================================================
    std::string anExt = ExpTxt ? "txt" : "dat";

    std::string aKHOut =   std::string("NKS-Assoc-CplIm2Hom@")
                        +  std::string(aHomolOutput)
                        +  std::string("@")
                        +  std::string("txt");

    std::string aKHOutDat =   std::string("NKS-Assoc-CplIm2Hom@")
                        +  std::string(aHomolOutput)
                        +  std::string("@")
                        +  std::string("dat");

    std::string aKHIn =   std::string("NKS-Assoc-CplIm2Hom@")
                       +  std::string(aNameHomol)
                       +  std::string("@")
                       +  std::string(anExt);

 //===========================================================


    if (bStrategie == "5")
    {
        vector<string> tempArbeRacine;
        vector<AbreHomol> aAbre = creatAbreFromPattern(aSetImages, aNameHomol, aFullPatternImages, aOriInput);
        tempArbeRacine = displayAbreHomol(aAbre, 0);

        Tiff_Im mTiffImg3(tempArbeRacine[0].c_str());
        Pt2dr centre_img(mTiffImg3.sz().x/2, mTiffImg3.sz().y/2);
        double diag = sqrt(pow((double)mTiffImg3.sz().x,2.) + pow((double)mTiffImg3.sz().y,2.));
        vector< vector<bool> > ColDec;

        double stat = 0;
        double all = 0;
        for (uint i=0;i<aAbre.size();i++)
        {
            cout<<aAbre[i].ImgRacine<<endl;
            string aNameImg1 = aAbre[i].ImgRacine;
            for(uint k=0; k<aAbre[i].ImgBranch.size(); k++)
            {
                cout<<" ++ "<<aAbre[i].ImgBranch[k]<<endl;
                string aNameImg2 = aAbre[i].ImgBranch[k];
                for(uint l=0; l<aAbre[i].Img3eme[k].size(); l++)
                {
                    cout<<"   + Com + "<<aAbre[i].Img3eme[k][l]<<endl;
                    //====Triplet Image==========//
                    aNameImg1 = aAbre[i].ImgRacine;
                    aNameImg2 = aAbre[i].ImgBranch[k];
                    string aNameImg3 = aAbre[i].Img3eme[k][l];
                    cout<<"   + Tri + "<<aNameImg1<<" "<<aNameImg2<<" "<<aNameImg3<<endl;
                    vector<bool> result = FiltreDe3img( aNameImg1,  aNameImg2,  aNameImg3,  aNameHomol,  aDirImages,  aPatImages,  aOriInput,  ExpTxt,  centre_img,  diag,  aDistRepr, aDistHom);
                    ColDec.push_back(result);
                }
                //=====decision=====
                vector<bool> decision; vector<double> decPoint;
                for(uint m=0; m<ColDec[0].size(); m++)
                {
                    bool dec = 0; double point=0;
                    for (uint n=0; n<ColDec.size(); n++)
                    {
                        dec = dec || ColDec[n][m];
                        if (ColDec[n][m])
                        {point++;}
                    }
                    decision.push_back(dec);
                    decPoint.push_back(point);
                }
                cout<<decision.size()<<" "<<ColDec[0].size()<<endl;
                //creat homol file with decision and pack homo b/w aNameImg1 aNameImg2
                //.....
                double totalImgCom = aAbre[i].Img3eme[k].size();
                double countVerif=0;
                for(uint o=0; o<decision.size(); o++)
                {
                    if (decision[o] && ((decPoint[o]/totalImgCom) > 0.95) )
                    {countVerif++; decision[o] = 1;}
                    else
                    {decision[o] = 0;}
                }
                cout<<"     *-*-*-*-*-*   "<<" NbPt Filtré = "<<countVerif<< " / "<<decision.size()<<endl<<endl;
                stat = stat + countVerif;
                all = all + decision.size();
                creatHomolFromPair(aNameImg1, aNameImg2, aNameHomol, aDirImages, aPatImages, aHomolOutput, ExpTxt, decision);
            }
        }
        cout<<endl<<endl<<"+-+ "<<stat/all*100<<"%"<<endl;

        //creer abre de couple et des collection de 3eme images correspondant
        //parcourir abre 1 ; prendre image racine
        //parcourir les image branche corresponde with img racine
        //search for abre of image branch current, it's abre 2
        //out list of 2 abre to img3eme correspond with abre 1 abre 2
    }

    //=============================================================================
    if (bStrategie == "6")
    {
        VerifParRepr aImgVerif(aSetImages, aDirImages, aPatImages, aNameHomol, aOriInput, aHomolOutput, ExpTxt, aDistHom, aDistRepr);
        aImgVerif.creatAbre();
        bool disp = 1;
        aImgVerif.displayAbreHomol(aImgVerif.mAbre, disp);
        aImgVerif.FiltragePtsHomo();
    }
    //=============================================================================
    if (bStrategie == "7")
    {
        VerifParRepr aImgVerif(aSetImages, aDirImages, aPatImages, aNameHomol, aOriInput, aHomolOutput, ExpTxt, aDistHom, aDistRepr);
        vector<AbreHomol> aAbre = aImgVerif.creatAbre();
        vector<string>  aAbreRacine= aImgVerif.displayAbreHomol(aImgVerif.mAbre, 1);

        for (uint i=0;i<aAbre.size();i++)
        {
            string aImg1 = aAbre[i].ImgRacine;
            for(uint k=0; k<aAbre[i].ImgBranch.size(); k++)
            {
                cout<<endl<<" ++ "<<aAbre[i].ImgBranch[k]<<endl;
                string aImg2 = aAbre[i].ImgBranch[k];
                CplImg aCouple(aImg1, aImg2, aNameHomol, aOriInput, aHomolOutput, aFullPatternImages, ExpTxt, aPropDiag, aCorel, aSizeVignette, aDisplayVignette, aFiltreBy1Img, aTauxGood, aSizeSearchAutour);
                aCouple.SupposeVecSruf1er(Pt2dr(0,0) , Pt2dr(0,0));
                vector< vector<bool> > ColDec;
                if (aAbre[i].Img3eme[k].size() > 0)
                {
                    for(uint l=0; l<aAbre[i].Img3eme[k].size(); l++)
                    {
                        cout<<"   + Com + "<<aAbre[i].Img3eme[k][l]<<endl;
                        //====Triplet Image==========//
                        aImg1 = aAbre[i].ImgRacine;
                        aImg2 = aAbre[i].ImgBranch[k];
                        string aImg3 = aAbre[i].Img3eme[k][l];
                        ColDec.push_back(aCouple.CalVectorSurface(aImg3, "sphere"));
                    }
                    vector<bool> decision; vector<double> decPoint;
                    for(uint m=0; m<ColDec[0].size(); m++)
                    {
                        bool dec = 0; double point=0;
                        for (uint n=0; n<ColDec.size(); n++)
                        {
                            dec = dec || ColDec[n][m];
                            if (ColDec[n][m])
                            {point++;}
                        }
                        decision.push_back(dec);
                        decPoint.push_back(point);
                    }
                    double countGood = 0;
                    for(uint h=0; h<decision.size(); h++)
                    {
                        if(decision[h] && aCouple.mFiltreBy1Img)
                        {
                            countGood++;
                        }
                        if((decPoint[h] > aCouple.mTauxGood) && !aCouple.mFiltreBy1Img )
                        {
                            countGood++;
                        }
                    }
                    aAbre[i].NbPtFiltre.push_back(countGood);
                    aAbre[i].NbPointHomo.push_back(decision.size());
                    cout<<endl<<countGood/decision.size()*100<<" % Pts conservé of "<<decision.size()<<endl;
                    //creat homol file with decision and pack homo b/w aImg1 aImg2
                    //.....
                    creatHomolFromPair(aImg1, aImg2, aNameHomol, aDirImages, aPatImages, aHomolOutput, ExpTxt, decision);
                }
            }
        }
        //Display result
        for(uint i=0; i<aAbre.size(); i++)
        {
            for (uint j=0; j<aAbre[i].ImgBranch.size(); j++)
            {
               cout<<"Couple ["<<aAbre[i].ImgRacine<<"] [";
                  cout<<aAbre[i].ImgBranch[j]<<"] Nb "<<aAbre[i].NbPtFiltre[j]<<" % Reste ";
                  cout<<aAbre[i].NbPtFiltre[j]/aAbre[i].NbPointHomo[j]*100<<" Of ";
                  cout<<aAbre[i].NbPointHomo[j]<<endl;
            }
        }
    }

    cout<<endl<<"use command SEL ./ img1 img2 KCpl=NKS-Assoc-CplIm2Hom@"<<aHomolOutput<< "@dat to view filtered point homomogues"<<endl<<endl;
    return EXIT_SUCCESS;
}




/*strategie à faire:
1)
Homol1_2 => P1 & P2, Homol2_3 => P2 & P3, triplet [P1, P2, P3].
Pt3d P3' reprojeter ves cam3 à partir P1 et P2
P3' = P3 ?
si OK => fabriquer Homol nouvel avec P1 P2 P3 pour tout les sens

2)
Homol1_2 => P1 & P2 => Pt3d ===reprojeter==> P3
Corellation entre P1 et P3
Corellation entre P2 et P3
si OK => garde P1 et P2
=> P3 juste pour validation point homo entre P1 et P2
Q: comment choisir le pose bien pour cam3 ? (avant ou apres cam1 et cam2)
    
3)
Homol cohérent, pas besoins l'orientation du caméra => si camera orientation est pas bonne, on peut eviter
Homol1_2 => P1 & P2, Homol2_3 => P2 & P3, triplet [P1, P2, P3].
P3 => chercher dans Homol3_1 => si trop loin avec P3 => pas bonne ????????????????????????????????????????
P3 => chercher dans Homol3_2 => si trop loin avec P3 => pas bonne couple P2 P3 ???????????????????????????
Comment faire une methode de validation plus efficace ?
*/

