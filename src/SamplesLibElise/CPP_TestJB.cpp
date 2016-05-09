#include "StdAfx.h"
#include "../src/uti_image/Digeo/MultiChannel.h"

//~ #define PRINT_EL_SYSTEM
#define EXECUTE_EL_SYSTEM

typedef struct{
	string name;
	int (*func)( int argc, char **argv );
} command_t;

int command_correctPlanarPolygons( int argc, char **argv );
int command_maskContent( int argc, char **argv );
int command_renameImageSet( int argc, char **argv );
int command_toto( int argc, char **argv );
int command_makeSets( int argc, char **argv );
int command_drawMatches( int argc, char **argv );

command_t commands[] = {
	{ "correctplanarpolygons", &command_correctPlanarPolygons },
	{ "maskcontent", &command_maskContent },
	{ "renameimageset", &command_renameImageSet },
	{ "toto", &command_toto },
	{ "makesets", &command_makeSets },
	{ "drawmatches", &command_drawMatches },
	{ "", NULL }
};

void correctPlanarPolygon( vector<Pt3dr> &aPolygon )
{
	if (aPolygon.size() < 3) ELISE_ERROR_EXIT("aPolygon.size() = " << aPolygon.size() << " < 3");
	Pt3dr u = vunit(aPolygon[1] - aPolygon[0]), minV = vunit(aPolygon[2] - aPolygon[0]);
	size_t minI = 2;
	REAL minScal = ElAbs(scal(u, minV));
	for (size_t i = 3; i < aPolygon.size(); i++)
	{
		Pt3dr v = vunit(aPolygon[i] - aPolygon[0]);
		REAL d = ElAbs(scal(u, v));
		if (d < minScal)
		{
			minScal = d;
			minI = i;
			minV = v;
		}
	}
	cout << "minI = " << minI << endl;
	cout << "minScal = " << minScal << endl;
	cout << "minV = " << minV << endl;
	Pt3dr n = u ^ minV;
	cout << "minV = " << minV << endl;
	ElMatrix<REAL> planToGlobalMatrix = MatFromCol(u, minV, n);
	if (planToGlobalMatrix.Det() < 1e-10) ELISE_ERROR_EXIT("matrix is not inversible");

	ElRotation3D planToGlobalRot(aPolygon[0], planToGlobalMatrix, true);
	ElRotation3D globalToPlanRot = planToGlobalRot.inv();

	//~ const size_t nbVertices = aPolygon.size();
	//~ ostringstream ss;
	//~ static int ii = 0;
	//~ const REAL extrudSize = 1e4;
	//~ ss << "polygon_" << (ii++) << ".ply";
	//~ ofstream f(ss.str().c_str());
	//~ f << "ply" << endl;
	//~ f << "format ascii 1.0" << endl;
	//~ f << "element vertex " << 4 * nbVertices << endl;
	//~ f << "property float x" << endl;
	//~ f << "property float y" << endl;
	//~ f << "property float z" << endl;
	//~ f << "property uchar red" << endl;
	//~ f << "property uchar green" << endl;
	//~ f << "property uchar blue" << endl;
	//~ f << "element face " << nbVertices << endl;
	//~ f << "property list uchar int vertex_indices" << endl;
	//~ f << "end_header" << endl;

	REAL zDiffSum = 0.;
	for (size_t i = 0; i < aPolygon.size(); i++)
	{
		Pt3dr p = globalToPlanRot.ImAff(aPolygon[i]);
		zDiffSum += ElAbs(p.z);
		aPolygon[i] = planToGlobalRot.ImAff(Pt3dr(p.x, p.y, 0.));

		//~ Pt3dr p0 = (i == 0) ? aPolygon[aPolygon.size() - 1] : aPolygon[i - 1], p1 = aPolygon[i], p2 = p1 + n * extrudSize, p3 = p0 + n * extrudSize;
		//~ f << p0.x << ' ' << p0.y << ' ' << p0.z << " 128 128 128" << endl;
		//~ f << p1.x << ' ' << p1.y << ' ' << p1.z << " 128 128 128" << endl;
		//~ f << p2.x << ' ' << p2.y << ' ' << p2.z << " 128 128 128" << endl;
		//~ f << p3.x << ' ' << p3.y << ' ' << p3.z << " 128 128 128" << endl;
	}

	//~ for (size_t i = 0; i < aPolygon.size(); i++)
		//~ f << 4 << ' ' << i * 4 << ' ' << i * 4 + 1 << ' ' << i * 4 + 2 << ' ' << i* 4 + 3 << endl;
	//~ f.close();
	cout << "zDiffSum = " << zDiffSum << endl;
}

void correctPlanarPolygons( vector<cItem> &aPolygons )
{
	for (size_t i = 0; i < aPolygons.size(); i++)
		correctPlanarPolygon(aPolygons[i].Pt());
}

void writePolygons( const cPolyg3D &aPolygons, const string &aFilename )
{
	cElXMLFileIn xmlFile(aFilename);
	cElXMLTree *xmlTree = ToXMLTree(aPolygons);
	xmlFile.PutTree(xmlTree);
	delete xmlTree;
}

//~ #if ELISE_QT_VERSION >=4
//~ #include "../saisieQT/include_QT/Cloud.h"
//~ #endif

