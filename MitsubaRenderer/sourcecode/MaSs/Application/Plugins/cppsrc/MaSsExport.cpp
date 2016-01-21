#pragma once

#include <stdio.h>
#include <string>
#include <array>

#include <xsi_application.h>
#include <xsi_context.h>
#include <xsi_pluginregistrar.h>
#include <xsi_status.h>
#include <xsi_argument.h>
#include <xsi_command.h>
#include <xsi_property.h>
#include <xsi_model.h>
#include <xsi_utils.h>
#include <xsi_camera.h>
#include <xsi_kinematics.h>
#include <xsi_scene.h>
#include <xsi_materiallibrary.h>
#include <xsi_material.h>
#include <xsi_shader.h>
#include <xsi_parameter.h>
#include <xsi_selection.h>
#include <xsi_project.h>
#include <xsi_light.h>
#include <xsi_nurbssurface.h>
#include <xsi_nurbssurfacemesh.h>
#include <xsi_polygonmesh.h>
#include <xsi_particlecloud.h>
#include <xsi_geometry.h>
#include <xsi_primitive.h>
#include <xsi_progressbar.h>
#include <xsi_comapihandler.h>

#include "tinyxml2.h"
#include "Constants.h"
#include "Camera.h"
#include "Shader.h"
#include "Material.h"
#include "Light.h"
#include "Mesh.h"
#include "ParticleCloud.h"

using namespace XSI;
using namespace tinyxml2;
using namespace std;


class MaSsExport
{
public:
	MaSsExport::MaSsExport(){}
	MaSsExport::~MaSsExport(){}

	CString FormatNumber(long s)
	{
		char buffer[32];
		::sprintf_s(buffer, 32, "%04d", s);
		return CString(buffer);
	}

	void GetPropertyMaSs(Property& oProp)
	{
		oProp = app.GetActiveSceneRoot().GetProperties().GetItem(L"MaSs");
	}

