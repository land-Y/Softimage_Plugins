#pragma once

#include "tinyxml2.h"
#include "Constants.h"
#include "BaseElement.h"
#include "Shader.h"

#include "xsi_shader.h"
#include "xsi_parameter.h"
#include "xsi_property.h"
#include "xsi_ref.h"
#include <xsi_status.h>
#include <xsi_material.h>
#include <xsi_materiallibrary.h>

using namespace tinyxml2;
using namespace XSI;

namespace Mitsuba
{
	class Material:public BaseElement
	{
	public:
		Material();
		~Material();

		void WriteMaterials(XMLDocument* doc, XMLElement* sceneElement, XSI::Material oMat);
		void WriteMeshMaterial(XMLDocument* doc, XMLElement* root, X3DObject& mesh);

	private:
		Mitsuba::Shader* m_shader;

		bool IsMaterialCorrect(XSI::Material& mat);
		bool IsSSSMaterialCorrect(XSI::Material& mat);
		bool IsMeshLight(XSI::Material& mat);
		XMLElement* GenerateMeshLightSegment(XMLDocument* doc, XSI::Shader& shader);
		void WriteSSSMaterials(XMLDocument* doc, XMLElement* sceneElement, XSI::Material oMat);
	};
}

