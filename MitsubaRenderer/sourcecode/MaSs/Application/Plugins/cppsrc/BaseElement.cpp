#include "BaseElement.h"

namespace Mitsuba
{
	BaseElement::BaseElement(){}
	BaseElement::~BaseElement(){}

	XMLElement* BaseElement::WriteElement(XMLDocument* doc, const char* nameElement, const char* nameAttribute, LONG value)
	{
		XMLElement* element = doc->NewElement(nameElement);
		element->SetAttribute(Constants::attrName, nameAttribute);
		element->SetAttribute(Constants::attrValue, value);

		return element;
	}

	XMLElement* BaseElement::WriteElement(XMLDocument* doc, const char* nameElement, const char* nameAttribute, float value)
	{
		XMLElement* element = doc->NewElement(nameElement);
		element->SetAttribute(Constants::attrName, nameAttribute);
		element->SetAttribute(Constants::attrValue, value);

		return element;
	}

	XMLElement* BaseElement::WriteElement(XMLDocument* doc, const char* nameElement, CString nameAttribute, CString value)
	{
		return BaseElement::WriteElement(doc, nameElement, nameAttribute.GetAsciiString(), value.GetAsciiString());
	}

	XMLElement* BaseElement::WriteElement(XMLDocument* doc, const char* nameElement, const char* nameAttribute, const char* value)
	{
		XMLElement* element = doc->NewElement(nameElement);
		element->SetAttribute(Constants::attrName, nameAttribute);
		element->SetAttribute(Constants::attrValue, value);

		return element;
	}

	XMLElement* BaseElement::WriteElementPosition(XMLDocument* doc, const char* nameElement, const char* nameAttribute, XSI::X3DObject& object)
	{
		XMLElement* element = doc->NewElement(nameElement);
		element->SetAttribute(Constants::attrName, nameAttribute);

		Kinematics kinem = object.GetKinematics();
		MATH::CTransformation mt = kinem.GetGlobal().GetTransform();
		element->SetAttribute("x", mt.GetPosX());
		element->SetAttribute("y", mt.GetPosY());
		element->SetAttribute("z", mt.GetPosZ());

		return element;
	}

	XMLElement* BaseElement::WriteElementTranslate(XMLDocument* doc, XSI::X3DObject& object)
	{
		XMLElement* element = doc->NewElement("translate");

		MATH::CTransformation transf = object.GetKinematics().GetGlobal().GetTransform();
		element->SetAttribute("x", transf.GetPosX());
		element->SetAttribute("y", transf.GetPosY());
		element->SetAttribute("z", transf.GetPosZ());

		return element;
	}

	XMLElement* BaseElement::WriteElementScale(XMLDocument* doc, XSI::X3DObject& object)
	{
		XMLElement* element = doc->NewElement("scale");

		MATH::CTransformation transf = object.GetKinematics().GetGlobal().GetTransform();
		element->SetAttribute("x", transf.GetSclX());
		element->SetAttribute("y", transf.GetSclY());
		element->SetAttribute("z", transf.GetSclZ());

		return element;
	}

	XMLElement* BaseElement::WriteElementRotation(XMLDocument* doc, XSI::X3DObject& object)
	{
		/*SIVector3 vec = GetMath().CreateVector3();
		Object angle;
		angle = rotation.GetAxisAngle2(vec);

		if (((double)angle) == 0)
			return;

		writer.WriteStartElement("rotate");
		writer.WriteAttributeString("x", vec.X.ToString());
		writer.WriteAttributeString("y", vec.Y.ToString());
		writer.WriteAttributeString("z", vec.Z.ToString());
		writer.WriteAttributeString("angle", GetMath().RadiansToDegrees((double)angle).ToString());
*/
		MATH::CTransformation transf = object.GetKinematics().GetGlobal().GetTransform();

		XMLElement* rotateElement = doc->NewElement("rotate");
		rotateElement->SetAttribute("x", "1");
		rotateElement->SetAttribute("angle", transf.GetRotX());
		rotateElement->SetAttribute("y", "1");
		rotateElement->SetAttribute("angle", transf.GetRotY());
		rotateElement->SetAttribute("z", "1");
		rotateElement->SetAttribute("angle", transf.GetRotZ());

		return rotateElement;
	}

	XMLElement* BaseElement::WriteSubElementRotation(XMLDocument* doc, const char* paramName, double value)
	{
		XMLElement* rotateElement = doc->NewElement("rotate");
		rotateElement->SetAttribute(paramName, "1");
		rotateElement->SetAttribute("angle", value);

		return rotateElement;
	}

	XMLElement* BaseElement::WriteRefMatSegment(XMLDocument* doc, X3DObject& mesh, XSI::Material& mat, bool isSSSName)
	{
		XMLElement* element = doc->NewElement("ref");
		CString sname;
		if (isSSSName) sname = mat.GetLibrary().GetName() + L".SSS" + mat.GetName();
		else sname = mat.GetLibrary().GetName() + L"." + mat.GetName();



		element->SetAttribute("id", sname.GetAsciiString());

		return element;
	}