	bool GenerateXMLScene(Property& oProp, CValue iIncrementValue)
	{
		//app.LogMessage(L"Export Scene!");
		if (!oProp.IsValid()) return false;

		CString filePath = oProp.GetParameter(L"Path").GetValue();
		if (filePath.IsEmpty())
		{
			app.LogMessage(L"Export Path is Empty!", siErrorMsg);
			return false;
		}

		CString fileName = oProp.GetParameter(L"FileName").GetValue();
		if (fileName.IsEmpty())
		{
			fileName = L"myscene";
		}

		if ((LONG)iIncrementValue != -1) fileName += (L"_" + FormatNumber((long)iIncrementValue));

		CString filePathComplete = CUtils::BuildPath(filePath, fileName + L".xml");

		CString source = oProp.GetParameter(L"Source").GetValue();
		if (source == L"selectedModel" || source == L"selectedObjects")
		{
			Selection selection = app.GetSelection();
			if (selection.GetCount() == 0)
			{
				app.LogMessage(L"Select Model or Objects!", siErrorMsg);
				return false;
			}
		}

		CValue increment = oProp.GetParameter(L"ShouldIncrement").GetValue();
		CValue isPolymesh = oProp.GetParameter(L"isPolymesh").GetValue();
		CValue isLight = oProp.GetParameter(L"isLight").GetValue();
		CValue isCamera = oProp.GetParameter(L"isCamera").GetValue();
		CValue width = oProp.GetParameter(L"Width").GetValue();
		CValue height = oProp.GetParameter(L"Height").GetValue();
		CString camera = oProp.GetParameter(L"Camera").GetValue();
		CValue shouldExportGeometry = oProp.GetParameter(L"ShouldExportGeometry").GetValue();
		CValue isPointClouds = oProp.GetParameter(L"isPointClouds").GetValue();
		CValue shouldExportStrands = oProp.GetParameter(L"ShouldExportStrands").GetValue();
		CString integrator = oProp.GetParameter(L"Integrator").GetValue();

		xsiui.CreateInstance(L"XSI.UIToolkit");
		rtn = xsiui.GetProperty(L"ProgressBar");
		bar = rtn;

		bar.PutProperty(L"Value", (LONG)0);
		rtn = bar.GetProperty(L"Value");
		bar.PutProperty(L"CancelEnabled", false);
		bar.PutProperty(L"Visible", true);
		bar.PutProperty(L"StatusText", CValue(L"Create XML document..."));

		XMLDocument* doc = new XMLDocument();
		XMLElement* sceneElement = doc->NewElement("scene");
		sceneElement->SetAttribute("version", "0.5.0");

		XMLElement* integrElement = doc->NewElement("integrator");
		integrElement->SetAttribute(Constants::attrType, integrator.GetAsciiString());
		sceneElement->InsertEndChild(integrElement);


		Mitsuba::Camera* mCamera = new Mitsuba::Camera();
		sceneElement->InsertEndChild(mCamera->WriteCamera(doc, (LONG)width, (LONG)height, (bool)isCamera, camera.GetAsciiString()));
		doc->InsertEndChild(sceneElement);

		Scene oScene = app.GetActiveProject().GetActiveScene();
		CRefArray oMatlibs = oScene.GetMaterialLibraries();

		bar.PutProperty(L"Caption", CValue(L"Export materials"));

		int mcount = 0;
		for (LONG i = 0; i < oMatlibs.GetCount(); i++)
		{
			MaterialLibrary oMatlib = oMatlibs.GetItem(i);
			CRefArray nbMaterials = oMatlib.GetItems();
			mcount += nbMaterials.GetCount();
		}

		bar.PutProperty(L"Maximum", CValue(mcount));
		bar.PutProperty(L"Value", (LONG)0);

		Mitsuba::Material* material = new Mitsuba::Material();
		for (LONG i = 0; i < oMatlibs.GetCount(); i++)
		{
			MaterialLibrary oMatlib = oMatlibs.GetItem(i);
			CRefArray nbMaterials = oMatlib.GetItems();

			for (LONG j = 0; j < nbMaterials.GetCount(); j++)
			{
				bar.PutProperty(L"Value", LONG(i+j));
				XSI::Material mat(nbMaterials.GetItem(j));
				CRefArray useds = mat.GetUsedBy();
				if (useds.GetCount() == 0) continue;

				material->WriteMaterials(doc, sceneElement, mat);
			}
		}
		delete material;

		CRefArray lightArray;
		CRefArray surfArray;
		CRefArray meshArray;
		CRefArray pointCloudsArray;

		if (source == L"allScene")
		{
			if (isLight)
			{
				lightArray = app.GetActiveSceneRoot().FindChildren(L"*", siLightPrimType, CStringArray());
			}

			if (isPolymesh)
			{
				meshArray = app.GetActiveSceneRoot().FindChildren(L"*", siPolyMeshType, CStringArray());
				surfArray = app.GetActiveSceneRoot().FindChildren(L"*", siSrfMeshPrimType, CStringArray());
			}

			if (isPointClouds)
			{
				pointCloudsArray = app.GetActiveSceneRoot().FindChildren(L"*", siPointCloudPrimType, CStringArray());
			}
		}
		else if(source == L"selectedModel")
		{
			Selection selection = app.GetSelection();
			CRef selected = selection.GetItem(0);
			X3DObject obj(selected);
			if(isLight)
			{
				lightArray = obj.FindChildren(L"*", siLightPrimType, CStringArray());
			}

			if(isPolymesh)
			{
				meshArray = obj.FindChildren(L"*", siPolyMeshType, CStringArray());
				surfArray = obj.FindChildren(L"*", siSrfMeshPrimType, CStringArray());
			}

			if(isPointClouds)
			{
				pointCloudsArray = obj.FindChildren(L"*", siPointCloudPrimType, CStringArray());
			}
		}
		else if(source == L"selectedObjects")
		{
			Selection selection = app.GetSelection();
			CRefArray selected = selection.GetArray();

			for(LONG i=0; i < selected.GetCount(); i++)
			{
				X3DObject obj(selected[i]);
				CString stype = obj.GetType();

				if(stype == L"polymsh" && isPolymesh) meshArray.Add(obj);
				else if(stype == L"surfmsh" && isPolymesh) surfArray.Add(obj);
				else if(stype == L"light" && isLight) lightArray.Add(obj);
				else if(stype == L"pointcloud" && isPointClouds) pointCloudsArray.Add(obj);
			}
		}

		if (isLight)
		{
			bar.PutProperty(L"Maximum", lightArray.GetCount());
			bar.PutProperty(L"Value", (LONG)0);
			bar.PutProperty(L"Step", (LONG)1);
			bar.PutProperty(L"Caption", CValue(L"Export lights..."));

			Mitsuba::Light* light = new Mitsuba::Light();
			for (int i = 0; i < lightArray.GetCount(); i++)
			{
				bar.PutProperty(L"Value", (LONG)i);
				light->WriteLights(doc, sceneElement, XSI::Light(lightArray.GetItem(i)));
			}
			delete light;
		}

		bar.PutProperty(L"StatusText", L"");
		if (isPolymesh)
		{
			bar.PutProperty(L"Maximum", surfArray.GetCount() + meshArray.GetCount());
			bar.PutProperty(L"Value", (LONG)0);
			bar.PutProperty(L"Step", (LONG)1);
			bar.PutProperty(L"Caption", CValue(L"Export meshes..."));
		
			Mitsuba::Mesh* mesh = new Mitsuba::Mesh();
			for (LONG i = 0; i < surfArray.GetCount(); i++)
			{
				X3DObject obj(surfArray.GetItem(i));
				bar.Call(L"Increment", rtn);
				bar.PutProperty(L"StatusText", obj.GetName());
				mesh->WriteMeshes(doc, sceneElement, obj, shouldExportGeometry, filePath);
			}

			for (LONG i = 0; i < meshArray.GetCount(); i++)
			{
				X3DObject obj(meshArray.GetItem(i));
				bar.PutProperty(L"StatusText", obj.GetName());
				bar.Call(L"Increment", rtn);
				mesh->WriteMeshes(doc, sceneElement, obj, shouldExportGeometry, filePath);
			}			
			delete mesh;
		}

		bar.PutProperty(L"StatusText", L"");
		//TODO
		/*if(isPointClouds)
		{
			bar.PutProperty(L"Maximum", pointCloudsArray.GetCount());
			bar.PutProperty(L"Value", (LONG)0);
			bar.PutProperty(L"Step", (LONG)1);
			bar.PutProperty(L"Caption", CValue(L"Export particlecloud..."));

			Mitsuba::ParticleCloud* particleCloud = new Mitsuba::ParticleCloud();
			for(LONG i=0; i < pointCloudsArray.GetCount(); i++)
			{
				XSI::ParticleCloud pointCloud(pointCloudsArray[i]);
				bar.Call(L"Increment", rtn);
				bar.PutProperty(L"StatusText", pointCloud.GetName());

				particleCloud->WritePointCloud(doc, sceneElement, pointCloud, shouldExportGeometry, filePath);				
			}
			delete particleCloud;
		}*/

		bar.PutProperty(L"StatusText", L"");

		doc->SaveFile(filePathComplete.GetAsciiString());
		delete doc;

		delete mCamera;

		bar.PutProperty(L"Visible", false);

		app.LogMessage(L"Export is done!");

		return true;
	}

private:
	Application app;
	CComAPIHandler xsiui;
	CComAPIHandler bar;
	CValue rtn;
};

