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
#include "XML_GEN/all_tpl.h"

#if (ELISE_QT_VERSION >= 4)
    #include "general/visual_mainwindow.h"
#endif

template <class Type> void VerifIn(const Type & aV,const Type * aTab,int aNb, const std::string & aMes)
{
     for (int aK=0 ; aK< aNb ; aK++)
     {
         if (aTab[aK] == aV)
         {
           return;
         }
     }
     std::cout << "\n\nIn context " << aMes << "\n";
     std::cout << "With value  " << aV << "\n";

     std::cout << "Allowed  values are   : "  ;
     for (int aK=0 ; aK< aNb ; aK++)
         std::cout <<  aTab[aK] << " " ;

     std::cout <<  "\n";
     ELISE_ASSERT(false,"Value is not in eligible set ");
}

class cAppliMalt
{
     public :
         cAppliMalt(int argc,char ** argv);
         const std::string & Com() {return mCom;}
         const std::string & ComOA() {return mComOA;}

         int Exe();
     private :

          void ReadType(const std::string & aType);
          void InitDefValFromType();
          void ShowParam();

          bool        mModeHelp;
          std::string mStrType;
          eTypeMalt   mType;

          std::string mFullName;
          std::string mDir;
          std::string mOutputDirectory;
          cInterfChantierNameManipulateur * mICNM;
          const cInterfChantierNameManipulateur::tSet * mSetIm;
          int         mNbIm;
          std::string mIms;
          std::string mOri;

          int         mSzW;
          int         mNbMinIV;
          double      mZRegul;
          int         mUseMasqTA;
          int         mZoomFinal;
          int         mZoomInit;
          int         mExe;
          int         mNbEtapeQ;
          bool        mOrthoQ;
          bool        mOrthoF;
          double      mZPas;
          std::string mRep;
          bool        mRepIsAnam;
          std::string mCom;
          std::string mComOA;
          std::string mComTaramaOA;
          std::string mDirTA;
          bool        mPurge;
          bool        mMkFPC;
          bool        mDoMEC;
          bool        mDoOrtho;
          bool        mRoundResol;
          bool        mUseRR;
          bool        mUnAnam;
          bool        mOrthoInAnam;
          bool        mDoubleOrtho;
          double      mZincCalc;
          std::string mDirMEC;
          std::string mDirOrthoF;
          double       mDefCor;
          double       mCostTrans;
          int          mEtapeInit;
          bool         mAffineLast;
          std::string  mImMaster;
          double       mResolOrtho;

          std::string  mImMNT;
          std::string  mImOrtho;
          double       mZMoy;
          bool         mIsSpherik;
          double      mLargMin;
          Pt2dr       mSzGlob;
          std::string  mMasqIm;
          std::string  mMasqImGlob;
          bool        mUseImSec;
          bool        mCorMS;
          bool        mUseGpu;
          double      mIncidMax;
          bool        mGenCubeCorrel;
          bool        mEZA;
          bool        mMaxFlow;
          int         mSzRec;
          std::vector<std::string> mEquiv;
          std::string mMasq3D;
};


int cAppliMalt::Exe()
{
    if (! mExe) return 0;
    int aRes = TopSystem(mCom.c_str());

    if (! mExe) return 0;

    if ((aRes==0) && ( mComTaramaOA !=""))
    {
        aRes = TopSystem(mComTaramaOA.c_str());
    }



    if ((aRes==0) && ( mComOA !=""))
    {
        aRes = TopSystem(mComOA.c_str());
    }



    if (!MMVisualMode) ShowParam();
    return aRes;
}

