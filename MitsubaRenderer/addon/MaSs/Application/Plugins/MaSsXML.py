import win32com
from win32com.client import constants as c
#from lxml import etree
try:
  from lxml import etree
  #print("running with lxml.etree")
except ImportError:
  try:
    # Python 2.5
    import xml.etree.cElementTree as etree
    #print("running with cElementTree on Python 2.5+")
  except ImportError:
    try:
      # Python 2.5
      import xml.etree.ElementTree as etree
      #print("running with ElementTree on Python 2.5+")
    except ImportError:
      try:
        # normal cElementTree install
        import cElementTree as etree
        #print("running with cElementTree")
      except ImportError:
        try:
          # normal ElementTree install
          import elementtree.ElementTree as etree
          #print("running with ElementTree")
        except ImportError:
          print("Failed to import ElementTree from any known place")

from win32com.client.dynamic import Dispatch
ap = win32com.client.Dispatch( 'XSI.Application' ).Application
XSIFactory = win32com.client.Dispatch( "XSI.Factory" )
lm = ap.LogMessage
XSIMath = Dispatch( 'XSI.Math' )


def indent(elem, level=0):
  i = "\n" + level*"  "
  if len(elem):
    if not elem.text or not elem.text.strip():
      elem.text = i + "  "
    if not elem.tail or not elem.tail.strip():
      elem.tail = i
    for elem in elem:
      indent(elem, level+1)
    if not elem.tail or not elem.tail.strip():
      elem.tail = i
  else:
    if level and (not elem.tail or not elem.tail.strip()):
      elem.tail = i
	
def PrintPropsOfSelectedObj():
	for i in range(0,ap.Selection.Count):
		oObj = ap.Selection(i)
		ap.LogMessage( "%s has %s properties" % \
			( oObj.Name, oObj.Properties.Count ) )



def GenerateXMLScene(isMesh, isLight, isCamera, width, height, cameraName, shouldExport, isPointClouds, shouldExportStrands, exportPath, strSource, integratorStr):
	strOut = ""
	

	root = etree.Element("scene")
	root.set("version", "0.5.0")

	root.append(GenerateIntegratorSegment(integratorStr))
	
	#add camera segment
	root.append(GenerateCameraSegment(width, height, isCamera, cameraName))
	
	#next gether information of materials
	#then we apply these materials to objects
	matLibs = ap.ActiveProject.ActiveScene.MaterialLibraries
	for mLib in matLibs:
		#enumerate all librarys
		for mat in mLib.Items:
			#the id of the mat is it's name
			#check is materail has Mitsuba shaders
			if IsMatCorrect(mat):
				root.append(GetMMaterial(mat, mLib.Name))
			elif IsSSSMatCorrect(mat):
				#append twise. One for plastic, other for dipole
				root.append(WriteSSSShader(mat.Shaders[0], mLib.Name + "." + mat.Name, True, False))
				root.append(WriteSSSShader(mat.Shaders[0], mLib.Name + "." +  "SSS" + mat.Name, True, True))
		
	#determ lists of exported items
	lightArray = []
	surfArray = []
	meshArray = []
	pointClouds = []
	if strSource == "allScene":
		if isLight:
			lightArray = ap.ActiveSceneRoot.FindChildren("*", "light")
		else:
			lightArray = []
		if isMesh:
			surfArray = ap.ActiveSceneRoot.FindChildren("*", "surfmsh")
			meshArray = ap.ActiveSceneRoot.FindChildren("*", "polymsh")
		else:
			surfArray = []
			meshArray = []
		if isPointClouds:
			pointClouds = ap.ActiveSceneRoot.FindChildren("*", "pointcloud")
		else:
			pointClouds = []

	elif strSource == "selectedModel":
		selection = ap.Selection(0)
		if isLight:
			lightArray = selection.FindChildren("", "light")
		else:
			lightArray = []
		if isMesh:
			surfArray = selection.FindChildren("", "surfmsh")
			meshArray = selection.FindChildren("", "polymsh")
		else:
			surfArray = []
			meshArray = []
		if isPointClouds:
			pointClouds = selection.FindChildren("", "pointcloud")
		else:
			pointClouds = []
	elif strSource == "selectedObjects":
		#find objects in selected family
		selections = ap.Selection
		lightArray = []
		surfArray = []
		meshArray = []
		pointClouds = []
		for obj in selections:
			if obj.Type == "polymsh":
				if isMesh:
					meshArray.append(obj)
			elif obj.Type == "surfmsh":
				if isMesh:
					surfArray.append(obj)
			elif obj.Type == "light":
				if isLight:
					lightArray.append(obj)
			elif obj.Type == "pointcloud":
				if isPointClouds:
					pointClouds.append(obj)
	else:
		lightArray = []
		surfArray = []
		meshArray = []
		pointClouds = []

	if isLight:
		#add light segment
		for l in lightArray:
			if ap.GetValue(l.FullName + ".visibility.rendvis") == True:
				if CheckCorrecLight(l):
					root.append(GenerateLightSegment(l))
	if isMesh:
		##add meshes
		#at first primitives
		for surf in surfArray:
			if ap.GetValue(surf.FullName + ".visibility.rendvis") == True:
				if isSurfCorrect(surf):
					root.append(GenerateMeshSegment(surf, True, ""))
		
		for mesh in meshArray:
			if ap.GetValue(mesh.FullName + ".visibility.rendvis") == True:
				if isMeshCorrect(mesh):
					root.append(GenerateMeshSegment(mesh, shouldExport, exportPath))

	if isPointClouds:
		for pointCloud in pointClouds:
			if ap.GetValue(pointCloud.FullName + ".visibility.rendvis") == True:
				segment = GeneratePointCloudSegment(pointCloud, shouldExportStrands, exportPath)
				if segment != None:
					for seg in segment:
						root.append(seg)

	
	#strOut = etree.tostring(root, pretty_print=True, encoding='utf-8')
	indent(root)
	strOut = etree.tostring(root, encoding='utf-8', method='xml')

	
	return strOut


def GenerateIntegratorSegment(integratorStr):
	root = etree.Element("integrator")
	root.set("type", integratorStr)
	return root

def IsMatCorrect(mat):
	if mat.Shaders.Count > 0:
		if mat.Shaders(0).Parameters("mShaderType") != None:
			shader = mat.Shaders(0).Parameters("mShaderType").Value
			return IsShaderCorrect(shader, mat.Shaders(0))
		else:
			return False
	else:
		return False
	return False

def IsSSSMatCorrect(mat):
	if mat.Shaders.Count > 0:
		if mat.Shaders(0).Parameters("mShaderType") != None:
			shader = mat.Shaders(0).Parameters("mShaderType").Value
			return isSSSShaderCorrect(shader)
		else:
			return False
	else:
		return False
	return False

def IsMeshLight(mat):
	if mat.Shaders.Count > 0:
		if mat.Shaders(0).Parameters("mShaderType") != None:
			if mat.Shaders(0).Parameters("mShaderType").Value == "mEmitter":
				return True
			else:
				return False
		else:
			return False
	else:
		return False



def isSSSShaderCorrect(shader):
	if shader == "mSSSDipole" or shader == "mRoughSSSDipole":
		return True
	else:
		return False

def IsShaderCorrect(shader, xsiShader):
	if shader == "mDiffuse" or shader == "mRoughDiffuse" or shader == "mDielectric" or shader == "mThinDielectric" or shader == "mRoughDielectric" or shader == "mConductor" or shader == "mRoughConductor" or shader == "mPlastic" or shader == "mRoughPlastic" or shader == "mCoating" or shader == "mRoughCoating" or shader == "mBumpMap" or shader == "mPhong" or shader == "mWard" or shader == "mMixtureBsdf" or shader == "mBlendBsdf" or shader == "mMask" or shader == "mTwoSided" or shader == "mDiffTrans" or shader == "mHk":
		if shader == "mBumpMap":
			if xsiShader.Parameters("texture") != None:
				if xsiShader.Parameters("texture").Sources.Count > 0:
					if xsiShader.Parameters("texture").Sources[0].Parent.Parameters("mShaderType") != None:
						shaderType = xsiShader.Parameters("texture").Sources[0].Parent.Parameters("mShaderType").Value
						if shaderType == "mBitmap" or shaderType == "mCheckboard" or shaderType == "mGridTexture" or shaderType == "mScale" or  shaderType == "mVertexColors" or shaderType == "mWireframe" or shaderType == "mCurvature":
							return True
						else: 
							return False
					else:						
						if len(xsiShader.Parameters("texture").Sources[0].Parent.Parameters("tex").Sources) == 1:
							return True
						else:
							return False
				else:
					return False
			else:
				return False
		return True
	else:
		return False

def GetMMaterial(mat, libName):
	#start write 0 shader
	root = WriteShader(mat.Shaders(0), libName + "." +  mat.Name, True)
	return root

def WriteSSSShader(shader, name, useId, isSSS):
	matName = shader.Parameters("mShaderName").Value
	if matName == "roughsssdipole":
		if isSSS:
			root = etree.Element("subsurface")
			if useId:
				root.set("id", name)
			root.set("type", "dipole")
			root.append(WriteMaterial(shader.Parameters("material").Value))
			root.append(GenerateFloatSegment(shader.Parameters("scale").Value, "scale"))
			root.append(GenerateIntegerSegment(shader.Parameters("irrSamples").Value, "irrSamples"))
			#write dipole segment
		else:
			#write other segment
			root = etree.Element("bsdf")
			if useId:
				root.set("id", name)
			root.set("type", "roughplastic")
			root.append(WriteDistribution(shader.Parameters("distribution").Value))
			root.append(WriteFloatOrTexture(shader.Parameters("alpha"), "alpha"))
			root.append(WriteIOR(True, shader.Parameters("intIOR").Value))
			root.append(WriteIOR(False, shader.Parameters("extIOR").Value))
			root.append(WriteTextureOrSpectrum(shader.Parameters("specularReflectance"), "specularReflectance"))
			spec = etree.SubElement(root, "spectrum")
			spec.set("name", "diffuseReflectance")
			spec.set("value", "0")
			b = etree.SubElement(root, "boolean")
			b.set("name", "nonlinear")
			b.set("value", FromBoolToboolString(shader.Parameters("nonlinear").Value))

	elif matName == "sssdipole":
		if isSSS:
			root = etree.Element("subsurface")
			if useId:
				root.set("id", name)
			root.set("type", "dipole")
			root.append(WriteMaterial(shader.Parameters("material").Value))
			root.append(GenerateFloatSegment(shader.Parameters("scale").Value, "scale"))
			root.append(GenerateIntegerSegment(shader.Parameters("irrSamples").Value, "irrSamples"))
			#write dipole segment
		else:
			#write other segment
			root = etree.Element("bsdf")
			if useId:
				root.set("id", name)
			root.set("type", "plastic")
			root.append(WriteIOR(True, shader.Parameters("intIOR").Value))
			root.append(WriteIOR(False, shader.Parameters("extIOR").Value))
			root.append(WriteTextureOrSpectrum(shader.Parameters("specularReflectance"), "specularReflectance"))
			spec = etree.SubElement(root, "spectrum")
			spec.set("name", "diffuseReflectance")
			spec.set("value", "0")
			b = etree.SubElement(root, "boolean")
			b.set("name", "nonlinear")
			b.set("value", FromBoolToboolString(shader.Parameters("nonlinear").Value))
	else:
		#write standard diffuse
		root = WriteStandardDiffuse()
		if useId:
			root.set("id", name)

	return root
	

	

