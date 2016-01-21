#pragma once
#include <string>
#include <vector>
#include <xsi_application.h>

using namespace std;
using namespace XSI;

class Constants
{
public:
	Constants::Constants(){}
	Constants::~Constants(){}

	static const vector<string> shaderTypeArray;
	static const vector<string> shaderSSSTypeArray;
	static const vector<string> lightTypeArray;
	static const vector<string> textureTypeArray;

	static const CString mLightPoint;
	static const CString mLightSpot;
	static const CString mLightDirectional;
	static const CString mLightAreaSphere;
	static const CString mLightAreaCylinder;
	static const CString mLightAreaRectangle;
	static const CString mLightAreaDisk;
	static const CString mLightAreaCube;
	static const CString mLightConstant;
	static const CString mLightSky;
	static const CString mLightSun;
	static const CString mLightSunSky;
	static const CString mLightEnvironment;

	static const CString mShaderName;
	static const CString mShaderType;
	static const CString mLightType;
	static const CString mSSSDipole;
	static const CString mRoughSSSDipole;
	static const CString mDielectric;	
	static const CString mDiffuse;
	static const CString mRoughdiffuse;
	static const CString mThindielectric;
	static const CString mRoughdielectric;
	static const CString mConductor;
	static const CString mRoughconductor;
	static const CString mPlastic;
	static const CString mRoughplastic;
	static const CString mCoating;
	static const CString mRoughcoating;	
	static const CString mPhong;
	static const CString mMixturebsdf;
	static const CString mBumpmap;
	static const CString mWard;
	static const CString mBlendbsdf;
	static const CString mMask;	
	static const CString mTwoSided;
	static const CString mDifftrans;
	static const CString mHk;

	static const CString mBitmap;
	static const CString mWireframe;
	static const CString mCheckboard;
	static const CString mGridTexture;
	static const CString mScale;
	static const CString mVertexColors;
	static const CString mCurvature;


	static const CString typeDielectric;
	static const CString typeDiffuse;
	static const CString typeRoughdiffuse;
	static const CString typeThindielectric;
	static const CString typeRoughdielectric;
	static const CString typeConductor;
	static const CString typeRoughconductor;
	static const CString typePlastic;
	static const CString typeRoughplastic;
	static const CString typeCoating;
	static const CString typeRoughcoating;
	static const CString typePhong;
	static const CString typeMixturebsdf;
	static const CString typeBumpmap;
	static const CString typeWard;
	static const CString typeBlendbsdf;
	static const CString typeMask;
	static const CString typeTwosided;
	static const CString typeDifftrans;
	static const CString typeHk;
	static const CString typeBitmap;
	static const CString typeWireframe;
	static const CString typeCheckerboard;
	static const CString typeGridTexture;
	static const CString typeScale;
	static const CString typeVertexColors;
	static const CString typeCurvature;


	static const CString paramOpacity;
	static const CString paramThickness;
	static const CString paramDistribution;
	static const CString paramSourceFileName;
	static const CString paramReflectance;
	static const CString paramAlpha;
	static const CString paramAlphaU;
	static const CString paramAlphaV;
	static const CString paramAs;
	static const CString paramUseFastApprox;
	static const CString paramIntIOR;
	static const CString paramExtIOR;
	static const CString paramSpecularReflectance;
	static const CString paramSpecularTransmittance;
	static const CString paramMaterial;
	static const CString paramExtEta;
	static const CString paramDiffuseReflectance;
	static const CString paramNonlinear;
	static const CString paramSigmaA;
	static const CString paramBsdf;
	static const CString paramTexture;
	static const CString paramTex;
	static const CString paramType;
	static const CString paramScale;
	static const CString paramExponent;
	static const CString paramVariant;
	static const CString paramWeights;
	static const CString paramWeight;
	static const CString paramBsdf1;
	static const CString paramBsdf2;
	static const CString paramTransmittance;
	static const CString paramWrapModeU;
	static const CString paramWrapModeV;
	static const CString paramFilterType;
	static const CString paramMaxAnisotropy;
	static const CString paramCache;
	static const CString paramUoffset;
	static const CString paramVoffset;
	static const CString paramUscale;
	static const CString paramVscale;
	static const CString paramCheckerboard;
	static const CString paramGridtexture;
	static const CString paramLineWidth;
	static const CString paramStepWidth;
	static const CString paramColor0;
	static const CString paramColor1;
	static const CString paramVertexcolors;
	static const CString paramInteriorColor;
	static const CString paramEdgeColor;
	static const CString paramCurvature;
	static const CString paramValue;
	static const CString paramInput;
	static const CString paramSSSRoughsssdipole;
	static const CString paramSSSSssdipole;
	static const CString paramIrrSamples;
	static const CString paramPlastic;
	static const CString paramRoughPlastic;

	static const CString paramCameraIrradiancemeter;
	static const CString paramCameraPerspective;
	static const CString paramCameraThinlens;
	static const CString paramCameraPerspective_rdist;
	static const CString paramCameraOrthographic;
	static const CString paramCameraTelecentric;

	static const char* attrTexture;
	static const char* attrType;
	static const char* attrName;
	static const char* attrValue;
	static const char* attrFilename;
	static const char* attrString;
	static const char* attrFloat;
	static const char* attrBoolean;
	static const char* attrInteger;
	static const char* attrSensor;
	static const char* attrFilm;
	static const char* attrHdrFilm;
	static const char* attrWidth;
	static const char* attrHeight;
	static const char* attrSpectrum;
	static const char* attrBsdf;
	static const char* attrDiffuse;
	static const char* attrVariant;
	static const char* attrMaterial;
	static const char* attrDistribution;
	static const char* attrIntIOR;
	static const char* attrExtIOR;
	static const char* attrSubsurface;
	static const char* attrDipole;
	static const char* attrTarget;
	static const char* attrOrigin;
	static const char* attrCutoffAngle;

	static bool NameInList(const vector<string> arrayNames, string sname);	
	static bool NameInList(const CRefArray attributeNames, CString sname);
};