int command_correctPlanarPolygons( int argc, char **argv )
{
	if (argc < 1) ELISE_ERROR_EXIT("an XML filename is needed");

	const string xmlFilename = argv[0];
	cPolyg3D polygons = StdGetFromSI(xmlFilename, Polyg3D);
	correctPlanarPolygons(polygons.Item());
	writePolygons(polygons, "corrected_polygons.xml");

	return EXIT_SUCCESS;
}


//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------

#if ELISE_QT_VERSION >=4
//~ #if 0
	#include "../saisieQT/include_QT/Cloud.h"
#endif

void getPlyBoundingBox( const string &aFilename, Pt3dr &oP0, Pt3dr &oP1 )
{
	#if ELISE_QT_VERSION >=4
	//~ #if 0
		GlCloud *ply = GlCloud::loadPly(aFilename);
		if ( !ply) ELISE_ERROR_EXIT("cannot load ply file [" << aFilename << ']');

		if (ply->size() == 0) return;

		float min[3], max[3];
		{
			QVector3D p = ply->getVertex(0).getPosition();
			min[0] = max[0] = p.x();
			min[1] = max[1] = p.y();
			min[2] = max[2] = p.z();
		}
		const int plySize = ply->size();
		for (int i = 1; i < plySize; i++)
		{
			QVector3D p = ply->getVertex(i).getPosition();

			if (p.x() < min[0]) min[0] = p.x();
			if (p.y() < min[1]) min[1] = p.y();
			if (p.z() < min[2]) min[2] = p.z();

			if (p.x() > max[0]) max[0] = p.x();
			if (p.y() > max[1]) max[1] = p.y();
			if (p.z() > max[2]) max[2] = p.z();
		}

		oP0 = Pt3dr((REAL)min[0], (REAL)min[1], (REAL)min[2]);
		oP1 = Pt3dr((REAL)max[0], (REAL)max[1], (REAL)max[2]);
	#else
		ELISE_ERROR_EXIT("getPlyBoundingBox: no Qt");
	#endif
}

void makeGrid( const Pt3dr &aP0, const Pt3dr &aSize, unsigned int aPointsPerAxis, const cMasqBin3D &aMask, list<Pt3dr> &oPoints )
{
	ELISE_DEBUG_ERROR(aSize.x <= 0. || aSize.y <= 0. || aSize.z <= 0., "makeGrid", "invalid box size = " << aSize);

	oPoints.clear();
	if (aPointsPerAxis == 0) return;

	Pt3dr pace = aSize / (REAL)(aPointsPerAxis - 1), p;
	const Pt3dr p1 = aP0 + aSize;
	REAL z = aP0.z;
	while (z <= p1.z)
	{
		REAL y = aP0.y;
		while (y <= p1.y)
		{
			REAL x = aP0.x;
			while (x <= p1.x)
			{
				p.x = x;
				p.y = y;
				p.z = z;
				if (aMask.IsInMasq(p)) oPoints.push_back(p);

				x += pace.x;
			}
			y += pace.y;
		}
		z += pace.z;
	}
}

bool writePly( const list<Pt3dr> &aPoints, const string &aFilename )
{
	ofstream f(aFilename.c_str());

	if ( !f) return false;

	f << "ply" << endl;
	f << "format ascii 1.0" << endl;
	f << "element vertex " << aPoints.size() << endl;
	f << "property float x" << endl;
	f << "property float y" << endl;
	f << "property float z" << endl;
	f << "property uchar red" << endl;
	f << "property uchar green" << endl;
	f << "property uchar blue" << endl;
	f << "element face 0" << endl;
	f << "property list uchar int vertex_indices" << endl;
	f << "end_header" << endl;

	list<Pt3dr>::const_iterator itPoint = aPoints.begin();
	while (itPoint != aPoints.end())
	{
		const Pt3dr &p = *itPoint++;
		f << p.x << ' ' << p.y << ' ' << p.z << " 128 128 128" << endl;
	}

	return true;
}

int command_maskContent( int argc, char ** argv )
{
	if (argc < 1) ELISE_ERROR_EXIT("a *_polyg3d.ply file is needed");

	const string maskFilename = argv[1];
	cMasqBin3D *masqBin3D = cMasqBin3D::FromSaisieMasq3d(maskFilename);

	if (masqBin3D == NULL) ELISE_ERROR_EXIT("cannot load mask 3d file [" << maskFilename << "]");

	Pt3dr bbP0, bbP1;
	if (argc >= 2)
	{
		const string plyFilename = argv[1];
		getPlyBoundingBox(plyFilename, bbP0, bbP1);
		cout << "--- bounding box [" << bbP0 << ", " << bbP1 << ']' << endl;

		list<Pt3dr> inMaskPoints;
		makeGrid(bbP0, bbP1 - bbP0, 1000, *masqBin3D, inMaskPoints);
		cout << "--- " << inMaskPoints.size() << " points in mask" << endl;

		const string outputFilename = "maskContent.ply";
		if ( !writePly(inMaskPoints, outputFilename)) ELISE_ERROR_EXIT("failed to write point cloud in [" << outputFilename << ']');
	}

	delete masqBin3D;

	return EXIT_SUCCESS;
}

//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------

size_t getNbDigits(size_t i_n, unsigned int i_base = 10)
{
	//~ double l = log((double)i_n) / log(i_base);
	//~ unsigned int i = (unsigned int)l;
	//~ if (l != double(i)) return i + 1;
	//~ return i;

	stringstream ss;
	ss << i_n;
	return ss.str().length();
}

