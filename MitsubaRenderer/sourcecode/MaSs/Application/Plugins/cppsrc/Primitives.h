#pragma once

#include "tinyxml2.h"
#include "Constants.h"
#include "BaseElement.h"
#include "Material.h"


namespace Mitsuba
{
	class Primitives :BaseElement
	{
	public:
		Primitives();
		~Primitives();

		XMLElement* WritePrimitive(XMLDocument* doc, X3DObject& mesh);

		XMLElement* GenerateDisk(XMLDocument* doc, X3DObject& obj, Property& prop);
		XMLElement* GenerateRectangle(XMLDocument* doc, X3DObject& obj, Property& prop);
		XMLElement* GenerateCylinder(XMLDocument* doc, X3DObject& obj, Property& prop);
		XMLElement* GenerateCube(XMLDocument* doc, X3DObject& obj, Property& prop);
		XMLElement* GenerateSphere(XMLDocument* doc, X3DObject& obj, Property& prop);

	};
}

