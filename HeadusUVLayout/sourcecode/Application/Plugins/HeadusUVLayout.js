// HeadusUVLayoutPlugin by Svyatoslav Shumikhin
// 26 june 2015
// email: sshumihin@gmail.com
//

var tmpObjName = "tmp.obj";
var tmpCMDName = "tmp.cmd";
var tmpObjImportName = "tmp.out";
var uvApp = "uvlayout.exe";
var app = Application;

function XSILoadPlugin( in_reg ){
	in_reg.Author = "Svyatoslav Shumikhin";
	in_reg.Name = "ss_HeadusUVLayout";
	in_reg.URL = "http://www.softimage.ru";
	in_reg.Email = "sshumihin@gmail.com";
	in_reg.Major = 1;
	in_reg.Minor = 12;
	
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
	
	app.LogMessage(strPluginName + " has been unloaded.",siVerbose);
	return true;
}

function HeadusUVTools_Define( in_ctxt )
{
	var oPSet = in_ctxt.Source;
	oPSet.AddParameter2( "LaunchUVL", siBool, 1, 0, 1,	null, null,	0, 0, "Launch UVLayout");
	oPSet.AddParameter2("Type", siInt4, 0, 0, 2,	null, null,	0, 0, "Type");
	oPSet.AddParameter2("UVs", siInt4, 0, 0, 2,	null, null,	0, 0, "UVs");
	oPSet.AddParameter2( "Weld", siBool, 0, 0, 1,	null, null,	0, 0, "Weld");
	oPSet.AddParameter2( "Clean", siBool, 0, 0, 1,	null, null,	0, 0, "Clean");
	oPSet.AddParameter2( "Detach", siBool, 0, 0, 1,	null, null,	0, 0, "Detach");
	oPSet.AddParameter2( "ReplaceObj", siBool, 0, 0, 1,	null, null,	0, 0, "Replace Objects (try)");
	oPSet.AddParameter2( "UVFix", siBool, 1, 0, 1,	null, null,	0, 0, "Fix UV");
	oPSet.AddParameter2( "UVReplace", siBool, 1, 0, 1,	null, null,	0, 0, "Replace UV");
	oPSet.AddParameter2( "DontDestroyImportedObj", siBool, 0, 0, 1,	null, null,	0, 0, "Don`t Destroy Copy");
	oPSet.AddParameter2( "StaticBitmapControl1", siString );
	oPSet.AddParameter2( "StaticBitmapControl2", siString );

	return true;
}

function HeadusUVTools_OnInit( )
{
	//Application.LogMessage ("StructureExplorer_OnInit called",siVerbose);
	HeadusUVTools_RebuildLayout();
}


