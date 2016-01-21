#include "ParticleCloud.h"

namespace Mitsuba
{
	ParticleCloud::ParticleCloud(){}
	ParticleCloud::~ParticleCloud(){}

	void ParticleCloud::WritePointCloud(XMLDocument* doc, XMLElement* sceneElement, XSI::ParticleCloud& pointCloud, bool shouldExportGeometry, CString filePath)
	{
		if (isVisibility(pointCloud))
		{
			sceneElement->InsertEndChild(GenerateParticleCloudSegment(doc, pointCloud, shouldExportGeometry, filePath));
		}
	}

	XMLElement* ParticleCloud::GenerateParticleCloudSegment(XMLDocument* doc, XSI::ParticleCloud& pointCloud, bool shouldExportGeometry, CString filePath)
	{
		XMLElement* root = doc->NewElement("");

		Geometry oGeom = pointCloud.GetActivePrimitive().GetGeometry();
		CRefArray attributes = oGeom.GetICEAttributes();

		if (Constants::NameInList(attributes, "StrandCount") && Constants::NameInList(attributes, "StrandPosition"))
		{
			if (shouldExportGeometry)
			{
				CString path = CUtils::BuildPath(filePath, pointCloud.GetFullName() + L".hair");
				WriteHairFile(oGeom, path);
			}

			XMLElement* element = doc->NewElement("shape");
			element->SetAttribute(Constants::attrType, "hair");

			element->InsertEndChild(WriteElement(doc, Constants::attrString, "filename", pointCloud.GetFullName() + L".hair"));

			CICEAttributeDataArrayLong dataArraySize;
			oGeom.GetICEAttributeFromName(L"Size").GetDataArray(dataArraySize);
			element->InsertEndChild(WriteElement(doc, Constants::attrFloat, "radius", dataArraySize[0]));

			Mitsuba::Material* material = new Mitsuba::Material();
			material->WriteMeshMaterial(doc, element, pointCloud);
			delete material;

			root->InsertEndChild(element);
		}
		else if (Constants::NameInList(attributes, "Shape"))
		{
			CICEAttributeDataArrayLong dataArrayNbPoints;
			oGeom.GetICEAttributeFromName(L"NbPoints").GetDataArray(dataArrayNbPoints);
			if (dataArrayNbPoints[0] > 0)
			{
				CICEAttributeDataArrayShape dataArrayShape;
				oGeom.GetICEAttributeFromName(L"Shape").GetDataArray(dataArrayShape);

				CICEAttributeDataArrayVector3f dataArrayPointPosition;
				oGeom.GetICEAttributeFromName(L"PointPosition").GetDataArray(dataArrayPointPosition);

				CICEAttributeDataArrayLong dataArraySize;
				oGeom.GetICEAttributeFromName(L"Size").GetDataArray(dataArraySize);

				CICEAttributeDataArrayRotationf dataArrayOrientation;
				oGeom.GetICEAttributeFromName(L"Orientation").GetDataArray(dataArrayOrientation);

				Mitsuba::Primitives* primitives = new Mitsuba::Primitives();
				for (ULONG i = 0; i < dataArrayShape.GetCount(); i++)
				{
					MATH::CVector3f position = dataArrayPointPosition[i];
					MATH::CShape shape = dataArrayShape[i].GetType();
					LONG size = dataArraySize[i];
					MATH::CRotationf rotation = dataArrayOrientation[i];

					//TODO
					/*if (shape == siICEShapeType::siICEShapeSphere)
					{
						root->InsertEndChild(primitives->GenerateSphere(doc, pointCloud, size, position));
					}
					else if (shape == siICEShapeType::siICEShapeBox)
					{
						root->InsertEndChild(primitives->GenerateCube(doc, pointCloud, size, rotation));
					}
					else if (shape == siICEShapeType::siICEShapeCylinder)
					{
						
						root->InsertEndChild(primitives->GenerateCylinder(doc, pointCloud, size, matrix));
					}
					else if (shape == siICEShapeType::siICEShapeRectangle)
					{
						root->InsertEndChild(primitives->GenerateRectangle(doc, pointCloud, size, rotation, position));
					}
					else if (shape == siICEShapeType::siICEShapeDisc)
					{
						root->InsertEndChild(primitives->GenerateDisk(doc, pointCloud, size, rotation));
					}*/
				}
			}
		}

		return root;
	}

	void ParticleCloud::WriteHairFile(XSI::Geometry& oGeom, CString hairFilePath)
	{
		ofstream f(hairFilePath.GetAsciiString(), ios::out | ios::binary);

		CICEAttributeDataArrayVector3f dataArrayPosition;
		oGeom.GetICEAttributeFromName(L"PointPosition").GetDataArray(dataArrayPosition);

		CICEAttributeDataArrayLong dataArrayStrandCount;
		oGeom.GetICEAttributeFromName(L"StrandCount").GetDataArray(dataArrayStrandCount);

		CICEAttributeDataArray2DVector3f dataArrayStrandPosition;
		oGeom.GetICEAttributeFromName(L"StrandPosition").GetDataArray2D(dataArrayStrandPosition);

		for (ULONG i = 0; i < dataArrayPosition.GetCount(); i++)
		{
			f << VectorToString(MATH::CVector3f(dataArrayPosition[i])) + "\n";

			//TODO
			for (LONG j = 0; j < dataArrayStrandCount[0]; j++)
			{
				//f << VectorToString(dataArrayStrandPosition[i][j]) + "\n";
			}

			f << "\n";
		}

		f.close();
	}
}
