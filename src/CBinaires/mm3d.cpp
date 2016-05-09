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

#define DEF_OFSET -12349876

int XLib_Main(int argc, char ** argv);

typedef int (*tCommande)  (int,char**);

class cArgLogCom
{
    public :

        cArgLogCom(int aNumArg,const std::string &aDirSup = "") :
            mNumArgDir (aNumArg),
            mDirSup    (aDirSup)
        {
        }

        int mNumArgDir ;
        std::string  mDirSup;

        static const cArgLogCom NoLog;
};

const cArgLogCom  cArgLogCom::NoLog(-1);

// MPD : suspecte un problème d'écrasement mutuel entre processus dans le logfile, inhibe temporairement pour
// valider / invalider le diagnostic
static bool DOLOG_MM3d = true;

FILE * FileLogMM3d(const std::string & aDir)
{
    // return  FopenNN(aDir+"mm3d-LogFile.txt","a+","Log File");
    std::string aName = aDir+"mm3d-LogFile.txt";
    FILE * aRes = 0;
    int aCpt = 0;
    int aCptMax = 20;
    while (aRes==0)
    {
        aRes = fopen(aName.c_str(),"a+");
        if (aRes ==0)
        {
             int aModulo = 1000;
             int aPId = mm_getpid();

             double aTimeSleep = ((aPId%aModulo) *  ((aCpt+1) /double(aCptMax)))  / double(aModulo * 20.0);
             SleepProcess (aTimeSleep);
        }
        aCpt++;
        ELISE_ASSERT(aCpt<aCptMax,"Too max test in FileLogMM3d");
    }
    return aRes;
}

#include <ctime>

void LogTime(FILE * aFp,const std::string & aMes)
{

  time_t rawtime;
  struct tm * timeinfo;

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );

  fprintf(aFp," PID : %d ;   %s %s",mm_getpid(),aMes.c_str(),asctime (timeinfo));
}

void LogIn(int  argc,char **  argv,const std::string & aDir,int aNumArgDir)
{
   if (! DOLOG_MM3d) return;
   FILE * aFp = FileLogMM3d(aDir);

   fprintf(aFp,"=================================================================\n");
   for (int aK=0 ; aK< argc ; aK++)
   {
       // MPD : je l'avais deja fait il y a 15 jours, ai pas du commite !!!!  Ca facilite copier-coller sur commande
       fprintf(aFp,"\"%s\" ",argv[aK]);
   }
   fprintf(aFp,"\n");
   LogTime(aFp,"[Beginning at ]");

   fclose(aFp);
}

void LogOut(int aRes,const std::string & aDir)
{
   if (! DOLOG_MM3d) return;

   FILE * aFp = FileLogMM3d(aDir);
   std::string aMes;
   if (aRes==0)
      aMes = "[Ending correctly at]";
   else
      aMes =  std::string("[Failing with code ") + ToString(aRes) +   " at ]" ;
   LogTime(aFp,aMes);
   fclose(aFp);
}




// CMMCom is a descriptor of a MicMac Command
class cMMCom
{
   public :
      cMMCom
      (
             const std::string & aName,
             tCommande  aCommand,
             const std::string & aComment,
             const cArgLogCom& aLog=cArgLogCom::NoLog
      ) :
          mName     (aName),
          mLowName  (StrToLower(aName)),
          mCommand  (aCommand),
          mComment  (aComment),
          mLog     (aLog)
      {
      }



      std::string  mName;
      std::string  mLowName;
      tCommande    mCommand;
      std::string  mComment;
      cArgLogCom  mLog;
};

class cCmpMMCom
{
public :

    cCmpMMCom(){}

    // Comparison; not case sensitive.
    bool operator ()(const cMMCom & aArg0, const cMMCom & aArg1)
    {
        string first  = aArg0.mName;
        string second = aArg1.mName;

        unsigned int i=0;
        while ((i < first.length()) && (i < second.length()))
        {
            if (tolower (first[i]) < tolower (second[i])) return true;
            else if (tolower (first[i]) > tolower (second[i])) return false;
            i++;
        }

        if (first.length() < second.length()) return true;
        else return false;
    }
};

extern int PHO_MI_main(int , char **);
extern int CCL_main(int , char **);
extern int ReprojImg_main(int , char **);
extern int TestRegEx_main(int , char **);
extern int InitOriLinear_main(int , char **);
extern int ExtractMesure2D_main(int , char **);
extern int TestElParseDir_main(int , char **);
extern int Kugelhupf_main(int , char **);
extern int FFTKugelhupf_main(int , char **);
extern int SimplePredict_main(int , char **);
extern int SimpleFusionCarte_main(int , char **);


extern int CPP_Martini_main(int , char **);
extern int CPP_MartiniGin_main(int , char **);

extern int CPP_SetExif(int argc,char **argv);

extern int GenerateAppLiLiaison_main(int argc,char **argv);
extern int TestNameCalib_main(int argc,char ** argv);
extern int Init3App_Main(int argc,char ** argv);

extern int  CPP_CmpOriCam_main(int argc, char** argv);


extern int CPP_ConvertBundleGen(int argc,char ** argv);


int TiePMS_main(int argc,char ** argv);
int TiePLine_main(int argc,char ** argv);
int TiePAll_main(int argc,char ** argv);

int  OneReechFid_main(int argc,char ** argv);

int TNR_main(int argc,char ** argv);
int  Apero2NVM_main(int argc,char ** argv);

int Vino_Main(int argc, char ** argv);
int XifGps2Txt_main(int argc,char ** argv);
int TestOscarTieP_main(int argc,char **argv) ;
int OriRedTie_main(int argc,char **argv) ;
int HomFusionPDVUnik_main(int argc,char **argv) ;
int TestDistM2C_main(int argc,char ** argv);