cAppliMalt::cAppliMalt(int argc,char ** argv) :
    mICNM       (0),
    mNbMinIV    (3),
    mUseMasqTA  (1),
    mZoomInit   (-1),
    mExe        (1),
    mZPas       (0.4),
    mDirTA      ("TA"),
    mPurge      (true),
    mMkFPC      (true),
    mDoMEC      (true),
    mDoOrtho     (true),
    mRoundResol  (false),
    mUseRR       (false),
    mUnAnam      (true),
    mOrthoInAnam (false),
    mDoubleOrtho  (false),
    mZincCalc     (0.3),
    mDirMEC       ("MEC-Malt/"),
    mDirOrthoF    (""),
    mDefCor       (0.2),
    mCostTrans    (2.0),
    mEtapeInit    (1),
    mAffineLast   (true),
    mImMaster     (""),
    mResolOrtho   (1.0),
    mImMNT        (""),
    mImOrtho      (""),
    mIsSpherik    (false),
    mLargMin      (25.0),
    mSzGlob       (0,0),
    mUseImSec     (false),
    mCorMS        (false),
    mUseGpu       (false),
    mGenCubeCorrel (false),
    mEZA           (false),
    mMaxFlow       (false),
    mSzRec         (50)
{

#if(ELISE_QT_VERSION >= 4)

    if (MMVisualMode)
    {
        QApplication app(argc, argv);

        LArgMain LAM;
        LAM << EAMC(mStrType,"Correlation mode",eSAM_None,ListOfVal(eTMalt_NbVals,"eTMalt_"));

        std::vector <cMMSpecArg> aVA = LAM.ExportMMSpec();

        cMMSpecArg aArg = aVA[0];

        list<string> liste_valeur_enum = listPossibleValues(aArg);

        QStringList items;
        list<string>::iterator it=liste_valeur_enum.begin();
        for (; it != liste_valeur_enum.end(); ++it)
            items << QString((*it).c_str());

        setStyleSheet(app);

        bool ok = false;
        int  defaultItem = 0;

        if(argc > 1)
            defaultItem = items.indexOf(QString(argv[1]));

        QInputDialog myDialog;
        QString item = myDialog.getItem(NULL, app.applicationName(),
                                             QString (aArg.Comment().c_str()), items, defaultItem, false, &ok);

        if (ok && !item.isEmpty())
            mStrType = item.toStdString();
        else
            return;

        ReadType(mStrType);
    }
    else
    {
        ELISE_ASSERT(argc >= 2,"Not enough arg");
        ReadType(argv[1]);
    }
#else
    ELISE_ASSERT(argc >= 2,"Not enough arg");
    ReadType(argv[1]);
#endif

    InitDefValFromType();

    Box2dr aBoxClip, aBoxTerrain;
    double aResolTerrain;
    double aRatioResolImage=1;

    bool mModePB = false;
    std::string mModeOri;

    int aNbProc = NbProcSys();
    double mPenalSelImBestNadir = -1;

    bool ForceNoIncid = false;
    bool mForceZFaisc = false;

    Pt2di  aPtDebug;

    ElInitArgMain
    (
        argc,argv,
        LArgMain()
                    << EAMC(mStrType,"Correlation mode (must be in allowed enumerated values)")
                    << EAMC(mFullName,"Full Name (Dir+Pattern)", eSAM_IsPatFile)
                    << EAMC(mOri,"Orientation", eSAM_IsExistDirOri),
        LArgMain()  << EAM(mImMaster,"Master",true," Master image must exist iff Mode=GeomImage, AUTO for Using result of AperoChImSecMM", eSAM_IsExistFileRP)
                    << EAM(mSzW,"SzW",true,"Correlation Window Size (1 means 3x3)")
                    << EAM(mCorMS,"CorMS",true,"New Multi Scale correlation option, def=false, available in image geometry")
                    << EAM(mUseGpu,"UseGpu",true,"Use Cuda acceleration, def=false", eSAM_IsBool)
                    << EAM(mZRegul,"Regul",true,"Regularization factor")
                    << EAM(mDirMEC,"DirMEC",true,"Subdirectory where the results will be stored")
                    << EAM(mDirOrthoF,"DirOF",true,"Subdirectory for ortho (def in Ortho-${DirMEC}) ")
                    << EAM(mUseMasqTA,"UseTA",true,"Use TA as Masq when it exists (Def is true)")
                    << EAM(mZoomFinal,"ZoomF",true,"Final zoom, (Def 2 in ortho,1 in MNE)",eSAM_IsPowerOf2)
                    << EAM(mZoomInit,"ZoomI",true,"Initial Zoom, (Def depends on number of images)",eSAM_NoInit)
                    << EAM(mZPas,"ZPas",true,"Quantification step in equivalent pixel (def=0.4)")
                    << EAM(mExe,"Exe",true,"Execute command (Def is true !!)", eSAM_IsBool)
                    << EAM(mRep,"Repere",true,"Local system of coordinates",eSAM_IsExistFileRP)
                    << EAM(mNbMinIV,"NbVI",true,"Number of Visible Images required (Def = 3)")
                    << EAM(mOrthoF,"HrOr",true,"Compute High Resolution Ortho")
                    << EAM(mOrthoQ,"LrOr",true,"Compute Low Resolution Ortho")
                    << EAM(mDirTA,"DirTA",true,"Directory of TA (for mask)",eSAM_IsDir)
                    << EAM(mPurge,"Purge",true,"Purge the directory of Results before compute")
                    << EAM(mDoMEC,"DoMEC",true,"Do the Matching")
                    << EAM(mDoOrtho,"DoOrtho",true,"Do the Ortho (Def=mDoMEC)")
                    << EAM(mUnAnam,"UnAnam",true,"Compute the un-anamorphosed DTM and ortho (Def context dependent)")
                    << EAM(mDoubleOrtho,"2Ortho",true,"Do both anamorphosed ans un-anamorphosed ortho (when applyable) ")
                    << EAM(mZincCalc,"ZInc",true,"Incertitude on Z (in proportion of average depth, def=0.3) ")
                    << EAM(mDefCor,"DefCor",true,"Default Correlation in un correlated pixels (Def=0.2) ")
                    << EAM(mCostTrans,"CostTrans",true,"Cost to change from correlation to uncorrelation (Def=2.0) ")
                    << EAM(mEtapeInit,"Etape0",true,"First Step (Def=1) ")
                    << EAM(mAffineLast,"AffineLast",true,"Affine Last Etape with Step Z/2 (Def=true) ")
                    << EAM(mResolOrtho,"ResolOrtho",true,"Resolution of ortho, relatively to images (Def=1.0; 0.5 means smaller images, i.e. decrease the resolution) ")
                    << EAM(mImMNT,"ImMNT",true,"Filter to select images used for matching (Def All, usable with ortho) ",eSAM_IsPatFile)
                    << EAM(mImOrtho,"ImOrtho",true,"Filter to select images used for ortho (Def All) ",eSAM_IsPatFile)
                    << EAM(mZMoy,"ZMoy",true,"Average value of Z", eSAM_NoInit)
                    << EAM(mIsSpherik,"Spherik",true,"If true the surface for rectification is a sphere")
                    << EAM(mLargMin,"WMI",true,"Mininum width of reduced images (to fix ZoomInit)")
                    << EAM(mMasqIm,"MasqIm",true,"Masq per Im; Def None; Use \"Masq\" for standard result of SaisieMasq", eSAM_NoInit)
                    << EAM(mMasqImGlob,"MasqImGlob",true,"Glob Masq per Im : if uses, give full name of masq (for ex toto.tif) ", eSAM_IsExistFileRP)
                    << EAM(mIncidMax,"IncMax",true,"Maximum incidence of image", eSAM_NoInit)
                    << EAM(aBoxClip,"BoxClip",true,"To Clip Computation, normalized image coordinates ([0,0,1,1] means full box)", eSAM_Normalize)
                    << EAM(aBoxTerrain,"BoxTerrain",true,"([Xmin,Ymin,Xmax,Ymax])")
                    << EAM(aResolTerrain,"ResolTerrain",true,"Ground Resol (Def automatically computed)", eSAM_NoInit)
                    << EAM(aRatioResolImage,"RRI",true,"Ratio Resol Image (f.e. if set to 0.8 and image resol is 2.0, will be computed at 1.6)", eSAM_NoInit)
                    << EAM(mRoundResol,"RoundResol",true,"Use rounding of resolution (def context dependent,tuning purpose)", eSAM_InternalUse)
                    << EAM(mGenCubeCorrel,"GCC",true,"Generate export for Cube Correlation")
                    << EAM(mEZA,"EZA",true,"Export Z Absolute")
                    << EAM(mEquiv,"Equiv",true,"Equivalent classes, as a set of pattern, def=None")
                    << EAM(mModeOri,"MOri",true,"Mode Orientation (GRID or RTO) if not XML frame camera", eSAM_NoInit)
                    << EAM(mMaxFlow,"MaxFlow",true,"Use MaxFlow(MinCut) instead of 2D ProgDyn (SGM), slower sometime better, Def=false ")
                    << EAM(mSzRec,"SzRec",true,"Sz of overlap between computation tiles, Def=50; for some rare side effects")
                    << EAM(mMasq3D,"Masq3D",true,"Name of 3D Masq", eSAM_IsExistFile)
                    << EAM(aNbProc,"NbProc",true,"Nb Proc Used")
                    << EAM(mPenalSelImBestNadir,"PSIBN",true,"Penal for Automatic Selection of Images to Best Nadir (Def=-1, dont use)", eSAM_InternalUse)
                    << EAM(ForceNoIncid,"InternalNoIncid",true,"Internal Use", eSAM_InternalUse)
                    << EAM(aPtDebug,"PtDebug",true,"Internal Use (Point of debuging)", eSAM_InternalUse)
                    << EAM(mForceZFaisc,"ForceZFais",true,"Force Z Faisecau evan with stenope camera", eSAM_InternalUse)
 
                );

    if (!MMVisualMode)
    {
      bool DoIncid = false;
#if CUDA_ENABLED == 0
      ELISE_ASSERT(!mUseGpu , "NO CUDA VERSION");
#endif

      if(mUseGpu && mSzW > 3) // TEMPORAIRE
          mSzW = 3;




      std::string mFullModeOri = "eGeomImageOri";
      mModePB = EAMIsInit(&mModeOri);
      if (mModePB)
      {
          ELISE_ASSERT(EAMIsInit(&mModeOri) , "MOri is Mandatory in PB");
          ELISE_ASSERT(EAMIsInit(&mZMoy)    , "ZMoy is Mandatory in PB");
          ELISE_ASSERT(EAMIsInit(&mZincCalc)    , "ZInc is Mandatory in PB");
          ELISE_ASSERT(EAMIsInit(&mZoomInit)    , "ZoomI is Mandatory in PB");

          if (mModeOri=="GRID")     mFullModeOri= "eGeomImageGrille";
          else if (mModeOri=="RTO") mFullModeOri= "eGeomImageRTO";
          else  {ELISE_ASSERT(false,"Unknown mode ori");}
      }



      mUseRR = EAMIsInit(&mRoundResol);


      if (!EAMIsInit(&mDoOrtho))
      {
          mDoOrtho=mDoMEC;
      }


      mUseImSec = (mImMaster == std::string("AUTO"));


      if (mEtapeInit!=1)
          mPurge = false;
      MakeFileDirCompl(mDirMEC);
      if (mDirOrthoF=="")
          mDirOrthoF = "Ortho-" + mDirMEC;
      MakeFileDirCompl(mDirOrthoF);


      if (mModeHelp)
          StdEXIT(-1);

      {
          int TabZF[4] ={1,2,4,8};
          VerifIn(mZoomFinal,TabZF,4,"ZoomFinal");
      }

#if (ELISE_windows)
      replace( mFullName.begin(), mFullName.end(), '\\', '/' );
#endif
      SplitDirAndFile(mDir,mIms,mFullName);
      std::string aFullNameCorrc = mDir + mIms;
      setInputDirectory(mDir);
      mOutputDirectory = (isUsingSeparateDirectories()?MMOutputDirectory():mDir);



      if (mUseImSec)
      {
          ELISE_ASSERT((mType==eGeomImage),"Illegal combination with UseImSec");
          mImMaster = mIms;
      }
      else if ((mImMaster!="") != (mType==eGeomImage))
      {
          std::cout << "Master Image =[" << mImMaster << "] , mode = " << mStrType << "\n";
          ELISE_ASSERT
                  (
                      false,
                      "Incoherence : master image must exist iff mode==GeomImage"
                      );
      }



      mICNM = cInterfChantierNameManipulateur::BasicAlloc(mDir);
      if (! mModePB)
      {
          mICNM->CorrecNameOrient(mOri);
      }
      mSetIm = mICNM->Get(mIms);
      mNbIm = (int)mSetIm->size();
      ELISE_ASSERT((mNbIm>=2)|mUseImSec,"Not Enough image in Pattern");

      std::string aKeyOri = "NKS-Assoc-Im2Orient@-" + mOri;
      double aSomZM = 0;
      double aSomResol = 0;
      int    aNbZM = 0;


      if ((! EAMIsInit(&mNbMinIV))  && (mNbIm< mNbMinIV) )
      {
           mNbMinIV = mNbIm;
      }

      if ((mNbIm < mNbMinIV) && (!mUseImSec))
      {
          std::cout << "For Nb Im = " << mNbIm << " NbVI= " << mNbMinIV << "\n";
          ELISE_ASSERT(false,"Nb image is < to min visible image ...");
      }


      bool hasNewGenImage =false;
      if (! mModePB)
      {
          // MPD : Ajout le 22/05/2015; car peut creer pb  si l'utilisateur a purge la directory
          MakeXmlXifInfo(mFullName,mICNM);
          for (int aKIm = 0; aKIm<mNbIm ; aKIm++)
          {
              const std::string & aNameIm = (*mSetIm)[aKIm];

/*
              std::string aNameOri = mICNM->Assoc1To1(aKeyOri,aNameIm,true);

              //ToDo: Faire evoluer ce code pour pouvoir gerer d'autres type d'orientation (Grille et RTO).
              // utilisation d'une ElCamera (avec cCameraModuleOrientation pour le cas des ModuleOrientation)

              CamStenope *  aCS = CamOrientGenFromFile(aNameOri,mICNM);
*/
              cBasicGeomCap3D * aCG =  mICNM->StdCamGenOfNames(mOri,aNameIm);

              if (aCG->DownCastCS() == 0)
              {
                 hasNewGenImage = true;
/*
                 Pt2dr aPMil = aCG->SzBasicCapt3D()/2.0;
                 Pt3dr aCOpt = aCG->OpticalCenterOfPixel()


                 std::cout << "IIIPPPP " << aCG->GetVeryRoughInterProf() 
                           << " P0 " << aCG->PMoyOfCenter()
                           << "\n"; getchar();
*/
              }

              if (aCG->HasRoughCapteur2Terrain())
              {
                  aSomZM += aCG->PMoyOfCenter().z;
                  aSomResol +=  aCG->GlobResol();
                  aNbZM++;
              }

              Pt2di aCorns[4];
              Box2di aBx(Pt2di(0,0), aCG->SzBasicCapt3D());
              aBx.Corners(aCorns);
              Pt2dr aP0(0,0);
              for (int aKC=0 ; aKC< 4 ; aKC++)
              {
                  aP0.SetSup(aCG->OrGlbImaM2C(Pt2dr(aCorns[aKC])));
              }


              mSzGlob = mSzGlob + aP0;
          }
          mSzGlob = mSzGlob / double(mNbIm);
      }

      if (hasNewGenImage)
         mFullModeOri= "eGeomGen";

if(0)
{
   for (int aK=0 ; aK<10 ; aK++) std::cout << "HASNEWIMAGE " << hasNewGenImage << "\n";
   getchar();
}

      bool ZMoyInit = EAMIsInit(&mZMoy)  && (mType != eGeomImage);
      bool IncMaxInit = EAMIsInit(&mIncidMax)  && (mType != eGeomImage);
      if (!ZMoyInit)
      {
          if (IncMaxInit)
          {
              ELISE_ASSERT(aNbZM!=0,"Cannot get ZMoy with IncMax");
              ZMoyInit = true;
              mZMoy = aSomZM / aNbZM;
          }
      }
      bool ResolTerrainIsInit = EAMIsInit(&aResolTerrain);
      bool aRSRT = false;
      if (!ResolTerrainIsInit)
      {
          if (IncMaxInit)
          {
              ELISE_ASSERT(aNbZM!=0,"Cannot get ZMoy with IncMax");
              ResolTerrainIsInit = true;
              aResolTerrain = aSomResol / aNbZM;
              aRSRT = true;
          }
      }


      bool IsOrthoXCSte = false;
      bool IsAnamXCsteOfCart = false;
      if (mType!=eGeomImage)
      {
          mRepIsAnam =   (mRep!="") && RepereIsAnam(mDir+mRep,IsOrthoXCSte,IsAnamXCsteOfCart);
      }
      mUnAnam = mUnAnam && IsOrthoXCSte;

      if (mUnAnam)
      {
          mOrthoInAnam = mOrthoF;
          if (! mDoubleOrtho)
          {
              mOrthoF = false;
              mOrthoQ = false;
          }
      }

      if (mZoomInit!=-1)
      {
          int TabZI[6] ={128,64,32,16,8,4};
          VerifIn(mZoomInit,TabZI,6,"Zoom Init");
      }
      else
      {
          if (mNbIm > 1000)
              mZoomInit = 128;
          else if (mNbIm > 100)
              mZoomInit = 64;
          else if (mNbIm > 10 )
              mZoomInit = 32;
          else
              mZoomInit = 32;

          double aWidth = ElMin(mSzGlob.x,mSzGlob.y);
          while (((aWidth/mZoomInit) < mLargMin) && (mZoomInit>16))
          {
              mZoomInit /=2;
          }
      }


      bool UseMTAOri = ( mUseMasqTA!=0 );

      mUseMasqTA =    UseMTAOri
              && ELISE_fp::exist_file(mDir+ELISE_CAR_DIR+ mDirTA +ELISE_CAR_DIR+"TA_LeChantier_Masq.tif");

      std::string FileMasqT = mUseMasqTA ? "MM-MasqTerrain.xml" : "EmptyXML.xml";

      if (mImMaster!="")
      {
          if (! EAMIsInit(&mDirMEC))
          {
              mDirMEC = "MM-Malt-Img-" + StdPrefix(mImMaster) +ELISE_CAR_DIR;
          }
          mUseMasqTA = UseMTAOri && ELISE_fp::exist_file(mDir+StdPrefix(mImMaster)+"_Masq.tif");
          if (mUseMasqTA)
              FileMasqT = "MM-MasqImage.xml";
      }

      // mZoomInit

      std::string aFileMM = "MM-Malt.xml";

      if (0)
      {
          std::cout << "TTTTESSTTTTTT  MALT  !!!!!!!!\n";//   getchar();
          aFileMM = "Test-MM-Malt.xml";
          mPurge = false;
          mMkFPC = false;
      }


      mNbEtapeQ =    1   // Num premiere etape
              + round_ni(log2(mZoomInit/ mZoomFinal))  // Si aucune dupl
              + 1   //  Dulication de pas a la premiere
              + (mAffineLast ? 1 : 0)  ;  // Raffinement de pas;

      std::cout << 3+ log2(mZoomInit/ mZoomFinal)  << "\n";
      ShowParam();


      mPurge = mPurge &&  mDoMEC;

      std::string  anArgCommuns =   std::string(" WorkDir=") + mDir
              +  std::string(" +ImPat=") + QUOTE(mIms)
              +  std::string(" +DirMEC=") + mDirMEC
              +  std::string(" +ZoomFinal=") + ToString(mZoomFinal)
              +  std::string(" +Ori=") + mOri
              +  std::string(" +ResolRelOrhto=") + ToString(1/(mResolOrtho*mZoomFinal))
              //   ==    +  std::string(" +DirTA=") + mDirTA
              +  std::string(" +NbProc=") + ToString(aNbProc)
              ;


      bool ModeFaisZ = mModePB| hasNewGenImage | mForceZFaisc;
      std::string aNameGeom = (mImMaster=="") ?
                  "eGeomMNTEuclid" :
                  (mIsSpherik? "eGeomMNTFaisceauPrChSpherik" : ( ModeFaisZ ? "eGeomMNTFaisceauIm1ZTerrain_Px1D" : "eGeomMNTFaisceauIm1PrCh_Px1D"));

      mCom =              MM3dBinFile_quotes("MICMAC")
              +  ToStrBlkCorr( Basic_XML_MM_File(aFileMM) )
              + anArgCommuns
              +  std::string(" +DirTA=") + mDirTA

              /*
                              +  std::string(" +DirTA=") + mDirTA
                              +  std::string(" WorkDir=") + mDir
                              +  std::string(" +ImPat=") + QUOTE(mIms)
                              +  std::string(" +DirMEC=") + mDirMEC
                              +  std::string(" +ZoomFinal=") + ToString(mZoomFinal)
                              +  std::string(" +Ori=") + mOri
                              +  std::string(" +ResolRelOrhto=") + ToString(1.0/mZoomFinal)
                              +  std::string(" +DirOrthoF=") + mDirOrthoF
        */


              +  std::string(" +DirOrthoF=") + mDirOrthoF
              +  std::string(" +ZRegul=") + ToString(mZRegul)
              +  std::string(" +SzW=") + ToString(mSzW)
              +  std::string(" +ZoomInit=") + ToString(mZoomInit)
              +  std::string(" +FileMasqT=") + FileMasqT

              +  std::string(" +ZPas=") + ToString(mZPas)
              +  std::string(" +DbleZPas=") + ToString(mZPas*2)
              +  std::string(" +DemiZPas=") + ToString(mZPas/2)
              +  std::string(" +NbMinIV=") + ToString(mNbMinIV)

              + std::string(" +FileOthoF=") + (mOrthoF ? "MM-Malt-OrthoFinal.xml" : "EmptyXML.xml")
              + std::string(" +FileOthoQ=") + (mOrthoQ ? "MM-Malt-OrthoQuick.xml" : "EmptyXML.xml")

              + std::string(" +FileUnAnam=") + (mUnAnam ? "MM-Malt-UnAnam.xml" : "EmptyXML.xml")

              + std::string(" +Purge=") + (mPurge ? "true" : "false")
              + std::string(" +MkFPC=") + (mMkFPC ? "true" : "false")
              + std::string(" +DoMEC=") + (mDoMEC ? "true" : "false")
              + std::string(" +UseGpu=") + (mUseGpu ? "true" : "false")
              + std::string(" +ZIncCalc=") + ToString(mZincCalc)
              + std::string(" +NbEtapeQuant=") + ToString(mNbEtapeQ)
              + std::string(" +DefCor=") + ToString(mDefCor)
              + std::string(" +CostTrans=") + ToString(mCostTrans)
              + std::string(" +Geom=") + aNameGeom
              ;

      mCom = mCom + " +RSRT=" + ToString(aRSRT);

      if (! mDoOrtho)
      {
          mCom =  mCom + " +ButDoOrtho=false";
      }

      if (EAMIsInit(&mMasqIm))
      {
          CorrecNameMasq(mDir,mFullName,mMasqIm);
      //SplitDirAndFile(mDir,mIms,mFullName);
          mCom =  mCom
                  +  std::string(" +UseMasqPerIm=true")
                  +  std::string(" +MasqPerIm=") + mMasqIm
                  ;
      }
      if (EAMIsInit(&mMasqImGlob))
      {
          // CorrecNameMasq(mDir,mFullName,mMasqIm);
      //SplitDirAndFile(mDir,mIms,mFullName);
          mCom =  mCom
                  +  std::string(" +UseGlobMasqPerIm=true")
                  +  std::string(" +GlobMasqPerIm=") + mMasqImGlob
                  ;
      }

      if (EAMIsInit(&aPtDebug))
      {
          mCom =  mCom
                  + "  +UsePtDebug=true"
                  + " +PtDebugX=" + ToString(aPtDebug.x)
                  + " +PtDebugY=" + ToString(aPtDebug.y)
                  + std::string(" ");
      }

      mCom =  mCom + " +ModeOriIm=" + mFullModeOri + " ";
      if (hasNewGenImage)
      {
         mCom =  mCom + "  +UseGenBundle=true ";
      }
      if (EAMIsInit(&mModeOri) || hasNewGenImage)
          mCom =  mCom 
                  + std::string(" +Conik=false")
                  +  std::string(" +ZIncIsProp=false ")

                  ;

     if (EAMIsInit(&mMasq3D))
     {
          mCom = mCom+ " +UseMasq3D=true +NameMasq3D=" + mMasq3D + " ";
     }


      if (mImMaster != "")
      {
          mCom =  mCom
                  + std::string(" +ImageMaster=") + mImMaster
                  + std::string(" +ImageMasterSsPost=") + StdPrefix(mImMaster)
                  + std::string(" +FileIm1=") + "MM-ImageMaster.xml"
                  + std::string(" +ZIncIsProp=") + "false"
                  + std::string(" +FullIm1=") + "true"
                  + std::string(" +PasInPixel=") + "false"
                  ;
      }

      if (mRep!="")
      {
         if (mType==eGeomImage)
         {
               cRepereCartesien aRC = StdGetFromPCP(mDir+mRep,RepereCartesien);
               mCom =  mCom
                       + std::string(" +SpecDirFaisc=true")
                       + std::string(" +DirFaisX=") + ToString(-aRC.Oz().x)
                       + std::string(" +DirFaisY=") + ToString(-aRC.Oz().y)
                       + std::string(" +DirFaisZ=") + ToString(-aRC.Oz().z)
                       + " "
                     ;
         }
         else
         {
             if (!mRepIsAnam)
             {
                 mCom = mCom +  std::string(" +Repere=") + mRep;
             }
             else
             {
                 DoIncid = true;
                 mCom =    mCom
                         +  std::string(" +DoAnam=true ")
                         +  std::string(" +ParamAnam=") + mRep;
             }
         }
      }

      if (!mAffineLast)
      {
          mCom = mCom +  " +FileZ1Raff=EmptyXML.xml";
      }

      if (mEtapeInit!=1)
      {
          mCom = mCom + " FirstEtapeMEC="+ToString(mEtapeInit);
      }


      if (mZoomFinal==1)
      {
          mCom = mCom + std::string(" +FileZ2PC=MM-Zoom2-PC.xml") ;
      }
      else if (mZoomFinal==4)
      {
          mCom = mCom + std::string(" +FileZ4PC=EmptyXML.xml") ;
      }
      else if (mZoomFinal==8)
      {
          mCom = mCom + std::string(" +FileZ4PC=EmptyXML.xml") ;
      }

      if (mZoomInit >= 128)
          mCom = mCom + std::string(" +FileZ64=MM-Zoom64.xml");
      if (mZoomInit >=64)
          mCom = mCom + std::string(" +FileZ32=MM-Zoom32.xml");
      if (mZoomInit <= 16)
          mCom = mCom + std::string(" +FileZ16==EmptyXML.xml");

      if (mUseImSec)
          mCom = mCom + std::string(" +UseImSec=true");
      if (mCorMS)
      {
          mCom = mCom + std::string(" +CorMS=true");
          if (mType!=eGeomImage) mCom = mCom + std::string(" +MSDense=false");
      }

      if (mGenCubeCorrel)
          mCom = mCom + std::string(" +GCC=true");

      if (EAMIsInit(&mEZA))
          mCom = mCom + std::string(" +EZA=") + ToString(mEZA);

      if (ZMoyInit)
      {
          mCom = mCom + " +FileZMoy=File-ZMoy.xml"
                  + " +ZMoy=" + ToString(mZMoy);
      }

      if (EAMIsInit(&aBoxClip))
      {
          mCom  =    mCom + " +UseClip=true "
                  +  std::string(" +X0Clip=") + ToString(aBoxClip._p0.x)
                  +  std::string(" +Y0Clip=") + ToString(aBoxClip._p0.y)
                  +  std::string(" +X1Clip=") + ToString(aBoxClip._p1.x)
                  +  std::string(" +Y1Clip=") + ToString(aBoxClip._p1.y) ;
      }

      if (EAMIsInit(&aRatioResolImage))
      {
           mCom  =    mCom + " +UseRatioResolImage=true " + " +RatioResolImage=" + ToString(aRatioResolImage);
      }
      if (ResolTerrainIsInit)
      {
          mCom  =    mCom + " +UseResolTerrain=true "
                  +  std::string(" +ResolTerrain=") + ToString(aResolTerrain);
      }
      if (EAMIsInit(&aBoxTerrain))
      {
          mCom  =    mCom + " +UseBoxTerrain=true "
                  +  std::string(" +X0Terrain=") + ToString(aBoxTerrain._p0.x)
                  +  std::string(" +Y0Terrain=") + ToString(aBoxTerrain._p0.y)
                  +  std::string(" +X1Terrain=") + ToString(aBoxTerrain._p1.x)
                  +  std::string(" +Y1Terrain=") + ToString(aBoxTerrain._p1.y) ;
      }

      if (EAMIsInit(&mMaxFlow)) mCom = mCom + " +AlgoMaxFlow=" + ToString(mMaxFlow);
      if (EAMIsInit(&mSzRec))   mCom = mCom + " +SzRec=" + ToString(mSzRec);

      if (mUseRR)
      {
          mCom = mCom + " +UseRR=true +RoundResol=" + ToString(mRoundResol);
      }


      if (mType==eGeomImage)
      {
          mCom = mCom + " +ModeAgrCor=eAggregMoyMedIm1Maitre";
      }

      if (EAMIsInit(&mIncidMax))
      {
          DoIncid = true;
          mCom   =  mCom + " +IncidMax=" + ToString(mIncidMax);
      }

      if (mEquiv.size() != 0)
      {
          mCom= mCom + "  +UseEqui=true";
          if (mEquiv.size()>0)
              mCom= mCom + " +UseClas1=true" + " +Clas1=" +QUOTE(mEquiv[0]);
          if (mEquiv.size()>1)
              mCom= mCom + " +UseClas2=true" + " +Clas2=" +QUOTE(mEquiv[1]);
          if (mEquiv.size()>2)
              mCom= mCom + " +UseClas3=true" + " +Clas3=" +QUOTE(mEquiv[2]);

          if (mEquiv.size()>3)
              ELISE_ASSERT(false,"too many equiv class for Malt, use MicMac");
      }
      if (mPenalSelImBestNadir>0)
      {
         mCom   =  mCom + " +DoIncid=true +DoMaskNadir=true ";
      }

      if (DoIncid && (!ForceNoIncid))
      {
         mCom   =  mCom + " +DoIncid=true ";
      }

      std::cout << mCom << "\n";
      // cInZRegulterfChantierNameManipulateur * aCINM = cInterfChantierNameManipulateur::BasicAlloc(aDir);

      if (mImMNT !="") mCom   =  mCom + std::string(" +ImMNT=")   + QUOTE(mImMNT);
      if (mImOrtho !="") mCom =  mCom + std::string(" +ImOrtho=") + QUOTE(mImOrtho);
      if (mOrthoInAnam)
      {
          std::string aFileOAM  = "MM-Malt-OrthoAnamOnly.xml";

          mComOA =  MMDir() +"bin"+ELISE_CAR_DIR+"MICMAC "
                  + MMDir() +"include"+ELISE_CAR_DIR+"XML_MicMac"+ELISE_CAR_DIR+aFileOAM // MM-Malt.xml
                  + anArgCommuns
                  +  std::string(" +DirTA=TA-UnAnam") ;

          mComOA =        mComOA
                  +  std::string(" +Repere=") + mRep
                  +  std::string(" +DirOrthoF=") +  "Ortho-UnAnam-" + mDirMEC
                  ;
          if (! IsAnamXCsteOfCart) mComOA = mComOA + " +UseRepere=false +UseAnam=true ";

          if (mImMNT !="") mComOA   =  mComOA + std::string(" +ImMNT=")   + mImMNT;
          if (mImOrtho !="") mComOA =  mComOA + std::string(" +ImOrtho=") + mImOrtho;
          std::cout << "\n\n" << mComOA << "\n";

           mComTaramaOA =     MMBinFile("Tarama") + " "
                           +  QUOTE(mFullName)           + " "
                           +  mOri                + " "
                           + std::string(" Zoom=16 ")
                           + std::string(" Out=TA-UnAnam ")
                           + " Repere=" + mRep
                           + std::string(" UnUseAXC=true");

// std::cout << "GEETTCHAR\n"; getchar();
      }
  }
}

