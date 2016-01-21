#pragma once

#include <vector>
#include <string>

#include "tinyxml2.h"
#include "Constants.h"

#include "xsi_application.h"
#include "xsi_string.h"
#include "xsi_parameter.h"
#include "xsi_shader.h"
#include "xsi_material.h"
#include "xsi_materiallibrary.h"
#include "xsi_imageclip2.h"
#include "xsi_property.h"
#include "xsi_x3dobject.h"
#include "xsi_kinematics.h"
#include "xsi_kinematicstate.h"
#include "xsi_transformation.h"
#include "xsi_vector3.h"
#include "xsi_vector3f.h"
#include "xsi_math.h"

using namespace tinyxml2;
using namespace XSI;

namespace Mitsuba
{
	class BaseElement
	{
	public:
		BaseElement();
		~BaseElement();

		XMLElement* WriteElement(XMLDocument* doc, const char* nameElement, const char* nameAttribute, LONG value);
		XMLElement* WriteElement(XMLDocument* doc, const char* nameElement, const char* nameAttribute, const char* value);
		XMLElement* WriteElement(XMLDocument* doc, const char* nameElement, CString nameAttribute, CString value);
		XMLElement* WriteElement(XMLDocument* doc, const char* nameElement, const char* nameAttribute, float value);

		CString FormatNumber(long s);

	protected:
		Application app;
		
		string FloatToString(double _value);
		string BoolToString(CValue value);
		float ColorToFloat(Parameter& param);
		string VectorToString(MATH::CVector3f vector);

		XSI::Shader FindShaderOnPort(XSI::Shader& shaderParam, CString targetPortName, bool isMShader);
		XSI::Shader FindShaderOnPort(XSI::Material& mat, CString targetPortName, bool isMShader);		

		XSI::ImageClip2 FindImageClipOnPort(XSI::Shader& shaderParam, CString targetPortName);

		bool isVisibility(X3DObject& obj);

		XMLElement* WriteElementPosition(XMLDocument* doc, const char* nameElement, const char* nameAttribute, XSI::X3DObject& object);		
		XMLElement* WriteElementTranslate(XMLDocument* doc, XSI::X3DObject& object);
		XMLElement* WriteElementRotation(XMLDocument* doc, XSI::X3DObject& object);
		XMLElement* WriteSubElementRotation(XMLDocument* doc, const char* paramName, double value);
		XMLElement* WriteElementScale(XMLDocument* doc, XSI::X3DObject& object);
		XMLElement* WriteRefMatSegment(XMLDocument* doc, X3DObject& mesh, XSI::Material& mat, bool isSSSName);
		XMLElement* WriteSpectrumElement(XMLDocument* doc, const char* paramName, XSI::Shader& shader);
		XMLElement* WriteSpectrumSegment(XMLDocument* doc, Parameter& param);

	private:
		XSI::Shader FindShaderOnPort(CRefArray& shaders, CString targetPortName, bool isMShader);
	};
}