const std::vector<cMMCom> & getAvailableCommands()
{
   static std::vector<cMMCom> aRes;
   if (aRes.empty())
   {
       aRes.push_back(cMMCom("TestOscar",TestOscarTieP_main,"Test tie points filtering "));
       aRes.push_back(cMMCom("OriRedTieP",OriRedTie_main,"Tie points filtering, using Martini results "));
       aRes.push_back(cMMCom("Vino",Vino_Main,"Image Viewer"));
       aRes.push_back(cMMCom("TripleSec",TNR_main,"Test Non Regression"));
       aRes.push_back(cMMCom("TiePMS",TiePMS_main," matches points of interest of two images"));
       aRes.push_back(cMMCom("TiePLine",TiePLine_main," matches points of interest of two images"));
       aRes.push_back(cMMCom("TiePAll",TiePAll_main," matches points of interest of two images"));

       aRes.push_back(cMMCom("Ann",Ann_main," matches points of interest of two images"));
       aRes.push_back(cMMCom("AperiCloud",AperiCloud_main," Visualization of camera in ply file",cArgLogCom(2)));
       aRes.push_back(cMMCom("Apero",Apero_main," Compute external and internal orientations"));
       aRes.push_back(cMMCom("Arsenic",Arsenic_main," IN DEV : Radiometric equalization from tie points"));
       aRes.push_back(cMMCom("Digeo",Digeo_main," In development- Will compute tie points "));
       aRes.push_back(cMMCom("AperoChImSecMM",AperoChImMM_main," Select secondary images for MicMac",cArgLogCom(2)));
       aRes.push_back(cMMCom("Apero2PMVS",Apero2PMVS_main," Convert Orientation from Apero-Micmac workflow to PMVS format"));
       aRes.push_back(cMMCom("Apero2Meshlab", Apero2Meshlab_main, "Convert Orientation from Apero-Micmac workflow to a meshlab-compatible format"));
       aRes.push_back(cMMCom("Bascule",Bascule_main," Generate orientations coherent with some physical information on the scene",cArgLogCom(2)));
       aRes.push_back(cMMCom("BatchFDC",BatchFDC_main," Tool for batching a set of commands"));
       aRes.push_back(cMMCom("Campari",Campari_main," Interface to Apero, for compensation of heterogeneous measures",cArgLogCom(2)));
       aRes.push_back(cMMCom("ChgSysCo",ChgSysCo_main," Change coordinate system of orientation",cArgLogCom(2)));
       aRes.push_back(cMMCom("CmpCalib",CmpCalib_main," Compare two  calibrations"));
       aRes.push_back(cMMCom("CmpOri",CPP_CmpOriCam_main," Compare two sets of orientation"));
       aRes.push_back(cMMCom("ConvertCalib",ConvertCalib_main," Conversion of calibration from one model 2 the other"));
       aRes.push_back(cMMCom("ReprojImg",ReprojImg_main," Reproject an image into geometry of another"));
       aRes.push_back(cMMCom("TestRegEx",TestRegEx_main," Test regular expression"));
       aRes.push_back(cMMCom("InitOriLinear",InitOriLinear_main," Initialize orientation for linear acquisition"));
       aRes.push_back(cMMCom("PHO_MI",PHO_MI_main," Filter homologue points from initial orientation to reduce number of observations"));
       aRes.push_back(cMMCom("ExtractMesure2D",ExtractMesure2D_main," Extract points from a 2D measures xml file"));
       aRes.push_back(cMMCom("Kugelhupf",Kugelhupf_main," Semi-automatic fiducial points determination"));
       aRes.push_back(cMMCom("FFTKugelhupf",FFTKugelhupf_main," Version of Kugelhupf using FFT, expecetd faster when it works (if ever ...)"));
       aRes.push_back(cMMCom("SimplePredict",SimplePredict_main," Project ground points on oriented cameras"));
       aRes.push_back(cMMCom("cod",cod_main," Do some stuff"));
       aRes.push_back(cMMCom("vic",vicod_main," Do some stuff"));
       aRes.push_back(cMMCom("genmail",genmail_main," Do some stuff"));
       aRes.push_back(cMMCom("CreateEpip",CreateEpip_main," Create epipolar images"));
       aRes.push_back(cMMCom("CoherEpip",CoherEpi_main," Test coherence between conjugate epipolar depth-map"));
       aRes.push_back(cMMCom("Dequant",Dequant_main," Tool for dequantifying an image"));
       aRes.push_back(cMMCom("Devlop",Devlop_main," Do some stuff"));
       aRes.push_back(cMMCom("TifDev",TiffDev_main," Develop raw-jpg-tif, in suitable tiff file"));

       aRes.push_back(cMMCom("Drunk", Drunk_main," Images distortion removing tool"));
       aRes.push_back(cMMCom("ElDcraw",ElDcraw_main," Do some stuff"));
       aRes.push_back(cMMCom("GCPBascule",GCPBascule_main," Relative to absolute using GCP",cArgLogCom(2)));
       aRes.push_back(cMMCom("GCPCtrl",GCPCtrl_main," Control accuracy with GCP"));

       aRes.push_back(cMMCom("CenterBascule",CentreBascule_main," Relative to absolute using embedded GPS",cArgLogCom(2)));

       aRes.push_back(cMMCom("GrapheHom",GrapheHom_main,"Compute XML-Visibility graph from approximate orientation",cArgLogCom(3)));
       aRes.push_back(cMMCom("GCPConvert",GCP_Txt2Xml_main,"Convert GCP from Txt 2 XML",cArgLogCom(3)));
       aRes.push_back(cMMCom("OriConvert",Ori_Txt2Xml_main,"Convert Orientation from Txt 2 XML",cArgLogCom(3)));
       aRes.push_back(cMMCom("OriExport",OriExport_main,"Export orientation from XML to XML or TXT with specified convention",cArgLogCom(3)));
       aRes.push_back(cMMCom("Apero2NVM",Apero2NVM_main,"Matthieu Moneyrond's convertor to VSfM, MVE, SURE, MeshRecon ",cArgLogCom(3)));
       aRes.push_back(cMMCom("XifGps2Xml",XifGps2Xml_main,"Create MicMac-Xml struct from GPS embedded in EXIF",cArgLogCom(2)));
	   aRes.push_back(cMMCom("XifGps2Txt",XifGps2Txt_main,"Export embedded EXIF GPS data 2 Txt",cArgLogCom(2)));
       aRes.push_back(cMMCom("GenXML2Cpp",GenXML2Cpp_main," Do some stuff"));
       aRes.push_back(cMMCom("GenCode",GenCode_main," Do some stuff"));
       aRes.push_back(cMMCom("GrShade",GrShade_main," Compute shading from depth image"));
       aRes.push_back(cMMCom("LumRas",LumRas_main," Compute image mixing with raking light",cArgLogCom(2)));


       aRes.push_back(cMMCom("StackFlatField",EstimFlatField_main,"Basic Flat Field estimation by image stacking"));
       aRes.push_back(cMMCom("Impaint",Impainting_main,"Basic Impainting"));
       aRes.push_back(cMMCom("Gri2Bin",Gri2Bin_main," Do some stuff"));
       aRes.push_back(cMMCom("MakeGrid",MakeGrid_main," Generate orientations in a grid format"));
       aRes.push_back(cMMCom("Malt",Malt_main," Simplified matching (interface to MicMac)",cArgLogCom(3)));
       aRes.push_back(cMMCom("CASA",CASA_main," Analytic Surface Estimation",cArgLogCom(2)));



       aRes.push_back(cMMCom("MMByP",MMByPair_main," Matching By Pair of images",cArgLogCom(2)));
       aRes.push_back(cMMCom("MM1P",MMOnePair_main," Matching One Pair of images",cArgLogCom(2)));

       aRes.push_back(cMMCom("ChantierClip",ChantierClip_main," Clip Chantier",cArgLogCom(2)));
       aRes.push_back(cMMCom("ClipIm",ClipIm_main," Clip Chantier",cArgLogCom(2)));


       aRes.push_back(cMMCom("MapCmd",MapCmd_main," Transforms a command working on a single file in a command working on a set of files"));
       aRes.push_back(cMMCom("Ori2Xml",Ori2XML_main,"Convert \"historical\" Matis'Ori format to xml "));
       aRes.push_back(cMMCom("MergePly",MergePly_main," Merge ply files"));
       aRes.push_back(cMMCom("MICMAC",MICMAC_main," Computes image matching from oriented images"));
       aRes.push_back(cMMCom("MMPyram",MMPyram_main," Computes pyram for micmac (internal use)",cArgLogCom(2)));

       aRes.push_back(cMMCom("MMCalcSzWCor",CalcSzWCor_main," Compute Image of Size of correlation windows (Atomic tool, for adaptive window in geom image)",cArgLogCom(2)));
       aRes.push_back(cMMCom("MpDcraw",MpDcraw_main," Interface to dcraw"));

       aRes.push_back(cMMCom("MMTestOrient",MMTestOrient_main," Tool for testing quality of orientation"));
       aRes.push_back(cMMCom("MMHomCorOri",MMHomCorOri_main," Tool to compute homologues for correcting orientation in epip matching"));
       aRes.push_back(cMMCom("MMInitialModel",MMInitialModel_main," Initial Model for MicMac ")); //  ,cArgLogCom(2)));
       aRes.push_back(cMMCom("MMTestAllAuto",MMAllAuto_main," Full automatic version for 1 view point, test mode ",cArgLogCom(2)));
       aRes.push_back(cMMCom("MM2DPosSism",MM2DPostSism_Main," Simplified interface for post 2D post sismic deformation ",cArgLogCom(2)));
       aRes.push_back(cMMCom("MMMergeCloud",MM_FusionNuage_main," Merging of low resol cloud, in preparation 2 MicMac ",cArgLogCom(2)));

       aRes.push_back(cMMCom("MergeDepthMap",FusionCarteProf_main," Merging of individual, stackable, depth maps "));
       aRes.push_back(cMMCom("SMDM",SimpleFusionCarte_main," Simplified Merging of individual, stackable, depth maps "));
       aRes.push_back(cMMCom("MyRename",MyRename_main," File renaming using posix regular expression "));
       aRes.push_back(cMMCom("Genere_Header_TiffFile",Genere_Header_TiffFile_main," Generate Header for internal tiling format "));


       aRes.push_back(cMMCom("Nuage2Ply",Nuage2Ply_main," Convert depth map into point cloud"));
       aRes.push_back(cMMCom("NuageBascule",NuageBascule_main," To Change geometry of depth map "));



       aRes.push_back(cMMCom("Pasta",Pasta_main," Compute external calibration and radial basic internal calibration"));
       aRes.push_back(cMMCom("PastDevlop",PastDevlop_main," Do some stuff"));
       aRes.push_back(cMMCom("Pastis",Pastis_main," Tie points detection"));
       //aRes.push_back(cMMCom("Poisson",Poisson_main," Mesh Poisson reconstruction by M. Khazdan"));
       aRes.push_back(cMMCom("Porto",Porto_main," Generates a global ortho-photo"));
       aRes.push_back(cMMCom("Prep4masq",Prep4masq_main," Generates files for making Masks (if SaisieMasq unavailable)"));
       aRes.push_back(cMMCom("Reduc2MM",Reduc2MM_main," Do some stuff"));
       aRes.push_back(cMMCom("ReducHom",ReducHom_main," Do some stuff"));
       aRes.push_back(cMMCom("RepLocBascule",RepLocBascule_main," Tool to define a local repair without changing the orientation",cArgLogCom(2)));
       aRes.push_back(cMMCom("SBGlobBascule",SBGlobBascule_main," Tool for 'scene based global' bascule",cArgLogCom(2)));
       aRes.push_back(cMMCom("HomolFilterMasq",HomFilterMasq_main," Tool for filter homologous points according to masq",cArgLogCom(2)));
       aRes.push_back(cMMCom("HomolMergePDVUnik",HomFusionPDVUnik_main," Tool for merge homologous point from unik point of view",cArgLogCom(2)));


       aRes.push_back(cMMCom("ScaleIm",ScaleIm_main," Tool for image scaling"));
       aRes.push_back(cMMCom("StatIm",StatIm_main," Tool for basic stat on an image"));
       aRes.push_back(cMMCom("ConvertIm",ConvertIm_main," Tool for convertion inside tiff-format"));
       aRes.push_back(cMMCom("PanelIm",MakePlancheImage_main,"Tool for creating a panel of images "));
       aRes.push_back(cMMCom("ScaleNuage",ScaleNuage_main," Tool for scaling internal representation of point cloud"));
       aRes.push_back(cMMCom("Sift",Sift_main," Tool for extracting points of interest using Lowe's SIFT method"));
       aRes.push_back(cMMCom("SysCoordPolyn",SysCoordPolyn_main," Tool for creating a polynomial coordinate system from a set of known pair of coordinate"));

       aRes.push_back(cMMCom("OldTapas",Tapas_main," Interface to Apero to compute external and internal orientations",cArgLogCom(3)));
       aRes.push_back(cMMCom("Tapas",New_Tapas_main,"NEW version !! Compatible . Call \"OldTapas\" if problem specific to this version",cArgLogCom(3)));
       aRes.push_back(cMMCom("NewTapas",New_Tapas_main,"Replace OldTapas - now same as Tapas",cArgLogCom(3)));

       aRes.push_back(cMMCom("Tapioca",Tapioca_main," Interface to Pastis for tie point detection and matching",cArgLogCom(3)));
       aRes.push_back(cMMCom("Tarama",Tarama_main," Compute a rectified image",cArgLogCom(2)));
       aRes.push_back(cMMCom("Martini",CPP_Martini_main," New orientation initialisation (uncomplete, still in dev...) ",cArgLogCom(2)));
       aRes.push_back(cMMCom("MartiniGin",CPP_MartiniGin_main," New orientation initialisation (uncomplete, still in dev...) ",cArgLogCom(2)));

       aRes.push_back(cMMCom("Tawny",Tawny_main," Interface to Porto to generate ortho-image",cArgLogCom(2,"../")));
       // aRes.push_back(cMMCom("Tawny",Tawny_main," Interface to Porto to generate ortho-image"));
       aRes.push_back(cMMCom("Tequila",Tequila_main," Texture mesh"));
       aRes.push_back(cMMCom("TiPunch",TiPunch_main," Compute mesh"));
       aRes.push_back(cMMCom("TestCam",TestCam_main," Test camera orientation convention"));
       aRes.push_back(cMMCom("TestDistM2C",TestDistM2C_main," Basic Test for problematic camera "));
       aRes.push_back(cMMCom("TestChantier",TestChantier_main," Test global acquisition"));

       aRes.push_back(cMMCom("TestKey",TestSet_main," Test Keys for Sets and Assoc"));
       aRes.push_back(cMMCom("TestNameCalib",TestNameCalib_main," Test Name of calibration"));
       aRes.push_back(cMMCom("TestMTD",TestMTD_main," Test meta data of image"));
       aRes.push_back(cMMCom("TestCmds",TestCmds_main," Test MM3D commands on micmac_data sets"));

       aRes.push_back(cMMCom("tiff_info",tiff_info_main," Tool for giving information about a tiff file"));
       aRes.push_back(cMMCom("to8Bits",to8Bits_main," Tool for converting 16 or 32 bit image in a 8 bit image."));
       aRes.push_back(cMMCom("Vodka",Vignette_main," IN DEV : Compute the vignette correction parameters from tie points",cArgLogCom(1)));
       aRes.push_back(cMMCom("mmxv",mmxv_main," Interface to xv (due to problem in tiff lib)"));
       aRes.push_back(cMMCom("CmpIm",CmpIm_main," Basic tool for images comparison"));
       aRes.push_back(cMMCom("ImMire",GenMire_main," For generation of some synthetic calibration image"));
       aRes.push_back(cMMCom("ImRandGray",GrayTexture_main," Generate Random Gray Textured Images"));
       aRes.push_back(cMMCom("Undist",Undist_main," Tool for removing images distortion"));

       aRes.push_back(cMMCom("CheckDependencies",CheckDependencies_main," check dependencies to third-party tools"));
       aRes.push_back(cMMCom("VV",VideoVisage_main," A very simplified tool for 3D model of visage out of video, just for fun"));

       aRes.push_back(cMMCom("XYZ2Im",XYZ2Im_main," tool to transform a 3D point (text file) to their 2D proj in cam or cloud"));
       aRes.push_back(cMMCom("Im2XYZ",Im2XYZ_main," tool to transform a 2D point (text file) to their 3D cloud homologous"));
       aRes.push_back(cMMCom("SplitMPO",SplitMPO_main,"tool to develop MPO stereo format in pair of images"));

       aRes.push_back(cMMCom("Sake", Sake_main, " Simplified MicMac interface for satellite images", cArgLogCom(3)));
       aRes.push_back(cMMCom("SateLib", SateLib_main, " Library of satellite images meta-data handling - early work in progress!"));
       aRes.push_back(cMMCom("XLib", XLib_Main, " Xeres Lib - early work in progress!")); 

#if (ELISE_QT_VERSION >= 4)
       aRes.push_back(cMMCom("SaisieAppuisInitQT",SaisieAppuisInitQT_main," Interactive tool for initial capture of GCP"));
       aRes.push_back(cMMCom("SaisieAppuisPredicQT",SaisieAppuisPredicQT_main," Interactive tool for assisted capture of GCP"));
       aRes.push_back(cMMCom("SaisieBascQT",SaisieBascQT_main," Interactive tool to capture information on the scene"));
       aRes.push_back(cMMCom("SaisieCylQT",SaisieCylQT_main," Interactive tool to capture information on the scene for cylinders"));
       aRes.push_back(cMMCom("SaisieMasqQT",SaisieMasqQT_main," Interactive tool to capture masq"));
       aRes.push_back(cMMCom("SaisieBoxQT",SaisieBoxQT_main," Interactive tool to capture 2D box"));
#endif

#if (ELISE_X11)
       aRes.push_back(cMMCom("MPDtest",MPDtest_main," My own test"));
       aRes.push_back(cMMCom("SaisieAppuisInit",SaisieAppuisInit_main," Interactive tool for initial capture of GCP",cArgLogCom(2)));
       aRes.push_back(cMMCom("SaisieAppuisPredic",SaisieAppuisPredic_main," Interactive tool for assisted capture of GCP"));
       aRes.push_back(cMMCom("SaisieBasc",SaisieBasc_main," Interactive tool to capture information on the scene"));
       aRes.push_back(cMMCom("SaisieCyl",SaisieCyl_main," Interactive tool to capture information on the scene for cylinders"));
       aRes.push_back(cMMCom("SaisieMasq",SaisieMasq_main," Interactive tool to capture masq"));
       aRes.push_back(cMMCom("SaisiePts",SaisiePts_main," Tool to capture GCP (low level, not recommended)"));
       aRes.push_back(cMMCom("SEL",SEL_main," Tool to visualize tie points"));
       aRes.push_back(cMMCom("MICMACSaisieLiaisons",MICMACSaisieLiaisons_main," Low level version of SEL, not recommended"));

#ifdef ETA_POLYGON

       //Etalonnage polygone
       aRes.push_back(cMMCom("Compens",Compens_main," Do some stuff"));
       aRes.push_back(cMMCom("CatImSaisie",CatImSaisie_main," Do some stuff"));
       aRes.push_back(cMMCom("CalibFinale",CalibFinale_main," Compute Final Radial distortion model"));
       aRes.push_back(cMMCom("CalibInit",CalibInit_main," Compute Initial Radial distortion model"));
       aRes.push_back(cMMCom("ConvertPolygone",ConvertPolygone_main," Do some stuff"));
       aRes.push_back(cMMCom("PointeInitPolyg",PointeInitPolyg_main," Do some stuff"));
       aRes.push_back(cMMCom("RechCibleDRad",RechCibleDRad_main," Do some stuff"));
       aRes.push_back(cMMCom("RechCibleInit",RechCibleInit_main," Do some stuff"));
       aRes.push_back(cMMCom("ScriptCalib",ScriptCalib_main," Do some stuff"));

#endif

#endif
       aRes.push_back(cMMCom("TestLib",SampleLibElise_main," To call the program illustrating the library"));
       aRes.push_back(cMMCom("FieldDep3d",ChamVec3D_main," To export results of matching as 3D shifting"));
       aRes.push_back(cMMCom("SupMntIm",SupMntIm_main," Tool for superposition of Mnt Im & level curve"));

       aRes.push_back(cMMCom("MMXmlXif",MakeMultipleXmlXifInfo_main," Generate Xml from Xif (internal use mainly)"));
       aRes.push_back(cMMCom("Init11P",Init11Param_Main," Init Internal & External from GCP using 11-parameters algo"));
       aRes.push_back(cMMCom("Aspro",Init3App_Main," Init  External orientation of calibrated camera from GCP "));
       aRes.push_back(cMMCom("Genepi",GenerateAppLiLiaison_main," Generate 3D/2d synthetical points from orientation"));


       aRes.push_back(cMMCom("DIV",Devideo_main,"Videos development (require ffmpeg)"));
       aRes.push_back(cMMCom("Liquor",Liquor_main,"Orientation specialized for linear acquisition"));
       aRes.push_back(cMMCom("Morito",Morito_main,"Merge set of Orientations with common values"));
       aRes.push_back(cMMCom("Donuts",Donuts_main,"Cyl to Torus (Donuts like)"));
       aRes.push_back(cMMCom("C3DC",C3DC_main,"Automatic Matching from Culture 3D Cloud project"));
       aRes.push_back(cMMCom("PIMs",MPI_main,"Per Image Matchings"));
       aRes.push_back(cMMCom("PIMs2Ply",MPI2Ply_main,"Generate Ply from Per Image Matchings"));
       aRes.push_back(cMMCom("PIMs2Mnt",MPI2Mnt_main,"Generate Mnt from Per Image Matchings"));


       aRes.push_back(cMMCom("AllDev",DoAllDev_main,"Force development of all tif/xif file"));
       aRes.push_back(cMMCom("SetExif",CPP_SetExif,"Modification of exif file (requires exiv2)"));
       aRes.push_back(cMMCom("Convert2GenBundle",CPP_ConvertBundleGen,"Import RPC or other to MicMac format, for adjustment, matching ..."));
    aRes.push_back(cMMCom("ReSampFid",OneReechFid_main,"Resampling using one fiducial mark"));

   }

   cCmpMMCom CmpMMCom;
   std::sort(aRes.begin(),aRes.end(),CmpMMCom);

   return aRes;
}

