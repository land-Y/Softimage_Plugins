#include "Light.h"

namespace Mitsuba
{
	Light::Light(){}
	Light::~Light(){}

	void Light::WriteLights(XMLDocument* doc, XMLElement* sceneElement, XSI::Light& light)
	{
		if (isVisibility(light) && isLightCorrect(light))
		{
			sceneElement->InsertEndChild(GenerateLightSegment(doc, light));
		}
	}



	XMLElement* Light::GenerateLightSegment(XMLDocument* doc, XSI::Light& light)
	{
		XMLElement* root = nullptr;
		string lightname = light.GetName().GetAsciiString();

		MATH::CTransformation lightTransform = light.GetKinematics().GetGlobal().GetTransform();

		Shader shader(light.GetShaders().GetItem(0));
		CString lightType = shader.GetParameterValue(Constants::mLightType);
		if (lightType == Constants::mLightAreaSphere)
		{
			root = doc->NewElement("shape");
			root->SetAttribute(Constants::attrType, "sphere");

			root->InsertEndChild(WriteElementPosition(doc, "point", "center", light));

			root->InsertEndChild(WriteElement(doc, Constants::attrFloat, "radius", shader.GetParameterValue(L"radius").GetAsText().GetAsciiString()));

			root->InsertEndChild(WriteEmitter(doc, "area", shader));
		}
		else if (lightType == Constants::mLightAreaCube)
		{
			root = doc->NewElement("shape");
			root->SetAttribute(Constants::attrType, "cube");

			XMLElement* transformElement = doc->NewElement("transform");
			transformElement->SetAttribute(Constants::attrName, "toWorld");

			XMLElement* scaleElement = doc->NewElement("scale");
			float lenx = shader.GetParameterValue(L"lengthX");
			scaleElement->SetAttribute("x", lenx / 2);

			float leny = shader.GetParameterValue(L"lengthY");
			scaleElement->SetAttribute("y", leny / 2);

			float lenz = shader.GetParameterValue(L"lengthZ");
			scaleElement->SetAttribute("z", lenz / 2);
			transformElement->InsertEndChild(scaleElement);

			transformElement->InsertEndChild(WriteSubElementRotation(doc, "x", lightTransform.GetRotX()));
			transformElement->InsertEndChild(WriteSubElementRotation(doc, "y", lightTransform.GetRotY()));
			transformElement->InsertEndChild(WriteSubElementRotation(doc, "z", lightTransform.GetRotZ()));
			transformElement->InsertEndChild(WriteElementTranslate(doc, light));

			root->InsertEndChild(transformElement);

			root->InsertEndChild(WriteEmitter(doc, "area", shader));

		}
		else if (lightType == Constants::mLightAreaCylinder)
		{
			root = doc->NewElement("shape");
			root->SetAttribute(Constants::attrType, "cylinder");

			XSI::Shader shader = light.GetShaders().GetItem(0);
			root->InsertEndChild(WriteElement(doc, Constants::attrFloat, "radius", shader.GetParameterValue(L"radius").GetAsText().GetAsciiString()));

			//TODO
			//write p0 and p1
			//


			root->InsertEndChild(WriteEmitter(doc, "area", shader));

		}
		else if (lightType == Constants::mLightAreaRectangle)
		{
			root = doc->NewElement("shape");
			root->SetAttribute(Constants::attrType, "rectangle");

			XMLElement* transformElement = doc->NewElement("transform");
			transformElement->SetAttribute(Constants::attrName, "toWorld");

			XMLElement* scaleElement = doc->NewElement("scale");
			float lenx = shader.GetParameterValue(L"lengthX");
			scaleElement->SetAttribute("x", lenx / 2);

			float leny = shader.GetParameterValue(L"lengthY");
			scaleElement->SetAttribute("y", leny / 2);

			transformElement->InsertEndChild(scaleElement);

			transformElement->InsertEndChild(WriteSubElementRotation(doc, "x", lightTransform.GetRotX()));
			transformElement->InsertEndChild(WriteSubElementRotation(doc, "y", lightTransform.GetRotY()));
			transformElement->InsertEndChild(WriteSubElementRotation(doc, "z", lightTransform.GetRotZ()));
			transformElement->InsertEndChild(WriteElementTranslate(doc, light));

			root->InsertEndChild(transformElement);

			root->InsertEndChild(WriteEmitter(doc, "area", shader));
		}
		else if (lightType == Constants::mLightAreaDisk)
		{
			root = doc->NewElement("shape");
			root->SetAttribute(Constants::attrType, "disk");

			XMLElement* transformElement = doc->NewElement("transform");
			transformElement->SetAttribute(Constants::attrName, "toWorld");

			XMLElement* scaleElement = doc->NewElement("scale");
			scaleElement->SetAttribute(Constants::attrValue, shader.GetParameterValue(L"radius").GetAsText().GetAsciiString());
			transformElement->InsertEndChild(scaleElement);

			transformElement->InsertEndChild(WriteSubElementRotation(doc, "x", lightTransform.GetRotX()));
			transformElement->InsertEndChild(WriteSubElementRotation(doc, "y", lightTransform.GetRotY()));
			transformElement->InsertEndChild(WriteSubElementRotation(doc, "z", lightTransform.GetRotZ()));
			transformElement->InsertEndChild(WriteElementTranslate(doc, light));
			root->InsertEndChild(transformElement);

			root->InsertEndChild(WriteEmitter(doc, "area", shader));
		}
		else if (lightType == Constants::mLightPoint)
		{
			root = WriteEmitter(doc, "point", shader);

			root->InsertEndChild(WriteElementPosition(doc, "point", "position", light));

		}
		else if (lightType == Constants::mLightSpot)
		{
			root = doc->NewElement("emitter");
			root->SetAttribute(Constants::attrType, "spot");

			XMLElement* transElement = doc->NewElement("transform");
			transElement->SetAttribute(Constants::attrName, "toWorld");

			XMLElement* lookatElement = doc->NewElement("lookat");
			CString svalue = CString(lightTransform.GetPosX()) + L", " + CString(lightTransform.GetPosY()) + L", " + CString(lightTransform.GetPosZ());
			lookatElement->SetAttribute(Constants::attrOrigin, svalue.GetAsciiString());

			X3DObject oInterest = light.GetInterest();
			MATH::CTransformation interestTransform = oInterest.GetKinematics().GetGlobal().GetTransform();
			CString svalue2 = CString(interestTransform.GetPosX()) + L", " + CString(interestTransform.GetPosY()) + L", " + CString(interestTransform.GetPosZ());
			lookatElement->SetAttribute(Constants::attrTarget, svalue2.GetAsciiString());

			transElement->InsertEndChild(lookatElement);
			root->InsertEndChild(transElement);


			root->InsertEndChild(WriteSpectrumElement(doc, "intensity", "intensity", shader));

			root->InsertEndChild(WriteElement(doc, Constants::attrFloat, Constants::attrCutoffAngle, float(light.GetParameterValue(L"LightCone"))));
			root->InsertEndChild(WriteElement(doc, Constants::attrFloat, "beamWidth", float(shader.GetParameterValue(L"beamWidth"))));

			//TODO
			//write texture segment
			//

		}
		else if (lightType == Constants::mLightDirectional)
		{
			root = doc->NewElement("emitter");
			root->SetAttribute(Constants::attrType, "directional");

			XMLElement* transElement = doc->NewElement("transform");
			transElement->SetAttribute(Constants::attrName, "toWorld");

			XMLElement* lookatElement = doc->NewElement("lookat");
			CString svalue = CString(lightTransform.GetPosX()) + L", " + CString(lightTransform.GetPosY()) + L", " + CString(lightTransform.GetPosZ());
			lookatElement->SetAttribute(Constants::attrOrigin, svalue.GetAsciiString());

			X3DObject oInterest = light.GetInterest();
			MATH::CTransformation interestTransform = oInterest.GetKinematics().GetGlobal().GetTransform();
			CString svalue2 = CString(interestTransform.GetPosX()) + L", " + CString(interestTransform.GetPosY()) + L", " + CString(interestTransform.GetPosZ());
			lookatElement->SetAttribute(Constants::attrTarget, svalue2.GetAsciiString());

			transElement->InsertEndChild(lookatElement);
			root->InsertEndChild(transElement);

			root->InsertEndChild(WriteSpectrumElement(doc, "irradiance", "intensity", shader));

		}
		else if (lightType == Constants::mLightConstant)
		{
			root = doc->NewElement("emitter");
			root->SetAttribute(Constants::attrType, "constant");

			root->InsertEndChild(WriteSpectrumElement(doc, "radiance", "intensity", shader));
		}
		else if (lightType == Constants::mLightSky)
		{
			root = doc->NewElement("emitter");
			root->SetAttribute(Constants::attrType, "sky");

			root->InsertEndChild(WriteElement(doc, Constants::attrFloat, "turbidity", shader.GetParameterValue(L"turbidity").GetAsText().GetAsciiString()));
			root->InsertEndChild(WriteSpectrumElement(doc, "albedo", "albedo", shader));

			//sunModulo = ap.GetValue(light.Name + ".light.mSky.sunDirection.x") * ap.GetValue(light.Name + ".light.mSky.sunDirection.x")
			//+ ap.GetValue(light.Name + ".light.mSky.sunDirection.y") * ap.GetValue(light.Name + ".light.mSky.sunDirection.y")
			//+ ap.GetValue(light.Name + ".light.mSky.sunDirection.z") * ap.GetValue(light.Name + ".light.mSky.sunDirection.z")

			Parameter sunDirParam = shader.GetParameter(L"sunDirection");
			float dirX = sunDirParam.GetParameterValue(L"x");
			float dirY = sunDirParam.GetParameterValue(L"y");
			float dirZ = sunDirParam.GetParameterValue(L"z");

			float sunModulo = dirX*dirX + dirY*dirY + dirZ*dirZ;
			if (sunModulo == 0)
			{
				root->InsertEndChild(WriteElement(doc, Constants::attrInteger, "year", shader.GetParameterValue(L"year").GetAsText().GetAsciiString()));
				root->InsertEndChild(WriteElement(doc, Constants::attrInteger, "month", shader.GetParameterValue(L"month").GetAsText().GetAsciiString()));
				root->InsertEndChild(WriteElement(doc, Constants::attrInteger, "day", shader.GetParameterValue(L"day").GetAsText().GetAsciiString()));

				root->InsertEndChild(WriteElement(doc, Constants::attrFloat, "hour", shader.GetParameterValue(L"hour").GetAsText().GetAsciiString()));
				root->InsertEndChild(WriteElement(doc, Constants::attrFloat, "minute", shader.GetParameterValue(L"minute").GetAsText().GetAsciiString()));
				root->InsertEndChild(WriteElement(doc, Constants::attrFloat, "second", shader.GetParameterValue(L"second").GetAsText().GetAsciiString()));
				root->InsertEndChild(WriteElement(doc, Constants::attrFloat, "latitude", shader.GetParameterValue(L"latitude").GetAsText().GetAsciiString()));
				root->InsertEndChild(WriteElement(doc, Constants::attrFloat, "longitude", shader.GetParameterValue(L"longitude").GetAsText().GetAsciiString()));
				root->InsertEndChild(WriteElement(doc, Constants::attrFloat, "timezone", shader.GetParameterValue(L"timezone").GetAsText().GetAsciiString()));
			}
			else
			{
				XMLElement* sunElement = doc->NewElement("vector");
				sunElement->SetAttribute(Constants::attrName, "sunDirection");
				sunElement->SetAttribute("x", dirX);
				sunElement->SetAttribute("y", dirY);
				sunElement->SetAttribute("z", dirZ);
				root->InsertEndChild(sunElement);
			}

			root->InsertEndChild(WriteElement(doc, Constants::attrFloat, "stretch", shader.GetParameterValue(L"stretch").GetAsText().GetAsciiString()));
			root->InsertEndChild(WriteElement(doc, Constants::attrInteger, "resolution", shader.GetParameterValue(L"resolution").GetAsText().GetAsciiString()));
			root->InsertEndChild(WriteElement(doc, Constants::attrFloat, "scale", shader.GetParameterValue(L"scale").GetAsText().GetAsciiString()));

			XMLElement* transformElement = doc->NewElement("transform");
			transformElement->SetAttribute(Constants::attrName, "toWorld");

			transformElement->InsertEndChild(WriteSubElementRotation(doc, "x", lightTransform.GetRotX()));
			transformElement->InsertEndChild(WriteSubElementRotation(doc, "y", lightTransform.GetRotY()));
			transformElement->InsertEndChild(WriteSubElementRotation(doc, "z", lightTransform.GetRotZ()));
			transformElement->InsertEndChild(WriteElementTranslate(doc, light));

			root->InsertEndChild(transformElement);

		}
		else if (lightType == Constants::mLightSun)
		{
			root = doc->NewElement("emitter");
			root->SetAttribute(Constants::attrType, "sun");

			root->InsertEndChild(WriteElement(doc, Constants::attrFloat, "turbidity", shader.GetParameterValue(L"turbidity").GetAsText().GetAsciiString()));

			Parameter sunDirParam = shader.GetParameter(L"sunDirection");
			float dirX = sunDirParam.GetParameterValue(L"x");
			float dirY = sunDirParam.GetParameterValue(L"y");
			float dirZ = sunDirParam.GetParameterValue(L"z");

			float sunModulo = dirX*dirX + dirY*dirY + dirZ*dirZ;
			if (sunModulo == 0)
			{
				root->InsertEndChild(WriteElement(doc, Constants::attrInteger, "year", shader.GetParameterValue(L"year").GetAsText().GetAsciiString()));
				root->InsertEndChild(WriteElement(doc, Constants::attrInteger, "month", shader.GetParameterValue(L"month").GetAsText().GetAsciiString()));
				root->InsertEndChild(WriteElement(doc, Constants::attrInteger, "day", shader.GetParameterValue(L"day").GetAsText().GetAsciiString()));

				root->InsertEndChild(WriteElement(doc, Constants::attrFloat, "hour", shader.GetParameterValue(L"hour").GetAsText().GetAsciiString()));
				root->InsertEndChild(WriteElement(doc, Constants::attrFloat, "minute", shader.GetParameterValue(L"minute").GetAsText().GetAsciiString()));
				root->InsertEndChild(WriteElement(doc, Constants::attrFloat, "second", shader.GetParameterValue(L"second").GetAsText().GetAsciiString()));
				root->InsertEndChild(WriteElement(doc, Constants::attrFloat, "latitude", shader.GetParameterValue(L"latitude").GetAsText().GetAsciiString()));
				root->InsertEndChild(WriteElement(doc, Constants::attrFloat, "longitude", shader.GetParameterValue(L"longitude").GetAsText().GetAsciiString()));
				root->InsertEndChild(WriteElement(doc, Constants::attrFloat, "timezone", shader.GetParameterValue(L"timezone").GetAsText().GetAsciiString()));
			}
			else
			{
				XMLElement* sunElement = doc->NewElement("vector");
				sunElement->SetAttribute(Constants::attrName, "sunDirection");
				sunElement->SetAttribute("x", dirX);
				sunElement->SetAttribute("y", dirY);
				sunElement->SetAttribute("z", dirZ);
				root->InsertEndChild(sunElement);
			}

			root->InsertEndChild(WriteElement(doc, Constants::attrInteger, "resolution", shader.GetParameterValue(L"resolution").GetAsText().GetAsciiString()));
			root->InsertEndChild(WriteElement(doc, Constants::attrFloat, "scale", shader.GetParameterValue(L"scale").GetAsText().GetAsciiString()));
			root->InsertEndChild(WriteElement(doc, Constants::attrFloat, "sunRadiusScale", shader.GetParameterValue(L"sunRadiusScale").GetAsText().GetAsciiString()));
		}
		else if (lightType == Constants::mLightSunSky)
		{
			root = doc->NewElement("emitter");
			root->SetAttribute(Constants::attrType, "sunsky");

			root->InsertEndChild(WriteElement(doc, Constants::attrFloat, "turbidity", shader.GetParameterValue(L"turbidity").GetAsText().GetAsciiString()));
			root->InsertEndChild(WriteSpectrumElement(doc, "albedo", "albedo", shader));

			Parameter sunDirParam = shader.GetParameter(L"sunDirection");
			float dirX = sunDirParam.GetParameterValue(L"x");
			float dirY = sunDirParam.GetParameterValue(L"y");
			float dirZ = sunDirParam.GetParameterValue(L"z");

			float sunModulo = dirX*dirX + dirY*dirY + dirZ*dirZ;
			if (sunModulo == 0)
			{
				root->InsertEndChild(WriteElement(doc, Constants::attrInteger, "year", shader.GetParameterValue(L"year").GetAsText().GetAsciiString()));
				root->InsertEndChild(WriteElement(doc, Constants::attrInteger, "month", shader.GetParameterValue(L"month").GetAsText().GetAsciiString()));
				root->InsertEndChild(WriteElement(doc, Constants::attrInteger, "day", shader.GetParameterValue(L"day").GetAsText().GetAsciiString()));

				root->InsertEndChild(WriteElement(doc, Constants::attrFloat, "hour", shader.GetParameterValue(L"hour").GetAsText().GetAsciiString()));
				root->InsertEndChild(WriteElement(doc, Constants::attrFloat, "minute", shader.GetParameterValue(L"minute").GetAsText().GetAsciiString()));
				root->InsertEndChild(WriteElement(doc, Constants::attrFloat, "second", shader.GetParameterValue(L"second").GetAsText().GetAsciiString()));
				root->InsertEndChild(WriteElement(doc, Constants::attrFloat, "latitude", shader.GetParameterValue(L"latitude").GetAsText().GetAsciiString()));
				root->InsertEndChild(WriteElement(doc, Constants::attrFloat, "longitude", shader.GetParameterValue(L"longitude").GetAsText().GetAsciiString()));
				root->InsertEndChild(WriteElement(doc, Constants::attrFloat, "timezone", shader.GetParameterValue(L"timezone").GetAsText().GetAsciiString()));
			}
			else
			{
				XMLElement* sunElement = doc->NewElement("vector");
				sunElement->SetAttribute(Constants::attrName, "sunDirection");
				sunElement->SetAttribute("x", dirX);
				sunElement->SetAttribute("y", dirY);
				sunElement->SetAttribute("z", dirZ);
				root->InsertEndChild(sunElement);
			}

			root->InsertEndChild(WriteElement(doc, Constants::attrInteger, "resolution", shader.GetParameterValue(L"resolution").GetAsText().GetAsciiString()));
			root->InsertEndChild(WriteElement(doc, Constants::attrFloat, "stretch", shader.GetParameterValue(L"stretch").GetAsText().GetAsciiString()));
			root->InsertEndChild(WriteElement(doc, Constants::attrFloat, "sunScale", shader.GetParameterValue(L"sunScale").GetAsText().GetAsciiString()));
			root->InsertEndChild(WriteElement(doc, Constants::attrFloat, "skyScale", shader.GetParameterValue(L"skyScale").GetAsText().GetAsciiString()));
			root->InsertEndChild(WriteElement(doc, Constants::attrFloat, "sunRadiusScale", shader.GetParameterValue(L"sunRadiusScale").GetAsText().GetAsciiString()));
		}
		else if (lightType == Constants::mLightEnvironment)
		{
			root = doc->NewElement("emitter");
			root->SetAttribute(Constants::attrType, "envmap");

			XSI::ImageClip2 imageShader = FindImageClipOnPort(shader, L"image");
			CString filename = imageShader.GetParameterValue(Constants::paramSourceFileName);
			root->InsertEndChild(WriteElement(doc, Constants::attrString, "filename", filename.GetAsciiString()));

			root->InsertEndChild(WriteElement(doc, Constants::attrFloat, "scale", shader.GetParameterValue(L"scale").GetAsText().GetAsciiString()));

			XMLElement* transformElement = doc->NewElement("transform");
			transformElement->SetAttribute(Constants::attrName, "toWorld");

			transformElement->InsertEndChild(WriteElementScale(doc, light));
			transformElement->InsertEndChild(WriteSubElementRotation(doc, "x", lightTransform.GetRotX()));
			transformElement->InsertEndChild(WriteSubElementRotation(doc, "y", lightTransform.GetRotY()));
			transformElement->InsertEndChild(WriteSubElementRotation(doc, "z", lightTransform.GetRotZ()));
			transformElement->InsertEndChild(WriteElementTranslate(doc, light));
			root->InsertEndChild(transformElement);

			root->InsertEndChild(WriteElement(doc, Constants::attrFloat, "gamma", shader.GetParameterValue(L"gamma").GetAsText().GetAsciiString()));
			root->InsertEndChild(WriteElement(doc, Constants::attrBoolean, "cache", shader.GetParameterValue(L"isCache").GetAsText().GetAsciiString()));
		}

		return root;
	}