// mDirOrthoF = "Ortho-" + mDirMEC;

void cAppliMalt::ReadType(const std::string & aType)
{
    mStrType = aType;
    StdReadEnum(mModeHelp,mType,mStrType,eNbTypesMNE);
}

void cAppliMalt::InitDefValFromType()
{
    switch (mType)
    {
    case eOrtho :
        mSzW = 2;
        mZRegul = 0.05;
        mZoomFinal = 2;
        mOrthoF = true;
        mOrthoQ = true;
        mAffineLast = (mZoomFinal != 1);
        break;

    case eUrbanMNE :
        mSzW = 1;
        mZRegul = 0.02;
        mZoomFinal = 1;
        mOrthoF = false;
        mOrthoQ = false;
        mAffineLast = false;
        break;

    case eGeomImage :
        mSzW = 1;
        mZRegul = 0.02;
        mZoomFinal = 1;
        mOrthoF = false;
        mOrthoQ = false;
        mAffineLast = false;
        break;

    case eNbTypesMNE :
        break;
    };
}

void  cAppliMalt::ShowParam()
{
    std::cout << "============= PARAMS ========== \n";

    std::cout <<  " -  SzWindow " << mSzW << "  (i.e. : " << 1+2*mSzW << "x" << 1+2*mSzW << ")\n";
    std::cout <<  " -  Regul " <<  mZRegul   << "\n";
    std::cout <<  " -  Final Zoom " <<  mZoomFinal   << "\n";
    std::cout <<  " -  Initial Zoom " <<  mZoomInit   << "\n";
    std::cout <<  " -  Use TA as Mask  " <<  (mUseMasqTA ? " Yes" : " No")   << "\n";
    std::cout <<  " -  Z Step : " <<  mZPas   << "\n";
    if (mRep!="")
        std::cout <<  " -  Repere  : " <<  mRep   << "\n";
    std::cout <<  " -  Nb Min Visible Images : " <<  mNbMinIV   << "\n";
    std::cout << "================================ \n";
}






int Malt_main(int argc,char ** argv)
{
    MMD_InitArgcArgv(argc,argv);
    cAppliMalt anAppli(argc,argv);


    int aRes = anAppli.Exe();
    BanniereMM3D();
    return aRes;
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