class cSuggest
{
     public :
        cSuggest(const std::string & aName,const std::string & aPat) :
             mName (aName),
             mPat  (aPat),
             mAutom (mPat,10)
        {
        }
        void Test(const cMMCom & aCom)
        {
            if (mAutom.Match(aCom.mLowName))
               mRes.push_back(aCom);
        }

        std::string          mName;
        std::string          mPat;
        cElRegex             mAutom;
        std::vector<cMMCom>  mRes;
};

int GenMain(int argc,char ** argv, const std::vector<cMMCom> & aVComs);

// =========================================================

//TestLib declarations
extern int  Sample_W0_main(int argc,char ** argv);
extern int  Sample_LSQ0_main(int argc,char ** argv);
extern int  Abdou_main(int argc,char ** argv);
extern int  Luc_main(int argc, char ** argv);
extern int  LucasChCloud_main(int argc,char ** argv);
extern int  ProjetInfo_main(int argc,char ** argv);
extern int  Matthieu_main(int argc,char ** argv);
extern int  TestJB_main(int argc,char ** argv);
extern int  RawCor_main(int argc,char ** argv);
extern int  CreateBlockEpip_main(int argc,char ** argv);
extern int  TD_GenereAppuis_main(int argc,char ** argv);
extern int  TD_Exemple_main(int argc,char ** argv);
extern int  TD_Sol1(int argc,char ** argv);
extern int  TD_Sol2(int argc,char ** argv);
extern int  TD_Sol3(int argc,char ** argv);

