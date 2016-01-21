#pragma once

#include "tinyxml2.h"
#include "Constants.h"
#include "BaseElement.h"
#include "Primitives.h"
#include "Material.h"
#include "ExportPLY.h"

namespace Mitsuba
{
	class Mesh : BaseElement
	{
	public:
		Mesh();
		~Mesh();

		void WriteMeshes(XMLDocument* doc, XMLElement* sceneElement, XSI::X3DObject& mesh, bool shouldExportGeometry, CString filePath);

	private:
		XMLElement* GenerateMeshSegment(XMLDocument* doc, X3DObject& mesh, bool shouldExport, CString exportPath);
		bool MeshIsMassPrimitive(X3DObject& mesh);		
	};
}