SICALLBACK MaSsExport_Init(CRef& in_ctxt)
{
	Context ctxt(in_ctxt);
	Command oCmd;
	oCmd = ctxt.GetSource();
	oCmd.PutDescription(L"Export scene to Mitsuba.");
	oCmd.PutTooltip(L"Export scene to Mitsuba.");
	oCmd.SetFlag(siSupportsKeyAssignment, false);
	oCmd.EnableReturnValue(true);

	ArgumentArray oArgs;
	oArgs = oCmd.GetArguments();
	oArgs.Add(L"IncrementValue", -1);

	return CStatus::OK;
}

SICALLBACK MaSsExport_Execute(CRef& in_ctxt)
{
	Context ctxt(in_ctxt);
	CValueArray args = ctxt.GetAttribute(L"Arguments");
	CValue IncrementValue = args[0];

	Application().LogMessage(L"MaSsExport_Execute called", siVerboseMsg);
	// 
	// TODO: Put your command implementation here.

	Application().LogMessage(L"Start Export!");

	MaSsExport* massExport = new MaSsExport();
	Property oProp;
	massExport->GetPropertyMaSs(oProp);

	if (oProp.IsValid())
	{
		massExport->GenerateXMLScene(oProp, IncrementValue);
	}
	else
	{
		Application().LogMessage(L"MaSs property not found!", siErrorMsg);
	}
	// 
	// Return a value by setting this attribute:
	ctxt.PutAttribute(L"ReturnValue", true);


	delete massExport;

	// Return CStatus::Fail if you want to raise a script error
	return CStatus::OK;
}




