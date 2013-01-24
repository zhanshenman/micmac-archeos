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
// File Automatically generated by eLiSe

#include "general/all.h"
#include "private/all.h"
#include "cEqAppuiYDRad5.h"


cEqAppuiYDRad5::cEqAppuiYDRad5():
    cElCompiledFonc(1)
{
   AddIntRef (cIncIntervale("Intr",0,10));
   AddIntRef (cIncIntervale("Orient",10,16));
   Close(false);
}



void cEqAppuiYDRad5::ComputeVal()
{
   double tmp0_ = mCompCoord[4];
   double tmp1_ = mCompCoord[3];
   double tmp2_ = mLocXIm-tmp1_;
   double tmp3_ = mLocYIm-tmp0_;
   double tmp4_ = (tmp2_)*(tmp2_);
   double tmp5_ = (tmp3_)*(tmp3_);
   double tmp6_ = tmp4_+tmp5_;
   double tmp7_ = (tmp6_)*(tmp6_);
   double tmp8_ = tmp7_*(tmp6_);
   double tmp9_ = tmp8_*(tmp6_);
   double tmp10_ = mCompCoord[10];
   double tmp11_ = mCompCoord[12];
   double tmp12_ = cos(tmp10_);
   double tmp13_ = cos(tmp11_);
   double tmp14_ = sin(tmp10_);
   double tmp15_ = mCompCoord[11];
   double tmp16_ = sin(tmp15_);
   double tmp17_ = -(tmp16_);
   double tmp18_ = sin(tmp11_);
   double tmp19_ = -(tmp14_);
   double tmp20_ = tmp12_*tmp17_;
   double tmp21_ = mCompCoord[13];
   double tmp22_ = mLocXTer-tmp21_;
   double tmp23_ = -(tmp18_);
   double tmp24_ = tmp14_*tmp17_;
   double tmp25_ = mCompCoord[14];
   double tmp26_ = mLocYTer-tmp25_;
   double tmp27_ = cos(tmp15_);
   double tmp28_ = mCompCoord[15];
   double tmp29_ = mLocZTer-tmp28_;

  mVal[0] = ((tmp0_+(tmp3_)*(1+(tmp6_)*mCompCoord[5]+tmp7_*mCompCoord[6]+tmp8_*mCompCoord[7]+tmp9_*mCompCoord[8]+tmp9_*(tmp6_)*mCompCoord[9]))-mCompCoord[2])/mCompCoord[0]-((tmp19_*tmp13_+tmp20_*tmp18_)*(tmp22_)+(tmp12_*tmp13_+tmp24_*tmp18_)*(tmp26_)+tmp27_*tmp18_*(tmp29_))/((tmp19_*tmp23_+tmp20_*tmp13_)*(tmp22_)+(tmp12_*tmp23_+tmp24_*tmp13_)*(tmp26_)+tmp27_*tmp13_*(tmp29_));

}


