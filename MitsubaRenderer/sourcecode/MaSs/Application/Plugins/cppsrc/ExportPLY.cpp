#include "ExportPLY.h"

namespace std
{
	ofstream& operator<<(ofstream& a_stream, const char* a_str)
	{
		if (a_str)
		{
			size_t len = ::strlen(a_str);
			if (len > 0)
				a_stream.write(a_str, len);
		}

		return a_stream;
	}

	ofstream& operator<<(ofstream& a_stream, float a_val)
	{
		a_stream.write(reinterpret_cast<const char*>(&a_val), sizeof(a_val));
		return a_stream;
	}


	ofstream& operator<<(ofstream& a_stream, unsigned int a_val)
	{
		a_stream.write(reinterpret_cast<const char*>(&a_val), sizeof(a_val));
		return a_stream;
	}

	ofstream& operator<<(ofstream& a_stream, unsigned char a_val)
	{
		a_stream.write(reinterpret_cast<const char*>(&a_val), sizeof(a_val));
		return a_stream;
	}

	ofstream& operator<<(ofstream& a_stream, int a_val)
	{
		a_stream.write(reinterpret_cast<const char*>(&a_val), sizeof(a_val));
		return a_stream;
	}

	ofstream& operator<<(ofstream& a_stream, LONG a_val)
	{
		a_stream.write(reinterpret_cast<const char*>(&a_val), sizeof(a_val));
		return a_stream;
	}

	ofstream& operator<<(ofstream& a_stream, const MATH::CVector3& a_val)
	{
		a_stream << static_cast<float>(a_val.GetX());
		a_stream << static_cast<float>(a_val.GetY());
		a_stream << static_cast<float>(a_val.GetZ());

		return a_stream;
	}
}

namespace Mitsuba
{
	ExportPLY::ExportPLY(){}
	ExportPLY::~ExportPLY(){}

	void ExportPLY::ExportPlyMesh(X3DObject& obj, CString exportPath, CString fileName, bool isSolid)
	{
		ofstream f(CUtils::BuildPath(exportPath, fileName).GetAsciiString(), ios::out | ios::binary);

		f << "ply" << endl;
		f << "format ascii 1.0" << endl;

		Geometry oGeom = obj.GetActivePrimitive().GetGeometry();
		CTriangleRefArray tris = oGeom.GetTriangles();
		int vertexcount = tris.GetCount() * 3;
		f << "element vertex " + to_string(vertexcount) << endl;
		f << "property float32 x" << endl;
		f << "property float32 y" << endl;
		f << "property float32 z" << endl;

		if (!isSolid)
		{
			f << "property float32 nx" << endl;
			f << "property float32 ny" << endl;
			f << "property float32 nz" << endl;
		}

		CGeometryAccessor ga = PolygonMesh(oGeom).GetGeometryAccessor();
		CRefArray sUVs = ga.GetUVs();
		LONG uvCount = sUVs.GetCount();

		ClusterProperty uvProp;
		CFloatArray uvValues;

		if (!isSolid && uvCount > 0)
		{
			uvProp = sUVs[0];
			uvProp.GetValues(uvValues);

			f << "property float32 s" << endl;
			f << "property float32 t" << endl;
		}

		CTriangleRefArray triangles = obj.GetActivePrimitive().GetGeometry().GetTriangles();
		int trianglecount = triangles.GetCount();
		f << "element face " + to_string(trianglecount) << endl;
		f << "property list uint8 int32 vertex_index" << endl;
		f << "end_header" << endl;

		//app.LogMessage(L"Tris Count: " + CString(triangles.GetCount()));

		CLongArray triNodeIds;
		ga.GetTriangleNodeIndices(triNodeIds);

		for (int i = 0; i < triangles.GetCount(); i++)
		{
			XSI::Triangle triangle(triangles.GetItem(i));
			for (int j = 2; j >= 0; j--)
			{
				CTriangleVertexRefArray vertArray = triangle.GetPoints();
				//app.LogMessage(L"Point Count: " + CString(vertArray.GetCount()));
				//app.LogMessage(L"Point index: " + vertArray.GetItem(j).GetAsText());
				XSI::TriangleVertex oVertex(vertArray.GetItem(j));
				MATH::CVector3 oVertexPos = oVertex.GetPosition();

				f << (float)oVertexPos.GetX();
				f << " ";
				f << (float)oVertexPos.GetY();
				f << " ";
				f << (float)oVertexPos.GetZ();
				f << " ";

				//app.LogMessage(L"Position " + CString(i + j) + ": " + CString(oVertexPos.GetX()) + L", " + CString(oVertexPos.GetY()) + L", " + CString(oVertexPos.GetZ()));
				if (!isSolid)
				{
					MATH::CVector3 oVertexNormal = oVertex.GetNormal();

					f << (float)oVertexNormal.GetX();
					f << " ";
					f << (float)oVertexNormal.GetY();
					f << " ";
					f << (float)oVertexNormal.GetZ();
					f << " ";
					//app.LogMessage(L"Normal: "+ CString(oVertexNormal.GetX())+L", "+CString(oVertexNormal.GetY())+L", "+CString(oVertexNormal.GetZ()));

					if (uvCount > 0)
					{
						int nodeId = triNodeIds[i * 3 + j];
						float u = uvValues[nodeId * 3 + 0];
						float v = 1 - uvValues[nodeId * 3 + 1];

						//app.LogMessage(L"u: "+ CString(u));
						//app.LogMessage(L"v: "+ CString(v));

						f << u;
						f << " ";
						f << v;
					}
				}
				f << endl;
				//app.LogMessage(L"clsArray[nCls][(i*3+2)-j]: "+CString(clsArray[nCls][2]));
			}
		}

		for (int i = 0; i < triangles.GetCount(); i++)
		{
			XSI::Triangle triangle(triangles.GetItem(i));
			if (!triangle.IsValid()) continue;

			f << "3 ";
			f << triangle.GetIndex() * 3;
			f << " ";
			f << triangle.GetIndex() * 3 + 1;
			f << " ";
			f << triangle.GetIndex() * 3 + 2;
			f << endl;
		}

		f.close();
	}