function HeadusUVTools_RebuildLayout()
{
	//var oPPGLayout = in_ctxt.Source;
	var oPPGLayout = PPG.PPGLayout;
	var oItem;
	oPPGLayout.Clear();

	oPPGLayout.AddTab("General");
	oPPGLayout.AddGroup("Export Options");
	oPPGLayout.AddItem( "LaunchUVL");
	oItem = oPPGLayout.AddItem( "Type" ); 
	oItem.UIItems = Array( "Poly", 0, "SUBD", 1); 
	oItem.Type = siControlCombo;
	oItem = oPPGLayout.AddItem( "UVs" ); 
	oItem.UIItems = Array( "New", 0, "Edit", 1); 
	oItem.Type = siControlCombo;
	oPPGLayout.AddItem( "Weld");
	oPPGLayout.AddItem( "Clean");
	oPPGLayout.AddItem( "Detach");
	oPPGLayout.EndGroup();

	oPPGLayout.AddGroup("Import Options");
	oPPGLayout.AddStaticText("If you want to import multiply objects \r\nyou should to select scene objects in right order!");
	oPPGLayout.AddItem( "ReplaceObj");
	if(!PPG.ReplaceObj.Value)
	{
		oPPGLayout.AddItem( "DontDestroyImportedObj");
		oPPGLayout.AddItem( "UVReplace");		
		oPPGLayout.AddItem( "UVFix");	
	}
	
	oPPGLayout.EndGroup();

	oItem = oPPGLayout.AddButton("ExportHeadus", "Export Object");
	oItem.SetAttribute(siUICX,250);
	oItem = oPPGLayout.AddButton("ImportHeadus", "Import Object");
	oItem.SetAttribute(siUICX,250);
	oItem = oPPGLayout.AddButton("ExitUVLayout", "Exit UVLayout");
	oItem.SetAttribute(siUICX,250);
	
	oPPGLayout.AddTab("Info");
	oPPGLayout.AddGroup( "Send Cut Edges to UVL" );
	oItem = oPPGLayout.AddItem( "StaticBitmapControl1", "Select edges for cutting and click Export button...", siControlBitmap ) ;

	var p = app.Plugins("ss_HeadusUVLayout");
	var fso = XSIFactory.CreateActiveXObject("Scripting.FileSystemObject");
	var pluginfolder = fso.GetParentFolderName( p.OriginPath );
	var logoPath = fso.BuildPath( pluginfolder, "Logo" );
	
	oItem.SetAttribute(siUIFilePath, logoPath + "\\xsi.bmp")
	
	oItem = oPPGLayout.AddItem( "StaticBitmapControl2", "And that's all!", siControlBitmap ) ;
	oItem.SetAttribute(siUIFilePath, logoPath + "\\uvl.bmp")
	oPPGLayout.EndGroup() ;
	
	PPG.Refresh();

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
	
	app.LogMessage(uvAppFullPath +" "+ oArgs);
	
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
		app.LogMessage(sTempPath + " <-- Created", siVerbose);
	}
	
	
	var oArgs = "-plugin";
	oArgs += (oPrefs.Parameters.Item("Type").Value == 0)?",Poly":",SUBD";
	var isEditUV = oPrefs.Parameters.Item("UVs").Value;
	oArgs += (isEditUV == 0)?",New":",Edit";
	if(oPrefs.Parameters.Item("Weld").Value) oArgs += ",Weld";
	if(oPrefs.Parameters.Item("Clean").Value) oArgs += ",Clean";
	if(oPrefs.Parameters.Item("Detach").Value) oArgs += ",Detach";

	// is layout running ?
	if(layoutProcessAlreadyRunning(uvApp))
	{	
		//send to exit
		var exitPath = XSIUtils.BuildPath(sTempPath, tmpCMDName);
		app.LogMessage("Exit UVLayout: " + exitPath);
		
		ExitUVLayout(uvApp, exitPath);
		app.LogMessage("Sending data to UVLayout...please wait", siInfo);
	}

	// export the obj to prefs.Default_Obj_Name
	var objPath = XSIUtils.BuildPath(sTempPath, tmpObjName);
	//ObjExport( FileName, FilePerObject, FilePerFrame, StartFrame, EndFrame, StepFrame, Polymsh, Surfmsh, Crvlist, Cloud, CoordinateSystem, Tesselation, Material, UV, UserNormal );
	app.ObjExport(objPath, 0, null, null, null, null, null, null, null, null, 0, false, true, isEditUV, false);

	//Write cut edges if we are export a single objecti
	var oSel = Selection(0);
	if(oSel.Type == "edgeSubComponent")
	{
		WriteCutEdges(oPrefs);
	}

	if(oPrefs.Parameters.Item("LaunchUVL").Value)
	{
		// boot up uvlayout
		app.LogMessage("Starting UVLayout...please wait", siInfo);
		UVLayoutLaunch(uvAppFullPath, oArgs, objPath);	
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
		app.LogMessage("Please, select object('s) for import!", siWarning);
		return false;
	}
	
	var oPrefsGlobal = CheckGlobalPrefs();
	var oPrefs = CheckPrefs();
	var sTempPath = oPrefsGlobal.Parameters.Item("TempFolder").Value;
	var uvImportPath = XSIUtils.BuildPath(sTempPath, tmpObjImportName);
	var fso = new ActiveXObject('Scripting.FileSystemObject');
	
	if (!fso.FileExists(uvImportPath))
	{
		app.LogMessage("File " + uvImportPath + " not found!", siWarning);
		return false;
	}

	//ShowSelected();
	var sObjects = new ActiveXObject( "XSI.Collection" );
	var oSel = app.Selection;	
	for(var i=0; i < oSel.Count; i++)
	{
		sObjects.Add(oSel(i));
	}
	
	//ObjImport( FileName, Group, hrc, Material, UV, UserNormal, UVwrapping );
	var iObjects = app.ObjImport(uvImportPath, 1, 0, true, true, false, false);
	//ShowSelected();
	
	if(iObjects.Count != sObjects.Count)
	{
		app.LogMessage("Selected objects count is not equal imported objects count!\nSelected: "
		+ sObjects.Count + ", Imported: " + iObjects.Count, siWarning);
		
		return false;
	}
	
	var isReplaceObj = oPrefs.Parameters.Item("ReplaceObj").Value;
	var isDontDestroy = oPrefs.Parameters.Item("DontDestroyImportedObj").Value;
	var isFixUV = oPrefs.Parameters.Item("UVFix").Value;
	var oColl = new ActiveXObject( "XSI.Collection" );
	
	for(var i=0; i < iObjects.Count; i++)
	{
		//app.LogMessage("Name: " + iObjects(i).Name);
		if(isReplaceObj)
		{
			var obj = sObjects(i);
			app.LogMessage("Name: " + obj.Name);
			var name = obj.Name;
			obj.Name += "_del";
			
			//app.DeleteObj( obj );
			oColl.Add(obj);
			iObjects(i).Name = name;
		}
		else
		{
			CopyUVs(oPrefs, iObjects(i), sObjects(i) );
			if(!isDontDestroy)
			{				
				//app.DeleteObj( iObjects(i) );
				oColl.Add( iObjects(i) );
			}
			
			if(isFixUV)
			{
				FixUVs( sObjects(i) );
			}
		}		
	}
	
	for(var i=0; i < oColl.Count; i++)
	{
		app.DeleteObj(oColl(i));
	}
	
	app.LogMessage("Import is done!", siInfo);

	return true;
}