void cEqAppuiYDRad5::ComputeValDeriv()
{
   double tmp0_ = mCompCoord[4];
   double tmp1_ = mCompCoord[3];
   double tmp2_ = mLocXIm-tmp1_;
   double tmp3_ = mLocYIm-tmp0_;
   double tmp4_ = (tmp2_)*(tmp2_);
   double tmp5_ = (tmp3_)*(tmp3_);
   double tmp6_ = tmp4_+tmp5_;
   double tmp7_ = (tmp6_)*(tmp6_);
   double tmp8_ = tmp7_*(tmp6_);
   double tmp9_ = tmp8_*(tmp6_);
   double tmp10_ = mCompCoord[10];
   double tmp11_ = mCompCoord[12];
   double tmp12_ = cos(tmp10_);
   double tmp13_ = cos(tmp11_);
   double tmp14_ = sin(tmp10_);
   double tmp15_ = mCompCoord[11];
   double tmp16_ = sin(tmp15_);
   double tmp17_ = -(tmp16_);
   double tmp18_ = sin(tmp11_);
   double tmp19_ = -(tmp14_);
   double tmp20_ = tmp12_*tmp17_;
   double tmp21_ = mCompCoord[13];
   double tmp22_ = mLocXTer-tmp21_;
   double tmp23_ = -(tmp18_);
   double tmp24_ = tmp14_*tmp17_;
   double tmp25_ = mCompCoord[14];
   double tmp26_ = mLocYTer-tmp25_;
   double tmp27_ = cos(tmp15_);
   double tmp28_ = mCompCoord[15];
   double tmp29_ = mLocZTer-tmp28_;
   double tmp30_ = mCompCoord[5];
   double tmp31_ = (tmp6_)*tmp30_;
   double tmp32_ = 1+tmp31_;
   double tmp33_ = mCompCoord[6];
   double tmp34_ = tmp7_*tmp33_;
   double tmp35_ = tmp32_+tmp34_;
   double tmp36_ = mCompCoord[7];
   double tmp37_ = tmp8_*tmp36_;
   double tmp38_ = tmp35_+tmp37_;
   double tmp39_ = mCompCoord[8];
   double tmp40_ = tmp9_*tmp39_;
   double tmp41_ = tmp38_+tmp40_;
   double tmp42_ = tmp9_*(tmp6_);
   double tmp43_ = mCompCoord[9];
   double tmp44_ = tmp42_*tmp43_;
   double tmp45_ = tmp41_+tmp44_;
   double tmp46_ = (tmp3_)*(tmp45_);
   double tmp47_ = tmp0_+tmp46_;
   double tmp48_ = mCompCoord[2];
   double tmp49_ = (tmp47_)-tmp48_;
   double tmp50_ = mCompCoord[0];
   double tmp51_ = ElSquare(tmp50_);
   double tmp52_ = -(1);
   double tmp53_ = tmp52_*(tmp2_);
   double tmp54_ = tmp53_+tmp53_;
   double tmp55_ = (tmp54_)*(tmp6_);
   double tmp56_ = tmp55_+tmp55_;
   double tmp57_ = (tmp56_)*(tmp6_);
   double tmp58_ = (tmp54_)*tmp7_;
   double tmp59_ = tmp57_+tmp58_;
   double tmp60_ = (tmp59_)*(tmp6_);
   double tmp61_ = (tmp54_)*tmp8_;
   double tmp62_ = tmp60_+tmp61_;
   double tmp63_ = tmp52_*(tmp3_);
   double tmp64_ = tmp63_+tmp63_;
   double tmp65_ = (tmp64_)*(tmp6_);
   double tmp66_ = tmp65_+tmp65_;
   double tmp67_ = (tmp66_)*(tmp6_);
   double tmp68_ = (tmp64_)*tmp7_;
   double tmp69_ = tmp67_+tmp68_;
   double tmp70_ = (tmp69_)*(tmp6_);
   double tmp71_ = (tmp64_)*tmp8_;
   double tmp72_ = tmp70_+tmp71_;
   double tmp73_ = tmp52_*tmp14_;
   double tmp74_ = tmp20_*tmp18_;
   double tmp75_ = tmp19_*tmp23_;
   double tmp76_ = tmp20_*tmp13_;
   double tmp77_ = tmp75_+tmp76_;
   double tmp78_ = (tmp77_)*(tmp22_);
   double tmp79_ = tmp12_*tmp23_;
   double tmp80_ = tmp24_*tmp13_;
   double tmp81_ = tmp79_+tmp80_;
   double tmp82_ = (tmp81_)*(tmp26_);
   double tmp83_ = tmp78_+tmp82_;
   double tmp84_ = tmp27_*tmp13_;
   double tmp85_ = tmp84_*(tmp29_);
   double tmp86_ = tmp83_+tmp85_;
   double tmp87_ = tmp19_*tmp13_;
   double tmp88_ = tmp87_+tmp74_;
   double tmp89_ = (tmp88_)*(tmp22_);
   double tmp90_ = tmp12_*tmp13_;
   double tmp91_ = tmp24_*tmp18_;
   double tmp92_ = tmp90_+tmp91_;
   double tmp93_ = (tmp92_)*(tmp26_);
   double tmp94_ = tmp89_+tmp93_;
   double tmp95_ = tmp27_*tmp18_;
   double tmp96_ = tmp95_*(tmp29_);
   double tmp97_ = tmp94_+tmp96_;
   double tmp98_ = -(tmp12_);
   double tmp99_ = tmp73_*tmp17_;
   double tmp100_ = -(tmp27_);
   double tmp101_ = tmp100_*tmp12_;
   double tmp102_ = tmp100_*tmp14_;
   double tmp103_ = tmp52_*tmp16_;
   double tmp104_ = ElSquare(tmp86_);
   double tmp105_ = tmp52_*tmp18_;
   double tmp106_ = -(tmp13_);

  mVal[0] = (tmp49_)/tmp50_-(tmp97_)/(tmp86_);

  mCompDer[0][0] = -(tmp49_)/tmp51_;
  mCompDer[0][1] = 0;
  mCompDer[0][2] = (tmp52_*tmp50_)/tmp51_;
  mCompDer[0][3] = (((tmp54_)*tmp30_+(tmp56_)*tmp33_+(tmp59_)*tmp36_+(tmp62_)*tmp39_+((tmp62_)*(tmp6_)+(tmp54_)*tmp9_)*tmp43_)*(tmp3_)*tmp50_)/tmp51_;
  mCompDer[0][4] = ((1+tmp52_*(tmp45_)+((tmp64_)*tmp30_+(tmp66_)*tmp33_+(tmp69_)*tmp36_+(tmp72_)*tmp39_+((tmp72_)*(tmp6_)+(tmp64_)*tmp9_)*tmp43_)*(tmp3_))*tmp50_)/tmp51_;
  mCompDer[0][5] = ((tmp6_)*(tmp3_)*tmp50_)/tmp51_;
  mCompDer[0][6] = (tmp7_*(tmp3_)*tmp50_)/tmp51_;
  mCompDer[0][7] = (tmp8_*(tmp3_)*tmp50_)/tmp51_;
  mCompDer[0][8] = (tmp9_*(tmp3_)*tmp50_)/tmp51_;
  mCompDer[0][9] = (tmp42_*(tmp3_)*tmp50_)/tmp51_;
  mCompDer[0][10] = -((((tmp98_*tmp13_+tmp99_*tmp18_)*(tmp22_)+(tmp73_*tmp13_+tmp74_)*(tmp26_))*(tmp86_)-(tmp97_)*((tmp98_*tmp23_+tmp99_*tmp13_)*(tmp22_)+(tmp73_*tmp23_+tmp76_)*(tmp26_)))/tmp104_);
  mCompDer[0][11] = -(((tmp101_*tmp18_*(tmp22_)+tmp102_*tmp18_*(tmp26_)+tmp103_*tmp18_*(tmp29_))*(tmp86_)-(tmp97_)*(tmp101_*tmp13_*(tmp22_)+tmp102_*tmp13_*(tmp26_)+tmp103_*tmp13_*(tmp29_)))/tmp104_);
  mCompDer[0][12] = -((((tmp105_*tmp19_+tmp13_*tmp20_)*(tmp22_)+(tmp105_*tmp12_+tmp13_*tmp24_)*(tmp26_)+tmp13_*tmp27_*(tmp29_))*(tmp86_)-(tmp97_)*((tmp106_*tmp19_+tmp105_*tmp20_)*(tmp22_)+(tmp106_*tmp12_+tmp105_*tmp24_)*(tmp26_)+tmp105_*tmp27_*(tmp29_)))/tmp104_);
  mCompDer[0][13] = -((tmp52_*(tmp88_)*(tmp86_)-(tmp97_)*tmp52_*(tmp77_))/tmp104_);
  mCompDer[0][14] = -((tmp52_*(tmp92_)*(tmp86_)-(tmp97_)*tmp52_*(tmp81_))/tmp104_);
  mCompDer[0][15] = -((tmp52_*tmp95_*(tmp86_)-(tmp97_)*tmp52_*tmp84_)/tmp104_);
}


