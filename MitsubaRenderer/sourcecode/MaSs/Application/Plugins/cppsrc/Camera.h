#pragma once

#include "tinyxml2.h"
#include "constants.h"
#include "BaseElement.h"

#include <xsi_application.h>
#include <xsi_camera.h>
#include <xsi_kinematics.h>
#include <xsi_model.h>


using namespace XSI;
using namespace tinyxml2;
using namespace std;

namespace Mitsuba
{
	class Camera : public BaseElement
	{
	public:
		Camera();
		~Camera();

		XMLElement* WriteCamera(XMLDocument* doc, LONG width, LONG height, bool isCamera, CString cameraName);

	private:
		void SetSensorDefault(XMLElement* element);
	};
}