extern int  TD_Exo0(int argc,char ** argv);
extern int  TD_Exo1(int argc,char ** argv);
extern int  TD_Exo2(int argc,char ** argv);
extern int  TD_Exo3(int argc,char ** argv);
extern int  TD_Exo4(int argc,char ** argv);
extern int  TD_Exo5(int argc,char ** argv);
extern int  TD_Exo6(int argc,char ** argv);
extern int  TD_Exo7(int argc,char ** argv);
extern int  TD_Exo8(int argc,char ** argv);
extern int  TD_Exo9(int argc,char ** argv);

extern int TD_Match1_main(int argc,char ** argv);
extern int TD_Match2_main(int argc,char ** argv);
extern int TD_Match3_main(int argc,char ** argv);
extern int TestER_main(int argc,char ** argv);

extern int  DocEx_Intro0_main(int,char **);
extern int  DocEx_Introd2_main(int,char **);
extern int  DocEx_Introfiltr_main(int,char **);
extern int  ImageRectification(int argc,char ** argv);
#if (ELISE_UNIX)
extern int  DocEx_Introanalyse_main(int,char **);
#endif
extern int VisuCoupeEpip_main(int,char **);

int ExoSimulTieP_main(int argc,char** argv);
int ExoMCI_main(int argc,char** argv);
int PseudoIntersect_main(int argc,char** argv);
int ExoCorrelEpip_main(int argc,char ** argv);
int ScaleModel_main(int argc,char ** argv);
int PLY2XYZ_main(int argc,char ** argv);
int ExportXmlGcp2Txt_main(int argc,char ** argv);
int ConvertRtk_main(int argc,char ** argv);
int MatchCenters_main( int argc,char ** argv);
int Panache_main(int argc,char ** argv);