def WriteShader(shader, name, useId):
	matName = shader.Parameters("mShaderName").Value
	lm("material name: " + matName)
	root = etree.Element("bsdf")
	root.set("type", shader.Parameters("mShaderName").Value)
	if useId:
		root.set("id", name)
	
	if matName == "diffuse":
		#write color or texture link
		root.append(WriteTextureOrSpectrum(shader.Parameters("reflectance"), "reflectance"))
		
	elif  matName == "roughdiffuse":
		root.append(WriteTextureOrSpectrum(shader.Parameters("reflectance"), "reflectance"))
		root.append(WriteFloatOrTexture(shader.Parameters("alpha"), "alpha"))
		b = etree.SubElement(root, "boolean")
		b.set("name", "useFastApprox")
		b.set("value", FromBoolToboolString(shader.Parameters("useFastApprox").Value))
	elif matName == "dielectric":
		root.append(WriteIOR(True, shader.Parameters("intIOR").Value))
		root.append(WriteIOR(False, shader.Parameters("extIOR").Value))
		root.append(WriteTextureOrSpectrum(shader.Parameters("specularReflectance"), "specularReflectance"))
		root.append(WriteTextureOrSpectrum(shader.Parameters("specularTransmittance"), "specularTransmittance"))
	elif matName == "thindielectric":
		root.append(WriteIOR(True, shader.Parameters("intIOR").Value))
		root.append(WriteIOR(False, shader.Parameters("extIOR").Value))
		root.append(WriteTextureOrSpectrum(shader.Parameters("specularReflectance"), "specularReflectance"))
		root.append(WriteTextureOrSpectrum(shader.Parameters("specularTransmittance"), "specularTransmittance"))
	elif matName == "roughdielectric":
		root.append(WriteDistribution(shader.Parameters("distribution").Value))
		if shader.Parameters("distribution").Value != "as":
			root.append(WriteFloatOrTexture(shader.Parameters("alpha"), "alpha"))
		else:
			root.append(WriteFloatOrTexture(shader.Parameters("alphaU"), "alphaU"))
			root.append(WriteFloatOrTexture(shader.Parameters("alphaV"), "alphaV"))
		root.append(WriteIOR(True, shader.Parameters("intIOR").Value))
		root.append(WriteIOR(False, shader.Parameters("extIOR").Value))
		root.append(WriteTextureOrSpectrum(shader.Parameters("specularReflectance"), "specularReflectance"))
		root.append(WriteTextureOrSpectrum(shader.Parameters("specularTransmittance"), "specularTransmittance"))
	elif matName == "conductor":
		root.append(WriteMaterial(shader.Parameters("material").Value))
		root.append(GenerateFloatSegment(shader.Parameters("extEta").Value, "extEta"))
		root.append(WriteTextureOrSpectrum(shader.Parameters("specularReflectance"), "specularReflectance"))
	elif matName == "roughconductor":
		root.append(WriteDistribution(shader.Parameters("distribution").Value))
		if shader.Parameters("distribution").Value != "as":
			root.append(WriteFloatOrTexture(shader.Parameters("alpha"), "alpha"))
		else:
			root.append(WriteFloatOrTexture(shader.Parameters("alphaU"), "alphaU"))
			root.append(WriteFloatOrTexture(shader.Parameters("alphaV"), "alphaV"))
		root.append(WriteMaterial(shader.Parameters("material").Value))
		root.append(GenerateFloatSegment(shader.Parameters("extEta").Value, "extEta"))
		root.append(WriteTextureOrSpectrum(shader.Parameters("specularReflectance"), "specularReflectance"))
	elif matName == "plastic":
		root.append(WriteIOR(True, shader.Parameters("intIOR").Value))
		root.append(WriteIOR(False, shader.Parameters("extIOR").Value))
		root.append(WriteTextureOrSpectrum(shader.Parameters("specularReflectance"), "specularReflectance"))
		root.append(WriteTextureOrSpectrum(shader.Parameters("diffuseReflectance"), "diffuseReflectance"))
		b = etree.SubElement(root, "boolean")
		b.set("name", "nonlinear")
		b.set("value", FromBoolToboolString(shader.Parameters("nonlinear").Value))
	elif matName == "roughplastic":
		root.append(WriteDistribution(shader.Parameters("distribution").Value))
		root.append(WriteFloatOrTexture(shader.Parameters("alpha"), "alpha"))
		root.append(WriteIOR(True, shader.Parameters("intIOR").Value))
		root.append(WriteIOR(False, shader.Parameters("extIOR").Value))
		root.append(WriteTextureOrSpectrum(shader.Parameters("specularReflectance"), "specularReflectance"))
		root.append(WriteTextureOrSpectrum(shader.Parameters("diffuseReflectance"), "diffuseReflectance"))
		b = etree.SubElement(root, "boolean")
		b.set("name", "nonlinear")
		b.set("value", FromBoolToboolString(shader.Parameters("nonlinear").Value))
	elif matName == "coating":
		root.append(WriteIOR(True, shader.Parameters("intIOR").Value))
		root.append(WriteIOR(False, shader.Parameters("extIOR").Value))
		root.append(GenerateFloatSegment(shader.Parameters("thickness").Value, "thickness"))
		root.append(WriteTextureOrSpectrum(shader.Parameters("sigmaA"), "sigmaA"))
		root.append(WriteTextureOrSpectrum(shader.Parameters("specularReflectance"), "specularReflectance"))
		#determ child BSDF
		root.append(AddChildBsdf(shader, "bsdf"))
	elif matName == "roughcoating":
		root.append(WriteDistribution(shader.Parameters("distribution").Value))
		root.append(WriteFloatOrTexture(shader.Parameters("alpha"), "alpha"))
		root.append(WriteIOR(True, shader.Parameters("intIOR").Value))
		root.append(WriteIOR(False, shader.Parameters("extIOR").Value))
		root.append(GenerateFloatSegment(shader.Parameters("thickness").Value, "thickness"))
		root.append(WriteTextureOrSpectrum(shader.Parameters("sigmaA"), "sigmaA"))
		root.append(WriteTextureOrSpectrum(shader.Parameters("specularReflectance"), "specularReflectance"))
		root.append(AddChildBsdf(shader, "bsdf"))
	elif matName == "bumpmap":
		root.append(AddChildBsdf(shader, "bsdf"))
		if shader.Parameters("texture").Sources(0).Parent.Parameters("tex") != None:
			text1 = etree.SubElement(root, "texture")
			text1.set("type", "scale")
			f1 = etree.SubElement(text1, "float")
			f1.set("name", "scale")
			f1.set("value", str(shader.Parameters("scale").Value))
			text1.append(GenerateTextureSegment(shader.Parameters("texture").Sources(0).Parent.Parameters("tex").Sources[0], ""))
		else:
			text1 = etree.SubElement(root, "texture")
			text1.set("type", "scale")
			f1 = etree.SubElement(text1, "float")
			f1.set("name", "scale")
			f1.set("value", str(shader.Parameters("scale").Value))

			text1.append(WriteBitmapSegment(shader.Parameters("texture").Sources(0).Parent, ""))

	elif matName == "phong":
		root.append(GenerateFloatSegment(shader.Parameters("exponent").Value, "exponent"))
		root.append(WriteTextureOrSpectrum(shader.Parameters("specularReflectance"), "specularReflectance"))
		root.append(WriteTextureOrSpectrum(shader.Parameters("diffuseReflectance"), "diffuseReflectance"))
	elif matName == "ward":
		root.append(WriteVariant(shader.Parameters("variant").Value))
		root.append(WriteFloatOrTexture(shader.Parameters("alphaU"), "alphaU"))
		root.append(WriteFloatOrTexture(shader.Parameters("alphaV"), "alphaV"))
		root.append(WriteTextureOrSpectrum(shader.Parameters("specularReflectance"), "specularReflectance"))
		root.append(WriteTextureOrSpectrum(shader.Parameters("diffuseReflectance"), "diffuseReflectance"))
	elif matName == "mixturebsdf":
		weight = etree.SubElement(root, "string")
		weight.set("name", "weights")
		weight.set("value", str(shader.Parameters("weights").Value) + ", " + str(1 - shader.Parameters("weights").Value))
		root.append(AddChildBsdf(shader, "bsdf1"))
		root.append(AddChildBsdf(shader, "bsdf2"))
	elif matName == "blendbsdf":
		root.append(WriteFloatOrTexture(shader.Parameters("weight"), "weight"))
		root.append(AddChildBsdf(shader, "bsdf1"))
		root.append(AddChildBsdf(shader, "bsdf2"))
	elif matName == "mask":
		root.append(WriteTextureOrSpectrum(shader.Parameters("opacity"), "opacity"))
		root.append(AddChildBsdf(shader, "bsdf"))
	elif matName == "twosided":
		root.append(AddChildBsdf(shader, "bsdf"))
	elif matName == "difftrans":
		root.append(WriteTextureOrSpectrum(shader.Parameters("transmittance"), "transmittance"))
	elif matName == "hk":
		root.append(WriteMaterial(shader.Parameters("material").Value))
		root.append(GenerateFloatSegment(shader.Parameters("thickness").Value, "thickness"))

	return root



def AddChildBsdf(shader, bsdfName):
	if shader.Parameters(bsdfName).Sources.Count != 0:
		if shader.Parameters(bsdfName).Sources(0).Parent.Type == "Shader":
			childBsfd = shader.Parameters(bsdfName).Sources(0).Parent
			if childBsfd.Parameters("mShaderType") != None:
				if IsShaderCorrect(childBsfd.Parameters("mShaderType").Value, childBsfd):
					root = WriteShader(childBsfd, "", False)
				else:
					#add standard diffuse
					root = WriteStandardDiffuse()
			else:
				#add standard diffuse
				root = WriteStandardDiffuse()
		else:
			#add standard diffuse
			root = WriteStandardDiffuse()
	else:
		#add standard diffuse
		root = WriteStandardDiffuse()
	return root

def WriteStandardDiffuse():
	root = etree.Element("bsdf")
	root.set("type", "diffuse")
	return root

def WriteMaterial(matName):
	root = etree.Element("string")
	root.set("name", "material")
	root.set("value", str(matName))
	return root

def WriteDistribution(value):
	root = etree.Element("string")
	root.set("name", "distribution")
	root.set("value", str(value))
	return root

def WriteVariant(value):
	root = etree.Element("string")
	root.set("name", "variant")
	root.set("value", str(value))
	return root

def WriteIOR(ver, value):
	#ver = True - intIOR
	#ver = False - extIOR
	root = etree.Element("float")
	if ver:
		root.set("name", "intIOR")
		root.set("value", str(value))
	else:
		root.set("name", "extIOR")
		root.set("value", str(value))
	return root
	