int command_renameImageSet( int argc, char **argv )
{
	if (argc < 2) ELISE_ERROR_EXIT("arg0 is a path + regular expression, arg1 is output prefix");

	cElPathRegex fullPattern(argv[0]);
	cElFilename outputBase(argv[1]);
	cout << "input pattern = [" << fullPattern.str() << "]" << endl;
	cout << "output base = [" << outputBase.str() << "]" << endl;

	list<cElFilename> filenames;
	fullPattern.getFilenames(filenames);

	const size_t nbdigits = getNbDigits((unsigned int)filenames.size(), 10);
	size_t iFilename = 0;
	list<cElFilename>::const_iterator itFilename = filenames.begin();
	while (itFilename != filenames.end())
	{
		const cElFilename src = (*itFilename++);
		const string srcStr = src.str();

		stringstream ss;
		ss << outputBase.str() << setw(nbdigits) << setfill('0') << (iFilename++) << getShortestExtension(srcStr);
		cElFilename dst(ss.str());

		cout << '[' << src.str() << "] -> " << dst.str() << endl;

		if (dst.exists())
			cerr << "[" << dst.str() << "] already exists" << endl;
		else if ( !src.copy(dst))
			cerr << "failed to copy [" << src.str() << "] to [" << dst.str() << "]" << endl;
	}

	return EXIT_SUCCESS;
}


//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------

void dump(const cFileOriMnt &aFileOriMnt, const string &aPrefix = string(), ostream &aStream = cout)
{
	aStream << aPrefix << "NameFileMnt     = [" << aFileOriMnt.NameFileMnt() << ']' << endl;
	if (aFileOriMnt.NameFileMasque().IsInit()) aStream << aPrefix << "NameFileMasque  = [" << aFileOriMnt.NameFileMasque().Val() << ']' << endl;
	aStream << aPrefix << "NombrePixels    = " << aFileOriMnt.NombrePixels() << endl;
	aStream << aPrefix << "OriginePlani    = " << aFileOriMnt.OriginePlani() << endl;
	aStream << aPrefix << "ResolutionPlani = " << aFileOriMnt.ResolutionPlani() << endl;
	aStream << aPrefix << "OrigineAlti     = " << aFileOriMnt.OrigineAlti() << endl;
	aStream << aPrefix << "ResolutionAlti  = " << aFileOriMnt.ResolutionAlti() << endl;
	if (aFileOriMnt.NumZoneLambert().IsInit()) aStream << aPrefix << "NumZoneLambert  = " << aFileOriMnt.NumZoneLambert().Val() << endl;
	aStream << aPrefix << "Geometrie       = " << eToString(aFileOriMnt.Geometrie()) << endl;
	if (aFileOriMnt.OrigineTgtLoc().IsInit()) aStream << aPrefix << "OrigineTgtLoc  = " << aFileOriMnt.OrigineTgtLoc().Val() << endl;
	if (aFileOriMnt.Rounding().IsInit()) aStream << aPrefix << "Rounding  = " << aFileOriMnt.Rounding().Val() << endl;
}

void dump(const cRepereCartesien &aRepereCartesien, const string &aPrefix = string(), ostream &aStream = cout)
{
	aStream << aPrefix << "ori = " << aRepereCartesien.Ori() << endl;
	aStream << aPrefix << "ox  = " << aRepereCartesien.Ox() << endl;
	aStream << aPrefix << "oy  = " << aRepereCartesien.Oy() << endl;
	aStream << aPrefix << "oz  = " << aRepereCartesien.Oz() << endl;
}

int command_toto( int argc, char **argv )
{
	#if 0
		if (argc != 2) ELISE_ERROR_RETURN("usage: src_directory src_directory");

		ctPath src(argv[0]), dst(argv[1]);
		cout << "src: [" << src.str() << ']' << endl;
		cout << "dst: [" << dst.str() << ']' << endl;
		if (src.copy(dst))
			cout << "copy succeeded" << endl;
		else
			cout << "copy failed" << endl;
	#endif

	#if 0
		const string xml_zmap = "/home/jbelvaux/data/Pierrerue_benjamin/Z_Num8_DeZoom2_STD-MALT.xml";
		cFileOriMnt fileOriMNT_zmap = StdGetFromPCP(xml_zmap, FileOriMnt);
		cout << '[' << xml_zmap << ']' << endl;
		dump(fileOriMNT_zmap, "\t");
		cout << endl;

		const string xml_ortho = "/home/jbelvaux/data/Pierrerue_benjamin/MTDOrtho.xml";
		cFileOriMnt fileOriMNT_ortho = StdGetFromPCP(xml_ortho, FileOriMnt);
		cout << '[' << xml_ortho << ']' << endl;
		dump(fileOriMNT_ortho, "\t");
		cout << endl;

		const string xml_repereCartesien = "/home/jbelvaux/data/Pierrerue_benjamin/Repere-Facade1.xml";
		cRepereCartesien repereCartesien = StdGetFromPCP(xml_repereCartesien, RepereCartesien);
		cout << '[' << xml_repereCartesien << ']' << endl;
		dump(repereCartesien, "\t");
		cout << endl;

		const Pt2dr p2d(367.36, 4224.);
		Pt2dr pp2d = ToMnt(fileOriMNT_ortho, p2d);
		cout << "toMNT(" << p2d << ") = " << pp2d << endl << endl;

		const Pt3dr p(367.36, 4224., -1.85795);
		Pt3dr pp = ToMnt(fileOriMNT_ortho, p);
		cout << "toMNT(" << p << ") = " << pp << endl;

		cChCoCart changeCoordCartesiennes = cChCoCart::Xml2El(repereCartesien);
		//~ cout << "f(0., 0., 0.) = " << changeCoordCartesiennes.FromLoc(Pt3dr(0., 0., 0.)) << endl;
		cout << "f(" << pp << ") = " << changeCoordCartesiennes.FromLoc(pp) << endl;
	#endif

	cElRegex pattern(".*Ori-.*/(UnCorMM-|)Orientation.*xml",10);
	if (pattern.Match("Ori-Toto"))
		cout << "yeah !" << endl;
	else
		cout << "oh no !" << endl;

	return EXIT_SUCCESS;
}


