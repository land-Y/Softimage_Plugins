#include "Mesh.h"

namespace Mitsuba
{
	Mesh::Mesh(){}
	Mesh::~Mesh(){}


	void Mesh::WriteMeshes(XMLDocument* doc, XMLElement* sceneElement, XSI::X3DObject& mesh, bool shouldExportGeometry, CString filePath)
	{
		if (isVisibility(mesh))
		{
			sceneElement->InsertEndChild(GenerateMeshSegment(doc, mesh, shouldExportGeometry, filePath));
		}
	}

	XMLElement* Mesh::GenerateMeshSegment(XMLDocument* doc, X3DObject& mesh, bool shouldExport, CString exportPath)
	{
		XMLElement* root = nullptr;

		if (MeshIsMassPrimitive(mesh))
		{
			Primitives* primitive = new Primitives();
			root = primitive->WritePrimitive(doc, mesh);
			delete primitive;
		}
		else
		{
			root = doc->NewElement("shape");
			root->SetAttribute(Constants::attrType, "ply");

			root->InsertEndChild(WriteElement(doc, Constants::attrString, Constants::attrFilename, (mesh.GetFullName() + L".ply").GetAsciiString()));

			XMLElement* trElement = doc->NewElement("transform");
			trElement->SetAttribute(Constants::attrName, "toWorld");
			root->InsertEndChild(trElement);

			MATH::CTransformation transform = mesh.GetKinematics().GetGlobal().GetTransform();
			trElement->InsertEndChild(WriteElementScale(doc, mesh));

			trElement->InsertEndChild(WriteSubElementRotation(doc, "x", transform.GetRotX()));
			trElement->InsertEndChild(WriteSubElementRotation(doc, "y", transform.GetRotY()));
			trElement->InsertEndChild(WriteSubElementRotation(doc, "z", transform.GetRotZ()));

			trElement->InsertEndChild(WriteElementTranslate(doc, mesh));

			Mitsuba::Material* material = new Mitsuba::Material();
			material->WriteMeshMaterial(doc, root, mesh);
			delete material;

			if (shouldExport)
			{
				bool isSolid = false;
				bool isSubdivide = false;
				LONG subLevel = 0;

				Property prop;
				mesh.GetPropertyFromName(L"MaSsObjectProperty", prop);
				if (prop.IsValid())
				{
					isSolid = prop.GetParameterValue(L"isSolid");
					isSubdivide = prop.GetParameterValue(L"isSubdivide");
					subLevel = prop.GetParameterValue(L"subLevel");
				}

				if (!isSubdivide)
				{
					Mitsuba::ExportPLY* exportPly = new Mitsuba::ExportPLY();
					exportPly->ExportPlyMeshBinary(mesh, exportPath, mesh.GetFullName() + L".ply", isSolid);
					delete exportPly;
				}
				else
				{
					/*#we should subdivide this mesh and then export
					#lm("We should subdivide the mesh")
					subOps = ap.ApplyOp("MeshSubdivideWithCenter", mesh)
					subMesh = ap.Selection(0)
					subOp = subOps(0)
					ap.SetValue(subOp.SubdivisionDepth, subLevel)
					ap.FreezeObj(subMesh, "", "")

					tM = mesh.Kinematics.Global.Transform.Matrix4
					oTrans = subMesh.Kinematics.Global.Transform
					tM.InvertInPlace()
					oTrans.SetMatrix4(tM)
					subMesh.Kinematics.Global.Transform = oTrans
					ap.ResetTransform(subMesh, "siCtr", "siSRT", "siXYZ")

					ExportPlyMesh02(subMesh, exportPath, mesh.FullName + ".ply")
					ap.DeleteObj(subMesh)*/
				}
			}
		}

		return root;
	}

	bool Mesh::MeshIsMassPrimitive(X3DObject& mesh)
	{
		CRefArray props = mesh.GetProperties();
		for (int i = 0; i < props.GetCount(); i++)
		{
			Property prop(props.GetItem(i));
			if (!prop.IsValid()) continue;

			CString propName = prop.GetName();
			propName.Lower();
			if (propName == L"masssphereprimitiveproperty" ||
				propName == L"masscubeprimitiveproperty" ||
				propName == L"masscylinderprimitiveproperty" ||
				propName == L"massrectangleprimitiveproperty" ||
				propName == L"massdiskprimitiveproperty")
			{
				return true;
			}
		}

		return false;
	}

}