def WriteTextureOrSpectrum(parameter, paramName):
	if parameter.Sources.Count == 0:
			#no texture connected
			root = GenerateSpectrumSegment(parameter.Value, paramName)
	else:
		#something connect. Check that this is texture
		if parameter.Sources[0].Parent.Parameters("mShaderType") != None:
			if parameter.Sources[0].Parent.Parameters("mShaderType").Value == "mBitmap":
				#this is texture shader
				root = WriteBitmapSegment(parameter.Sources[0].Parent, paramName)
			elif parameter.Sources[0].Parent.Parameters("mShaderType").Value == "mCheckboard":
				root = WriteBitmapSegment(parameter.Sources[0].Parent, paramName)
			elif parameter.Sources[0].Parent.Parameters("mShaderType").Value == "mGridTexture":
				root = WriteBitmapSegment(parameter.Sources[0].Parent, paramName)
			elif parameter.Sources[0].Parent.Parameters("mShaderType").Value == "mScale":
				root = WriteBitmapSegment(parameter.Sources[0].Parent, paramName)
			elif parameter.Sources[0].Parent.Parameters("mShaderType").Value == "mVertexColors":
				root = WriteBitmapSegment(parameter.Sources[0].Parent, paramName)
			elif parameter.Sources[0].Parent.Parameters("mShaderType").Value == "mWireframe":
				root = WriteBitmapSegment(parameter.Sources[0].Parent, paramName)
			elif parameter.Sources[0].Parent.Parameters("mShaderType").Value == "mCurvature":
				root = WriteBitmapSegment(parameter.Sources[0].Parent, paramName)
			else:
				root = GenerateSpectrumSegment(parameter.Value, paramName)
		else:
			if parameter.Sources[0].Parent.Parameters("tex") != None:
				if len(parameter.Sources[0].Parent.Parameters("tex").Sources) == 1:
					root = GenerateTextureSegment(parameter.Sources[0].Parent.Parameters("tex").Sources[0], paramName)
				else:
					root = GenerateSpectrumSegment(parameter.Value, paramName)
			else:
				#this is not a texture
				root = GenerateSpectrumSegment(parameter.Value, paramName)

	return root

def WriteBitmapSegment(shader, pointName):#pointName - name of a property of the previous shader
	root = etree.Element("texture")
	if shader.Parameters("mShaderName").Value == "bitmap":
		root.set("type", "bitmap")
		if pointName != "":
			root.set("name", pointName)
		s1 = etree.SubElement(root, "string")
		s1.set("name", "filename")
		s1.set("value", str(shader.Parameters("texture").Sources[0].Parameters("SourceFileName").Value))
		s2 = etree.SubElement(root, "string")
		s2.set("name", "wrapModeU")
		s2.set("value", shader.Parameters("wrapModeU").Value)
		s3 = etree.SubElement(root, "string")
		s3.set("name", "wrapModeV")
		s3.set("value", shader.Parameters("wrapModeV").Value)
		s4 = etree.SubElement(root, "string")
		s4.set("name", "filterType")
		s4.set("value", shader.Parameters("filterType").Value)
		f1 = etree.SubElement(root, "float")
		f1.set("name", "maxAnisotropy")
		f1.set("value", str(shader.Parameters("maxAnisotropy").Value))
		b1 = etree.SubElement(root, "boolean")
		b1.set("name", "cache")
		b1.set("value", FromBoolToboolString(shader.Parameters("cache").Value))
		f2 = etree.SubElement(root, "float")
		f2.set("name", "uoffset")
		f2.set("value", str(shader.Parameters("uoffset").Value))
		f3 = etree.SubElement(root, "float")
		f3.set("name", "voffset")
		f3.set("value", str(shader.Parameters("voffset").Value))
		f4 = etree.SubElement(root, "float")
		f4.set("name", "uscale")
		f4.set("value", str(shader.Parameters("uscale").Value))
		f5 = etree.SubElement(root, "float")
		f5.set("name", "vscale")
		f5.set("value", str(shader.Parameters("vscale").Value))
	elif shader.Parameters("mShaderName").Value == "checkerboard":
		root.set("type", "checkerboard")
		if pointName != "":
			root.set("name", pointName)
		root.append(GenerateSpectrumSegment(shader.Parameters("color0").Value, "color0"))
		root.append(GenerateSpectrumSegment(shader.Parameters("color1").Value, "color1"))
		root.append(GenerateFloatSegment(shader.Parameters("uoffset").Value, "uoffset"))
		root.append(GenerateFloatSegment(shader.Parameters("voffset").Value, "voffset"))
		root.append(GenerateFloatSegment(shader.Parameters("uscale").Value, "uscale"))
		root.append(GenerateFloatSegment(shader.Parameters("vscale").Value, "vscale"))
	elif shader.Parameters("mShaderName").Value == "gridtexture":
		root.set("type", "gridtexture")
		if pointName != "":
			root.set("name", pointName)
		root.append(GenerateSpectrumSegment(shader.Parameters("color0").Value, "color0"))
		root.append(GenerateSpectrumSegment(shader.Parameters("color1").Value, "color1"))
		root.append(GenerateFloatSegment(shader.Parameters("uoffset").Value, "uoffset"))
		root.append(GenerateFloatSegment(shader.Parameters("voffset").Value, "voffset"))
		root.append(GenerateFloatSegment(shader.Parameters("uscale").Value, "uscale"))
		root.append(GenerateFloatSegment(shader.Parameters("vscale").Value, "vscale"))
		root.append(GenerateFloatSegment(shader.Parameters("lineWidth").Value, "lineWidth"))
	elif shader.Parameters("mShaderName").Value == "scale":
		root.set("type", "scale")
		if pointName != "":
			root.set("name", pointName)
		root.append(GenerateFloatSegment( shader.Parameters("value").Value, "scale"))
		root.append(WriteTextureOrSpectrum( shader.Parameters("input"), ""))
	elif shader.Parameters("mShaderName").Value == "vertexcolors":
		root.set("type", "vertexcolors")
		if pointName != "":
			root.set("name", pointName)
	elif shader.Parameters("mShaderName").Value == "wireframe":
		root.set("type", "wireframe")
		if pointName != "":
			root.set("name", pointName)
		root.append(GenerateSpectrumSegment( shader.Parameters("interiorColor").Value, "interiorColor"))
		root.append(GenerateSpectrumSegment( shader.Parameters("edgeColor").Value, "edgeColor"))
		root.append(GenerateFloatSegment(shader.Parameters("lineWidth").Value, "lineWidth"))
		root.append(GenerateFloatSegment(shader.Parameters("stepWidth").Value, "stepWidth"))
	elif shader.Parameters("mShaderName").Value == "curvature":
		root.set("type", "curvature")
		if pointName != "":
			root.set("name", pointName)
		s1 = etree.SubElement(root, "string")
		s1.set("name", "curvature")
		s1.set("value", shader.Parameters("curvature").Value)
		root.append(GenerateFloatSegment(shader.Parameters("scale").Value, "scale"))

	return root

def WriteFloatOrTexture(parameter, paramName):
	if parameter.Sources.Count == 0:
		root = GenerateFloatSegment(FromColorToFloat(parameter.Value), paramName)
	else:
		if parameter.Sources[0].Parent.Parameters("tex") != None:
			if len(parameter.Sources[0].Parent.Parameters("tex").Sources) == 1:
				root = GenerateTextureSegment(parameter.Sources[0].Parent.Parameters("tex").Sources[0], paramName)
			else:
				root = GenerateFloatSegment(FromColorToFloat(parameter.Value), paramName)
		else:
			root = GenerateFloatSegment(FromColorToFloat(parameter.Value), paramName)
	return root

def GenerateFloatSegment(value, name):
	root = etree.Element("float")
	root.set("name", name)
	root.set("value", str(value))
	return root

def GenerateIntegerSegment(value, name):
	root = etree.Element("integer")
	root.set("name", name)
	root.set("value", str(value))
	return root

def GenerateSpectrumSegment(color, name):
	root = etree.Element("spectrum")
	root.set("name", name)
	root.set("value", str(color.Red) + ", " + str(color.Green) + ", " + str(color.Blue))
	return root

def GenerateTextureSegment(texSource, name):
	root = etree.Element("texture")
	root.set("type", "bitmap")
	if name != "":
		root.set("name", name)
	string = etree.SubElement(root, "string")
	string.set("name", "filename")
	string.set("value", str(texSource.Parameters("SourceFileName").Value))
	return root

def FromBoolToboolString(var):
	if var == "True":
		return "true"
	elif var == "False":
		return "false"
	else:
		return "false"

def FromColorToFloat(color):
	return ((color.Red + color.Green + color.Blue) / 3)


def GenerateCameraSegment(width, height, isCamera, cameraName):
	#this is standard parameters of a sensor(camera)
	root = etree.Element("sensor")
	cams = ap.ActiveSceneRoot.FindChildren("*", "camera")
	oCam = cams[0]
	if isCamera:#obtain the camera object
		for cam in cams:
			if cam.Name == cameraName:
				oCam = cam
	if isCamera:	
		#is the camera have property
		if oCam.GetPropertyFromName2("MaSsCameraProperty"):
			root.set("type", str(oCam.GetPropertyFromName2("MaSsCameraProperty").Parameters("Type").Value))
		else:
			#there is no such property
			root.set("type", "perspective")
			#add this roperty
			oCam.AddProperty("MaSsCameraProperty")

	else:
		root.set("type", "perspective")
		
	film = etree.SubElement(root, "film")
	film.set("type", "hdrfilm")
	int1 = etree.SubElement(film, "integer")
	int1.set("name", "width")
	int1.set("value", str(width))
	int2 = etree.SubElement(film, "integer")
	int2.set("name", "height")
	int2.set("value", str(height))
	
	#if camera should be export
	if isCamera:
		strType = oCam.GetPropertyFromName2("MaSsCameraProperty").Parameters("Type").Value
		if strType != "irradiancemeter":
			strPos = str(oCam.Kinematics.Global.PosX.Value) + ", " + str(oCam.Kinematics.Global.PosY.Value) + ", " + str(oCam.Kinematics.Global.PosZ.Value)
			
			oInterest = oCam.Interest
			strTarget = str(oInterest.Kinematics.Global.PosX.Value) + ", " + str(oInterest.Kinematics.Global.PosY.Value) + ", " + str(oInterest.Kinematics.Global.PosZ.Value)
			
			transform = etree.SubElement(root, "transform")
			transform.set("name", "toWorld")
			#add positions of a camera
			lookAt = etree.SubElement(transform, "lookat")
			#position
			lookAt.set("origin", strPos)
			#target
			lookAt.set("target", strTarget)
			#upVector
			lookAt.set("up", "0, 1, 0")
		if strType == "perspective" or strType == "thinlens" or strType == "perspective_rdist":
			#fov parameter
			fov = etree.SubElement(root, "float")
			fov.set("name", "fov")
			fov.set("value", str(oCam.Parameters("fov").Value))
			
			#fovAxis parameter
			#lm(oCam.Parameters("fovtype").Value)1 - horizontal, 0 - vertical
			fovAxis = etree.SubElement(root, "string")
			fovAxis.set("name", "fovAxis")
			strFovAxis = ""
			if oCam.Parameters("fovtype").Value == 1:
				strFovAxis = "x"
			elif oCam.Parameters("fovtype").Value == 0:
				strFovAxis = "y"
			fovAxis.set("value", strFovAxis)
		if strType == "perspective" or strType == "thinlens" or strType == "orthographic" or strType == "telecentric" or strType == "perspective_rdist":
			#near and far plane
			nearClip = etree.SubElement(root, "float")
			nearClip.set("name", "nearClip")
			nearClip.set("value", str(oCam.Parameters("near").Value))
			farClip = etree.SubElement(root, "float")
			farClip.set("name", "farClip")
			farClip.set("value", str(oCam.Parameters("far").Value))
		if strType == "thinlens" or strType == "telecentric":
			#add focus parameters
			focusDist = etree.SubElement(root, "float")
			focusDist.set("name", "focusDistance")
			focusDist.set("value", str(oCam.GetPropertyFromName2("MaSsCameraProperty").Parameters("focusDistance").Value))
			aperture = etree.SubElement(root, "float")
			aperture.set("name", "apertureRadius")
			aperture.set("value", str(oCam.GetPropertyFromName2("MaSsCameraProperty").Parameters("apertureRadius").Value))
	
	return root
	