//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------

class ImageSet
{
public:
	class Info
	{
	public:
		string mModel;
		Pt2di mSize;
		double mFocal;

		Info(const cElFilename &aFilename)
		{
			cMetaDataPhoto metaData = cMetaDataPhoto::CreateExiv2(aFilename.str());

			//~ cXmlXifInfo xmlMetaData = MDT2Xml(metaData);
			//~ MakeFileXML(xmlMetaData, StdMetaDataFilename(basename, false)); // false = binary

			mModel = metaData.Cam();
			mSize = metaData.SzImTifOrXif();
			mFocal = metaData.FocMm();
		}

		bool operator !=(const Info &aInfo) const
		{
			return mModel != aInfo.mModel || mSize != aInfo.mSize || mFocal != aInfo.mFocal;
		}
	};

	ImageSet(const Info &aInfo):
		mInfo(aInfo)
	{
	}

	void dump(const string &aPrefix = string(), ostream &aStream = cout) const;

	void move(const string &aBasename) const
	{
		if (mFilenames.empty()) return;

		size_t iFilename = 0;
		size_t nbDigits = getNbDigits(mFilenames.size() - 1);
		list<cElFilename>::const_iterator itFilename = mFilenames.begin();
		while (itFilename != mFilenames.end())
		{
			const string extension = getShortestExtension(itFilename->m_basename);
			ostringstream ss;
			ss << aBasename << '_' << setw(nbDigits) << setfill('0') << iFilename++ << extension;
			const cElFilename newFilename(itFilename->m_path, ss.str());

			cout << "\t[" << itFilename->str() << "] -> [" << newFilename.str() << ']' << endl;
			itFilename->move(newFilename);

			itFilename++;
		}
	}

	Info mInfo;
	list<cElFilename> mFilenames;
};

ostream & operator <<(ostream &aStream, const ImageSet::Info &aInfo)
{
	return (aStream << '[' << aInfo.mModel << "] " << aInfo.mSize.x << 'x' << aInfo.mSize.y << ' ' << aInfo.mFocal);
}

void ImageSet::dump(const string &aPrefix, ostream &aStream) const
{
	aStream << aPrefix << mInfo << endl; 
	list<cElFilename>::const_iterator itFilename = mFilenames.begin();
	while (itFilename != mFilenames.end()) aStream << aPrefix << "\t[" << (*itFilename++).str() << ']' << endl;
}

void add(list<ImageSet> &aSets, const cElFilename &aFilename)
{
	ImageSet::Info info(aFilename);
	list<ImageSet>::iterator it = aSets.begin();
	while (it != aSets.end() && it->mInfo != info) it++;

	if (it == aSets.end())
	{
		aSets.push_back(ImageSet(info));
		it = --aSets.rbegin().base();
	}

	it->mFilenames.push_back(aFilename);
}

void move(list<ImageSet> &aSets, const string &aBase)
{
	if (aSets.empty()) return;

	size_t nbDigits = getNbDigits(aSets.size() - 1);
	size_t iSet = 0;
	list<ImageSet>::iterator it = aSets.begin();
	while (it != aSets.end())
	{
		cout << it->mInfo << ": " << it->mFilenames.size() << " image" << toS(it->mFilenames.size()) << endl;
		ostringstream ss;
		ss << aBase << setw(nbDigits) << setfill('0') << iSet++;
		(*it++).move(ss.str());
	}
}

void dump(const list<ImageSet> &aSets, const string &aPrefix = string(), ostream &aStream = cout)
{
	cout << aSets.size() << " set" << toS(aSets.size()) << endl;
	size_t iSet = 0;
	list<ImageSet>::const_iterator itSet = aSets.begin();
	while (itSet != aSets.end())
	{
		aStream << aPrefix << iSet++ << ": ";
		(*itSet++).dump(aPrefix, aStream);
	}
}

int command_makeSets(int argc, char **argv)
{
	if (argc < 1) ELISE_ERROR_RETURN("missing image filename pattern");

	cElPathRegex fullPattern(argv[0]);

	if ( !fullPattern.m_path.exists()) ELISE_ERROR_RETURN("pattern directory [" << fullPattern.m_path.str() << "] does not existing");

	list<cElFilename> filenames;
	fullPattern.getFilenames(filenames);
	filenames.sort();

	const size_t nbImages = filenames.size();
	if (nbImages == 0) ELISE_ERROR_RETURN("pattern defines no image filename");

	cout << "--- " << nbImages << " image" << toS(nbImages) << endl;

	list<ImageSet> sets;
	list<cElFilename>::const_iterator itFilename = filenames.begin();
	while (itFilename != filenames.end())
		add(sets, *itFilename++);

	const size_t nbSets = sets.size();
	cout << "--- " << nbSets << " set" << toS(nbSets) << endl;

	//~ dump(sets);
	move(sets, "set");

	return EXIT_SUCCESS;
}