	XMLElement* BaseElement::WriteSpectrumSegment(XMLDocument* doc, Parameter& param)
	{
		//app.LogMessage(L"Spectrum Segment: param: " + param.GetName());
		CString color = param.GetParameterValue(L"red").GetAsText() + L", " + param.GetParameterValue(L"green").GetAsText() + L", " + param.GetParameterValue(L"blue").GetAsText();
		return WriteElement(doc, Constants::attrSpectrum, param.GetName(), color);
	}

	XMLElement* BaseElement::WriteSpectrumElement(XMLDocument* doc, const char* paramName, XSI::Shader& shader)
	{	
		app.LogMessage(L"WriteSpectrumElement: shader: " + shader.GetName() + L", param: " + CString(paramName));
		XMLElement* spectrumElement = doc->NewElement("spectrum");
		spectrumElement->SetAttribute(Constants::attrName, paramName);

		Parameter color = shader.GetParameter(CString(paramName));
		app.LogMessage(L"WriteSpectrumElement: get color: " + color.GetName());
		float red = color.GetParameterValue(L"red");
		float green = color.GetParameterValue(L"green");
		float blue = color.GetParameterValue(L"blue");
		
		string svalue3 = FloatToString(red) + ", " + FloatToString(green) + ", " + FloatToString(blue);
		spectrumElement->SetAttribute(Constants::attrValue, svalue3.c_str());

		return spectrumElement;
	}



	CString BaseElement::FormatNumber(long s)
	{
		char buffer[32];
		::sprintf_s(buffer, 32, "%04d", s);
		return CString(buffer);
	}

	string BaseElement::FloatToString(double _value)
	{
		char buf[256];
		sprintf_s(buf, "%g", _value);

		return buf;
	}

	string BaseElement::BoolToString(CValue value)
	{
		return value.GetAsText().GetAsciiString();
	}

	float BaseElement::ColorToFloat(Parameter& param)
	{
		float r = param.GetParameterValue(L"red");
		float g = param.GetParameterValue(L"green");
		float b = param.GetParameterValue(L"blue");
		
		return (r+g+b)/3;
	}

	string BaseElement::VectorToString(MATH::CVector3f vector)
	{
		return  to_string(vector.GetX()) + " " + to_string(vector.GetY()) +" "+ to_string(vector.GetZ());
	}

	XSI::Shader BaseElement::FindShaderOnPort(XSI::Shader& shaderParam, CString targetPortName, bool isMShader)
	{
		return FindShaderOnPort(shaderParam.GetShaders(), targetPortName, isMShader);
	}

	XSI::Shader BaseElement::FindShaderOnPort(XSI::Material& mat, CString targetPortName, bool isMShader)
	{
		return FindShaderOnPort(mat.GetShaders(), targetPortName, isMShader);
	}

	XSI::Shader BaseElement::FindShaderOnPort(CRefArray& shaders, CString targetPortName, bool isMShader)
	{
		//app.LogMessage(L"Shaders count: " + CString(shaders.GetCount()));
		XSI::Shader mShader;
		for (long j = 0; j < shaders.GetCount(); j++)
		{
			XSI::Shader lShader(shaders[j]);
			CRefArray lTargets = lShader.GetShaderParameterTargets(L"out");
			for (int i = 0; i < lTargets.GetCount(); i++)
			{
				Parameter lTarget(lTargets.GetItem(i));
				//app.LogMessage(L"FindShaderOnPort: Target on port: " + lTarget.GetName());
				if (lTarget.GetName().IsEqualNoCase(targetPortName))
				{
					if (isMShader)
					{
						Parameter oParam = lShader.GetParameter(Constants::mShaderType);
						if (oParam.IsValid())
						{
							mShader = lShader;
							break;
						}
					}
					else
					{
						mShader = lShader;
						break;
					}
				}
			}
		}

		return mShader;
	}
	
	XSI::ImageClip2 BaseElement::FindImageClipOnPort(XSI::Shader& shaderParam, CString targetPortName)
	{
		//app.LogMessage(L"FindImageClipOnPort: targetPortName: " + targetPortName);

		CRefArray imageclips = shaderParam.GetImageClips();
		XSI::ImageClip2 imageClip;
		for (long j = 0; j < imageclips.GetCount(); j++)
		{
			XSI::ImageClip2 lImageClip(imageclips[j]);
			
			Parameter lTarget = lImageClip.GetShaderParameterTargets()[0];
			if (lTarget.GetName().IsEqualNoCase(targetPortName))
			{
				//app.LogMessage(L"FindImageClipOnPort: image found!");
				imageClip = lImageClip;
				break;
			}
		}

		return imageClip;
	}

	bool BaseElement::isVisibility(X3DObject& obj)
	{
		Property prop;
		obj.GetPropertyFromName(L"visibility", prop);
		if (!prop.IsValid()) return false;

		return prop.GetParameterValue(L"rendvis");
	}

}