def CheckCorrecLight(light):
	if light.Shaders(0) != None:
		if light.Shaders(0).Parameters("mLightType") == None:
			return False
		else:
			type = str(light.Shaders(0).Parameters("mLightType").Value)
			if type == "mPoint" or type == "mSpot" or type == "mDirectional" or type == "mAreaSphere" or type == "mAreaCylinder" or type == "mAreaRectangle" or type == "mAreaDisk" or type == "mAreaCube" or type == "mConstant" or type == "mSky" or type == "mSun" or type == "mSunSky" or type == "mEnvironment":
				if type == "mEnvironment":#check that image connect to port
					if len(light.Shaders(0).Parameters("image").Sources) == 0:
						return False
					else:
						return True
				else:
					return True
			else:
				return False
	else:
		return False
	
	
def GenerateLightSegment(light):
	type = str(light.Shaders(0).Parameters("mLightType").Value)
	if type == "mAreaSphere":
		root = etree.Element("shape")
		root.set("type", "sphere")
		#position
		point = etree.SubElement(root, "point")
		point.set("name", "center")
		point.set("x", str(light.Kinematics.Global.PosX.Value))
		point.set("y", str(light.Kinematics.Global.PosY.Value))
		point.set("z", str(light.Kinematics.Global.PosZ.Value))
		float = etree.SubElement(root, "float")
		float.set("name", "radius")
		float.set("value", str(light.Shaders(0).Parameters("radius").Value))
		
		#emitter section
		emitter = etree.SubElement(root, "emitter")
		emitter.set("type", "area")
		spec = etree.SubElement(emitter, "spectrum")
		spec.set("name", "radiance")
		spec.set("value", str(light.Shaders(0).Parameters("intensity").Value * light.Shaders(0).Parameters("color").Value.Red) + " " + str(light.Shaders(0).Parameters("intensity").Value * light.Shaders(0).Parameters("color").Value.Green) + " " + str(light.Shaders(0).Parameters("intensity").Value * light.Shaders(0).Parameters("color").Value.Blue))
		#bl = etree.SubElement(emitter, "blackbody")
		#bl.set("name", "spectrumProperty")
		#bl.set("temperature", str(light.Shaders(0).Parameters("temperature").Value) + "K")
	elif type == "mAreaCube":
		root = etree.Element("shape")
		root.set("type", "cube")
		
		#position section
		tr = etree.SubElement(root, "transform")
		tr.set("name", "toWorld")
		scale = etree.SubElement(tr, "scale")
		scale.set("x", str(light.Shaders(0).Parameters("lengthX").Value / 2.0))
		scale.set("y", str(light.Shaders(0).Parameters("lengthY").Value / 2.0))
		scale.set("z", str(light.Shaders(0).Parameters("lengthZ").Value / 2.0))
		rotx = etree.SubElement(tr, "rotate")
		rotx.set("x", "1")
		rotx.set("angle", str(light.Kinematics.Global.RotX.Value))
		roty = etree.SubElement(tr, "rotate")
		roty.set("y", "1")
		roty.set("angle", str(light.Kinematics.Global.RotY.Value))
		rotz = etree.SubElement(tr, "rotate")
		rotz.set("z", "1")
		rotz.set("angle", str(light.Kinematics.Global.RotZ.Value))
		pos = etree.SubElement(tr, "translate")
		pos.set("x", str(light.Kinematics.Global.PosX.Value))
		pos.set("y", str(light.Kinematics.Global.PosY.Value))
		pos.set("z", str(light.Kinematics.Global.PosZ.Value))
		
		#emitter section
		emitter = etree.SubElement(root, "emitter")
		emitter.set("type", "area")
		spec = etree.SubElement(emitter, "spectrum")
		spec.set("name", "radiance")
		spec.set("value", str(light.Shaders(0).Parameters("intensity").Value * light.Shaders(0).Parameters("color").Value.Red) + " " + str(light.Shaders(0).Parameters("intensity").Value * light.Shaders(0).Parameters("color").Value.Green) + " " + str(light.Shaders(0).Parameters("intensity").Value * light.Shaders(0).Parameters("color").Value.Blue))
		
	elif type == "mAreaCylinder":
		root = etree.Element("shape")
		root.set("type", "cylinder")
		float = etree.SubElement(root, "float")
		float.set("name", "radius")
		float.set("value", str(light.Shaders(0).Parameters("radius").Value))
		
		#position
		#points = light.FindChildren("", c.siNullPrimType)
		#startPoint = points[0]
		#emdPoint = points[1]
		#startPoint = light.FindChild("startPoint", c.siNullPrimType)
		pointS = etree.SubElement(root, "point")
		pointS.set("name", "p0")
		#calculate start position
		currentTransform = light.Kinematics.Local.GetTransform2(None).Matrix4
		targetLocalMatrix = XSIMath.CreateMatrix4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, -1, 1)
		mat = XSIMath.CreateMatrix4()
		mat.Mul(targetLocalMatrix, currentTransform)
		pointS.set("x", str(mat.Value(3, 0)))
		pointS.set("y", str(mat.Value(3, 1)))
		pointS.set("z", str(mat.Value(3, 2)))

		#pointS.set("x", str(startPoint.Kinematics.Global.PosX.Value))
		#pointS.set("y", str(startPoint.Kinematics.Global.PosY.Value))
		#pointS.set("z", str(startPoint.Kinematics.Global.PosZ.Value))
		
		pointS = etree.SubElement(root, "point")
		pointS.set("name", "p1")
		#and end position
		targetLocalMatrix = XSIMath.CreateMatrix4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1)
		mat = XSIMath.CreateMatrix4()
		mat.Mul(targetLocalMatrix, currentTransform)
		pointS.set("x", str(mat.Value(3, 0)))
		pointS.set("y", str(mat.Value(3, 1)))
		pointS.set("z", str(mat.Value(3, 2)))
		#pointS.set("x", str(emdPoint.Kinematics.Global.PosX.Value))
		#pointS.set("y", str(emdPoint.Kinematics.Global.PosY.Value))
		#pointS.set("z", str(emdPoint.Kinematics.Global.PosZ.Value))
		#emitter section
		emitter = etree.SubElement(root, "emitter")
		emitter.set("type", "area")
		spec = etree.SubElement(emitter, "spectrum")
		spec.set("name", "radiance")
		spec.set("value", str(light.Shaders(0).Parameters("intensity").Value * light.Shaders(0).Parameters("color").Value.Red) + " " + str(light.Shaders(0).Parameters("intensity").Value * light.Shaders(0).Parameters("color").Value.Green) + " " + str(light.Shaders(0).Parameters("intensity").Value * light.Shaders(0).Parameters("color").Value.Blue))
	elif type == "mAreaRectangle":
		root = etree.Element("shape")
		root.set("type", "rectangle")
		
		#position
		tr = etree.SubElement(root, "transform")
		tr.set("name", "toWorld")
		scale = etree.SubElement(tr, "scale")
		scale.set("x", str(light.Shaders(0).Parameters("lengthX").Value / 2.0))
		scale.set("y", str(light.Shaders(0).Parameters("lengthY").Value / 2.0))
		rotx = etree.SubElement(tr, "rotate")
		rotx.set("x", "1")
		rotx.set("angle", str(light.Kinematics.Global.RotX.Value))
		roty = etree.SubElement(tr, "rotate")
		roty.set("y", "1")
		roty.set("angle", str(light.Kinematics.Global.RotY.Value))
		rotz = etree.SubElement(tr, "rotate")
		rotz.set("z", "1")
		rotz.set("angle", str(light.Kinematics.Global.RotZ.Value))
		pos = etree.SubElement(tr, "translate")
		pos.set("x", str(light.Kinematics.Global.PosX.Value))
		pos.set("y", str(light.Kinematics.Global.PosY.Value))
		pos.set("z", str(light.Kinematics.Global.PosZ.Value))
		
		#emitter section
		emitter = etree.SubElement(root, "emitter")
		emitter.set("type", "area")
		spec = etree.SubElement(emitter, "spectrum")
		spec.set("name", "radiance")
		spec.set("value", str(light.Shaders(0).Parameters("intensity").Value * light.Shaders(0).Parameters("color").Value.Red) + " " + str(light.Shaders(0).Parameters("intensity").Value * light.Shaders(0).Parameters("color").Value.Green) + " " + str(light.Shaders(0).Parameters("intensity").Value * light.Shaders(0).Parameters("color").Value.Blue))
	elif type == "mAreaDisk":
		root = etree.Element("shape")
		root.set("type", "disk")
		
		#position
		tr = etree.SubElement(root, "transform")
		tr.set("name", "toWorld")
		scale = etree.SubElement(tr, "scale")
		scale.set("value", str(light.Shaders(0).Parameters("radius").Value))
		rotx = etree.SubElement(tr, "rotate")
		rotx.set("x", "1")
		rotx.set("angle", str(light.Kinematics.Global.RotX.Value))
		roty = etree.SubElement(tr, "rotate")
		roty.set("y", "1")
		roty.set("angle", str(light.Kinematics.Global.RotY.Value))
		rotz = etree.SubElement(tr, "rotate")
		rotz.set("z", "1")
		rotz.set("angle", str(light.Kinematics.Global.RotZ.Value))
		pos = etree.SubElement(tr, "translate")
		pos.set("x", str(light.Kinematics.Global.PosX.Value))
		pos.set("y", str(light.Kinematics.Global.PosY.Value))
		pos.set("z", str(light.Kinematics.Global.PosZ.Value))
		
		#emitter section
		emitter = etree.SubElement(root, "emitter")
		emitter.set("type", "area")
		spec = etree.SubElement(emitter, "spectrum")
		spec.set("name", "radiance")
		spec.set("value", str(light.Shaders(0).Parameters("intensity").Value * light.Shaders(0).Parameters("color").Value.Red) + " " + str(light.Shaders(0).Parameters("intensity").Value * light.Shaders(0).Parameters("color").Value.Green) + " " + str(light.Shaders(0).Parameters("intensity").Value * light.Shaders(0).Parameters("color").Value.Blue))
	else:
		#check conected shader
		if type == "mPoint":
			root = etree.Element("emitter")
			root.set("type", "point")
			#get spectrum
			spec = etree.SubElement(root, "spectrum")
			spec.set("name", "intensity")
			spec.set("value", str(light.Shaders(0).Parameters("intensity").Value * light.Shaders(0).Parameters("color").Value.Red) + " " + str(light.Shaders(0).Parameters("intensity").Value * light.Shaders(0).Parameters("color").Value.Green) + " " + str(light.Shaders(0).Parameters("intensity").Value * light.Shaders(0).Parameters("color").Value.Blue))
			#and get position
			point = etree.SubElement(root, "point")
			point.set("name", "position")
			point.set("x", str(light.Kinematics.Global.PosX.Value))
			point.set("y", str(light.Kinematics.Global.PosY.Value))
			point.set("z", str(light.Kinematics.Global.PosZ.Value))
		elif type == "mSpot":
			root = etree.Element("emitter")
			root.set("type", "spot")
			#position and target
			transform = etree.SubElement(root, "transform")
			transform.set("name", "toWorld")
			lookAt = etree.SubElement(transform, "lookat")
			lookAt.set("origin", str(light.Kinematics.Global.PosX.Value) + ", " + str(light.Kinematics.Global.PosY.Value) + ", " + str(light.Kinematics.Global.PosZ.Value))
			interest = light.Interest
			lookAt.set("target", str(interest.Kinematics.Global.PosX.Value) + ", " + str(interest.Kinematics.Global.PosY.Value) + ", " + str(interest.Kinematics.Global.PosZ.Value))
			
			#intensity
			spec = etree.SubElement(root, "spectrum")
			spec.set("name", "intensity")
			spec.set("value", str(light.Shaders(0).Parameters("intensity").Value * light.Shaders(0).Parameters("color").Value.Red) + " " + str(light.Shaders(0).Parameters("intensity").Value * light.Shaders(0).Parameters("color").Value.Green) + " " + str(light.Shaders(0).Parameters("intensity").Value * light.Shaders(0).Parameters("color").Value.Blue))
			
			#cutoffAngle
			ang = etree.SubElement(root, "float")
			ang.set("name", "cutoffAngle")
			ang.set("value", str(light.LightCone.Value))
			
			#beamWidth
			beam = etree.SubElement(root, "float")
			beam.set("name", "beamWidth")
			beam.set("value", str(light.Shaders(0).Parameters("beamWidth").Value))
			
			#texture
			if len(light.Shaders(0).Parameters("texture").Sources) > 0:#something connect
				if light.Shaders(0).Parameters("texture").Sources[0].Parent.Parameters("tex") != None:#this is a texture
					#lm("here")
					root.append(GenerateTextureSegment(light.Shaders(0).Parameters("texture").Sources[0].Parent.Parameters("tex").Sources[0], "texture"))
					#if len(light.Shaders(0).Parameters("texture").Sources[0].Parent.Parameters("tex").Sources) == 1:
					#	texture = etree.SubElement(root, "texture")
					#	texture.set("name", "texture")
					#	texture.set("value", str(light.Shaders(0).Parameters("texture").Sources[0].Parent.Parameters("tex").Sources[0].Parameters("SourceFileName").Value))
		elif type == "mDirectional":
			root = etree.Element("emitter")
			root.set("type", "directional")
			
			transform = etree.SubElement(root, "transform")
			transform.set("name", "toWorld")
			lookAt = etree.SubElement(transform, "lookat")
			lookAt.set("origin", str(light.Kinematics.Global.PosX.Value) + ", " + str(light.Kinematics.Global.PosY.Value) + ", " + str(light.Kinematics.Global.PosZ.Value))
			#calculate target position
			currentTransform = light.Kinematics.Local.GetTransform2(None).Matrix4
			targetLocalMatrix = XSIMath.CreateMatrix4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, -10, 1)
			mat = XSIMath.CreateMatrix4()
			mat.Mul(targetLocalMatrix, currentTransform)
			lookAt.set("target", str(mat.Value(3, 0)) + ", " + str(mat.Value(3, 1)) + ", " + str(mat.Value(3, 2)))
			
			#intensity
			spec = etree.SubElement(root, "spectrum")
			spec.set("name", "irradiance")
			spec.set("value", str(light.Shaders(0).Parameters("intensity").Value * light.Shaders(0).Parameters("color").Value.Red) + " " + str(light.Shaders(0).Parameters("intensity").Value * light.Shaders(0).Parameters("color").Value.Green) + " " + str(light.Shaders(0).Parameters("intensity").Value * light.Shaders(0).Parameters("color").Value.Blue))
		elif type == "mConstant":
			root = etree.Element("emitter")
			root.set("type", "constant")
			spec = etree.SubElement(root, "spectrum")
			spec.set("name", "radiance")
			spec.set("value", str(light.Shaders(0).Parameters("intensity").Value * light.Shaders(0).Parameters("color").Value.Red) + " " + str(light.Shaders(0).Parameters("intensity").Value * light.Shaders(0).Parameters("color").Value.Green) + " " + str(light.Shaders(0).Parameters("intensity").Value * light.Shaders(0).Parameters("color").Value.Blue))
		elif type == "mSky":
			root = etree.Element("emitter")
			root.set("type", "sky")
			fl1 = etree.SubElement(root, "float")
			fl1.set("name", "turbidity")
			fl1.set("value", str(light.Shaders(0).Parameters("turbidity").Value))
			spec = etree.SubElement(root, "spectrum")
			spec.set("name", "albedo")
			spec.set("value", str(light.Shaders(0).Parameters("albedo").Value.Red) + ", " + str(light.Shaders(0).Parameters("albedo").Value.Green)  + ", " + str(light.Shaders(0).Parameters("albedo").Value.Blue))
						
			sunModulo = ap.GetValue(light.Name + ".light.mSky.sunDirection.x") * ap.GetValue(light.Name + ".light.mSky.sunDirection.x") + ap.GetValue(light.Name + ".light.mSky.sunDirection.y") * ap.GetValue(light.Name + ".light.mSky.sunDirection.y") + ap.GetValue(light.Name + ".light.mSky.sunDirection.z") * ap.GetValue(light.Name + ".light.mSky.sunDirection.z")
			#sunModulo = 0
			if sunModulo == 0:
				int1 = etree.SubElement(root, "integer")
				int1.set("name", "year")
				int1.set("value", str(light.Shaders(0).Parameters("year").Value))
				
				int2 = etree.SubElement(root, "integer")
				int2.set("name", "month")
				int2.set("value", str(light.Shaders(0).Parameters("month").Value))
				
				int3 = etree.SubElement(root, "integer")
				int3.set("name", "day")
				int3.set("value", str(light.Shaders(0).Parameters("day").Value))
				
				f1 = etree.SubElement(root, "float")
				f1.set("name", "hour")
				f1.set("value", str(light.Shaders(0).Parameters("hour").Value))
				f2 = etree.SubElement(root, "float")
				f2.set("name", "minute")
				f2.set("value", str(light.Shaders(0).Parameters("minute").Value))
				f3 = etree.SubElement(root, "float")
				f3.set("name", "second")
				f3.set("value", str(light.Shaders(0).Parameters("second").Value))
				float1 = etree.SubElement(root, "float")
				float1.set("name", "latitude")
				float1.set("value", str(light.Shaders(0).Parameters("latitude").Value))
				float2 = etree.SubElement(root, "float")
				float2.set("name", "longitude")
				float2.set("value", str(light.Shaders(0).Parameters("longitude").Value))
				float3 = etree.SubElement(root, "float")
				float3.set("name", "timezone")
				float3.set("value", str(light.Shaders(0).Parameters("timezone").Value))
			else:
				vec = etree.SubElement(root, "vector")
				vec.set("name", "sunDirection")
				vec.set("x", str(ap.GetValue(light.Name + ".light.mSky.sunDirection.x")))
				vec.set("y", str(ap.GetValue(light.Name + ".light.mSky.sunDirection.y")))
				vec.set("z", str(ap.GetValue(light.Name + ".light.mSky.sunDirection.z")))
			f4 = etree.SubElement(root, "float")
			f4.set("name", "stretch")
			f4.set("value", str(light.Shaders(0).Parameters("stretch").Value))
			
			int = etree.SubElement(root, "integer")
			int.set("name", "resolution")
			int.set("value", str(light.Shaders(0).Parameters("resolution").Value))
			
			f5 = etree.SubElement(root, "float")
			f5.set("name", "scale")
			f5.set("value", str(light.Shaders(0).Parameters("scale").Value))
			tr = etree.SubElement(root, "transform")
			tr.set("name", "toWorld")
			rotx = etree.SubElement(tr, "rotate")
			rotx.set("x", "1")
			rotx.set("angle", str(light.Kinematics.Global.RotX.Value))
			roty = etree.SubElement(tr, "rotate")
			roty.set("y", "1")
			roty.set("angle", str(light.Kinematics.Global.RotY.Value))
			rotz = etree.SubElement(tr, "rotate")
			rotz.set("z", "1")
			rotz.set("angle", str(light.Kinematics.Global.RotZ.Value))
			pos = etree.SubElement(tr, "translate")
			pos.set("x", str(light.Kinematics.Global.PosX.Value))
			pos.set("y", str(light.Kinematics.Global.PosY.Value))
			pos.set("z", str(light.Kinematics.Global.PosZ.Value))
		elif type == "mSun":
			root = etree.Element("emitter")
			root.set("type", "sun")
			fl1 = etree.SubElement(root, "float")
			fl1.set("name", "turbidity")
			fl1.set("value", str(light.Shaders(0).Parameters("turbidity").Value))
									
			sunModulo = ap.GetValue(light.Name + ".light.mSun.sunDirection.x") * ap.GetValue(light.Name + ".light.mSun.sunDirection.x") + ap.GetValue(light.Name + ".light.mSun.sunDirection.y") * ap.GetValue(light.Name + ".light.mSun.sunDirection.y") + ap.GetValue(light.Name + ".light.mSun.sunDirection.z") * ap.GetValue(light.Name + ".light.mSun.sunDirection.z")
			#sunModulo = 0
			if sunModulo == 0:
				int1 = etree.SubElement(root, "integer")
				int1.set("name", "year")
				int1.set("value", str(light.Shaders(0).Parameters("year").Value))
				
				int2 = etree.SubElement(root, "integer")
				int2.set("name", "month")
				int2.set("value", str(light.Shaders(0).Parameters("month").Value))
				
				int3 = etree.SubElement(root, "integer")
				int3.set("name", "day")
				int3.set("value", str(light.Shaders(0).Parameters("day").Value))
				
				f1 = etree.SubElement(root, "float")
				f1.set("name", "hour")
				f1.set("value", str(light.Shaders(0).Parameters("hour").Value))
				f2 = etree.SubElement(root, "float")
				f2.set("name", "minute")
				f2.set("value", str(light.Shaders(0).Parameters("minute").Value))
				f3 = etree.SubElement(root, "float")
				f3.set("name", "second")
				f3.set("value", str(light.Shaders(0).Parameters("second").Value))
				float1 = etree.SubElement(root, "float")
				float1.set("name", "latitude")
				float1.set("value", str(light.Shaders(0).Parameters("latitude").Value))
				float2 = etree.SubElement(root, "float")
				float2.set("name", "longitude")
				float2.set("value", str(light.Shaders(0).Parameters("longitude").Value))
				float3 = etree.SubElement(root, "float")
				float3.set("name", "timezone")
				float3.set("value", str(light.Shaders(0).Parameters("timezone").Value))
			else:
				vec = etree.SubElement(root, "vector")
				vec.set("name", "sunDirection")
				vec.set("x", str(ap.GetValue(light.Name + ".light.mSun.sunDirection.x")))
				vec.set("y", str(ap.GetValue(light.Name + ".light.mSun.sunDirection.y")))
				vec.set("z", str(ap.GetValue(light.Name + ".light.mSun.sunDirection.z")))
			
			int = etree.SubElement(root, "integer")
			int.set("name", "resolution")
			int.set("value", str(light.Shaders(0).Parameters("resolution").Value))
			
			f5 = etree.SubElement(root, "float")
			f5.set("name", "scale")
			f5.set("value", str(light.Shaders(0).Parameters("scale").Value))
			f6 = etree.SubElement(root, "float")
			f6.set("name", "sunRadiusScale")
			f6.set("value", str(light.Shaders(0).Parameters("sunRadiusScale").Value))
		elif type == "mSunSky":
			root = etree.Element("emitter")
			root.set("type", "sunsky")
			fl1 = etree.SubElement(root, "float")
			fl1.set("name", "turbidity")
			fl1.set("value", str(light.Shaders(0).Parameters("turbidity").Value))
			spec = etree.SubElement(root, "spectrum")
			spec.set("name", "albedo")
			spec.set("value", str(light.Shaders(0).Parameters("albedo").Value.Red) + ", " + str(light.Shaders(0).Parameters("albedo").Value.Green)  + ", " + str(light.Shaders(0).Parameters("albedo").Value.Blue))
									
			sunModulo = ap.GetValue(light.Name + ".light.mSunSky.sunDirection.x") * ap.GetValue(light.Name + ".light.mSunSky.sunDirection.x") + ap.GetValue(light.Name + ".light.mSunSky.sunDirection.y") * ap.GetValue(light.Name + ".light.mSunSky.sunDirection.y") + ap.GetValue(light.Name + ".light.mSunSky.sunDirection.z") * ap.GetValue(light.Name + ".light.mSunSky.sunDirection.z")
			#sunModulo = 0
			if sunModulo == 0:
				int1 = etree.SubElement(root, "integer")
				int1.set("name", "year")
				int1.set("value", str(light.Shaders(0).Parameters("year").Value))
				
				int2 = etree.SubElement(root, "integer")
				int2.set("name", "month")
				int2.set("value", str(light.Shaders(0).Parameters("month").Value))
				
				int3 = etree.SubElement(root, "integer")
				int3.set("name", "day")
				int3.set("value", str(light.Shaders(0).Parameters("day").Value))
				
				f1 = etree.SubElement(root, "float")
				f1.set("name", "hour")
				f1.set("value", str(light.Shaders(0).Parameters("hour").Value))
				f2 = etree.SubElement(root, "float")
				f2.set("name", "minute")
				f2.set("value", str(light.Shaders(0).Parameters("minute").Value))
				f3 = etree.SubElement(root, "float")
				f3.set("name", "second")
				f3.set("value", str(light.Shaders(0).Parameters("second").Value))
				float1 = etree.SubElement(root, "float")
				float1.set("name", "latitude")
				float1.set("value", str(light.Shaders(0).Parameters("latitude").Value))
				float2 = etree.SubElement(root, "float")
				float2.set("name", "longitude")
				float2.set("value", str(light.Shaders(0).Parameters("longitude").Value))
				float3 = etree.SubElement(root, "float")
				float3.set("name", "timezone")
				float3.set("value", str(light.Shaders(0).Parameters("timezone").Value))
			else:
				vec = etree.SubElement(root, "vector")
				vec.set("name", "sunDirection")
				vec.set("x", str(ap.GetValue(light.Name + ".light.mSunSky.sunDirection.x")))
				vec.set("y", str(ap.GetValue(light.Name + ".light.mSunSky.sunDirection.y")))
				vec.set("z", str(ap.GetValue(light.Name + ".light.mSunSky.sunDirection.z")))
			
			int = etree.SubElement(root, "integer")
			int.set("name", "resolution")
			int.set("value", str(light.Shaders(0).Parameters("resolution").Value))
			
			f4 = etree.SubElement(root, "float")
			f4.set("name", "stretch")
			f4.set("value", str(light.Shaders(0).Parameters("stretch").Value))
			
			f5 = etree.SubElement(root, "float")
			f5.set("name", "sunScale")
			f5.set("value", str(light.Shaders(0).Parameters("sunScale").Value))
			f55 = etree.SubElement(root, "float")
			f55.set("name", "skyScale")
			f55.set("value", str(light.Shaders(0).Parameters("skyScale").Value))
			f6 = etree.SubElement(root, "float")
			f6.set("name", "sunRadiusScale")
			f6.set("value", str(light.Shaders(0).Parameters("sunRadiusScale").Value))
		elif type == "mEnvironment":
			root = etree.Element("emitter")
			root.set("type", "envmap")
			stri = etree.SubElement(root, "string")
			stri.set("name", "filename")
			stri.set("value", str(light.Shaders(0).Parameters("image").Sources[0].Parameters("SourceFileName").Value))
			fl1 = etree.SubElement(root, "float")
			fl1.set("name", "scale")
			fl1.set("value", str(light.Shaders(0).Parameters("scale").Value))
			tr = etree.SubElement(root, "transform")
			tr.set("name", "toWorld")
			scale = etree.SubElement(tr, "scale")
			scale.set("x", str(light.Kinematics.Global.SclX.Value))
			scale.set("y", str(light.Kinematics.Global.SclY.Value))
			scale.set("y", str(light.Kinematics.Global.SclZ.Value))
			rotx = etree.SubElement(tr, "rotate")
			rotx.set("x", "1")
			rotx.set("angle", str(light.Kinematics.Global.RotX.Value))
			roty = etree.SubElement(tr, "rotate")
			roty.set("y", "1")
			roty.set("angle", str(light.Kinematics.Global.RotY.Value))
			rotz = etree.SubElement(tr, "rotate")
			rotz.set("z", "1")
			rotz.set("angle", str(light.Kinematics.Global.RotZ.Value))
			pos = etree.SubElement(tr, "translate")
			pos.set("x", str(light.Kinematics.Global.PosX.Value))
			pos.set("y", str(light.Kinematics.Global.PosY.Value))
			pos.set("z", str(light.Kinematics.Global.PosZ.Value))
			
			fl2 = etree.SubElement(root, "float")
			fl2.set("name", "gamma")
			fl2.set("value", str(light.Shaders(0).Parameters("gamma").Value))
			
			bo = etree.SubElement(root, "boolean")
			bo.set("name", "cache")
			if light.Shaders(0).Parameters("isCache").Value:
				logStr = "true"
			else:
				logStr = "false"
			bo.set("value", logStr)
	return root
	