//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------

class Match
{
public:
	Pt2dr mP0, mP1;
	REAL mWeight;
};

ostream & operator <<(ostream &aStream, const Match &aMatch)
{
	return aStream << aMatch.mP0 << ' ' << aMatch.mP1 << ' ' << aMatch.mWeight;
}

bool read_ascii_matches(const string aFilename, vector<Match> &oMatches)
{
	oMatches.clear();
	ifstream f(aFilename.c_str());

	if ( !f)
	{
		ELISE_DEBUG_ERROR(true, "read_ascii_matches", "failed to open [" << aFilename << "] for reading");
		return false;
	}

	list<Match> matches;
	Match match;
	while ( !f.eof())
	{
		f >> match.mP0.x >> match.mP0.y >> match.mP1.x >> match.mP1.y;

		if ( !f.eof()) matches.push_back(match);
	}

	oMatches.resize(matches.size());
	copy(matches.begin(), matches.end(), oMatches.begin());

	return true;
}

bool read_binary_matches(const string aFilename, vector<Match> &oMatches)
{
	oMatches.clear();
	ifstream f(aFilename.c_str(), ios::binary);

	if ( !f)
	{
		ELISE_DEBUG_ERROR(true, "read_binary_matches", "failed to open [" << aFilename << "] for reading");
		return false;
	}

	INT4 nbDimensions;
	f.read((char *)&nbDimensions, sizeof(nbDimensions));
	INT4 nbMatches;
	f.read((char *)&nbMatches, sizeof(nbMatches));

	if (nbMatches < 0)
	{
		ELISE_DEBUG_ERROR(true, "read_binary_matches", "nbMatches = " << nbMatches);
		return false;
	}

	oMatches.resize((size_t)nbMatches);
	Match *itMatch = oMatches.data();
	int nbElements;
	REAL buffer[5];
	for (int i = 0; i < nbMatches; i++)
	{
		f.read((char *)&nbElements, sizeof(nbElements));

		if (nbElements != 2)
		{
			ELISE_DEBUG_ERROR(true, "read_binary_matches", "nbElements = " << nbElements << " != nbDimensions = " << nbDimensions);
			return false;
		}

		f.read((char *)buffer, sizeof(buffer));
		itMatch->mP0 = Pt2dr(buffer[1], buffer[2]);
		itMatch->mP1 = Pt2dr(buffer[3], buffer[4]);
		(*itMatch++).mWeight = buffer[0];
	}

	return true;
}

void getGrayImage(const string &aFilename, Im2D_U_INT1 &oGrayImage)
{
	if ( !Tiff_Im::IsTiff(aFilename.c_str())) ELISE_ERROR_EXIT("[" << aFilename << "] is not a TIFF file");

	MultiChannel<U_INT1> channels;
	if ( !channels.read_tiff(aFilename)) ELISE_ERROR_EXIT("failed to read TIFF image [" << aFilename << "]");

	cout << '\t' << "-- [" << aFilename << "] read: " << channels.width() << 'x' << channels.height() << 'x' << channels.nbChannels() << ' ' << eToString(channels.typeEl()) << endl;

	if (channels.typeEl() != GenIm::u_int1 || channels.nbChannels() == 2) ELISE_ERROR_EXIT("invalid image format: " << channels.nbChannels() << ' ' << eToString(channels.typeEl()));

	channels.toGrayScale(oGrayImage);
}

void getGrayImages(const string &aFilename, Im2D_U_INT1 &oGrayImage0, Im2D_U_INT1 &oGrayImage1, Im2D_U_INT1 &oGrayImage2)
{
	getGrayImage(aFilename, oGrayImage0);

	oGrayImage1.Resize(oGrayImage0.sz());
	oGrayImage1.dup(oGrayImage0);

	oGrayImage2.Resize(oGrayImage0.sz());
	oGrayImage2.dup(oGrayImage0);
}

void drawPoint(Im2D_U_INT1 &aImage, int aX, int aY, U_INT1 aValue)
{
	if (aX >=0 && aX < aImage.tx() && aY >= 0 && aY < aImage.ty()) aImage.data()[aY][aX] = aValue;
}

void drawPoint(Im2D_U_INT1 &aImage, const Pt2dr &aPoint, U_INT1 aValue, unsigned int aSize)
{
	const int sizei = (int)aSize;
	int x = round_ni(aPoint.x), y = round_ni(aPoint.y);

	if (x < 0 || x >= aImage.tx() || y < 0 || y >= aImage.ty())
	{
		ELISE_WARNING("point (" << x << ',' << y << ") out of rectangle " << aImage.sz());
		return;
	}

	drawPoint(aImage, x, y, aValue);
	for (int j = -sizei; j <= sizei; j++)
		for (int i = -sizei; i < sizei; i++)
		{
			drawPoint(aImage, x + i, y + j, aValue);
		}
}

