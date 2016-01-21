#pragma once

#include <fstream>

#include "tinyxml2.h"
#include "Constants.h"
#include "BaseElement.h"
#include "Material.h"
#include "Primitives.h"

#include "xsi_particlecloud.h"
#include "xsi_geometry.h"
#include "xsi_primitive.h"
#include "xsi_iceattribute.h"
#include "xsi_ref.h"
#include "xsi_utils.h"
#include "xsi_iceattributedataarray.h"
#include "xsi_iceattributedataarray2D.h"




namespace Mitsuba
{
	class ParticleCloud :BaseElement
	{
	public:
		ParticleCloud();
		~ParticleCloud();

		void WritePointCloud(XMLDocument* doc, XMLElement* sceneElement, XSI::ParticleCloud& pointCloud, bool shouldExportGeometry, CString filePath);

	private:
		void WriteHairFile(XSI::Geometry& oGeom, CString hairFilePath);
		XMLElement* GenerateParticleCloudSegment(XMLDocument* doc, XSI::ParticleCloud& pointCloud, bool shouldExportGeometry, CString filePath);
	};
}