int  CheckOri_main(int argc,char ** argv);
int  NLD_main(int argc,char ** argv);
int  ResToTxt_main(int argc,char ** argv);
int  SelTieP_main(int argc,char ** argv);
int  Ortho2TieP_main(int argc,char ** argv);
int  Idem_main(int argc,char ** argv);
// int RHH_main(int argc,char **argv);

extern int BasculePtsInRepCam_main(int argc,char ** argv);
extern int BasculeCamsInRepCam_main(int argc,char ** argv);

int CheckOneOrient_main(int argc,char ** argv);
int CheckAllOrient_main(int argc,char ** argv);
int ChekBigTiff_main(int,char**);
int GenTriplet_main(int argc,char ** argv);

int CPP_GenOneHomFloat(int argc,char ** argv);
int CPP_GenAllHomFloat(int argc,char ** argv);
int CPP_GenOneImP3(int argc,char ** argv);
int CPP_GenAllImP3(int argc,char ** argv);
int CPP_OptimTriplet_main(int argc,char ** argv);
int CPP_AllOptimTriplet_main(int argc,char ** argv);
int CPP_NewSolGolInit_main(int argc, char ** argv);

int CPP_NewGenTriOfCple(int argc, char ** argv);
int CPP_TestBundleGen(int argc,char ** argv)   ;
int PlyGCP_main(int argc,char ** argv);



int SysCalled_main (int argc,char** argv);
int SysCall_main (int argc,char** argv);


int TestARCam_main(int argc,char ** argv);
int CPP_TestPhysMod_Main(int argc,char ** argv);

int OneReechHom_main(int argc,char ** argv);
int AllReechHom_main(int argc,char ** argv);
int RTI_main(int argc,char ** argv);
int RTIRecalRadiom_main(int argc,char ** argv);
int RTIMed_main(int argc,char ** argv);

int TestNewMergeTieP_main(int argc,char ** argv);
int TestStephane_Main(int argc,char ** argv);