void save_tiff(const string &aFilename, Im2D_U_INT1 &aRed, Im2D_U_INT1 &aGreen, Im2D_U_INT1 &aBlue)
{
	ELISE_DEBUG_ERROR(aRed.sz() != aGreen.sz() || aRed.sz() != aBlue.sz(), "save_tiff", "invalid sizes " << aRed.sz() << ' ' << aGreen.sz() << ' ' << aBlue.sz());

	ELISE_COPY
	(
		aRed.all_pts(),
		Virgule(aRed.in(), aGreen.in(), aBlue.in()),
		Tiff_Im(
			aFilename.c_str(),
			aRed.sz(),
			GenIm::u_int1,
			Tiff_Im::No_Compr,
			Tiff_Im::RGB,
			Tiff_Im::Empty_ARG ).out()
	);

	if ( !ELISE_fp::exist_file(aFilename)) ELISE_ERROR_EXIT("failed to save TIFF image [" << aFilename << "]");
	cout << '\t' << "-- TIFF file [" << aFilename << "] created" << endl;
}

bool is_binary_matches(const string &aFilename)
{
	ifstream f(aFilename.c_str(), ios::binary);
	INT4 nbDimensions;
	f.read((char *)&nbDimensions, 4);
	return nbDimensions == 128;
}

string clipShortestExtension(const std::string &aFilename);

void ElSystem(const string &aCommand)
{
	#ifdef PRINT_EL_SYSTEM
		cout << "ElSystem: [" << aCommand << "]" << endl;
	#endif

	#ifdef EXECUTE_EL_SYSTEM
		int result = System(aCommand + " >/dev/null 2>&1");
		if (result != 0) ELISE_ERROR_EXIT("command [" << aCommand << "] failed (" << result << ")");
	#endif
}

string scaled_filename(const string &aFilename, int aScale)
{
	stringstream ss;
	ss << clipShortestExtension(aFilename) << '.' << setw(3) << setfill('0') << aScale << ".tif";
	return ss.str();
}

string points_filename(const string &aFilename, const string &aDetectTool)
{
	stringstream ss;
	ss << clipShortestExtension(aFilename) << '.' << aDetectTool << ".dat";
	return ss.str();
}

string original_size_points_filename(const string &aSrcFilename)
{
	return clipShortestExtension(aSrcFilename) + ".100.dat";
}

string match_filename(const string &aSrcFilename0, const string &aSrcFilename1, int aScale, const string &aDetectTool)
{
	stringstream ss;
	ss << clipShortestExtension(aSrcFilename0) << '_' << clipShortestExtension(aSrcFilename1) << '.' << setw(3) << setfill('0') << aScale << '.' << aDetectTool << ".result";
	return ss.str();
}

string drawn_matches_filename(const string aMatchFilename, const string &aImageFilename)
{
	return clipShortestExtension(aMatchFilename) + "." + clipShortestExtension(aImageFilename) + ".tif";
}

void scale_image(const string &aSrcFilename, int aScale, const string &aDstFilename)
{
	stringstream ss;
	ss << g_externalToolHandler.get("convert").callName() << " " << aSrcFilename << " -resize " << aScale << "% " << aDstFilename;
	ElSystem(ss.str());

	if ( !ELISE_fp::exist_file(aDstFilename)) ELISE_ERROR_EXIT("failed to create scaled image [" << aDstFilename << "]");
	cout << '\t' << "-- scaled image [" << aDstFilename << "] created" << endl;
}

void detect_points(const string &aSrcFilename, const string &aDetectTool, const string &aDetectToolOptions, const string &aDstFilename)
{
	stringstream ss;
	ss << MM3dBinFile(aDetectTool) << aDetectToolOptions << ' ' << aSrcFilename << " -o " << aDstFilename;
	ElSystem(ss.str());

	if ( !ELISE_fp::exist_file(aDstFilename)) ELISE_ERROR_EXIT("failed to create points file [" << aDstFilename << "]");
	cout << '\t' << "-- points file [" << aDstFilename << "] created" << endl;
}

void toOriginalSize(const string &aSrcFilename, double aScale, const string &aDstFilename)
{
	vector<DigeoPoint> points;
	if ( !DigeoPoint::readDigeoFile(aSrcFilename, true, points)) ELISE_ERROR_EXIT("failed to read digeo file [" << aSrcFilename << "]"); // true = aStoreMultipleAngles

	DigeoPoint *it = points.data();
	size_t i = points.size();
	while (i--)
	{
		it->x *= aScale;
		(*it++).y *= aScale;
	}

	if ( !DigeoPoint::writeDigeoFile(aDstFilename, points)) ELISE_ERROR_EXIT("failed to write digeo file [" << aDstFilename << "]");
	cout << '\t' << "-- digeo file [" << aDstFilename << "] created" << endl;
}

string drawMatches_process_image(const string &aFilename, int aScale, const string &aDetectTool, const string &aDetectToolOptions)
{
	string scaledFilename = scaled_filename(aFilename, aScale);
	scale_image(aFilename, aScale, scaledFilename);

	string pointsFilename = points_filename(scaledFilename, aDetectTool);
	detect_points(scaledFilename, aDetectTool, aDetectToolOptions, pointsFilename);

	double scaleToOriginal = 100./double(aScale);
	string originalSizePointsFilename = pointsFilename;
	if (aScale != 100)
	{
		originalSizePointsFilename = original_size_points_filename(pointsFilename);
		toOriginalSize(pointsFilename, scaleToOriginal, originalSizePointsFilename);
	}

	return originalSizePointsFilename;
}