def isMeshCorrect(mesh):
	#check is mesh visible for render
	if mesh.Properties("visibility").Parameters("rendvis").Value:
		return True
	else:
		return False

def isSurfCorrect(surf):
	if surf.GetPropertyFromName2("MaSsSpherePrimitiveProperty") or surf.GetPropertyFromName2("MaSsCubePrimitiveProperty") or surf.GetPropertyFromName2("MaSsCylinderPrimitiveProperty") or surf.GetPropertyFromName2("MaSsRectanglePrimitiveProperty") or surf.GetPropertyFromName2("MaSsDiskPrimitiveProperty"):
		return True
	else:
		return False

def GenerateMeshLightSegment(shader):
	root = etree.Element("emitter")
	root.set("type", "area")
	root.append(GenerateSpectrumSegment(shader.Parameters("radiance").Value, "radiance"))
	return root

def GenerateSphere(mesh, radius, posX, posY, posZ):
	root = etree.Element("shape")
	root.set("type", "sphere")
	#position
	point = etree.SubElement(root, "point")
	point.set("name", "center")
	point.set("x", str(posX))
	point.set("y", str(posY))
	point.set("z", str(posZ))
	float = etree.SubElement(root, "float")
	float.set("name", "radius")
	float.set("value", str(radius))

	#write a shader
	mat = mesh.Materials[0]
	if IsMatCorrect(mat):
		root.append(GenerateObjectMatSegment(mesh, False))
	elif IsSSSMatCorrect(mat):
		root.append(GenerateObjectMatSegment(mesh,False))
		root.append(GenerateObjectMatSegment(mesh, True))
	elif IsMeshLight(mat):
		root.append(GenerateMeshLightSegment(mat.Shaders[0]))

	return root