const std::vector<cMMCom> & TestLibAvailableCommands()
{
   static std::vector<cMMCom> aRes;
   if (aRes.empty())
   {
       aRes.push_back(cMMCom("Exo0",TD_Exo0,"Some stuff "));
       aRes.push_back(cMMCom("Exo1",TD_Exo1,"Some stuff "));
       aRes.push_back(cMMCom("Exo2",TD_Exo2,"Some stuff "));
       aRes.push_back(cMMCom("Exo3",TD_Exo3,"Some stuff "));
       aRes.push_back(cMMCom("Exo4",TD_Exo4,"Some stuff "));
       aRes.push_back(cMMCom("Exo5",TD_Exo5,"Some stuff "));
       aRes.push_back(cMMCom("Exo6",TD_Exo6,"Some stuff "));
       aRes.push_back(cMMCom("Exo7",TD_Exo7,"Some stuff "));
       aRes.push_back(cMMCom("Exo8",TD_Exo8,"Some stuff "));
       aRes.push_back(cMMCom("Exo9",TD_Exo9,"Some stuff "));


       aRes.push_back(cMMCom("Stephane",TestStephane_Main,"In test funtction for Stephane Guinard "));
       aRes.push_back(cMMCom("TestNewMergeTieP",TestNewMergeTieP_main,"Some consitency check on Merge TieP "));
       aRes.push_back(cMMCom("TestARCam",TestARCam_main,"Some consitency check on camera "));
       aRes.push_back(cMMCom("SysCall",SysCall_main,"Some stuff "));
       aRes.push_back(cMMCom("SysCalled",SysCalled_main,"Some stuff "));

       aRes.push_back(cMMCom("PrepSift",PreparSift_Main,"Some stuff "));
       aRes.push_back(cMMCom("TD1",TD_Match1_main,"Some stuff "));
       aRes.push_back(cMMCom("TD2",TD_Match2_main,"Some stuff "));
       aRes.push_back(cMMCom("TD3",TD_Match3_main,"Some stuff "));

       aRes.push_back(cMMCom("X1",TD_Sol1,"Some stuff "));
       aRes.push_back(cMMCom("X2",TD_Sol2,"Some stuff "));
       aRes.push_back(cMMCom("X3",TD_Sol3,"Some stuff "));
       aRes.push_back(cMMCom("W0",Sample_W0_main,"Test on Graphic Windows "));
       aRes.push_back(cMMCom("LSQ0",Sample_LSQ0_main,"Basic Test on Least Square library "));
       aRes.push_back(cMMCom("Tests_Luc", Luc_main, "tests de Luc"));
       aRes.push_back(cMMCom("Abdou",Abdou_main,"Exemples fonctions abdou"));
       aRes.push_back(cMMCom("CheckOri",CheckOri_main,"Difference between two sets of orientations"));
       aRes.push_back(cMMCom("NLD",NLD_main,"test"));
       aRes.push_back(cMMCom("RTT",ResToTxt_main,"Transform residuals from GCPBascule into a readable file"));
       aRes.push_back(cMMCom("SelTieP",SelTieP_main,"Select Tie Points with favorable angles"));
       aRes.push_back(cMMCom("Ortho2TieP",Ortho2TieP_main,"Select Tie Points from the orthophotography"));
       aRes.push_back(cMMCom("Idem",Idem_main,"Interpolate DEM on GCP & CP"));
       aRes.push_back(cMMCom("TestSI",Matthieu_main,"Test SelectionInfos"));
       aRes.push_back(cMMCom("TestJB",TestJB_main,"random stuff"));
       aRes.push_back(cMMCom("TestER",TestER_main,"ER test workplace"));
       aRes.push_back(cMMCom("PI",ProjetInfo_main,"Projet Info"));
       // aRes.push_back(cMMCom("RawCor",RawCor_main,"Test for correcting green or red RAWs"));
       aRes.push_back(cMMCom("LucasChCloud",LucasChCloud_main,"Examples functions modifying cloud "));

       aRes.push_back(cMMCom("BlocEpip",CreateBlockEpip_main,"Epip by bloc (internal use to // epip) "));
       aRes.push_back(cMMCom("MMSMA",MMSymMasqAR_main,"Symetrise Masque Alle-Retour (internal use in MM1P) "));
       aRes.push_back(cMMCom("TD_GenApp",TD_GenereAppuis_main,"TD Generate GCP"));
       aRes.push_back(cMMCom("TD_Test",TD_Exemple_main,"Test TD "));
       aRes.push_back(cMMCom("DocI0",DocEx_Intro0_main,"Introduction 0 of example from DocElise  "));
       aRes.push_back(cMMCom("DocID2",DocEx_Introd2_main,"Introduction to D2 of example from DocElise  "));
       aRes.push_back(cMMCom("DocIntrofiltre",DocEx_Introfiltr_main,"Introduction to filter example from DocElise  "));
#if (ELISE_UNIX)
       aRes.push_back(cMMCom("DocIntroanalyse",DocEx_Introanalyse_main,"Introduction to image analysis from DocElise  "));
#endif
       aRes.push_back(cMMCom("VCE",VisuCoupeEpip_main,"Visualization of epipolar pair (cut)  "));
       aRes.push_back(cMMCom("RIE",ReechInvEpip_main,"Visualization of epipolar pair (cut)  "));

       aRes.push_back(cMMCom("MCI",ExoMCI_main,"Exercise for multi correlation in image geometry  "));
       aRes.push_back(cMMCom("ECE",ExoCorrelEpip_main,"Exercise for correlation in epipolar "));
       aRes.push_back(cMMCom("ESTP",ExoSimulTieP_main,"Tie points simulation  "));
       aRes.push_back(cMMCom("TDEpi",TDEpip_main,"Test epipolar matcher  "));
   
       aRes.push_back(cMMCom("PseudoIntersect",PseudoIntersect_main,"Pseudo Intersection of 2d points from N images",cArgLogCom(2)));
       aRes.push_back(cMMCom("Export2Ply",Export2Ply_main,"Tool to generate a ply file from TEXT or XML file, tuning",cArgLogCom(2)));
       aRes.push_back(cMMCom("ScaleModel",ScaleModel_main,"Tool for simple scaling a model",cArgLogCom(2)));
       aRes.push_back(cMMCom("Ply2Xyz",PLY2XYZ_main,"Tool to export in TxT file XYZ columns only from a .ply file",cArgLogCom(2)));
       aRes.push_back(cMMCom("ExportXmlGcp2Txt",ExportXmlGcp2Txt_main,"Tool to export xml GCPs file to Txt file",cArgLogCom(2)));
       aRes.push_back(cMMCom("Panache",Panache_main,"Tool to export profile along axis given a line draw on Orthoimage",cArgLogCom(2)));
	   aRes.push_back(cMMCom("ConvRtk",ConvertRtk_main,"Tool to extract X_Y_Z_Ix_Iy_Iz from Rtklib output file",cArgLogCom(2)));
	   aRes.push_back(cMMCom("MatchCenters",MatchCenters_main,"Tool to match Gps positions and Camera Centers",cArgLogCom(2)));

       aRes.push_back(cMMCom("RHH",RHH_main,"In dev estimation of global 2D homography  "));
       aRes.push_back(cMMCom("RHHComputHom",RHHComputHom_main,"Internal : compute Hom for // in RHH  "));

       aRes.push_back(cMMCom("XmlXif",MakeOneXmlXifInfo_main,"Internal : generate Xml to accelerate Xif extraction  "));

       aRes.push_back(cMMCom("Xml2Dmp",Xml2Dmp_main,"Convert XML to Dump  "));
       aRes.push_back(cMMCom("Dmp2Xml",Dmp2Xml_main,"Convert Dump to Xml  "));

        aRes.push_back(cMMCom("AddAffinity", AddAffinity_main, "Add an affinity, tuning"));
        aRes.push_back(cMMCom("TP2GCP",ServiceGeoSud_TP2GCP_main,"Tie Points to Ground Control Points (for GeoSud services)"));
        aRes.push_back(cMMCom("Ortho",ServiceGeoSud_Ortho_main,"Compute a basic Ortho from a DTM and a satellite image (for GeoSud services)"));
        aRes.push_back(cMMCom("GeoSud",ServiceGeoSud_GeoSud_main,""));
        aRes.push_back(cMMCom("Surf",ServiceGeoSud_Surf_main,""));
        aRes.push_back(cMMCom("ImageRectification",ImageRectification,"Rectify individual aerial images, ground is assumed to be a plane"));
// #if (ELISE_QT_VERSION >= 4)
        aRes.push_back(cMMCom("Masq3Dto2D",Masq3Dto2D_main,"Create a 2D Masq from Nuage and 3D Masq "));
// #endif
        aRes.push_back(cMMCom("MergeCloud",CPP_AppliMergeCloud,"Tool for merging overlapping depth maps from different view points"));
        aRes.push_back(cMMCom("MMEnvlop",MMEnveloppe_Main,"Compute initial envelope surface for MMEpi "));
        aRes.push_back(cMMCom("PlySphere",PlySphere_main,"Tool to generate a sphere of point, ply format, tuning"));
        aRes.push_back(cMMCom("PlyGCP",PlyGCP_main,"Tool to generate a visualization of ply"));
        aRes.push_back(cMMCom("San2Ply",San2Ply_main,"Generate a Ply visualisation of an Analytical Surface"));

        aRes.push_back(cMMCom("CASALL",CASALL_main,"Compute Analytic Surface Automatically  low level"));
        aRes.push_back(cMMCom("CalcAutoCorrel",CalcAutoCorrel_main,"Compute and Store Auto Correlation (if not already done)"));

        aRes.push_back(cMMCom("CLIC",CCL_main,"Cam Light Imag Correc)"));
        aRes.push_back(cMMCom("MMEnvStatute",MMEnvStatute_main,"Envelope for mode statue"));
        aRes.push_back(cMMCom("TopoBasc",TopoSurf_main,"Topological analysis before bascule"));


        aRes.push_back(cMMCom("Check1Hom",CheckOneHom_main,"Check One File Homologue"));
        aRes.push_back(cMMCom("CheckAllHom",CheckAllHom_main,"Check All File Homologue"));
        aRes.push_back(cMMCom("Check1Tiff",CheckOneTiff_main,"Check All File Homologue"));
        aRes.push_back(cMMCom("CheckAllTiff",CheckAllTiff_main,"Check All File Homologue"));
        aRes.push_back(cMMCom("CheckBigTiff",ChekBigTiff_main,"Check creation of a big file"));


        aRes.push_back(cMMCom("Check1Ori",CheckOneOrient_main,"Check One Orientation"));
        aRes.push_back(cMMCom("CheckAllOri",CheckAllOrient_main,"Check a Folder of Orientation"));

        aRes.push_back(cMMCom("BasculePtsInRepCam",BasculePtsInRepCam_main,"Compute GCP in cam repair"));
        aRes.push_back(cMMCom("BasculeCamsInRepCam",BasculeCamsInRepCam_main,"Compute GCP in cam repair"));
        aRes.push_back(cMMCom("NO_Ori2Im",TestNewOriImage_main,"Test New Orientation"));
        aRes.push_back(cMMCom("NO_AllOri2Im",TestAllNewOriImage_main,"Test New Orientation"));
        aRes.push_back(cMMCom("NO_GenTripl",GenTriplet_main,"New Orientation : select triplet"));

        aRes.push_back(cMMCom("NO_OneHomFloat",CPP_GenOneHomFloat,"New Orientation : generate merged float point of one image"));
        aRes.push_back(cMMCom("NO_AllHomFloat",CPP_GenAllHomFloat,"New Orientation : generate float point of all image"));
        aRes.push_back(cMMCom("NO_OneImTriplet",CPP_GenOneImP3,"New Orientation : generate triple of one image"));
        aRes.push_back(cMMCom("NO_AllImTriplet",CPP_GenAllImP3,"New Orientation : generate triple of all imaget"));
        aRes.push_back(cMMCom("NO_OneImOptTrip",CPP_OptimTriplet_main,"New Orientation : otimize triplet"));
        aRes.push_back(cMMCom("NO_AllImOptTrip",CPP_AllOptimTriplet_main,"New Orientation : otimize triplet"));
        aRes.push_back(cMMCom("NO_SolInit3",CPP_NewSolGolInit_main,"New Orientation : sol init from triplet"));

        aRes.push_back(cMMCom("NO_GenTriOfCple",CPP_NewGenTriOfCple,"New Orientation : select triple of one edge"));

        aRes.push_back(cMMCom("OriMatis2MM",MatisOri2MM_main,"Convert from Matis to MicMac"));

        aRes.push_back(cMMCom("TestBundleGen",CPP_TestBundleGen,"Unitary test for new bundle gen"));

        aRes.push_back(cMMCom("TestPhysMod",CPP_TestPhysMod_Main,"Unitary test for new bundle gen"));

        aRes.push_back(cMMCom("TestParseDir",TestElParseDir_main," Test Parse Dir"));
        aRes.push_back(cMMCom("OneReechHom",OneReechHom_main," Resample image using homography"));
        aRes.push_back(cMMCom("AllReechHom",AllReechHom_main," Resample multiple image using homography"));
        aRes.push_back(cMMCom("RTI",RTI_main," RTI prototype"));
        aRes.push_back(cMMCom("RTI_RR",RTIRecalRadiom_main," RTI recalage radiom"));
        aRes.push_back(cMMCom("RTIMed",RTIMed_main," RTI calc median image"));
    }

    cCmpMMCom CmpMMCom;
    std::sort(aRes.begin(),aRes.end(),CmpMMCom);

   return aRes;
}

