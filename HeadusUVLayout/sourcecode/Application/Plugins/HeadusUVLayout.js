// HeadusUVLayoutPlugin by Svyatoslav Shumikhin

var tmpObjName = "tmp.obj";
var tmpCMDName = "tmp.cmd";
var uvApp = "uvlayout.exe";

function XSILoadPlugin( in_reg ){
	in_reg.Author = "Svyatoslav Shumikhin";
	in_reg.Name = "ss_HeadusUVLayout";
	in_reg.URL = "http://www.softimage.ru";
	in_reg.Major = 1;
	in_reg.Minor = 9;
	
	//RegistrationInsertionPoint - do not remove this line	
	in_reg.RegisterProperty("HeadusUVTools");
	in_reg.RegisterCommand("ExportHeadus","ExportHeadus");
	in_reg.RegisterCommand("ImportHeadus","ImportHeadus");
	in_reg.RegisterMenu(siMenuMainTopLevelID ,"Headus UVTools",true,true);

	return true;
}

function HeadusUVTools_Init( in_ctxt )
{
	var oMenu;
	oMenu = in_ctxt.Source;
	var oSub1 = oMenu.AddCommandItem("Export UV Mesh","ExportHeadus");	
	var oSub2 = oMenu.AddCommandItem("Import UV Mesh","ImportHeadus");
	oSub2 = oMenu.AddSeparatorItem();
	oSub2 = oMenu.AddCallbackItem("Options","OpenOptions");
	oSub2 = oMenu.AddCallbackItem("Global Options","OpenGlobalOptions");	
	return true;
}

function XSIUnloadPlugin( in_reg ){
	var strPluginName;
	strPluginName = in_reg.Name;
	
	Application.LogMessage(strPluginName + " has been unloaded.",siVerbose);
	return true;
}

function HeadusUVTools_Define( in_ctxt )
{
	var oPSet = in_ctxt.Source;
	oPSet.AddParameter2("Type", siInt4, 0, 0, 2,	null, null,	0, 0, "Type");
	oPSet.AddParameter2("UVs", siInt4, 0, 0, 2,	null, null,	0, 0, "UVs");
	oPSet.AddParameter2( "Weld", siBool, 0);
	oPSet.AddParameter2( "Clean", siBool, 0);
	oPSet.AddParameter2( "Detach", siBool, 0);
	oPSet.AddParameter2( "UVFix", siBool, 0, 0, 1,	null, null,	0, 0, "Fix UV");
	oPSet.AddParameter2( "UVReplace", siBool, 0, 0, 1,	null, null,	0, 0, "UV Replace");
	oPSet.AddParameter2( "DontDestroyImportedObj", siBool, 0, 0, 1,	null, null,	0, 0, "Dont Destroy Imported Object");

	return true;
}

function HeadusUVTools_DefineLayout( in_ctxt )
{
	var oPPGLayout = in_ctxt.Source;
	oPPGLayout.Clear();

	oPPGLayout.AddGroup("Export Options");
	var oItem = oPPGLayout.AddItem( "Type" ); 
	oItem.UIItems = Array( "Poly", 0, "SUBD", 1); 
	oItem.Type = siControlCombo;
	oItem = oPPGLayout.AddItem( "UVs" ); 
	oItem.UIItems = Array( "New", 0, "Edit", 1); 
	oItem.Type = siControlCombo;
	oPPGLayout.AddItem( "Weld", "Weld");
	oPPGLayout.AddItem( "Clean", "Clean");
	oPPGLayout.AddItem( "Detach", "Detach");
	oPPGLayout.EndGroup();

	oPPGLayout.AddGroup("Import Options");
	oPPGLayout.AddItem( "DontDestroyImportedObj", "Don`t Destroy Copy");
	oPPGLayout.AddItem( "UVFix", "UV fix");
	oPPGLayout.AddItem( "UVReplace");
	oPPGLayout.EndGroup();

	oItem = oPPGLayout.AddButton("ExportHeadus", "Export Object");
	oItem.SetAttribute(siUICX,250);
	oItem = oPPGLayout.AddButton("ImportHeadus", "Import Object");
	oItem.SetAttribute(siUICX,250);
	oItem = oPPGLayout.AddButton("ExitUVLayout", "Exit UVLayout");
	oItem.SetAttribute(siUICX,250);

	return true;
}

// ExportHeadus COMMAND
//************************************************
function ExportHeadus_Init( in_ctxt ){
	var oCmd;
	oCmd = in_ctxt.Source;
	oCmd.Description = "";
	oCmd.SetFlag(siCannotBeUsedInBatch,true);
	oCmd.ReturnValue = true;
	
	return true;
}

