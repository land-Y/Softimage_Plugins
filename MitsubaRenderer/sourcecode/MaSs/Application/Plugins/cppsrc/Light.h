#pragma once

#include "tinyxml2.h"
#include "Constants.h"
#include "BaseElement.h"

#include "xsi_light.h"

namespace Mitsuba
{
	class Light : public BaseElement
	{
	public:
		Light();
		~Light();

		void WriteLights(XMLDocument* doc, XMLElement* sceneElement, XSI::Light& light);
		bool isLightCorrect(XSI::Light& light);
		XMLElement* GenerateLightSegment(XMLDocument* doc, XSI::Light& light);

	private:
		XMLElement* WriteEmitter(XMLDocument* doc, const char* typeName, XSI::Shader& shader);
		XMLElement* WriteSpectrumElement(XMLDocument* doc, const char* attrName, const char* paramName, XSI::Shader& shader);
	};
}