	bool Light::isLightCorrect(XSI::Light& light)
	{
		if (light.GetShaders().GetCount() == 0) return false;

		Shader shader(light.GetShaders().GetItem(0));
		Parameter oParam = shader.GetParameter(Constants::mLightType);
		if (!oParam.IsValid()) return false;

		CString value = oParam.GetValue();
		if (!Constants::NameInList(Constants::lightTypeArray, value.GetAsciiString())) return false;

		if (value == L"mEnvironment")
		{
			Parameter oiParam = shader.GetParameter(L"image");
			if (!oiParam.IsValid()) return false;

			if (oiParam.GetSources().GetCount() == 0)  return false;
		}

		return true;
	}

	XMLElement* Light::WriteEmitter(XMLDocument* doc, const char* typeName, XSI::Shader& shader)
	{
		XMLElement* emitterElement = doc->NewElement("emitter");
		emitterElement->SetAttribute(Constants::attrType, typeName);

		XMLElement* spectrumElement = doc->NewElement("spectrum");
		spectrumElement->SetAttribute(Constants::attrName, "radiance");
		float intens = shader.GetParameterValue(L"intensity");

		Parameter color = shader.GetParameter(L"color");
		float red = color.GetParameterValue(L"red");
		float green = color.GetParameterValue(L"green");
		float blue = color.GetParameterValue(L"blue");

		string svalue = FloatToString(intens*red) + " " + FloatToString(intens*green) + " " + FloatToString(intens* blue);
		spectrumElement->SetAttribute(Constants::attrValue, svalue.c_str());

		emitterElement->InsertEndChild(spectrumElement);

		return emitterElement;
	}

	XMLElement* Light::WriteSpectrumElement(XMLDocument* doc, const char* attrName, const char* paramName, XSI::Shader& shader)
	{
		XMLElement* spectrumElement = doc->NewElement("spectrum");
		spectrumElement->SetAttribute(Constants::attrName, attrName);

		float intens = shader.GetParameterValue(paramName);
		Parameter color = shader.GetParameter(L"color");
		float red = color.GetParameterValue(L"red");
		float green = color.GetParameterValue(L"green");
		float blue = color.GetParameterValue(L"blue");

		string svalue3 = FloatToString(intens*red) + " " + FloatToString(intens*green) + " " + FloatToString(intens* blue);
		spectrumElement->SetAttribute(Constants::attrValue, svalue3.c_str());

		return spectrumElement;
	}

}