function ExportHeadus_Execute( )
{
	if(Selection(0) == null)
	{
		LogMessage("Please, Select Object for Import!");
		return false;
	}
	
	var oPrefsGlobal = CheckGlobalPrefsFirst();
	if(oPrefsGlobal == null) return false;
	
	var oPrefs = CheckPrefs();

	sTempPath = oPrefsGlobal.Parameters.Item("TempFolder").Value;
	
	// check/get preferences
	var oParam = oPrefsGlobal.Parameters.Item("HeadusLocation");
	var uvAppFullPath = XSIUtils.BuildPath(oParam.Value, uvApp);
	
	LogMessage(uvAppFullPath +" "+ oArgs);
	
	var fso = new ActiveXObject('Scripting.FileSystemObject');	
	if (!fso.FileExists(uvAppFullPath))
	{
		Application.LogMessage("Please check the UV Layout Home Path is correctly set!", siWarning);
		return false;
	}
  
	// do we have a folder to store stuff in ?
	if( !fso.FolderExists(sTempPath))
	{
		sTempPath = fso.CreateFolder(sTempPath);
		LogMessage(sTempPath + " <-- Created", siVerbose);
	}
	
	
	var oArgs = "-plugin";
	oArgs += (oPrefs.Parameters.Item("Type").Value == 0)?",Poly":",SUBD";
	oArgs += (oPrefs.Parameters.Item("UVs").Value == 0)?",New":",Edit";
	if(oPrefs.Parameters.Item("Weld").Value) oArgs += ",Weld";
	if(oPrefs.Parameters.Item("Clean").Value) oArgs += ",Clean";
	if(oPrefs.Parameters.Item("Detach").Value) oArgs += ",Detach";

	// is layout running ?
	if(layoutProcessAlreadyRunning(uvApp))
	{	
		//send to exit
		var exitPath = XSIUtils.BuildPath(sTempPath, tmpCMDName);
		LogMessage("Exit UVLayout: " + exitPath);
		
		ExitUVLayout(uvApp, exitPath);
		Application.LogMessage("Sending data to UVLayout...please wait", siInfo);
	}

	// export the obj to prefs.Default_Obj_Name
	var objPath = XSIUtils.BuildPath(sTempPath, tmpObjName);
	Application.ObjExport(objPath, null, null, null, null, null, null, null, null, null, 0, false, false, null, false);
						
	// boot up uvlayout
	Application.LogMessage("Starting UVLayout...please wait", siInfo);
	UVLayoutLaunch(uvAppFullPath, oArgs, objPath);
	
	var oSel = Selection(0);
	if(oSel.Type == "edgeSubComponent")
	{
		WriteCutEdges(oPrefs);
	}
	
	return true;
}

//**************************************************
// ImportHeadus COMMAND
//************************************************
function ImportHeadus_Init( in_ctxt )
{
	var oCmd;
	oCmd = in_ctxt.Source;
	oCmd.Description = "";
	oCmd.SetFlag(siCannotBeUsedInBatch,true);
	oCmd.ReturnValue = true;

	return true;
}

function ImportHeadus_Execute( )
{
	if(Selection.Count == 0)
	{
		Application.LogMessage("Please, select object for replace!", siWarning);
		return false;
	}
	
	var oObject = Selection(0);
	if(Selection(0).Type != "polymsh")
	{
		oObject = oObject.SubComponent.Parent3DObject;
	}
	
	var oPrefsGlobal = CheckGlobalPrefs();
	var oPrefs = CheckPrefs();
	var sTempPath = oPrefsGlobal.Parameters.Item("TempFolder").Value;
	var uvImportPath = XSIUtils.BuildPath(sTempPath, "tmp.out");
	var fso = new ActiveXObject('Scripting.FileSystemObject');
	
	if (fso.FileExists(uvImportPath))
	{
		var iObj = Application.ObjImport(uvImportPath, 1, 0, false, true, false, false)(0);
		Application.LogMessage(oObject.Name + " import Successful", siInfo);	
	
		CopyUVs(oPrefs, iObj, oObject );
		if(!oPrefs.Parameters.Item("DontDestroyImportedObj").Value)
		{
			Application.DeleteObj( iObj );
		}
		if(oPrefs.Parameters.Item("UVfix").Value)
		{
			FixUVs( oObject );
		}
		Application.SelectObj( oObject );
	}
	else
	{
		Application.LogMessage("File not found!", siWarning);
		return false;
	}
	return true;
}