def GenerateCube(mesh, lX, lY, lZ, rX, rY, rZ, posX, posY, posZ):
	root = etree.Element("shape")
	root.set("type", "cube")
	#position section
	tr = etree.SubElement(root, "transform")
	tr.set("name", "toWorld")
	scale = etree.SubElement(tr, "scale")
	scale.set("x", str(lX))
	scale.set("y", str(lY))
	scale.set("z", str(lZ))
	rotx = etree.SubElement(tr, "rotate")
	rotx.set("x", "1")
	rotx.set("angle", str(rX))
	roty = etree.SubElement(tr, "rotate")
	roty.set("y", "1")
	roty.set("angle", str(rY))
	rotz = etree.SubElement(tr, "rotate")
	rotz.set("z", "1")
	rotz.set("angle", str(rZ))
	pos = etree.SubElement(tr, "translate")
	pos.set("x", str(posX))
	pos.set("y", str(posY))
	pos.set("z", str(posZ))
	#write a shader
	mat = mesh.Materials[0]
	if IsMatCorrect(mat):
		root.append(GenerateObjectMatSegment(mesh, False))
	elif IsSSSMatCorrect(mat):
		root.append(GenerateObjectMatSegment(mesh,False))
		root.append(GenerateObjectMatSegment(mesh, True))
	elif IsMeshLight(mat):
		root.append(GenerateMeshLightSegment(mat.Shaders[0]))

	return root

