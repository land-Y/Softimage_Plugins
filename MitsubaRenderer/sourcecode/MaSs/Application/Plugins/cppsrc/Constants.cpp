#include "constants.h"

const vector<string> Constants::shaderTypeArray = { "mDiffuse", "mRoughDiffuse", "mDielectric", "mThinDielectric", "mRoughDielectric", "mConductor", "mRoughConductor",
"mPlastic", "mRoughPlastic", "mCoating", "mRoughCoating", "mBumpMap", "mPhong", "mWard", "mMixtureBsdf", "mBlendBsdf", "mMask", "mTwoSided",
"mDiffTrans", "mHk" };

const vector<string> Constants::shaderSSSTypeArray = { "mSSSDipole", "mRoughSSSDipole"};


const vector<string> Constants::lightTypeArray = { "mPoint", "mSpot", "mDirectional", "mAreaSphere", "mAreaCylinder", "mAreaRectangle", "mAreaDisk",
"mAreaCube", "mConstant", "mSky", "mSun", "mSunSky", "mEnvironment" };

const vector<string> Constants::textureTypeArray = { "mBitmap", "mCheckboard", "mGridTexture", "mScale", "mVertexColors", "mWireframe", "mCurvature" };

const CString Constants::mLightType = L"mLightType";
const CString Constants::mLightPoint = L"mPoint";
const CString Constants::mLightSpot = L"mSpot";
const CString Constants::mLightDirectional = L"mDirectional";
const CString Constants::mLightAreaSphere = L"mAreaSphere";
const CString Constants::mLightAreaCylinder = L"mAreaCylinder";
const CString Constants::mLightAreaRectangle = L"mAreaRectangle";
const CString Constants::mLightAreaDisk = L"mAreaDisk";
const CString Constants::mLightAreaCube = L"mAreaCube";
const CString Constants::mLightConstant = L"mConstant";
const CString Constants::mLightSky = L"mSky";
const CString Constants::mLightSun = L"mSun";
const CString Constants::mLightSunSky = L"mSunSky";
const CString Constants::mLightEnvironment = L"mEnvironment";

const CString Constants::mShaderName = L"mShaderName";
const CString Constants::mShaderType = L"mShaderType";

const CString Constants::mSSSDipole = L"mSSSDipole";
const CString Constants::mRoughSSSDipole = L"mRoughSSSDipole";
const CString Constants::mDielectric = L"mDielectric";
const CString Constants::mDiffuse = L"mDiffuse";
const CString Constants::mRoughdiffuse = L"mRoughDiffuse";
const CString Constants::mThindielectric = L"mThinDielectric";
const CString Constants::mRoughdielectric = L"mRoughDielectric";
const CString Constants::mConductor = L"mConductor";
const CString Constants::mRoughconductor = L"mRoughConductor";
const CString Constants::mPlastic = L"mPlastic";
const CString Constants::mRoughplastic = L"mRoughPlastic";
const CString Constants::mCoating = L"mCoating";
const CString Constants::mRoughcoating = L"mRoughCoating";
const CString Constants::mPhong = L"mPhong";
const CString Constants::mMixturebsdf = L"mMixtureBsdf";
const CString Constants::mBumpmap = L"mBumpMap";
const CString Constants::mWard = L"mWard";
const CString Constants::mBlendbsdf = L"mBlendBsdf";
const CString Constants::mMask = L"mMask";
const CString Constants::mTwoSided = L"mTwoSided";
const CString Constants::mDifftrans = L"mDiffTrans";
const CString Constants::mHk = L"mHk";

const CString Constants::mBitmap = L"mBitMap";
const CString Constants::mWireframe = L"mWireframe";
const CString Constants::mCheckboard = L"mCheckboard";
const CString Constants::mGridTexture = L"mGridTexture";
const CString Constants::mScale = L"mScale";
const CString Constants::mVertexColors = L"mVertexColors";
const CString Constants::mCurvature = L"mCurvature";

const CString Constants::typeDielectric = L"dielectric";
const CString Constants::typeDiffuse = L"diffuse";
const CString Constants::typeRoughdiffuse = L"roughdiffuse";
const CString Constants::typeThindielectric = L"thindielectric";
const CString Constants::typeRoughdielectric = L"roughdielectric";
const CString Constants::typeConductor = L"conductor";
const CString Constants::typeRoughconductor = L"roughconductor";
const CString Constants::typePlastic = L"plastic";
const CString Constants::typeRoughplastic = L"roughplastic";
const CString Constants::typeCoating = L"coating";
const CString Constants::typeRoughcoating = L"roughcoating";
const CString Constants::typePhong = L"phong";
const CString Constants::typeMixturebsdf = L"mixturebsdf";
const CString Constants::typeBumpmap = L"bumpmap";
const CString Constants::typeWard = L"ward";
const CString Constants::typeBlendbsdf = L"blendbsdf";
const CString Constants::typeMask = L"mask";
const CString Constants::typeTwosided = L"twosided";
const CString Constants::typeDifftrans = L"difftrans";
const CString Constants::typeHk = L"hk";
const CString Constants::typeBitmap = L"bitmap";
const CString Constants::typeWireframe = L"wireframe";
const CString Constants::typeCheckerboard = L"checkerboard";
const CString Constants::typeGridTexture = L"gridtexture";
const CString Constants::typeScale = L"scale";
const CString Constants::typeVertexColors = L"vertexcolors";
const CString Constants::typeCurvature = L"curvature";