function UVLayoutLaunch(uvApp, oArgs, FileSpec)
{
	var cmd = uvApp + " " + oArgs + " " + FileSpec;
	Logmessage("This command line will be executed:\n" + cmd)
	XSIUtils.LaunchProcess( cmd, false) ;
	
	return true;
}

function layoutProcessAlreadyRunning(app)
{
	var iReturn = 0x10;
	var goForth = 0x20;
	var objWMIService = GetObject("winmgmts:\\\\.\\root\\CIMV2");
	
	// not the best idea ever but WQL WHERE was driving me mad
	var oColl = objWMIService.ExecQuery("SELECT * FROM Win32_Process", "WQL", iReturn | goForth);

	var e = new Enumerator(oColl);
	for (; !e.atEnd(); e.moveNext() ) 
	{
		var oItem = e.item();

		if(oItem.Name == app)
		{
			return true;
			/*
			Logmessage("Name: " + oItem.Name);
			Logmessage("ProcId: " + oItem.ProcessId);
			Logmessage("ParProcId: " + oItemm.ParentProcessId);
			*/
		}
	}
	return false;
}
function CopyUVs(oPrefs, oSrc, oDest )
{
	//first off, look for the uvs on the source obj
	var oSampleSrc = null;
	var oClusters = oSrc.ActivePrimitive.Geometry.Clusters;
	
	for ( var i = 0; i < oClusters.Count; i++ )
	{
		var oCls = oClusters(i);
		if ( oCls.Type == siSampledPointCluster )
		{
			oSampleSrc = oCls;
			break;
		}
	}

	if ( oSampleSrc == null)
	{
		Application.LogMessage( "No UVs on source object " + oSrc.Name );
		return false;
	}

	//look for a sample cluster on the destination and create one if it's not there.
	var oSampleDest = null;
	
	oClusters = oDest.ActivePrimitive.Geometry.Clusters;
	for ( var i = 0; i < oClusters.Count; i++ )
	{
		var oCls = oClusters(i);
		if ( oCls.type == siSampledPointCluster )
		{
			oSampleDest = oCls;
			break;
		}
	}

	var oSampleDestUV;
	if ( oSampleDest == null)
	{	
		//oSampleDest = Application.SICreateCluster( siSampledPointCluster, "Texture_Coordinates_UVL", oDest, 1+4 )(0);
		oSampleDest  = oDest.ActivePrimitive.Geometry.AddCluster(  siSampledPointCluster, "Texture_Coordinates_UVL");		
	}
	
	if(oPrefs.Parameters.Item("UVReplace").Value)
	{
		oSampleDestUV = oSampleDest.LocalProperties(0);
	}
	else
	{
		oSampleDestUV = oSampleDest.AddProperty("Texture Projection", false, "UVL_Property");
	}
	
	var oSrcUV = oSampleSrc.LocalProperties(0);
	//Application.CopyPaste( oSrcUV, "", oSampleDestUV );
	Application.CopyUVW(oSrcUV);
	Application.PasteUVW(oSampleDestUV);
}

function FixUVs( oObj )
{
	var oClusters = oObj.ActivePrimitive.geometry.clusters;

	// Look for our sample cluster
	for(var i=0; i < oClusters.count; i++)
	{
		var oCls = oClusters(i);
		if ( oCls.type == siSampledPointCluster )
		{
			var oCluster = oCls;
			break;
		}
	}
	
	var oUVspace = oCluster.LocalProperties(oCluster.LocalProperties.count-1);

	var aSamples = new Array();

	// build the lut
	var oFacets = oObj.ActivePrimitive.Geometry.Facets;
	
	var oFacet;
	var oSamples;
	for( var i=0; i < oFacets.Count; i++)
	{
		oFacet = oFacets.item(i);
		oSamples = oFacet.Samples;
		
		for( var j=0; j < oSamples.Count; j++)
		{
			var oSample = oSamples.item(j);
			aSamples.push( oSample.Index );
		}
	}
	
	var vbaElements = new VBArray(oUVspace.Elements.Array);
	var vbaIndices = new VBArray(oCluster.Elements.Array);
	var aElements = vbaElements.toArray();
	var aIndices = vbaIndices.toArray();

	var aFixed = [aElements.length];

	// and rebuild the uvs
	for(var i=0; i < aIndices.length; i++)
	{
		var idx = aSamples[aIndices[i]];
		aFixed[idx*3] = aElements[i*3];
		aFixed[idx*3+1] = aElements[i*3+1];
		aFixed[idx*3+2] = aElements[i*3+2];
	}
	
	oUVspace.Elements.Array = aFixed;
}