	void ExportPLY::ExportPlyMeshBinary(X3DObject& obj, CString exportPath, CString fileName, bool isSolid)
	{
		ofstream f(CUtils::BuildPath(exportPath, fileName).GetAsciiString(), ios::out | ios::binary);

		f << "ply\n";
		f << "format binary_little_endian 1.0\n";

		Geometry oGeom = obj.GetActivePrimitive().GetGeometry();
		CTriangleRefArray tris = oGeom.GetTriangles();
		unsigned int vertexcount = tris.GetCount() * 3;
		string str = "element vertex " + to_string(vertexcount) + "\n";
		f << str;
		f << "property float x\n";
		f << "property float y\n";
		f << "property float z\n";

		if (!isSolid)
		{
			f << "property float nx\n";
			f << "property float ny\n";
			f << "property float nz\n";
		}

		CGeometryAccessor ga = PolygonMesh(oGeom).GetGeometryAccessor();
		CRefArray sUVs = ga.GetUVs();
		LONG uvCount = sUVs.GetCount();

		ClusterProperty uvProp;
		CFloatArray uvValues;

		if (!isSolid && uvCount > 0)
		{
			uvProp = sUVs[0];
			uvProp.GetValues(uvValues);

			f << "property float s\n";
			f << "property float t\n";
		}

		CTriangleRefArray triangles = obj.GetActivePrimitive().GetGeometry().GetTriangles();
		unsigned int trianglecount = triangles.GetCount();
		string str2 = "element face " + to_string(trianglecount) + "\n";
		f << str2;
		f << "property list uchar uint vertex_indices\n";
		f << "end_header\n";

		//app.LogMessage(L"Tris Count: " + CString(triangles.GetCount()));

		CLongArray triNodeIds;
		ga.GetTriangleNodeIndices(triNodeIds);

		for (UINT i = 0; i < trianglecount; i++)
		{
			XSI::Triangle triangle(triangles.GetItem(i));
			for (int j = 2; j >= 0; j--)
			{
				CTriangleVertexRefArray vertArray = triangle.GetPoints();
				//app.LogMessage(L"Point Count: " + CString(vertArray.GetCount()));
				//app.LogMessage(L"Point index: " + vertArray.GetItem(j).GetAsText());
				XSI::TriangleVertex oVertex(vertArray.GetItem(j));
				MATH::CVector3 oVertexPos = oVertex.GetPosition();

				f << oVertexPos;
				//app.LogMessage(L"Position " + CString(i + j) + ": " + CString(oVertexPos.GetX()) + L", " + CString(oVertexPos.GetY()) + L", " + CString(oVertexPos.GetZ()));
				if (!isSolid)
				{
					MATH::CVector3 oVertexNormal = oVertex.GetNormal();
					f << oVertexNormal;

					if (uvCount > 0)
					{
						int nodeId = triNodeIds[i * 3 + j];
						float u = uvValues[nodeId * 3 + 0];
						float v = 1 - uvValues[nodeId * 3 + 1];

						//app.LogMessage(L"u: "+ CString(u));
						//app.LogMessage(L"v: "+ CString(v));

						f << u;
						f << v;
					}
				}
				//app.LogMessage(L"clsArray[nCls][(i*3+2)-j]: "+CString(clsArray[nCls][2]));
			}
		}

		for (UINT i = 0; i < trianglecount; i++)
		{
			XSI::Triangle triangle(triangles.GetItem(i));
			if (!triangle.IsValid()) continue;

			f << (unsigned char)3;
			f << (UINT)(triangle.GetIndex() * 3);
			f << (UINT)(triangle.GetIndex() * 3 + 1);
			f << (UINT)(triangle.GetIndex() * 3 + 2);
		}

		f.close();
	}

}