function ShowSelected()
{
	var oSel = app.Selection;
	for(var i =0; i < oSel.Count; i++)
	{
		app.LogMessage("Selected: " + i + ", " + oSel(i).Name);
	}
}

function UVLayoutLaunch(uvApp, oArgs, FileSpec)
{
	var cmd = uvApp + " " + oArgs + " " + FileSpec;
	app.Logmessage("This command line will be executed:\n" + cmd)
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
		app.LogMessage( "No UVs on source object " + oSrc.Name );
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
		if(oSampleDestUV == null)
		{
			oSampleDestUV = oSampleDest.AddProperty("Texture Projection", false, "UVL_Property");
		}
	}
	else
	{
		oSampleDestUV = oSampleDest.AddProperty("Texture Projection", false, "UVL_Property");
	}
	
	var oSrcUV = oSampleSrc.LocalProperties(0);
	//Application.CopyPaste( oSrcUV, "", oSampleDestUV );
	app.CopyUVW(oSrcUV);
	app.PasteUVW(oSampleDestUV);
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

function HeadusUVTools_ReplaceObj_OnChanged()
{
	HeadusUVTools_RebuildLayout();
}


function HeadusUVTools_ExportHeadus_OnClicked()
{
	if(Selection.Item(0) == null)
	{
		app.LogMessage("Please, Select Object for Export!");
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
	var oPrefsGlobal = CheckGlobalPrefsFirst();
	if(oPrefsGlobal == null) return false;
	
	sTempPath = oPrefsGlobal.Parameters.Item("TempFolder").Value;
	var cmdPath = XSIUtils.BuildPath(sTempPath, tmpCMDName);
	ExitUVLayout(uvApp, cmdPath);
	
	return true;
}
function ExitUVLayout(uvApp, cmdPath)
{
	if(!layoutProcessAlreadyRunning(uvApp)) return false;
	
	var fso = new ActiveXObject('Scripting.FileSystemObject');
	var tf = fso.CreateTextFile(cmdPath, true);
	tf.WriteLine("exit");
	tf.Close();

	while(layoutProcessAlreadyRunning(uvApp))
	{
		continue;
	}
	
	app.LogMessage("UVLayout is Closed!");
	
	return true;
}

function WriteCMDFile(cutPath)
{
	var fso = new ActiveXObject('Scripting.FileSystemObject');
	var cmdPath = XSIUtils.BuildPath(sTempPath, tmpCMDName);
	var tf = fso.CreateTextFile(cmdPath, true);	
	tf.WriteLine("cut " + cutPath);
	tf.Close();
	
	app.LogMessage("CMD file is writed!");
	
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
	app.LogMessage("Edges: " + sEdges);
	
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