function HeadusUVTools_ExportHeadus_OnClicked()
{
	if(Selection.Item(0) == null)
	{
		LogMessage("Please, Select Object for Export!");
		return false;
	}
	
	ExportHeadus();
	
	return true;
}

function HeadusUVTools_ImportHeadus_OnClicked()
{	
	ImportHeadus();
	
	return true;
}

function HeadusUVTools_ExitUVLayout_OnClicked()
{
	sTempPath = PPG.TempFolder.Value;
	var cmdPath = XSIUtils.BuildPath(sTempPath, tmpCMDName);
	ExitUVLayout(uvApp, cmdPath);
	
	return true;
}
function ExitUVLayout(uvApp, cmdPath)
{
	var fso = new ActiveXObject('Scripting.FileSystemObject');
	var tf = fso.CreateTextFile(cmdPath, true);
	tf.WriteLine("exit");
	tf.Close();

	while(layoutProcessAlreadyRunning(uvApp))
	{
		continue;
	}
	
	LogMessage("UVLayout is Closed!");
	
	return true;
}

function WriteCMDFile(cutPath)
{
	var fso = new ActiveXObject('Scripting.FileSystemObject');
	var cmdPath = XSIUtils.BuildPath(sTempPath, tmpCMDName);
	var tf = fso.CreateTextFile(cmdPath, true);	
	tf.WriteLine("cut " + cutPath);
	tf.Close();
	
	LogMessage("CMD file is writed!");
	
	return true;
}

function WriteCutEdges(oPrefs)
{
	var fileEdges = "cutedges.txt";
	var oSel = Selection;
	if(oSel(0).Type != "edgeSubComponent") return false;
	
	var cutEdges = "";
	var sEdges = "";
	var vertCount = 0;
	for(var i=0; i < oSel.Count; i++)
	{
		if(i>0)
		{
			var oObj = oSel(i-1).SubComponent.Parent3DObject;
			var oGeometry = oObj.ActivePrimitive.Geometry;
			var oPoints = oGeometry.Points;
			vertCount += oPoints.Count;
		}
		
		var oComp = oSel(i).SubComponent.ComponentCollection;
		for(var j=0; j < oComp.Count; j++)
		{
			var oPnts = oComp(j).Points;
			var tmp = (oPnts(0).Index+1 + vertCount) +"," + (oPnts(1).Index+1 + vertCount) + "\n";
			cutEdges += tmp;
			sEdges += oComp(j).Index + ",";
		}
	}
	LogMessage("Edges: " + sEdges);
	
	var fso = new ActiveXObject('Scripting.FileSystemObject');
	var cutPath = XSIUtils.BuildPath(sTempPath, fileEdges);
	var tf = fso.CreateTextFile(cutPath, true);	
	tf.Write(cutEdges);
	tf.Close();
	
	WriteCMDFile(cutPath);
	
	return true;
}

function OpenGlobalOptions()
{
	var oPrefs = CheckGlobalPrefs();
	InspectObj(oPrefs,null,null,siLock);
}

function CheckGlobalPrefsFirst()
{
	var oPrefs = Application.Preferences;
	var oPrefsUVL = oPrefs.Categories("HeadusUVLayoutOptions");
	if (oPrefsUVL == null)
	{
		oPrefsUVL = ActiveSceneRoot.AddProperty("HeadusUVLayoutOptions");
		var bCancelled = InspectObj (oPrefsUVL, null, "Select path", siModal, false);
		if ( !bCancelled )
		{
			InstallCustomPreferences(oPrefsUVL);
			oPrefsUVL = oPrefs.Categories("HeadusUVLayoutOptions");
			OpenOptions();
		}
		else
		{
			DeleteObj(oPrefs);
			return null;
		}
	}
	return oPrefsUVL;
}

function CheckGlobalPrefs()
{
	var oPrefs = Application.Preferences;
	var oPrefsUVL = oPrefs.Categories("HeadusUVLayoutOptions");
	if (oPrefsUVL == null)
	{
		oPrefsUVL = ActiveSceneRoot.AddProperty("HeadusUVLayoutOptions");
		InstallCustomPreferences(oPrefsUVL);
		oPrefsUVL = oPrefs.Categories("HeadusUVLayoutOptions");
	}
	return oPrefsUVL;
}

function OpenOptions()
{
	var oPrefs = CheckPrefs();
	InspectObj(oPrefs,null,null,siLock);
}

function CheckPrefs()
{
	var oPrefs = Application.ActiveSceneRoot.Properties.Item("HeadusUVTools");
	if (oPrefs == null)
	{
		oPrefs = ActiveSceneRoot.AddProperty("HeadusUVTools");
		OpenGlobalOptions();
	}
	return oPrefs;
}