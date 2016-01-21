#include "Camera.h"

namespace Mitsuba
{
	Camera::Camera(){}
	Camera::~Camera(){}

	XMLElement* Camera::WriteCamera(XMLDocument* doc, LONG width, LONG height, bool isCamera, CString cameraName)
	{
		XMLElement* sensorElement = doc->NewElement(Constants::attrSensor);

		XMLElement* filmElement = doc->NewElement(Constants::attrFilm);
		filmElement->SetAttribute(Constants::attrType, Constants::attrHdrFilm);

		filmElement->InsertEndChild(WriteElement(doc, Constants::attrInteger, Constants::attrWidth, width));
		filmElement->InsertEndChild(WriteElement(doc, Constants::attrInteger, Constants::attrHeight, height));

		sensorElement->InsertEndChild(filmElement);

		if (!isCamera)
		{
			SetSensorDefault(sensorElement);
		}
		else
		{
			X3DObject oCam = app.GetActiveSceneRoot().FindChild2(cameraName, siCameraPrimType, CStringArray());
			if (!oCam.IsValid())
			{
				SetSensorDefault(sensorElement);
			}
			else
			{
				Property oProp;
				oCam.GetPropertyFromName(L"MaSsCameraProperty", oProp);
				if (!oProp.IsValid())
				{
					SetSensorDefault(sensorElement);
					//add property? Why?
					//oCam.AddProperty(L"MaSsCameraProperty");
				}
				else
				{
					CValue vtype = oProp.GetParameterValue(L"Type");
					CString stype = vtype.GetAsText();
					sensorElement->SetAttribute(Constants::attrType, stype.GetAsciiString());

					if (stype != Constants::paramCameraIrradiancemeter)
					{
						KinematicState kState = oCam.GetKinematics().GetGlobal();
						CString strPos = kState.GetParameterValue("PosX");
						strPos += ", ";
						strPos += kState.GetParameterValue("PosY");
						strPos += ", ";
						strPos += kState.GetParameterValue("PosZ");

						XSI::Camera myCam(oCam);
						X3DObject oInterest = myCam.GetInterest();

						KinematicState kStateInt = oInterest.GetKinematics().GetGlobal();
						CString strIntPos = kStateInt.GetParameterValue("PosX");
						strIntPos += ", ";
						strIntPos += kStateInt.GetParameterValue("PosY");
						strIntPos += ", ";
						strIntPos += kStateInt.GetParameterValue("PosZ");

						XMLElement* transfromElement = doc->NewElement("transform");
						transfromElement->SetAttribute(Constants::attrName, "toWorld");
						sensorElement->InsertEndChild(transfromElement);

						XMLElement* lookatElement = doc->NewElement("lookat");
						lookatElement->SetAttribute("origin", strPos.GetAsciiString());
						lookatElement->SetAttribute("target", strIntPos.GetAsciiString());
						lookatElement->SetAttribute("up", "0, 1, 0");
						transfromElement->InsertEndChild(lookatElement);

					}

					if (stype == Constants::paramCameraPerspective || stype == Constants::paramCameraThinlens || stype == Constants::paramCameraPerspective_rdist)
					{
						sensorElement->InsertEndChild(WriteElement(doc, Constants::attrFloat, "fov", oCam.GetParameterValue(L"fov").GetAsText().GetAsciiString()));

						CString sFovAxis = L"";
						CValue fovtype = oCam.GetParameterValue(L"fovtype");
						if (fovtype == 1)
						{
							sFovAxis = L"x";
						}
						else if (fovtype == 0)
						{
							sFovAxis = L"y";
						}
						sensorElement->InsertEndChild(WriteElement(doc, Constants::attrString, "fovAxis", sFovAxis.GetAsciiString()));
					}

					if (stype == Constants::paramCameraPerspective || stype == Constants::paramCameraThinlens || stype == Constants::paramCameraOrthographic
						|| stype == Constants::paramCameraTelecentric || stype == Constants::paramCameraPerspective_rdist)
					{
						sensorElement->InsertEndChild(WriteElement(doc, Constants::attrFloat, "nearClip", oCam.GetParameterValue(L"near").GetAsText().GetAsciiString()));
						sensorElement->InsertEndChild(WriteElement(doc, Constants::attrFloat, "farClip", oCam.GetParameterValue(L"far").GetAsText().GetAsciiString()));
					}

					if (stype == Constants::paramCameraThinlens || stype == Constants::paramCameraTelecentric)
					{
						sensorElement->InsertEndChild(WriteElement(doc, Constants::attrFloat, "focusDistance", oProp.GetParameterValue(L"focusDistance").GetAsText().GetAsciiString()));
						sensorElement->InsertEndChild(WriteElement(doc, Constants::attrFloat, "apertureRadius", oProp.GetParameterValue(L"apertureRadius").GetAsText().GetAsciiString()));
					}
				}
			}
		}

		return sensorElement;
	}

	void Camera::SetSensorDefault(XMLElement* element)
	{
		element->SetAttribute(Constants::attrType, Constants::paramCameraPerspective.GetAsciiString());
	}	
}