int SampleLibElise_main(int argc, char ** argv)
{
    return GenMain(argc, argv, TestLibAvailableCommands());
}

//SateLib declarations
extern int RPC_main(int argc, char ** argv);
extern int NewRefineModel_main(int argc, char **argv);
extern int RefineModel_main(int argc, char **argv);
extern int RefineJitter_main(int argc, char **argv);
extern int ApplyParralaxCor_main(int argc, char **argv);
extern int Dimap2Grid_main(int argc, char **argv);
extern int DigitalGlobe2Grid_main(int argc, char **argv);
extern int Aster2Grid_main(int argc, char **argv);
extern int AsterDestrip_main(int argc, char **argv);
extern int SATtoBundle_main(int argc,char ** argv);
extern int SATvalid_main(int argc,char ** argv);
extern int CPP_SATDef2D_main(int argc,char ** argv);
extern int CPP_TestRPCDirectGen(int argc,char ** argv);
extern int DoTile_main(int argc,char ** argv);

const std::vector<cMMCom> & SateLibAvailableCommands()
{
    static std::vector<cMMCom> aRes;

    aRes.push_back(cMMCom("RPC", RPC_main, "test functions for upcoming RPC functions"));
    aRes.push_back(cMMCom("Dimap2Grid", Dimap2Grid_main, "Create a Grid file from a Dimap (SPOT or Pleiades) "));
    aRes.push_back(cMMCom("DigitalGlobe2Grid", DigitalGlobe2Grid_main, "Create a Grid file from a DigitalGlobe RPB file (WorldView/Geoeye/IKONOS...) "));
	aRes.push_back(cMMCom("Aster2Grid", Aster2Grid_main, "Creates a Grid file from the meta-data of an Aster Images"));
	aRes.push_back(cMMCom("ApplyParralaxCor", ApplyParralaxCor_main, "Apply parralax correction from MMTestOrient to an image"));
    aRes.push_back(cMMCom("RefineModel", RefineModel_main, "Refine an approximate model "));
	aRes.push_back(cMMCom("Refine", NewRefineModel_main, "Refine an approximate model "));
	aRes.push_back(cMMCom("RefineJitter", RefineJitter_main, "/!\\ V0.01 Highly experimental /!\\ Refine a grid with Affine + jitter model based on SIFT obs"));
    aRes.push_back(cMMCom("AsterDestrip", AsterDestrip_main, "Destrip Aster Images "));
    aRes.push_back(cMMCom("SATtoBundle", SATtoBundle_main, "Export a satellite image to a grid of bundles"));
    aRes.push_back(cMMCom("SATValid", SATvalid_main, "Validate the prj function by either retrieving the line of optical centers or the provided GCPs"));
    aRes.push_back(cMMCom("SATD2D", CPP_SATDef2D_main, "Visualize 2D deformation fields of a pushbroom image"));
    aRes.push_back(cMMCom("TestRPC", CPP_TestRPCDirectGen, "Test the calculation of direct RPCs"));
    aRes.push_back(cMMCom("ImageTiling", DoTile_main, "Tile an image pair to selected size"));
    cCmpMMCom CmpMMCom;
    std::sort(aRes.begin(), aRes.end(), CmpMMCom);

    return aRes;
}

int SateLib_main(int argc, char ** argv)
{
    return GenMain(argc, argv, SateLibAvailableCommands());
}

//================= XLib =======================

