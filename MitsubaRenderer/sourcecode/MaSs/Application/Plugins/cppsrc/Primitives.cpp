#include "Primitives.h"

namespace Mitsuba
{
	Primitives::Primitives(){}
	Primitives::~Primitives(){}

	XMLElement* Primitives::WritePrimitive(XMLDocument* doc, X3DObject& mesh)
	{
		XMLElement* element = nullptr;

		CRefArray props = mesh.GetProperties();
		for (int i = 0; i < props.GetCount(); i++)
		{
			Property prop(props.GetItem(i));
			if (!prop.IsValid()) continue;

			CString propName = prop.GetName();
			propName.Lower();
			if (propName == L"masssphereprimitiveproperty")
			{ 
				element = GenerateSphere(doc, mesh, prop);
			}
			else if (propName == L"masscubeprimitiveproperty")
			{
				element = GenerateCube(doc, mesh, prop);
			}
			else if(propName == L"masscylinderprimitiveproperty")
			{
				element = GenerateCylinder(doc, mesh, prop);
			}
			else if(propName == L"massrectangleprimitiveproperty")
			{
				element = GenerateRectangle(doc, mesh, prop);
			}
			else if(propName == L"massdiskprimitiveproperty")
			{
				element = GenerateDisk(doc, mesh, prop);
			}
		}		

		return element;

	}

	XMLElement* Primitives::GenerateDisk(XMLDocument* doc, X3DObject& obj, Property& prop)
	{
		XMLElement* root = doc->NewElement("shape");
		root->SetAttribute(Constants::attrType, "disk");

		XMLElement* trElement = doc->NewElement("transform");
		trElement->SetAttribute(Constants::attrName, "toWorld");		

		MATH::CTransformation transform = obj.GetKinematics().GetGlobal().GetTransform();

		XMLElement* scElement = doc->NewElement("scale");
		scElement->SetAttribute(Constants::attrValue, prop.GetParameterValue(L"radius").GetAsText().GetAsciiString());
		trElement->InsertEndChild(scElement);

		trElement->InsertEndChild(WriteSubElementRotation(doc, "x", (transform.GetRotX()-90)));
		trElement->InsertEndChild(WriteSubElementRotation(doc, "y", transform.GetRotY()));
		trElement->InsertEndChild(WriteSubElementRotation(doc, "z", transform.GetRotZ()));

		trElement->InsertEndChild(WriteElementTranslate(doc, obj));
		root->InsertEndChild(trElement);

		Mitsuba::Material* material = new Mitsuba::Material();
		material->WriteMeshMaterial(doc, root, obj);
		delete material;
		
		return root;
	}

	XMLElement* Primitives::GenerateRectangle(XMLDocument* doc, X3DObject& obj, Property& prop)
	{
		XMLElement* root = doc->NewElement("shape");
		root->SetAttribute(Constants::attrType, "rectangle");

		XMLElement* trElement = doc->NewElement("transform");
		trElement->SetAttribute(Constants::attrName, "toWorld");

		MATH::CTransformation transform = obj.GetKinematics().GetGlobal().GetTransform();

		float lx = prop.GetParameterValue(L"lengthX");
		float ly = prop.GetParameterValue(L"lengthY");

		XMLElement* scElement = doc->NewElement("scale");
		scElement->SetAttribute("x", lx / 2);
		scElement->SetAttribute("y", ly / 2);
		trElement->InsertEndChild(scElement);

		trElement->InsertEndChild(WriteSubElementRotation(doc, "x", (transform.GetRotX()-90)));
		trElement->InsertEndChild(WriteSubElementRotation(doc, "y", transform.GetRotY()));
		trElement->InsertEndChild(WriteSubElementRotation(doc, "z", transform.GetRotZ()));

		trElement->InsertEndChild(WriteElementTranslate(doc, obj));
		root->InsertEndChild(trElement);

		Mitsuba::Material* material = new Mitsuba::Material();
		material->WriteMeshMaterial(doc, root, obj);
		delete material;

		return root;
	}