void match_points(const string &aSrcFilename0, const string &aSrcFilename1, const string &aDstFilename)
{
	stringstream ss;
	ss << MM3dBinFile("Ann") << aSrcFilename0 << ' ' << aSrcFilename1 << ' ' << aDstFilename;
	ElSystem(ss.str());

	if ( !ELISE_fp::exist_file(aDstFilename)) ELISE_ERROR_EXIT("failed to create matches file [" << aDstFilename << "]");
	cout << '\t' << "-- matches file [" << aDstFilename << "] created" << endl;
}

void drawDetectedPoints(const string &aFilename, Im2D_U_INT1 &aImage)
{
	vector<DigeoPoint> points;
	if ( !DigeoPoint::readDigeoFile(aFilename, true, points)) ELISE_ERROR_EXIT("failed to read digeo file [" << aFilename << "]"); // true = aStoreMultipleAngles

	cout << '\t' << "-- [" << aFilename << "] read: " << points.size() << " points" << endl;

	DigeoPoint *it = points.data();
	size_t i = points.size();
	while (i--)
	{
		drawPoint(aImage, Pt2dr(it->x, it->y), 255, 5);
		it++;
	}
}

void readMatches(const string &aFilename, vector<Match> &oMatches)
{
	if ( !cElFilename(aFilename).exists()) ELISE_ERROR_EXIT("matches file [" << aFilename << "] does not exist");

	bool isBinary = is_binary_matches(aFilename), readOK;
	readOK = isBinary ? read_binary_matches(aFilename, oMatches) : read_ascii_matches(aFilename, oMatches);

	if ( !readOK)  ELISE_ERROR_EXIT("failed to read matches file [" << aFilename << "] ");

	cout << '\t' << "-- [" << aFilename << "] read: " << oMatches.size() << " matches (" << (isBinary ? "binary" : "ASCII") << ")" << endl;
}

void drawMatches(const string &aFilename, Im2D_U_INT1 &aImage0, Im2D_U_INT1 &aImage1)
{
	vector<Match> matches;
	readMatches(aFilename, matches);

	const Match *it = matches.data();
	size_t i = matches.size();
	while (i--)
	{
		drawPoint(aImage0, it->mP0, 255, 5);
		drawPoint(aImage1, it->mP1, 255, 5);
		it++;
	}
}

void drawPoints(const string &aImageFilename0, const string &aPointsFilename0, const string &aImageFilename1, const string &aPointsFilename1, const string &aMatchFilename)
{
	Im2D_U_INT1 red0, green0, blue0;
	getGrayImages(aImageFilename0, red0, green0, blue0);
	drawDetectedPoints(aPointsFilename0, blue0);

	Im2D_U_INT1 red1, green1, blue1;
	getGrayImages(aImageFilename1, red1, green1, blue1);
	drawDetectedPoints(aPointsFilename1, blue1);

	drawMatches(aMatchFilename, green0, green1);

	string dstImageFilename0 = drawn_matches_filename(aMatchFilename, aImageFilename0);
	save_tiff(dstImageFilename0, red0, green0, blue0);

	string dstImageFilename1 = drawn_matches_filename(aMatchFilename, aImageFilename1);
	save_tiff(dstImageFilename1, red1, green1, blue1);
}

void drawMatches_process_scale(const string &aFilename0, const string &aFilename1, int aScale, const string &aDetectTool, const string &aDetectToolOptions)
{
	ELISE_DEBUG_ERROR(aScale <= 0, "process_scale", "invalid scale " << aScale);
	ELISE_DEBUG_ERROR(aDetectTool.empty(), "process_scale", "invalid detecting tool [" << aDetectTool << "]");

	cout << "--- scale " << aScale << endl;

	string pointsFilename0 = drawMatches_process_image(aFilename0, aScale, aDetectTool, aDetectToolOptions);
	string pointsFilename1 = drawMatches_process_image(aFilename1, aScale, aDetectTool, aDetectToolOptions);

	string matchFilename = match_filename(aFilename0, aFilename1, aScale, aDetectTool);
	match_points(pointsFilename0, pointsFilename1, matchFilename);

	drawPoints(aFilename0, pointsFilename0, aFilename1, pointsFilename1, matchFilename);
}

#include "../src/uti_image/Digeo/Digeo.h"