const CString Constants::paramOpacity = L"opacity";
const CString Constants::paramThickness = L"thickness";
const CString Constants::paramDistribution = L"distribution";
const CString Constants::paramTransmittance = L"transmittance";
const CString Constants::paramSourceFileName = L"SourceFileName";
const CString Constants::paramReflectance = L"reflectance";
const CString Constants::paramAlpha = L"alpha";
const CString Constants::paramAlphaU = L"alphaU";
const CString Constants::paramAlphaV = L"alphaV";
const CString Constants::paramAs = L"as";
const CString Constants::paramUseFastApprox = L"useFastApprox";
const CString Constants::paramIntIOR = L"intIOR";
const CString Constants::paramExtIOR = L"extIOR";
const CString Constants::paramSpecularReflectance = L"specularReflectance";
const CString Constants::paramSpecularTransmittance = L"specularTransmittance";
const CString Constants::paramMaterial = L"material";
const CString Constants::paramExtEta = L"extEta";
const CString Constants::paramDiffuseReflectance = L"diffuseReflectance";
const CString Constants::paramNonlinear = L"nonlinear";
const CString Constants::paramSigmaA = L"sigmaA";
const CString Constants::paramBsdf = L"bsdf";
const CString Constants::paramTexture = L"texture";
const CString Constants::paramTex = L"tex";
const CString Constants::paramType = L"type";
const CString Constants::paramScale = L"scale";
const CString Constants::paramExponent = L"exponent";
const CString Constants::paramVariant = L"variant";
const CString Constants::paramWeights = L"weights";
const CString Constants::paramWeight = L"weight";
const CString Constants::paramBsdf1 = L"bsdf1";
const CString Constants::paramBsdf2 = L"bsdf2";
const CString Constants::paramWrapModeU = L"wrapModeU";
const CString Constants::paramWrapModeV = L"wrapModeV";
const CString Constants::paramFilterType = L"filterType";
const CString Constants::paramMaxAnisotropy = L"maxAnisotropy";
const CString Constants::paramCache = L"cache";
const CString Constants::paramUoffset = L"uoffset";
const CString Constants::paramVoffset = L"voffset";
const CString Constants::paramUscale = L"uscale";
const CString Constants::paramVscale = L"vscale";
const CString Constants::paramCheckerboard = L"checkerboard";
const CString Constants::paramGridtexture = L"gridtexture";
const CString Constants::paramLineWidth = L"lineWidth";
const CString Constants::paramStepWidth = L"stepWidth";
const CString Constants::paramColor0 = L"color0";
const CString Constants::paramColor1 = L"color1";
const CString Constants::paramVertexcolors = L"vertexcolors";
const CString Constants::paramInteriorColor = L"interiorColor";
const CString Constants::paramEdgeColor = L"edgeColor";
const CString Constants::paramCurvature = L"curvature";
const CString Constants::paramValue = L"value";
const CString Constants::paramInput = L"input";
const CString Constants::paramSSSRoughsssdipole = L"roughsssdipole";
const CString Constants::paramSSSSssdipole = L"sssdipole";
const CString Constants::paramIrrSamples = L"irrSamples";
const CString Constants::paramPlastic = L"plastic";
const CString Constants::paramRoughPlastic = L"roughplastic";

const CString Constants::paramCameraIrradiancemeter = L"irradiancemeter";
const CString Constants::paramCameraPerspective = L"perspective";
const CString Constants::paramCameraThinlens = L"thinlens";
const CString Constants::paramCameraPerspective_rdist = L"perspective_rdist";
const CString Constants::paramCameraOrthographic = L"orthographic";
const CString Constants::paramCameraTelecentric = L"telecentric";

const char* Constants::attrTexture = "texture";
const char* Constants::attrType = "type";
const char* Constants::attrName = "name";
const char* Constants::attrValue = "value";
const char* Constants::attrTarget = "target";
const char* Constants::attrOrigin = "origin";
const char* Constants::attrFilename = "filename";
const char* Constants::attrString = "string";
const char* Constants::attrFloat = "float";
const char* Constants::attrBoolean = "boolean";
const char* Constants::attrInteger = "integer";
const char* Constants::attrSensor = "sensor";
const char* Constants::attrFilm = "film";
const char* Constants::attrHdrFilm = "hdrfilm";
const char* Constants::attrWidth = "width";
const char* Constants::attrHeight = "height";
const char* Constants::attrSpectrum = "spectrum";
const char* Constants::attrBsdf = "bsdf";
const char* Constants::attrDiffuse = "diffuse";
const char* Constants::attrVariant = "variant";
const char* Constants::attrMaterial = "material";
const char* Constants::attrDistribution = "distribution";
const char* Constants::attrIntIOR = "intIOR";
const char* Constants::attrExtIOR = "extIOR";
const char* Constants::attrSubsurface = "subsurface";
const char* Constants::attrDipole = "dipole";
const char* Constants::attrCutoffAngle = "cutoffAngle";


bool Constants::NameInList(const vector<string> arrayNames, string sname)
{	
	return find(arrayNames.begin(), arrayNames.end(), sname) != arrayNames.end();
}

bool Constants::NameInList(const CRefArray attributeNames, CString sname)
{
	for (int i = 0; i < attributeNames.GetCount(); i++)
	{
		if (attributeNames.GetItem(i).GetAsText() == sname)
		{
			return true;
		}
	}
	return false;
}