	XMLElement* Primitives::GenerateCylinder(XMLDocument* doc, X3DObject& obj, Property& prop)
	{
		XMLElement* root = doc->NewElement("shape");
		root->SetAttribute(Constants::attrType, "cylinder");

		root->InsertEndChild(WriteElement(doc, Constants::attrFloat, "radius", CString(prop.GetParameterValue(L"radius"))));

		MATH::CTransformation transform = obj.GetKinematics().GetGlobal().GetTransform();
		MATH::CMatrix4 matrix = obj.GetKinematics().GetLocal().GetTransform().GetMatrix4();

		//TODO
		XMLElement* p0Element = doc->NewElement("point");
		p0Element->SetAttribute(Constants::attrName, "point0");
		p0Element->SetAttribute("x", "0");
		p0Element->SetAttribute("y", "0");
		p0Element->SetAttribute("z", "0");
		root->InsertEndChild(p0Element);

		//TODO
		XMLElement* p1Element = doc->NewElement("point");
		p1Element->SetAttribute(Constants::attrName, "point1");
		p1Element->SetAttribute("x", "0");
		p1Element->SetAttribute("y", "1");
		p1Element->SetAttribute("z", "0");
		root->InsertEndChild(p1Element);

		Mitsuba::Material* material = new Mitsuba::Material();
		material->WriteMeshMaterial(doc, root, obj);
		delete material;

		return root;
	}

	XMLElement* Primitives::GenerateCube(XMLDocument* doc, X3DObject& obj, Property& prop)
	{
		XMLElement* root = doc->NewElement("shape");
		root->SetAttribute(Constants::attrType, "cube");

		XMLElement* trElement = doc->NewElement("transform");
		trElement->SetAttribute(Constants::attrName, "toWorld");

		MATH::CTransformation transform = obj.GetKinematics().GetGlobal().GetTransform();
						
		float lx = prop.GetParameterValue(L"lengthX");
		float ly = prop.GetParameterValue(L"lengthY");
		float lz = prop.GetParameterValue(L"lengthZ");

		XMLElement* scElement = doc->NewElement("scale");
		scElement->SetAttribute("x", lx / 2);
		scElement->SetAttribute("y", ly / 2);
		scElement->SetAttribute("z", lz / 2);
		trElement->InsertEndChild(scElement);
		
		trElement->InsertEndChild(WriteSubElementRotation(doc, "x", transform.GetRotX()));
		trElement->InsertEndChild(WriteSubElementRotation(doc, "y", transform.GetRotY()));
		trElement->InsertEndChild(WriteSubElementRotation(doc, "z", transform.GetRotZ()));

		trElement->InsertEndChild(WriteElementTranslate(doc, obj));
		root->InsertEndChild(trElement);

		Mitsuba::Material* material = new Mitsuba::Material();
		material->WriteMeshMaterial(doc, root, obj);
		delete material;

		return root;
	}

	XMLElement* Primitives::GenerateSphere(XMLDocument* doc, X3DObject& obj, Property& prop)
	{
		XMLElement* root = doc->NewElement("shape");
		root->SetAttribute(Constants::attrType, "sphere");

		MATH::CTransformation transform = obj.GetKinematics().GetGlobal().GetTransform();

		XMLElement* trElement = doc->NewElement("point");
		trElement->SetAttribute(Constants::attrName, "center");
		trElement->SetAttribute("x", transform.GetPosX());
		trElement->SetAttribute("y", transform.GetPosY());
		trElement->SetAttribute("z", transform.GetPosZ());
		root->InsertEndChild(trElement);

		CValue radius = prop.GetParameterValue(L"radius");
		root->InsertEndChild(WriteElement(doc, Constants::attrFloat, "radius", radius.GetAsText().GetAsciiString()));

		Mitsuba::Material* material = new Mitsuba::Material();
		material->WriteMeshMaterial(doc, root, obj);
		delete material;

		return root;
	}
}