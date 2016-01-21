#pragma once

#include "tinyxml2.h"
#include "Constants.h"
#include "BaseElement.h"

#include "xsi_shader.h"
#include "xsi_parameter.h"
#include "xsi_property.h"
#include "xsi_ref.h"
#include <xsi_status.h>

using namespace tinyxml2;
using namespace XSI;

namespace Mitsuba
{
	class Shader : public BaseElement
	{
	public:
		Shader();
		~Shader();

		XMLElement* WriteShader(XMLDocument* doc, XSI::Shader& shader, CString nameMat, bool useId);
		XMLElement* WriteSSSShader(XMLDocument* doc, XSI::Shader& shader, CString nameMat, bool useID, bool isSSS);
		bool IsShaderCorrect(CString shaderName, XSI::Shader& xsiShader);
		bool IsSSSShaderCorrect(CString shaderName);

	private:
		XMLElement* WriteTextureOrSpectrum(XMLDocument* doc, Parameter& param, CString pName);
		XMLElement* WriteFloatOrTexture(XMLDocument* doc, Parameter& param, CString name);
		XMLElement* AddChildBsdf(XMLDocument* doc, XSI::Shader& shader, CString pName);
		XMLElement* WriteBitmap(XMLDocument* doc, XSI::Shader& shader, CString pName);
		XMLElement* GenerateTextureSegment(XMLDocument* doc, XSI::Shader& shader, CString pName);
		XMLElement* WriteVariantSegment(XMLDocument* doc, CValue value);
		XMLElement* WriteMaterialSegment(XMLDocument* doc, CValue value);
		XMLElement* WriteDistribution(XMLDocument* doc, CValue value);
		XMLElement* WriteIOR(XMLDocument* doc, bool param, CValue value);
		XMLElement* WriteStandartDiffuse(XMLDocument* doc);

		bool ShaderIsImageType(XSI::Shader& shader);
	};
}