extern int XeresTest_Main(int,char**);
extern int XeresTieP_Main(int,char**);
extern int XeresMergeTieP_Main(int,char**);
extern int XeresHomMatch_main(int,char**);
extern int XeresReNameInit_main(int,char**);
extern int XeresCalibMain_main(int,char**);

const std::vector<cMMCom> & XLibAvailableCommands()
{
    static std::vector<cMMCom> aRes;

    if (aRes.empty())
    {
        aRes.push_back(cMMCom("Test", XeresTest_Main, "test Xeres"));
        aRes.push_back(cMMCom("TieP", XeresTieP_Main, "Xeres tie points"));
        aRes.push_back(cMMCom("MergeTieP", XeresMergeTieP_Main, "Xeres : merge tie points"));
        aRes.push_back(cMMCom("MatchGr"  , XeresHomMatch_main , "Xeres : generate graph for mathcing"));
        aRes.push_back(cMMCom("ReName0"  , XeresReNameInit_main , "Xeres : Rename image for Xeres convention"));
        aRes.push_back(cMMCom("Calib"  , XeresCalibMain_main , "Xeres : Pipeline for calibration images"));
    }

    cCmpMMCom CmpMMCom;
    std::sort(aRes.begin(), aRes.end(), CmpMMCom);

    return aRes;
}

int XLib_Main(int argc, char ** argv)
{
    return GenMain(argc, argv, XLibAvailableCommands());
}

//=====================================

int GenMain(int argc,char ** argv, const std::vector<cMMCom> & aVComs)
{
   if ((argc==1) || ((argc==2) && (std::string(argv[1])=="-help")))
   {
       BanniereMM3D();

       std::cout << "mm3d : Allowed commands \n";
       for (unsigned int aKC=0 ; aKC<aVComs.size() ; aKC++)
       {
            std::cout  << " " << aVComs[aKC].mName << "\t" << aVComs[aKC].mComment << "\n";
       }
       return EXIT_SUCCESS;
   }

   if ((argc>=2) && (argv[1][0] == 'v') && (argv[1]!=std::string("vic")))
   {
       ELISE_ASSERT(ELISE_QT_VERSION > 0, std::string("Qt not installed, " + std::string(argv[1]) + " not available").c_str() );

       std::string cmds[] = { std::string("vMICMAC"), std::string("vApero"), std::string("vAnn"), std::string("vCalibFinale"),
                              std::string("vCalibInit"), std::string("vMergeDepthMap"), std::string("vPastis"),
                              std::string("vPointeInitPolyg"), std::string("vPorto"), std::string("vRechCibleDRad"),
                              std::string("vRechCibleInit"), std::string("vReduc2MM"), std::string("vReducHom"),
                              std::string("vSaisiePts"), std::string("vScriptCalib"), std::string("vSift"),
                              std::string("vSysCoordPolyn"), std::string("vTestChantier"), std::string("vvic")
                            };
       std::vector <std::string> vCmds(cmds, cmds + 19);
       if (std::find(vCmds.begin(), vCmds.end(), argv[1])!=vCmds.end())
       {
           ELISE_ASSERT(false, (argv[1] + std::string(" not available")).c_str() );
       }

       MMVisualMode = true;
       argv[1]++;
   }

   // MPD : deplace sinon core dump qd argc==1
   // Pour l'analyse de la ligne de commande, on ne peut pas desactiver le bloquage de l'exe via l'option ExitOnBrkp
   // puisqu le XML n'a pas encore ete analyse, on change donc provisoirement le comportement par defaut
   // bool aValInit_TheExitOnBrkp=TheExitOnBrkp;
   // TheExitOnBrkp=true;
   MMD_InitArgcArgv( argc, argv );
   #if(ELISE_QT_VERSION >= 4)
        initQtLibraryPath();
   #endif
    // TheExitOnBrkp=true;

   // On reactive le blocage par defaut
   // TheExitOnBrkp=aValInit_TheExitOnBrkp;

   std::string aCom = argv[1];
   // std::string aLowCom = current_program_subcommand();
   std::string aLowCom =  StrToLower(aCom);  // MPD modif, sinon suggestions de marche pas en TestLib

   std::vector<cSuggest *> mSugg;

//  std::cout << "JJJJJ " << aLowCom << " " << aCom  << " " << StrToLower(aCom) << "\n";

   cSuggest *PatMach    = new cSuggest("Pattern Match",aLowCom);
   cSuggest *PrefMach   = new cSuggest("Prefix Match",aLowCom+".*");
   cSuggest *SubMach    = new cSuggest("Subex Match",".*"+aLowCom+".*");
   mSugg.push_back(PatMach);
   mSugg.push_back(PrefMach);
   mSugg.push_back(SubMach);

   const cMMCom *toExecute = NULL;
   for (unsigned int aKC=0 ; aKC<aVComs.size() ; aKC++)
   {
       if (StrToLower(aVComs[aKC].mName)==StrToLower(aCom))
       {
          toExecute = &aVComs[aKC];
          break;
       }
       for (int aKS=0 ; aKS<int(mSugg.size()) ; aKS++)
       {
            mSugg[aKS]->Test(aVComs[aKC]);
       }
   }

	// use suggestion if there is only one and no exact match has been found

  // MPD  : personnellement je desaprouve cette facilite qui est en fait source de confusion
	if ( (!MPD_MM())  &&  (toExecute == NULL && PrefMach->mRes.size() == 1))
	{
		toExecute = &PrefMach->mRes.front();
		cout << "using [" << toExecute->mName << ']' << endl;
	}

	if (toExecute != NULL)
	{
		cArgLogCom aLog = toExecute->mLog;
		bool DoLog = (aLog.mNumArgDir >0) && (aLog.mNumArgDir<argc);
		string outDirectory;
		if (DoLog)
		{
			outDirectory = (isUsingSeparateDirectories() ? MMLogDirectory() : DirOfFile(argv[aLog.mNumArgDir]) + aLog.mDirSup);
			LogIn(argc, argv, outDirectory,aLog.mNumArgDir);
		}

		int aRes = toExecute->mCommand(argc - 1, argv + 1);
		if (DoLog) LogOut(aRes, outDirectory);

		delete PatMach;
		delete PrefMach; 
		delete SubMach;

		if (Chol16Byte) std::cout << "WARN : 16 BYTE ACCURACY FOR LEAST SQUARE\n";

		return aRes;
	}

   for (unsigned int aKS=0 ; aKS<mSugg.size() ; aKS++)
   {
       if (! mSugg[aKS]->mRes.empty())
       {
           std::cout << "Suggest by " << mSugg[aKS]->mName << "\n";
           for (unsigned int aKC=0 ; aKC<mSugg[aKS]->mRes.size() ; aKC++)
           {
               std::cout << "    " << mSugg[aKS]->mRes[aKC].mName << "\n";
           }
           delete PatMach;
           delete PrefMach;
           delete SubMach;
           return EXIT_FAILURE;
       }
   }



   std::cout << "For command = " << argv[1] << "\n";
   ELISE_ASSERT(false,"Unknown command in mm3d");

   delete PatMach;
   delete PrefMach;
   delete SubMach;
   return  EXIT_FAILURE;
}


int main(int argc,char ** argv)
{

    return GenMain(argc,argv, getAvailableCommands());
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