def GenerateCylinder(mesh, radius, transformMatrix):
	root = etree.Element("shape")
	root.set("type", "cylinder")
	float = etree.SubElement(root, "float")
	float.set("name", "radius")
	float.set("value", str(radius))
	#position
	pointS = etree.SubElement(root, "point")
	pointS.set("name", "p0")
	currentTransform = transformMatrix
	targetLocalMatrix = XSIMath.CreateMatrix4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, -0.5, 0, 1)
	mat = XSIMath.CreateMatrix4()
	mat.Mul(targetLocalMatrix, currentTransform)
	pointS.set("x", str(mat.Value(3, 0)))
	pointS.set("y", str(mat.Value(3, 1)))
	pointS.set("z", str(mat.Value(3, 2)))
	pointS = etree.SubElement(root, "point")
	pointS.set("name", "p1")
	currentTransform = transformMatrix
	targetLocalMatrix = XSIMath.CreateMatrix4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0.5, 0, 1)
	mat = XSIMath.CreateMatrix4()
	mat.Mul(targetLocalMatrix, currentTransform)
	pointS.set("x", str(mat.Value(3, 0)))
	pointS.set("y", str(mat.Value(3, 1)))
	pointS.set("z", str(mat.Value(3, 2)))
	#write a shader
	mat = mesh.Materials[0]
	if IsMatCorrect(mat):
		root.append(GenerateObjectMatSegment(mesh, False))
	elif IsSSSMatCorrect(mat):
		root.append(GenerateObjectMatSegment(mesh,False))
		root.append(GenerateObjectMatSegment(mesh, True))
	elif IsMeshLight(mat):
		root.append(GenerateMeshLightSegment(mat.Shaders[0]))

	return root

def GenerateRectangle(mesh, lX, lY, rotX, rotY, rotZ, posX, posY, posZ):
	root = etree.Element("shape")
	root.set("type", "rectangle")
	
	#position
	tr = etree.SubElement(root, "transform")
	tr.set("name", "toWorld")
	scale = etree.SubElement(tr, "scale")
	scale.set("x", str(lX))
	scale.set("y", str(lY))
	rotx = etree.SubElement(tr, "rotate")
	rotx.set("x", "1")
	rotx.set("angle", str(rotX))
	roty = etree.SubElement(tr, "rotate")
	roty.set("y", "1")
	roty.set("angle", str(rotY))
	rotz = etree.SubElement(tr, "rotate")
	rotz.set("z", "1")
	rotz.set("angle", str(rotZ))
	pos = etree.SubElement(tr, "translate")
	pos.set("x", str(posX))
	pos.set("y", str(posY))
	pos.set("z", str(posZ))
	#add two side mat
	#bsdf = etree.SubElement(root, "bsdf")
	#bsdf.set("type", "twosided")
	#bsdf2 = etree.SubElement(bsdf, "bsdf")
	#bsdf2.set("type", "diffuse")
	#write a shader
	mat = mesh.Materials[0]
	if IsMatCorrect(mat):
		root.append(GenerateObjectMatSegment(mesh, False))
	elif IsSSSMatCorrect(mat):
		root.append(GenerateObjectMatSegment(mesh,False))
		root.append(GenerateObjectMatSegment(mesh, True))
	elif IsMeshLight(mat):
		root.append(GenerateMeshLightSegment(mat.Shaders[0]))

	return root

def GenerateDisk(mesh, radius, rotX, rotY, rotZ, posX, posY, posZ):
	root = etree.Element("shape")
	root.set("type", "disk")
	
	#position
	tr = etree.SubElement(root, "transform")
	tr.set("name", "toWorld")
	scale = etree.SubElement(tr, "scale")
	scale.set("value", str(radius))
	rotx = etree.SubElement(tr, "rotate")
	rotx.set("x", "1")
	rotx.set("angle", str(rotX))
	roty = etree.SubElement(tr, "rotate")
	roty.set("y", "1")
	roty.set("angle", str(rotY))
	rotz = etree.SubElement(tr, "rotate")
	rotz.set("z", "1")
	rotz.set("angle", str(rotZ))
	pos = etree.SubElement(tr, "translate")
	pos.set("x", str(posX))
	pos.set("y", str(posY))
	pos.set("z", str(posZ))
	#add two side mat
	#bsdf = etree.SubElement(root, "bsdf")
	#bsdf.set("type", "twosided")
	#bsdf2 = etree.SubElement(bsdf, "bsdf")
	#bsdf2.set("type", "diffuse")
	#write a shader
	mat = mesh.Materials[0]
	if IsMatCorrect(mat):
		root.append(GenerateObjectMatSegment(mesh, False))
	elif IsSSSMatCorrect(mat):
		root.append(GenerateObjectMatSegment(mesh,False))
		root.append(GenerateObjectMatSegment(mesh, True))
	elif IsMeshLight(mat):
		root.append(GenerateMeshLightSegment(mat.Shaders[0]))

	return root

def GenerateMeshSegment(mesh, shouldExport, exportPath):
	#root = etree.Element("shape")
	#is mesh contains property of a mPrimitive
	if mesh.GetPropertyFromName2("MaSsSpherePrimitiveProperty"):
		root = GenerateSphere(mesh, mesh.GetPropertyFromName2("MaSsSpherePrimitiveProperty").Parameters("radius").Value, mesh.Kinematics.Global.PosX.Value, mesh.Kinematics.Global.PosY.Value, mesh.Kinematics.Global.PosZ.Value)
		
	elif mesh.GetPropertyFromName2("MaSsCubePrimitiveProperty"):
		root = GenerateCube(mesh, mesh.GetPropertyFromName2("MaSsCubePrimitiveProperty").Parameters("lengthX").Value / 2.0, mesh.GetPropertyFromName2("MaSsCubePrimitiveProperty").Parameters("lengthY").Value / 2.0, mesh.GetPropertyFromName2("MaSsCubePrimitiveProperty").Parameters("lengthZ").Value / 2.0, mesh.Kinematics.Global.RotX.Value, mesh.Kinematics.Global.RotY.Value, mesh.Kinematics.Global.RotZ.Value, mesh.Kinematics.Global.PosX.Value, mesh.Kinematics.Global.PosY.Value, mesh.Kinematics.Global.PosZ.Value)
		
	elif mesh.GetPropertyFromName2("MaSsCylinderPrimitiveProperty"):
		root = GenerateCylinder(mesh, mesh.GetPropertyFromName2("MaSsCylinderPrimitiveProperty").Parameters("radius").Value, mesh.Kinematics.Local.GetTransform2(None).Matrix4)
		
	elif mesh.GetPropertyFromName2("MaSsRectanglePrimitiveProperty"):
		root = GenerateRectangle(mesh, mesh.GetPropertyFromName2("MaSsRectanglePrimitiveProperty").Parameters("lengthX").Value / 2.0, mesh.GetPropertyFromName2("MaSsRectanglePrimitiveProperty").Parameters("lengthY").Value / 2.0, mesh.Kinematics.Global.RotX.Value - 90, mesh.Kinematics.Global.RotY.Value, mesh.Kinematics.Global.RotZ.Value,  mesh.Kinematics.Global.PosX.Value, mesh.Kinematics.Global.PosY.Value, mesh.Kinematics.Global.PosZ.Value)


	elif mesh.GetPropertyFromName2("MaSsDiskPrimitiveProperty"):
		root = GenerateDisk(mesh, mesh.GetPropertyFromName2("MaSsDiskPrimitiveProperty").Parameters("radius").Value, mesh.Kinematics.Global.RotX.Value - 90, mesh.Kinematics.Global.RotY.Value, mesh.Kinematics.Global.RotZ.Value, mesh.Kinematics.Global.PosX.Value, mesh.Kinematics.Global.PosY.Value, mesh.Kinematics.Global.PosZ.Value)

	else:
		#this is regular poly mesh. Should be export as ply and write coresponding in the section
		root = etree.Element("shape")
		root.set("type", "ply")
		fl = etree.SubElement(root, "string")
		fl.set("name", "filename")
		fl.set("value", mesh.FullName + ".ply")

		tr = etree.SubElement(root, "transform")
		tr.set("name", "toWorld")
		scale = etree.SubElement(tr, "scale")
		scale.set("x", str(mesh.Kinematics.Global.SclX.Value))
		scale.set("y", str(mesh.Kinematics.Global.SclY.Value))
		scale.set("z", str(mesh.Kinematics.Global.SclZ.Value))
		rotx = etree.SubElement(tr, "rotate")
		rotx.set("x", "1")
		rotx.set("angle", str(mesh.Kinematics.Global.RotX.Value))
		roty = etree.SubElement(tr, "rotate")
		roty.set("y", "1")
		roty.set("angle", str(mesh.Kinematics.Global.RotY.Value))
		rotz = etree.SubElement(tr, "rotate")
		rotz.set("z", "1")
		rotz.set("angle", str(mesh.Kinematics.Global.RotZ.Value))
		pos = etree.SubElement(tr, "translate")
		pos.set("x", str(mesh.Kinematics.Global.PosX.Value))
		pos.set("y", str(mesh.Kinematics.Global.PosY.Value))
		pos.set("z", str(mesh.Kinematics.Global.PosZ.Value))



		#next we should parse shader
		#write a shader
		mat = mesh.Materials[0]
		if IsMatCorrect(mat):
			root.append(GenerateObjectMatSegment(mesh, False))
		elif IsSSSMatCorrect(mat):
			root.append(GenerateObjectMatSegment(mesh,False))
			root.append(GenerateObjectMatSegment(mesh, True))
		elif IsMeshLight(mat):
			root.append(GenerateMeshLightSegment(mat.Shaders[0]))



		if shouldExport:
			#ap.ObjExport(exportPath + "\\" + mesh.FullName + ".obj", 1, True, 1, 1, 1, True, False, False, False, None, True, False, True, False)
			
			#collection =  XSIFactory.CreateActiveXObject("XSI.Collection")
			#collection.Add(mesh)
			#ap.SIObjExport(collection, exportPath + "\\" + mesh.FullName, None, False, False, True, True)
			#lm(exportPath + "\\" + mesh.FullName)
			
			#determ properties of a mesh
			if mesh.GetPropertyFromName2("MaSsObjectProperty"):
				isSolid = mesh.GetPropertyFromName2("MaSsObjectProperty").Parameters("isSolid").Value
				isSubdivide = mesh.GetPropertyFromName2("MaSsObjectProperty").Parameters("isSubdivide").Value
				subLevel = mesh.GetPropertyFromName2("MaSsObjectProperty").Parameters("subLevel").Value
			else:
				#there is no such property
				isSolid = False
				isSubdivide = False

			if isSolid:
				if not isSubdivide:
					ExportPlyMesh02(mesh, exportPath, mesh.FullName + ".ply")
				else:
					#we should subdivide this mesh and then export
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
					ap.DeleteObj(subMesh)
			else:
				if not isSubdivide:
					ExportPlyMesh(mesh, exportPath, mesh.FullName + ".ply")
				else:
					subOps = ap.ApplyOp("MeshSubdivideWithCenter", mesh)
					subMesh = ap.Selection(0)
					subOp = subOps(0)
					ap.SetValue(subOp.SubdivisionDepth, subLevel)
					ap.TransferAllPropertiesAcrossGenOp(subOp, subMesh, "", True, "")

					ap.FreezeObj(subMesh, "", "")
					tM = mesh.Kinematics.Global.Transform.Matrix4
					oTrans = subMesh.Kinematics.Global.Transform
					tM.InvertInPlace()
					oTrans.SetMatrix4(tM)
					subMesh.Kinematics.Global.Transform = oTrans
					ap.ResetTransform(subMesh, "siCtr", "siSRT", "siXYZ")

					ExportPlyMesh(subMesh, exportPath, mesh.FullName + ".ply")
					ap.DeleteObj(subMesh)

			



	return root

