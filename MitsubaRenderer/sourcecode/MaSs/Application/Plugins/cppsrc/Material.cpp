#include "Material.h"


namespace Mitsuba
{
	Material::Material()
	{
		m_shader = new Shader();
	}

	Material::~Material()
	{
		delete m_shader;
	}

	void Material::WriteMaterials(XMLDocument* doc, XMLElement* sceneElement, XSI::Material oMat)
	{
		XSI::Shader mShader = FindShaderOnPort(oMat, L"surface", true);
		if (mShader.IsValid())
		{
			CString matName;
			CString shaderType = mShader.GetParameterValue(Constants::mShaderType);
			if (Constants::NameInList(Constants::shaderTypeArray, shaderType.GetAsciiString()))
			{				
				matName = oMat.GetLibrary().GetName() + L"." + oMat.GetName();
				sceneElement->InsertEndChild(m_shader->WriteShader(doc, mShader, matName, true));
			}
			else if (Constants::NameInList(Constants::shaderSSSTypeArray, shaderType.GetAsciiString()))
			{
				matName = oMat.GetLibrary().GetName() + L"." + oMat.GetName();
				CString matSSSName = oMat.GetLibrary().GetName() + L".SSS" + oMat.GetName();
				sceneElement->InsertEndChild(m_shader->WriteSSSShader(doc, mShader, matName, true, false));
				sceneElement->InsertEndChild(m_shader->WriteSSSShader(doc, mShader, matSSSName, true, true));
			}
			else
			{
				app.LogMessage(L"Material not correct: " + oMat.GetName(), siWarningMsg);
			}
		}
	}

	void Material::WriteMeshMaterial(XMLDocument* doc, XMLElement* root, X3DObject& mesh)
	{
		XSI::Material mat = mesh.GetMaterial();
		if (IsMaterialCorrect(mat))
		{
			root->InsertEndChild(WriteRefMatSegment(doc, mesh, mat, false));
		}
		else if (IsSSSMaterialCorrect(mat))
		{
			root->InsertEndChild(WriteRefMatSegment(doc, mesh, mat, false));
			root->InsertEndChild(WriteRefMatSegment(doc, mesh, mat, true));
		}
		else if (IsMeshLight(mat))
		{
			root->InsertEndChild(GenerateMeshLightSegment(doc, XSI::Shader(mat.GetShaders().GetItem(0))));
		}
	}

	bool Material::IsMaterialCorrect(XSI::Material& mat)
	{
		CRefArray shaders = mat.GetShaders();

		if (shaders.GetCount() == 0) return false;

		XSI::Shader shader = shaders.GetItem(0);
		Parameter param = shader.GetParameter(Constants::mShaderType);
		if (!param.IsValid()) return false;

		if (!m_shader->IsShaderCorrect(param.GetValue(), shader)) return false;

		return true;
	}

	bool Material::IsSSSMaterialCorrect(XSI::Material& mat)
	{
		CRefArray shaders = mat.GetShaders();

		if (shaders.GetCount() == 0) return false;

		XSI::Shader shader = shaders.GetItem(0);
		Parameter param = shader.GetParameter(Constants::mShaderType);
		if (!param.IsValid()) return false;

		if (!m_shader->IsSSSShaderCorrect(param.GetValue())) return false;

		return true;
	}

	bool Material::IsMeshLight(XSI::Material& mat)
	{
		CRefArray shaders = mat.GetShaders();
		if (shaders.GetCount() == 0) return false;

		XSI::Shader shader = shaders.GetItem(0);
		Parameter param = shader.GetParameter(Constants::mShaderType);
		if (!param.IsValid()) return false;

		if (param.GetValue().GetAsText() != L"mEmitter") return false;

		return true;
	}

	XMLElement* Material::GenerateMeshLightSegment(XMLDocument* doc, XSI::Shader& shader)
	{
		XMLElement* element = doc->NewElement("emitter");
		element->SetAttribute(Constants::attrType, "area");
		element->InsertEndChild(WriteSpectrumElement(doc, "radiance", shader));

		return element;
	}


}