void cEqAppuiYDRad5::ComputeValDerivHessian()
{
  ELISE_ASSERT(false,"Foncteur cEqAppuiYDRad5 Has no Der Sec");
}

void cEqAppuiYDRad5::SetXIm(double aVal){ mLocXIm = aVal;}
void cEqAppuiYDRad5::SetXTer(double aVal){ mLocXTer = aVal;}
void cEqAppuiYDRad5::SetYIm(double aVal){ mLocYIm = aVal;}
void cEqAppuiYDRad5::SetYTer(double aVal){ mLocYTer = aVal;}
void cEqAppuiYDRad5::SetZTer(double aVal){ mLocZTer = aVal;}



double * cEqAppuiYDRad5::AdrVarLocFromString(const std::string & aName)
{
   if (aName == "XIm") return & mLocXIm;
   if (aName == "XTer") return & mLocXTer;
   if (aName == "YIm") return & mLocYIm;
   if (aName == "YTer") return & mLocYTer;
   if (aName == "ZTer") return & mLocZTer;
   return 0;
}


cElCompiledFonc::cAutoAddEntry cEqAppuiYDRad5::mTheAuto("cEqAppuiYDRad5",cEqAppuiYDRad5::Alloc);


cElCompiledFonc *  cEqAppuiYDRad5::Alloc()
{  return new cEqAppuiYDRad5();
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