def GenerateObjectMatSegment(mesh, isSSSName):
	root = etree.Element("ref")
	if isSSSName:
		root.set("id", mesh.Materials[0].Library.Name + "." + "SSS" + str(mesh.Materials[0].Name))
	else:
		root.set("id", mesh.Materials[0].Library.Name + "." + str(mesh.Materials[0].Name))
	return root

def GetVertexCount(mesh):
	#return mesh.GetActivePrimitive2().Geometry.Points.Count
	return mesh.GetActivePrimitive2().Geometry.Triangles.Count * 3

def GetVertexCount02(mesh):
	return mesh.GetActivePrimitive2().Geometry.Points.Count

def GetPolygonsCount02(mesh):
	return mesh.GetActivePrimitive2().Geometry.Facets.Count

def GetPolygonsCount(mesh):
	#return mesh.GetActivePrimitive2().Geometry.Facets.Count
	return mesh.GetActivePrimitive2().Geometry.Triangles.Count

def ExportPlyMesh02(mesh, path, fileName):
	file = open(path + "\\" + fileName, "w")
	#write standard strings
	file.write("ply" + "\n")
	file.write("format ascii 1.0" + "\n")
	file.write("element vertex " + str(GetVertexCount02(mesh)) + "\n")
	file.write("property float32 x" + "\n")
	file.write("property float32 y" + "\n")
	file.write("property float32 z" + "\n")
	
	
	file.write("element face " + str(GetPolygonsCount02(mesh)) + "\n")
	file.write("property list uint8 int32 vertex_index" + "\n")
	file.write("end_header" + "\n")

	#next write coordinates of points
	for point in mesh.GetActivePrimitive2().Geometry.Points:
		file.write(str(point.Position.X) + " " + str(point.Position.Y) + " " + str(point.Position.Z) + "\n")

	#next write indexes of points per polygon
	for faset in mesh.GetActivePrimitive2().Geometry.Facets:
		file.write(str(faset.Points.Count) + " ") 
		for p in faset.Points:
			file.write(str(p.Index) + " ")
		file.write("\n")

	file.close()

def ExportPlyMesh(mesh, path, fileName):
	#lm("Start export to " + path + "\\" + fileName)
	samples = mesh.ActivePrimitive.Geometry.Clusters.Filter( "Sample" )
	if len(samples) == 0:
		isUV = False
	else:
		if len(samples[0].Properties.Filter( "uvspace" )) == 0:
			isUV = False
		else:
			isUV = True

	file = open(path + "\\" + fileName, "w")
	#write standard strings
	file.write("ply" + "\n")
	file.write("format ascii 1.0" + "\n")
	#next vertex count
	vertexCount = GetVertexCount(mesh)
	file.write("element vertex " + str(vertexCount) + "\n")
	#add properties for vertex
	file.write("property float32 x" + "\n")
	file.write("property float32 y" + "\n")
	file.write("property float32 z" + "\n")
	
	#point normal
	file.write("property float32 nx" + "\n")
	file.write("property float32 ny" + "\n")
	file.write("property float32 nz" + "\n")
	#uv coordinates
	if isUV:
		file.write("property float32 s" + "\n")
		file.write("property float32 t" + "\n")
	#polygons
	file.write("element face " + str(GetPolygonsCount(mesh)) + "\n")
	file.write("property list uint8 int32 vertex_index" + "\n")
	file.write("end_header" + "\n")

	

	if isUV:
		sample = samples[0]
		uvProperty = sample.Properties.Filter( "uvspace" )[0]
		uArray = uvProperty.Elements.Array[0]
		vArray = uvProperty.Elements.Array[1]

	triangles = mesh.ActivePrimitive.Geometry.Triangles
	lastTriangleIndex = 0
	for polygon in mesh.ActivePrimitive.Geometry.Polygons:
		#find number of vertices
		vertexCount = polygon.NbPoints
		numberOfTriangles = vertexCount - 2
		#lm("polygon " + str(polygon.Index) + " number of triangles=" + str(numberOfTriangles))
		#nodes of this polygon
		nodes = polygon.Nodes
		polygonVertices = polygon.Vertices
		#vertex indexes of triangulation of this polygon. Triples of indexes form triangles
		vertexIndexes = polygon.TriangleSubIndexArray
		
		#next enumerate triangles of current polygon
		for trIndex in range(numberOfTriangles):
			#and vertices of this triangle
			for delta in range(3):
				vIndex = trIndex * 3 + delta
				nodeIndex = nodes(vertexIndexes[vIndex]).Index#this is node index of a current vertex

				#point = mesh.GetActivePrimitive2().Geometry.Triangles[trIndex].Points[delta]
				point = mesh.GetActivePrimitive2().Geometry.Points[polygonVertices[vertexIndexes[trIndex*3 + delta]].Index]
				pos = point.Position
				file.write(str(pos.X) + " " + str(pos.Y) + " " + str(pos.Z) + " ")
				#next - normal

				#normal = point.Normal
				normal = triangles[lastTriangleIndex + trIndex].Points[delta].Normal
				file.write(str(normal.X) + " " + str(normal.Y) + " " + str(normal.Z) + " ")
				#uv coordinates
				if isUV:
					file.write(str(uArray[nodeIndex]) + " " + str(vArray[nodeIndex]))
				file.write("\n")
		lastTriangleIndex = lastTriangleIndex + numberOfTriangles

	#for triangle in mesh.GetActivePrimitive2().Geometry.Triangles:
	#	for point in triangle.Points:
	#		pos = point.Position
	#		file.write(str(pos.X) + " " + str(pos.Y) + " " + str(pos.Z) + " ")
	#		#next - normal
	#		normal = point.Normal
	#		file.write(str(normal.X) + " " + str(normal.Y) + " " + str(normal.Z))
	#		file.write("\n")
	#next - triangles
	for triangle in mesh.GetActivePrimitive2().Geometry.Triangles:
		file.write("3" + " " + str(3 * triangle.Index) + " " + str(3 * triangle.Index + 1) + " " + str(3 * triangle.Index + 2))

		#for point in triangle.Points:
		#	file.write(" " + str(point.Index))
		file.write("\n")
	#lm("Done!")

	file.close()

def isAttributeName(attributes, name):
	for a in attributes:
		if a.Name == name:
			return True
	return False

#generate hairs or meshes for object pc
def GeneratePointCloudSegment(pointCloud, shouldExport, path):
	pc = pointCloud.GetActivePrimitive2().Geometry
	attributes = pc.ICEAttributes

	if isAttributeName(attributes, "StrandCount") and isAttributeName(attributes, "StrandPosition"):
		#there are strands. Export it
		if shouldExport:
			GenerateHairFile(pc, path + "\\" + pointCloud.FullName +  ".hair")

		#next write data to xml
		root = etree.Element("shape")
		root.set("type", "hair")
		s1 = etree.SubElement(root, "string")
		s1.set("name", "filename")
		s1.set("value", str(pointCloud.FullName +  ".hair"))
		root.append(GenerateFloatSegment(pc.GetICEAttributeFromName("Size").DataArray[0], "radius"))

		#write a shader
		mat = pointCloud.Materials[0]
		if IsMatCorrect(mat):
			root.append(GenerateObjectMatSegment(pointCloud, False))
		elif IsSSSMatCorrect(mat):
			root.append(GenerateObjectMatSegment(pointCloud,False))
			root.append(GenerateObjectMatSegment(pointCloud, True))
		elif IsMeshLight(mat):
			root.append(GenerateMeshLightSegment(pointCloud.Shaders[0]))

		outArray = []
		outArray.append(root)

		return outArray
	else:
		if isAttributeName(attributes, "Shape"):
			#this shape should be one of known
			outArray = []
			if pointCloud.GetActivePrimitive2().Geometry.GetICEAttributeFromName("NbPoints").DataArray[0] > 0:
				for i in range(len(pointCloud.GetActivePrimitive2().Geometry.GetICEAttributeFromName("Shape").DataArray)):
					position = pointCloud.GetActivePrimitive2().Geometry.GetICEAttributeFromName("PointPosition").DataArray[i]
					shape = pointCloud.GetActivePrimitive2().Geometry.GetICEAttributeFromName("Shape").DataArray[i].Type
					size = pointCloud.GetActivePrimitive2().Geometry.GetICEAttributeFromName("Size").DataArray[i]
					rotation = pointCloud.GetActivePrimitive2().Geometry.GetICEAttributeFromName("Orientation").DataArray[i]

					if pointCloud.GetActivePrimitive2().Geometry.GetICEAttributeFromName("Shape").DataArray[i].Type == 4:
						#sphere
						outArray.append(GenerateSphere(pointCloud, size, position.X, position.Y, position.Z))

					elif pointCloud.GetActivePrimitive2().Geometry.GetICEAttributeFromName("Shape").DataArray[i].Type == 5:
						#box
						outArray.append(GenerateCube(pointCloud, size, size, size, rotation.RotX, rotation.RotY, rotation.RotZ, position.X, position.Y, position.Z))

					elif pointCloud.GetActivePrimitive2().Geometry.GetICEAttributeFromName("Shape").DataArray[i].Type == 6:
						#cylinder
						m = XSIMath.CreateMatrix3()
						rotation.GetMatrix3(m)
						values = m.Get2()
						
						outArray.append(GenerateCylinder(pointCloud, size, XSIMath.CreateMatrix4(values[0], values[1], values[2], 0, values[3], values[4], values[5], 0, values[6], values[7], values[8], 0, position.X, position.Y, position.Z, 1)))

					elif pointCloud.GetActivePrimitive2().Geometry.GetICEAttributeFromName("Shape").DataArray[i].Type == 3:
						#rectangle
						outArray.append(GenerateRectangle(pointCloud, size, size, rotation.RotX, rotation.RotY, rotation.RotZ, position.X, position.Y, position.Z))

					elif pointCloud.GetActivePrimitive2().Geometry.GetICEAttributeFromName("Shape").DataArray[i].Type == 2:
						#disk
						outArray.append(GenerateDisk(pointCloud, size, rotation.RotX, rotation.RotY, rotation.RotZ, position.X, position.Y, position.Z))

			if len(outArray) > 0:
				return outArray
			else:
				return None

		else:
			return None

def VectorToString(vector):
	return str(vector.X) + " " + str(vector.Y) + " " + str(vector.Z)

def GenerateHairFile(pc, filePath):
	#lm("Start write: " + filePath)
	file = open(filePath, "w")
	strandsNumber = len(pc.GetICEAttributeFromName("PointPosition").DataArray)
	strandSegments = pc.GetICEAttributeFromName("StrandCount").DataArray[0]
	#lm("Thera are " + str(strandsNumber) + " strands. In each strand " + str(strandSegments) + " segments")
	for i in range(strandsNumber):
		#write i-th strand
		file.write(VectorToString(pc.GetICEAttributeFromName("PointPosition").DataArray[i]) + "\n")

		for j in range(strandSegments):
			file.write(VectorToString(pc.GetICEAttributeFromName("StrandPosition").DataArray2D[i][j]) + "\n")

		file.write("\n")
	file.close()
	#lm("Done!")