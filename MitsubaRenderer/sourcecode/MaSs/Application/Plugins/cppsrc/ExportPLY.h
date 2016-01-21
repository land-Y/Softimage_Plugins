#pragma once

#include <fstream>
#include <string>

#include "xsi_application.h"
#include "xsi_geometry.h"
#include "xsi_x3dobject.h"
#include "xsi_primitive.h"
#include "xsi_point.h"
#include "xsi_facet.h"
#include "xsi_material.h"
#include "xsi_utils.h"
#include "xsi_geometryaccessor.h"
#include "xsi_polygonmesh.h"
#include "xsi_triangle.h"
#include <xsi_trianglevertex.h>
#include "xsi_vertex.h"
#include <xsi_ref.h>
#include <xsi_vector3.h>
#include <xsi_math.h>

using namespace XSI;
using namespace std;

namespace Mitsuba
{
	class ExportPLY
	{
	public:
		ExportPLY();
		~ExportPLY();

		void ExportPlyMesh(X3DObject& obj, CString exportPath, CString fileName, bool isSolid);
		void ExportPlyMeshBinary(X3DObject& obj, CString exportPath, CString fileName, bool isSolid);

	private:
		Application app;
	};
}