void convolute()
{
	const string filename = "61.030.tif";

	REAL sigma = 3.2;
	int nbShift = 15;
	double epsilon = 0.001;
	int surEch = 10;
	ConvolutionKernel1D<INT> kernel;
	integralGaussianKernel<INT>(sigma, nbShift, epsilon, surEch, kernel);
	const vector<INT> &c = kernel.coefficients();

	cout << "kernel.size() = " << kernel.size() << endl;

	for (size_t i = 0; i < kernel.size(); i++)
		cout << c[i] << ' ';
	cout << endl;

	ConvolutionHandler<U_INT2> convolutionHandler;
	cConvolSpec<U_INT2> *convolution1d = convolutionHandler.getConvolution(kernel);

	cout << "convolution is " << ( !convolution1d->IsCompiled() ? "not " : "") << "compiled" << endl;
 

	Tiff_Im tiff(filename.c_str());
	cout << "[" << filename << "]: " << tiff.sz() << 'x' << tiff.nb_chan() << ' ' << eToString(tiff.type_el()) << endl;
	Im2DGen src_gen = tiff.ReadIm();
	Im2D_U_INT2 image0;
	if (tiff.type_el() != GenIm::u_int1) ELISE_ERROR_EXIT("bad type");
	{
		U_INT1 *src = ((Im2D_U_INT1 *) &src_gen)->data_lin();

		image0.Resize(tiff.sz());

		cout << "src_gen.sz() = " << src_gen.sz() << " image0.sz() = " << image0.sz() << endl;

		U_INT2 *dst = image0.data_lin();
		size_t i = size_t(image0.tx()) * size_t(image0.ty());
		while (i--) *dst++ = (U_INT2)(*src++) * 257;
	}

	Im2D_U_INT2 image1(image0.tx(), image0.ty());
	Im2D_U_INT2 *src = &image0, *dst = &image1;
	int nbConvol = 10;
	while (nbConvol--)
	{
		convolution<U_INT2>((const U_INT2 **)src->data(), src->tx(), src->ty(), *convolution1d, dst->data());
		ElSwap<Im2D_U_INT2 *>(src, dst);
	}

	Im2D_U_INT1 imageToWrite(src->tx(), src->ty());
	{
		U_INT2 *itSrc = src->data_lin();
		U_INT1 *itDst = imageToWrite.data_lin();
		size_t i = size_t(src->tx()) * size_t(src->ty());
		while (i--) *itDst++ = (U_INT1)((*itSrc++) / 257);
	}
	ELISE_COPY
	(
		imageToWrite.all_pts(),
		imageToWrite.in(),
		Tiff_Im(
			"toto.tif",
			imageToWrite.sz(),
			GenIm::u_int1,
			Tiff_Im::No_Compr,
			Tiff_Im::BlackIsZero,
			Tiff_Im::Empty_ARG ).out()
	);
}

int command_drawMatches(int argc, char **argv)
{
	//~ convolute();
	//~ return 0;

	if (argc < 2) ELISE_ERROR_RETURN("usage: match_filename image_filename0 image_filename1");

	//~ const string detectTool = "Sift";
	//~ const string detectToolOptions = "-f0";
	const string detectTool = "Digeo";
	string detectToolOptions;
	if ( !detectToolOptions.empty() && detectToolOptions[0] != ' ') detectToolOptions = string(" ") + detectToolOptions;
	cout << "--- detecting tool: " << '[' << detectTool << detectToolOptions << ']' << endl;

	string imageFilename0 = argv[0], imageFilename1 = argv[1];
	if ( !cElFilename(imageFilename0).exists()) ELISE_ERROR_RETURN("image file [" << imageFilename0 << "] does not exist");
	if ( !cElFilename(imageFilename1).exists()) ELISE_ERROR_RETURN("image file [" << imageFilename1 << "] does not exist");
	cout << "--- images: [" << imageFilename0 << "] [" << imageFilename1 << "]" << endl;

	//~ if ( !cElFilename(matchesFilename).exists()) ELISE_ERROR_RETURN("matches' file [" << matchesFilename << "] does not exist");
	//~ bool isBinary = is_binary_matches(matchesFilename), readOK;
	//~ vector<Match> matches;
	//~ readOK = isBinary ? read_binary_matches(matchesFilename, matches) : read_ascii_matches(matchesFilename, matches);
	//~ if ( !readOK)  ELISE_ERROR_RETURN("failed to read matches file [" << matchesFilename << "] ");
	//~ cout << "--- [" << matchesFilename << "]: " << matches.size() << " matches (" << (isBinary ? "binary" : "ASCII") << ")" << endl;

	//~ for (size_t i = 0; i < matches.size(); i++)
		//~ cout << i << ": " << matches[i] << endl;

	//~ Im2D_U_INT1 gray0, green0;
	//~ getGrayImages(imageFilename0, gray0, green0);
//~ 
	//~ Im2D_U_INT1 gray1, green1;
	//~ getGrayImages(imageFilename1, gray1, green1);

	const int pace = 10;
	// __DEL
	//~ for (double scale = 100; scale >= pace; scale -= pace)
	for (double scale = 0; scale <= 100; scale += pace)
		drawMatches_process_scale(imageFilename0, imageFilename1, scale, detectTool, detectToolOptions);

	//~ const Match *itMatch = matches.data();
	//~ size_t iMatch = matches.size();
	//~ while (iMatch--)
	//~ {
		//~ drawPoint(green0, itMatch->mP0, 255, 5);
		//~ drawPoint(green1, itMatch->mP1, 255, 5);
		//~ itMatch++;
	//~ }
//~ 
	//~ save_tiff(gray0, green0);
	//~ save_tiff(gray1, green1);

	return EXIT_SUCCESS;
}

//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------

int TestJB_main( int argc, char **argv )
{
	string command;

	if (argc > 1)
	{
		command = argv[1];
		for (size_t i = 0; i < command.size(); i++)
			command[i] = tolower(command[i]);
	}

	int res = EXIT_FAILURE;
	command_t *itCommand = commands;
	while (itCommand->func != NULL)
	{
		if (itCommand->name == command)
		{
			res = (*itCommand->func)( argc-2, argv+2 );
			break;
		}
		itCommand++;
	}

	if (itCommand->func == NULL)
	{
		cout << "command [" << command << "] is not valid" << endl;
		cout << "commands are :" << endl;
		itCommand = commands;
		while (itCommand->func != NULL)
		{
			cout << '\t' << itCommand->name << endl;
			itCommand++;
		}
	}

	return res;
}
