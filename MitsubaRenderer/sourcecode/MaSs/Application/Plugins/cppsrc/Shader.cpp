#include "Shader.h"

namespace Mitsuba
{
	Shader::Shader(){}
	Shader::~Shader(){}

	XMLElement* Shader::WriteShader(XMLDocument* doc, XSI::Shader& shader, CString nameMat, bool useId)
	{
		CString shaderName = shader.GetParameterValue(Constants::mShaderType);
		XMLElement* rootElement = doc->NewElement(Constants::paramBsdf.GetAsciiString());

		if (useId) rootElement->SetAttribute("id", nameMat.GetAsciiString());
		//app.LogMessage(L"WriteShader: shader name: " + shaderName);

		if (shaderName == Constants::mDiffuse)
		{
			rootElement->SetAttribute(Constants::paramType.GetAsciiString(), Constants::typeDiffuse.GetAsciiString());

			Parameter oParam = shader.GetParameter(Constants::paramReflectance);
			CRef cref = oParam.GetParent();
			//app.LogMessage(L"WriteShader! parent: " + cref.GetAsText() );
			//app.LogMessage(L"WriteShader! matname: " + shaderName + L", shadername: " + shader.GetName() + L", paramReflectance: " + oParam.GetName());
			rootElement->InsertEndChild(WriteTextureOrSpectrum(doc, oParam, Constants::paramReflectance));
		}
		else if (shaderName == Constants::mRoughdiffuse)
		{
			rootElement->SetAttribute(Constants::paramType.GetAsciiString(), Constants::typeRoughdiffuse.GetAsciiString());

			rootElement->InsertEndChild(WriteTextureOrSpectrum(doc, shader.GetParameter(Constants::paramReflectance), Constants::paramReflectance));
			rootElement->InsertEndChild(WriteFloatOrTexture(doc, shader.GetParameter(Constants::paramAlpha), Constants::paramAlpha));
			rootElement->InsertEndChild(WriteElement(doc, Constants::attrBoolean, Constants::paramUseFastApprox, BoolToString(shader.GetParameterValue(Constants::paramUseFastApprox)).c_str()));
		}
		else if (shaderName == Constants::mDielectric)
		{
			rootElement->SetAttribute(Constants::paramType.GetAsciiString(), Constants::typeDielectric.GetAsciiString());

			rootElement->InsertEndChild(WriteIOR(doc, true, shader.GetParameterValue(Constants::paramIntIOR)));
			rootElement->InsertEndChild(WriteIOR(doc, false, shader.GetParameterValue(Constants::paramExtIOR)));
			rootElement->InsertEndChild(WriteTextureOrSpectrum(doc, shader.GetParameter(Constants::paramSpecularReflectance), Constants::paramSpecularReflectance));
			rootElement->InsertEndChild(WriteTextureOrSpectrum(doc, shader.GetParameter(Constants::paramSpecularTransmittance), Constants::paramSpecularTransmittance));
		}
		else if (shaderName == Constants::mThindielectric)
		{
			rootElement->SetAttribute(Constants::paramType.GetAsciiString(), Constants::typeThindielectric.GetAsciiString());

			rootElement->InsertEndChild(WriteIOR(doc, true, shader.GetParameterValue(Constants::paramIntIOR)));
			rootElement->InsertEndChild(WriteIOR(doc, false, shader.GetParameterValue(Constants::paramExtIOR)));
			rootElement->InsertEndChild(WriteTextureOrSpectrum(doc, shader.GetParameter(Constants::paramSpecularReflectance), Constants::paramSpecularReflectance));
			rootElement->InsertEndChild(WriteTextureOrSpectrum(doc, shader.GetParameter(Constants::paramSpecularTransmittance), Constants::paramSpecularTransmittance));
		}
		else if (shaderName == Constants::mRoughdielectric)
		{
			rootElement->SetAttribute(Constants::paramType.GetAsciiString(), Constants::typeRoughdielectric.GetAsciiString());

			CValue distrValue = shader.GetParameterValue(Constants::paramDistribution);
			rootElement->InsertEndChild(WriteDistribution(doc, distrValue));

			if (distrValue == Constants::paramAs)
			{
				rootElement->InsertEndChild(WriteFloatOrTexture(doc, shader.GetParameter(Constants::paramAlphaU), Constants::paramAlphaU));
				rootElement->InsertEndChild(WriteFloatOrTexture(doc, shader.GetParameter(Constants::paramAlphaV), Constants::paramAlphaV));
			}
			else
			{
				rootElement->InsertEndChild(WriteFloatOrTexture(doc, shader.GetParameter(Constants::paramAlpha), Constants::paramAlpha));
			}

			rootElement->InsertEndChild(WriteIOR(doc, true, shader.GetParameterValue(Constants::paramIntIOR)));
			rootElement->InsertEndChild(WriteIOR(doc, false, shader.GetParameterValue(Constants::paramExtIOR)));
			rootElement->InsertEndChild(WriteTextureOrSpectrum(doc, shader.GetParameter(Constants::paramSpecularReflectance), Constants::paramSpecularReflectance));
			rootElement->InsertEndChild(WriteTextureOrSpectrum(doc, shader.GetParameter(Constants::paramSpecularTransmittance), Constants::paramSpecularTransmittance));
		}
		else if (shaderName == Constants::mConductor)
		{
			rootElement->SetAttribute(Constants::paramType.GetAsciiString(), Constants::typeConductor.GetAsciiString());

			rootElement->InsertEndChild(WriteMaterialSegment(doc, shader.GetParameterValue(Constants::paramMaterial)));
			rootElement->InsertEndChild(WriteElement(doc, Constants::attrFloat, Constants::paramExtEta, shader.GetParameterValue(Constants::paramExtEta).GetAsText()));
			rootElement->InsertEndChild(WriteTextureOrSpectrum(doc, shader.GetParameter(Constants::paramSpecularReflectance), Constants::paramSpecularReflectance));
		}
		else if (shaderName == Constants::mRoughconductor)
		{
			rootElement->SetAttribute(Constants::paramType.GetAsciiString(), Constants::typeRoughconductor.GetAsciiString());

			CValue distrValue = shader.GetParameterValue(Constants::paramDistribution);
			rootElement->InsertEndChild(WriteDistribution(doc, distrValue));

			if (distrValue == Constants::paramAs)
			{
				rootElement->InsertEndChild(WriteFloatOrTexture(doc, shader.GetParameter(Constants::paramAlphaU), Constants::paramAlphaU));
				rootElement->InsertEndChild(WriteFloatOrTexture(doc, shader.GetParameter(Constants::paramAlphaV), Constants::paramAlphaV));
			}
			else
			{
				rootElement->InsertEndChild(WriteFloatOrTexture(doc, shader.GetParameter(Constants::paramAlpha), Constants::paramAlpha));
			}

			rootElement->InsertEndChild(WriteMaterialSegment(doc, shader.GetParameterValue(Constants::paramMaterial)));
			rootElement->InsertEndChild(WriteElement(doc, Constants::attrFloat, Constants::paramExtEta, shader.GetParameterValue(Constants::paramExtEta).GetAsText()));
			rootElement->InsertEndChild(WriteTextureOrSpectrum(doc, shader.GetParameter(Constants::paramSpecularReflectance), Constants::paramSpecularReflectance));
		}
		else if (shaderName == Constants::mPlastic)
		{
			rootElement->SetAttribute(Constants::paramType.GetAsciiString(), Constants::typePlastic.GetAsciiString());

			rootElement->InsertEndChild(WriteIOR(doc, true, shader.GetParameterValue(Constants::paramIntIOR)));
			rootElement->InsertEndChild(WriteIOR(doc, false, shader.GetParameterValue(Constants::paramExtIOR)));
			rootElement->InsertEndChild(WriteTextureOrSpectrum(doc, shader.GetParameter(Constants::paramSpecularReflectance), Constants::paramSpecularReflectance));
			rootElement->InsertEndChild(WriteTextureOrSpectrum(doc, shader.GetParameter(Constants::paramDiffuseReflectance), Constants::paramDiffuseReflectance));
			rootElement->InsertEndChild(WriteElement(doc, Constants::attrBoolean, Constants::paramNonlinear, shader.GetParameterValue(Constants::paramNonlinear).GetAsText()));
		}
		else if (shaderName == Constants::mRoughplastic)
		{
			rootElement->SetAttribute(Constants::paramType.GetAsciiString(), Constants::typeRoughplastic.GetAsciiString());

			CValue distrValue = shader.GetParameterValue(Constants::paramDistribution);
			rootElement->InsertEndChild(WriteDistribution(doc, distrValue));
			rootElement->InsertEndChild(WriteFloatOrTexture(doc, shader.GetParameter(Constants::paramAlpha), Constants::paramAlpha));
			rootElement->InsertEndChild(WriteIOR(doc, true, shader.GetParameterValue(Constants::paramIntIOR)));
			rootElement->InsertEndChild(WriteIOR(doc, false, shader.GetParameterValue(Constants::paramExtIOR)));
			rootElement->InsertEndChild(WriteTextureOrSpectrum(doc, shader.GetParameter(Constants::paramSpecularReflectance), Constants::paramSpecularReflectance));
			rootElement->InsertEndChild(WriteTextureOrSpectrum(doc, shader.GetParameter(Constants::paramDiffuseReflectance), Constants::paramDiffuseReflectance));
			rootElement->InsertEndChild(WriteElement(doc, Constants::attrBoolean, Constants::paramNonlinear, shader.GetParameterValue(Constants::paramNonlinear).GetAsText()));
		}
		else if (shaderName == Constants::mCoating)
		{
			rootElement->SetAttribute(Constants::paramType.GetAsciiString(), Constants::typeCoating.GetAsciiString());

			rootElement->InsertEndChild(WriteIOR(doc, true, shader.GetParameterValue(Constants::paramIntIOR)));
			rootElement->InsertEndChild(WriteIOR(doc, false, shader.GetParameterValue(Constants::paramExtIOR)));
			rootElement->InsertEndChild(WriteElement(doc, Constants::attrFloat, Constants::paramThickness, shader.GetParameterValue(Constants::paramThickness).GetAsText()));
			rootElement->InsertEndChild(WriteTextureOrSpectrum(doc, shader.GetParameter(Constants::paramSigmaA), Constants::paramSigmaA));
			rootElement->InsertEndChild(WriteTextureOrSpectrum(doc, shader.GetParameter(Constants::paramSpecularReflectance), Constants::paramSpecularReflectance));

			rootElement->InsertEndChild(AddChildBsdf(doc, shader, Constants::paramBsdf));
		}
		else if (shaderName == Constants::mRoughcoating)
		{
			rootElement->SetAttribute(Constants::paramType.GetAsciiString(), Constants::typeRoughcoating.GetAsciiString());

			CValue distrValue = shader.GetParameterValue(Constants::paramDistribution);
			rootElement->InsertEndChild(WriteDistribution(doc, distrValue));
			rootElement->InsertEndChild(WriteFloatOrTexture(doc, shader.GetParameter(Constants::paramAlpha), Constants::paramAlpha));
			rootElement->InsertEndChild(WriteIOR(doc, true, shader.GetParameterValue(Constants::paramIntIOR)));
			rootElement->InsertEndChild(WriteIOR(doc, false, shader.GetParameterValue(Constants::paramExtIOR)));
			rootElement->InsertEndChild(WriteElement(doc, Constants::attrFloat, Constants::paramThickness, shader.GetParameterValue(Constants::paramThickness).GetAsText()));
			rootElement->InsertEndChild(WriteTextureOrSpectrum(doc, shader.GetParameter(Constants::paramSigmaA), Constants::paramSigmaA));
			rootElement->InsertEndChild(WriteTextureOrSpectrum(doc, shader.GetParameter(Constants::paramSpecularReflectance), Constants::paramSpecularReflectance));

			rootElement->InsertEndChild(AddChildBsdf(doc, shader, Constants::paramBsdf));
		}
		else if (shaderName == Constants::mBumpmap)
		{
			rootElement->SetAttribute(Constants::paramType.GetAsciiString(), Constants::typeBumpmap.GetAsciiString());

			rootElement->InsertEndChild(AddChildBsdf(doc, shader, Constants::paramBsdf));

			Parameter oParam = shader.GetParameter(Constants::paramTexture);
			if (oParam.IsValid())
			{
				if (oParam.GetSources().GetCount() > 0)
				{
					XMLElement* tex1Element = doc->NewElement(Constants::paramTexture.GetAsciiString());
					tex1Element->SetAttribute(Constants::paramType.GetAsciiString(), Constants::paramScale.GetAsciiString());
					rootElement->InsertEndChild(tex1Element);

					tex1Element->InsertEndChild(WriteElement(doc, Constants::attrFloat, Constants::paramScale, shader.GetParameterValue(Constants::paramScale).GetAsText()));

					tex1Element->InsertEndChild(WriteTextureOrSpectrum(doc, oParam, L""));
				}
			}
		}
		else if (shaderName == Constants::mPhong)
		{
			rootElement->SetAttribute(Constants::paramType.GetAsciiString(), Constants::typePhong.GetAsciiString());

			rootElement->InsertEndChild(WriteElement(doc, Constants::attrFloat, Constants::paramExponent, shader.GetParameterValue(Constants::paramExponent).GetAsText()));
			rootElement->InsertEndChild(WriteTextureOrSpectrum(doc, shader.GetParameter(Constants::paramSpecularReflectance), Constants::paramSpecularReflectance));
			rootElement->InsertEndChild(WriteTextureOrSpectrum(doc, shader.GetParameter(Constants::paramDiffuseReflectance), Constants::paramDiffuseReflectance));
		}
		else if (shaderName == Constants::mWard)
		{
			rootElement->SetAttribute(Constants::paramType.GetAsciiString(), Constants::typeWard.GetAsciiString());

			rootElement->InsertEndChild(WriteVariantSegment(doc, shader.GetParameterValue(Constants::paramVariant)));
			rootElement->InsertEndChild(WriteFloatOrTexture(doc, shader.GetParameter(Constants::paramAlphaU), Constants::paramAlphaU));
			rootElement->InsertEndChild(WriteFloatOrTexture(doc, shader.GetParameter(Constants::paramAlphaV), Constants::paramAlphaV));
			rootElement->InsertEndChild(WriteTextureOrSpectrum(doc, shader.GetParameter(Constants::paramSpecularReflectance), Constants::paramSpecularReflectance));
			rootElement->InsertEndChild(WriteTextureOrSpectrum(doc, shader.GetParameter(Constants::paramDiffuseReflectance), Constants::paramDiffuseReflectance));
		}
		else if (shaderName == Constants::mMixturebsdf)
		{
			rootElement->SetAttribute(Constants::paramType.GetAsciiString(), Constants::typeMixturebsdf.GetAsciiString());

			XMLElement* weElement = doc->NewElement(Constants::attrString);
			weElement->SetAttribute(Constants::attrName, Constants::paramWeights.GetAsciiString());
			float value = shader.GetParameterValue(Constants::paramWeights);
			weElement->SetAttribute(Constants::attrValue, (FloatToString(value) + ", " + FloatToString(1 - value)).c_str());
			rootElement->InsertEndChild(weElement);

			rootElement->InsertEndChild(AddChildBsdf(doc, shader, Constants::paramBsdf1));
			rootElement->InsertEndChild(AddChildBsdf(doc, shader, Constants::paramBsdf2));
		}
		else if (shaderName == Constants::mBlendbsdf)
		{
			rootElement->SetAttribute(Constants::paramType.GetAsciiString(), Constants::typeBlendbsdf.GetAsciiString());

			rootElement->InsertEndChild(WriteFloatOrTexture(doc, shader.GetParameter(Constants::paramWeight), Constants::paramWeight));
			rootElement->InsertEndChild(AddChildBsdf(doc, shader, Constants::paramBsdf1));
			rootElement->InsertEndChild(AddChildBsdf(doc, shader, Constants::paramBsdf2));
		}
		else if (shaderName == Constants::mMask)
		{
			rootElement->SetAttribute(Constants::paramType.GetAsciiString(), Constants::typeMask.GetAsciiString());

			rootElement->InsertEndChild(WriteTextureOrSpectrum(doc, shader.GetParameter(Constants::paramOpacity), Constants::paramOpacity));
			rootElement->InsertEndChild(AddChildBsdf(doc, shader, Constants::paramBsdf));
		}
		else if (shaderName == Constants::mTwoSided)
		{
			rootElement->SetAttribute(Constants::paramType.GetAsciiString(), Constants::typeTwosided.GetAsciiString());

			rootElement->InsertEndChild(AddChildBsdf(doc, shader, Constants::paramBsdf));
		}
		else if (shaderName == Constants::mDifftrans)
		{
			rootElement->SetAttribute(Constants::paramType.GetAsciiString(), Constants::typeDifftrans.GetAsciiString());

			rootElement->InsertEndChild(WriteTextureOrSpectrum(doc, shader.GetParameter(Constants::paramTransmittance), Constants::paramTransmittance));
		}
		else if (shaderName == Constants::mHk)
		{
			rootElement->SetAttribute(Constants::paramType.GetAsciiString(), Constants::typeHk.GetAsciiString());

			rootElement->InsertEndChild(WriteMaterialSegment(doc, shader.GetParameterValue(Constants::paramMaterial)));
			rootElement->InsertEndChild(WriteElement(doc, Constants::attrFloat, Constants::paramThickness, shader.GetParameterValue(Constants::paramThickness).GetAsText()));
		}

		return rootElement;
	}

	XMLElement* Shader::WriteSSSShader(XMLDocument* doc, XSI::Shader& shader, CString nameMat, bool useID, bool isSSS)
	{
		XMLElement* root = nullptr;

		//app.LogMessage(L"Mat SSS: " + nameMat);
		CString shaderTypeName = shader.GetParameterValue(Constants::mShaderType);
		//app.LogMessage(L"Write SSS: " + shaderTypeName);
		if (shaderTypeName == Constants::mRoughSSSDipole)
		{
			
			if (isSSS)
			{
				root = doc->NewElement(Constants::attrSubsurface);
				if (useID) root->SetAttribute("id", nameMat.GetAsciiString());
				root->SetAttribute(Constants::attrType, Constants::attrDipole);

				root->InsertEndChild(WriteMaterialSegment(doc, shader.GetParameterValue(Constants::paramMaterial)));
				root->InsertEndChild(WriteElement(doc, Constants::attrFloat, Constants::paramScale, shader.GetParameterValue(Constants::paramScale)));
				root->InsertEndChild(WriteElement(doc, Constants::attrInteger, Constants::paramIrrSamples, shader.GetParameterValue(Constants::paramIrrSamples)));
			}
			else
			{
				root = doc->NewElement(Constants::attrBsdf);
				if (useID) root->SetAttribute("id", nameMat.GetAsciiString());
				root->SetAttribute(Constants::attrType, Constants::paramRoughPlastic.GetAsciiString());

				root->InsertEndChild(WriteDistribution(doc, shader.GetParameterValue(Constants::paramDistribution)));
				root->InsertEndChild(WriteFloatOrTexture(doc, shader.GetParameter(Constants::paramAlpha), Constants::paramAlpha));
				root->InsertEndChild(WriteIOR(doc, true, shader.GetParameterValue(Constants::paramIntIOR)));
				root->InsertEndChild(WriteIOR(doc, false, shader.GetParameterValue(Constants::paramExtIOR)));
				root->InsertEndChild(WriteTextureOrSpectrum(doc, shader.GetParameter(Constants::paramSpecularReflectance), Constants::paramSpecularReflectance));

				root->InsertEndChild(WriteElement(doc, Constants::attrSpectrum, Constants::paramDiffuseReflectance, "0"));
				root->InsertEndChild(WriteElement(doc, Constants::attrBoolean, Constants::paramNonlinear, BoolToString(shader.GetParameterValue(Constants::paramNonlinear)).c_str()));
			}
		}
		else if (shaderTypeName == Constants::mSSSDipole)
		{
			if (isSSS)
			{
				root = doc->NewElement(Constants::attrSubsurface);
				if (useID) root->SetAttribute("id", nameMat.GetAsciiString());
				root->SetAttribute(Constants::attrType, Constants::attrDipole);

				root->InsertEndChild(WriteMaterialSegment(doc, shader.GetParameterValue(Constants::paramMaterial)));
				root->InsertEndChild(WriteElement(doc, Constants::attrFloat, Constants::paramScale, shader.GetParameterValue(Constants::paramScale)));
				root->InsertEndChild(WriteElement(doc, Constants::attrInteger, Constants::paramIrrSamples, shader.GetParameterValue(Constants::paramIrrSamples)));
			}
			else
			{
				root = doc->NewElement(Constants::attrBsdf);
				if (useID) root->SetAttribute("id", nameMat.GetAsciiString());
				root->SetAttribute(Constants::attrType, Constants::paramPlastic.GetAsciiString());

				root->InsertEndChild(WriteIOR(doc, true, shader.GetParameterValue(Constants::paramIntIOR)));
				root->InsertEndChild(WriteIOR(doc, false, shader.GetParameterValue(Constants::paramExtIOR)));
				root->InsertEndChild(WriteTextureOrSpectrum(doc, shader.GetParameter(Constants::paramSpecularReflectance), Constants::paramSpecularReflectance));

				root->InsertEndChild(WriteElement(doc, Constants::attrSpectrum, Constants::paramDiffuseReflectance, "0"));
				root->InsertEndChild(WriteElement(doc, Constants::attrBoolean, Constants::paramNonlinear, BoolToString(shader.GetParameterValue(Constants::paramNonlinear)).c_str()));
			}
		}
		else
		{
			root = WriteStandartDiffuse(doc);
			if (useID) root->SetAttribute("id", nameMat.GetAsciiString());
		}

		return root;
	}

	XMLElement* Shader::WriteTextureOrSpectrum(XMLDocument* doc, Parameter& param, CString pName)
	{
		XMLElement* root = nullptr;

		//app.LogMessage(L"WriteTextureOrSpectrum! Param: " + param.GetName() + L", shader: " + param.GetParent().GetAsText() + L", pName: " + pName);
		CRef source = param.GetSource();
		if (source.IsValid())
		{
			//app.LogMessage(L"cref shader: " + source.GetAsText());
			XSI::Shader paramShader(param.GetParent());
			XSI::Shader mShader = FindShaderOnPort(paramShader, param.GetName(), false);
			if (mShader.IsValid())
			{
				//app.LogMessage(L"WriteTextureOrSpectrum! Find shader: " + mShader.GetName() + L", on param: " + param.GetName());
				if (Constants::NameInList(Constants::textureTypeArray, mShader.GetParameterValue(Constants::mShaderType).GetAsText().GetAsciiString()))
				{
					//app.LogMessage(L"WriteTextureOrSpectrum 2! param in texturelist");
					root = WriteBitmap(doc, mShader, pName);
				}
				else if (ShaderIsImageType(mShader))
				{
					XSI::ImageClip2 mImage = FindImageClipOnPort(mShader, Constants::paramTex);
					if (mImage.IsValid())
					{
						//app.LogMessage(L"WriteTextureOrSpectrum 3! shader is Image: " + mShader.GetName());
						//app.LogMessage(L"WriteTextureOrSpectrum 33! textures count: " + CString(mShader.GetImageClips().GetCount()));
						root = GenerateTextureSegment(doc, mShader, pName);
					}
					else app.LogMessage(L"WriteTextureOrSpectrum! Param " + param.GetName() + L" - ImageClip not found!  ImageClip: " + mImage.GetName(), siWarningMsg);
				}
				else app.LogMessage(L"WriteTextureOrSpectrum! Shader is not Image: " + mShader.GetName(), siWarningMsg);
			}
			else app.LogMessage(L"WriteTextureOrSpectrum! Param " + param.GetName() + L" - shader not found!", siWarningMsg);
		}
		else app.LogMessage(L"WriteTextureOrSpectrum! Param " + param.GetName() + L" have not source!", siWarningMsg);

		if (root == nullptr) root = WriteSpectrumSegment(doc, param);

		return root;
	}

	XMLElement* Shader::WriteFloatOrTexture(XMLDocument* doc, Parameter& param, CString pName)
	{
		XMLElement* root = nullptr;

		//app.LogMessage(L"WriteTextureOrSpectrum! Param: " + param.GetName() + L", shader: " + param.GetParent().GetAsText() + L", pName: " + pName);
		CRef source = param.GetSource();
		if (source.IsValid())
		{
			//app.LogMessage(L"cref shader: " + source.GetAsText());
			XSI::Shader paramShader(param.GetParent());
			XSI::Shader mShader = FindShaderOnPort(paramShader, param.GetName(), false);
			if (mShader.IsValid())
			{
				if (ShaderIsImageType(mShader))
				{
					root = GenerateTextureSegment(doc, mShader, pName);
				}
				else
				{
					app.LogMessage(L"WriteTextureOrSpectrum! shader is wrong", siWarningMsg);
				}
			}
		}

		if (root == nullptr) root = WriteElement(doc, Constants::attrFloat, pName.GetAsciiString(), ColorToFloat(param));

		return root;
	}

	XMLElement* Shader::WriteIOR(XMLDocument* doc, bool param, CValue value)
	{
		const char* attr = nullptr;
		if (param) attr = Constants::attrIntIOR;
		else attr = Constants::attrExtIOR;

		return WriteElement(doc, Constants::attrFloat, attr, value.GetAsText());
	}

	XMLElement* Shader::WriteDistribution(XMLDocument* doc, CValue value)
	{
		return WriteElement(doc, Constants::attrString, Constants::attrDistribution, value.GetAsText());
	}

	XMLElement* Shader::WriteMaterialSegment(XMLDocument* doc, CValue value)
	{
		return WriteElement(doc, Constants::attrString, Constants::attrMaterial, value.GetAsText());
	}

	XMLElement* Shader::WriteVariantSegment(XMLDocument* doc, CValue value)
	{
		return WriteElement(doc, Constants::attrString, Constants::attrVariant, value.GetAsText());
	}

	XMLElement* Shader::WriteStandartDiffuse(XMLDocument* doc)
	{
		XMLElement* root = doc->NewElement(Constants::attrBsdf);
		root->SetAttribute(Constants::attrType, Constants::attrDiffuse);
		return root;
	}

	XMLElement* Shader::AddChildBsdf(XMLDocument* doc, XSI::Shader& shader, CString pName)
	{
		XSI::Shader oShader = FindShaderOnPort(shader, pName, true);
		if (!oShader.IsValid()) return WriteStandartDiffuse(doc);

		return WriteShader(doc, oShader, L"", false);
	}

	XMLElement* Shader::WriteBitmap(XMLDocument* doc, XSI::Shader& shader, CString pName)
	{
		XMLElement* root = doc->NewElement(Constants::attrTexture);

		if (!pName.IsEmpty()) root->SetAttribute(Constants::attrName, pName.GetAsciiString());

		Parameter oParam = shader.GetParameter(Constants::mShaderType);
		CValue oParamName = shader.GetParameterValue(Constants::mShaderType);
		//app.LogMessage(L"Bitmap name: " + CString(oParamName));
		if (oParamName == Constants::mBitmap)
		{
			root->SetAttribute(Constants::attrType, Constants::typeBitmap.GetAsciiString());

			XMLElement* s1Element = doc->NewElement(Constants::attrString);
			s1Element->SetAttribute(Constants::attrName, Constants::attrFilename);

			Parameter oParam = shader.GetParameter(Constants::paramTexture);
			if (oParam.IsValid())
			{
				if (oParam.GetSources().GetCount() > 0)
				{
					CRef source = oParam.GetSource();
					if (source.IsValid())
					{
						XSI::Shader oShader(source);
						CValue value = oShader.GetParameterValue(Constants::paramSourceFileName);
						s1Element->SetAttribute(Constants::attrValue, value.GetAsText().GetAsciiString());
					}
				}
			}
			root->InsertEndChild(s1Element);

			root->InsertEndChild(WriteElement(doc, Constants::attrString, Constants::paramWrapModeU, shader.GetParameterValue(Constants::paramWrapModeU).GetAsText()));
			root->InsertEndChild(WriteElement(doc, Constants::attrString, Constants::paramWrapModeV, shader.GetParameterValue(Constants::paramWrapModeV).GetAsText()));
			root->InsertEndChild(WriteElement(doc, Constants::attrString, Constants::paramFilterType, shader.GetParameterValue(Constants::paramFilterType).GetAsText()));

			root->InsertEndChild(WriteElement(doc, Constants::attrFloat, Constants::paramMaxAnisotropy, shader.GetParameterValue(Constants::paramMaxAnisotropy).GetAsText()));
			root->InsertEndChild(WriteElement(doc, Constants::attrBoolean, Constants::paramCache, shader.GetParameterValue(Constants::paramCache).GetAsText()));
			root->InsertEndChild(WriteElement(doc, Constants::attrFloat, Constants::paramUoffset, shader.GetParameterValue(Constants::paramUoffset).GetAsText()));
			root->InsertEndChild(WriteElement(doc, Constants::attrFloat, Constants::paramVoffset, shader.GetParameterValue(Constants::paramVoffset).GetAsText()));
			root->InsertEndChild(WriteElement(doc, Constants::attrFloat, Constants::paramUscale, shader.GetParameterValue(Constants::paramUscale).GetAsText()));
			root->InsertEndChild(WriteElement(doc, Constants::attrFloat, Constants::paramVscale, shader.GetParameterValue(Constants::paramVscale).GetAsText()));
		}
		else if (oParamName == Constants::mCheckboard)
		{
			root->SetAttribute(Constants::attrType, Constants::typeCheckerboard.GetAsciiString());

			root->InsertEndChild(WriteSpectrumSegment(doc, shader.GetParameter(Constants::paramColor0)));
			root->InsertEndChild(WriteSpectrumSegment(doc, shader.GetParameter(Constants::paramColor1)));
			root->InsertEndChild(WriteElement(doc, Constants::attrFloat, Constants::paramUoffset, shader.GetParameterValue(Constants::paramUoffset).GetAsText()));
			root->InsertEndChild(WriteElement(doc, Constants::attrFloat, Constants::paramVoffset, shader.GetParameterValue(Constants::paramVoffset).GetAsText()));
			root->InsertEndChild(WriteElement(doc, Constants::attrFloat, Constants::paramUscale, shader.GetParameterValue(Constants::paramUscale).GetAsText()));
			root->InsertEndChild(WriteElement(doc, Constants::attrFloat, Constants::paramVscale, shader.GetParameterValue(Constants::paramVscale).GetAsText()));
		}
		else if (oParamName == Constants::mGridTexture)
		{
			root->SetAttribute(Constants::attrType, Constants::typeGridTexture.GetAsciiString());

			root->InsertEndChild(WriteSpectrumSegment(doc, shader.GetParameter(Constants::paramColor0)));
			root->InsertEndChild(WriteSpectrumSegment(doc, shader.GetParameter(Constants::paramColor1)));
			root->InsertEndChild(WriteElement(doc, Constants::attrFloat, Constants::paramUoffset, shader.GetParameterValue(Constants::paramUoffset).GetAsText()));
			root->InsertEndChild(WriteElement(doc, Constants::attrFloat, Constants::paramVoffset, shader.GetParameterValue(Constants::paramVoffset).GetAsText()));
			root->InsertEndChild(WriteElement(doc, Constants::attrFloat, Constants::paramUscale, shader.GetParameterValue(Constants::paramUscale).GetAsText()));
			root->InsertEndChild(WriteElement(doc, Constants::attrFloat, Constants::paramVscale, shader.GetParameterValue(Constants::paramVscale).GetAsText()));
			root->InsertEndChild(WriteElement(doc, Constants::attrFloat, Constants::paramLineWidth, shader.GetParameterValue(Constants::paramLineWidth).GetAsText()));
		}
		else if (oParamName == Constants::mScale)
		{
			root->SetAttribute(Constants::attrType, Constants::typeScale.GetAsciiString());

			root->InsertEndChild(WriteElement(doc, Constants::attrFloat, Constants::paramScale, shader.GetParameterValue(Constants::paramValue).GetAsText()));
			root->InsertEndChild(WriteTextureOrSpectrum(doc, shader.GetParameter(Constants::paramInput), L""));
		}
		else if (oParamName == Constants::mVertexColors)
		{
			root->SetAttribute(Constants::attrType, Constants::typeVertexColors.GetAsciiString());
		}
		else if (oParamName == Constants::mWireframe)
		{
			root->SetAttribute(Constants::attrType, Constants::typeWireframe.GetAsciiString());

			root->InsertEndChild(WriteSpectrumSegment(doc, shader.GetParameter(Constants::paramInteriorColor)));
			root->InsertEndChild(WriteSpectrumSegment(doc, shader.GetParameter(Constants::paramEdgeColor)));
			root->InsertEndChild(WriteElement(doc, Constants::attrFloat, Constants::paramLineWidth, shader.GetParameterValue(Constants::paramLineWidth).GetAsText()));
			root->InsertEndChild(WriteElement(doc, Constants::attrFloat, Constants::paramStepWidth, shader.GetParameterValue(Constants::paramStepWidth).GetAsText()));
		}
		else if (oParamName == Constants::mCurvature)
		{
			root->SetAttribute(Constants::attrType, Constants::typeCurvature.GetAsciiString());
			root->InsertEndChild(WriteElement(doc, Constants::attrString, Constants::paramCurvature, shader.GetParameterValue(Constants::paramCurvature).GetAsText()));
			root->InsertEndChild(WriteElement(doc, Constants::attrFloat, Constants::paramScale, shader.GetParameterValue(Constants::paramScale).GetAsText()));
		}

		return root;
	}
	
	XMLElement* Shader::GenerateTextureSegment(XMLDocument* doc, XSI::Shader& shader, CString pName)
	{
		XSI::ImageClip2 imageClip = FindImageClipOnPort(shader, Constants::paramTex);
		if (!imageClip.IsValid()) return nullptr;

		XMLElement* root = doc->NewElement(Constants::attrTexture);
		root->SetAttribute(Constants::attrType, Constants::typeBitmap.GetAsciiString());
		if (!pName.IsEmpty()) root->SetAttribute(Constants::attrName, pName.GetAsciiString());

		root->InsertEndChild(WriteElement(doc, Constants::attrString, Constants::attrFilename, imageClip.GetFileName().GetAsciiString()));
		return root;
	}

	bool Shader::IsShaderCorrect(CString shaderName, XSI::Shader& xsiShader)
	{
		return Constants::NameInList(Constants::shaderTypeArray, shaderName.GetAsciiString());

		/*if (shaderName == L"mBumpMap")
		{
		Parameter oParam = xsiShader.GetParameter(Constants::paramTexture);
		if (!oParam.IsValid()) return false;
		if (oParam.GetSources().GetCount() == 0) return false;

		CRef source = oParam.GetSource();
		XSI::Shader oShader(source);

		XSI::Shader oShaderParent = oShader.GetParent();
		Parameter opParam = oShaderParent.GetParameter(L"mShaderType");
		if (opParam.IsValid())
		{
		CString shaderName2 = opParam.GetValue();
		if (shaderName2 == L"mBitmap" ||
		shaderName2 == L"mCheckboard" ||
		shaderName2 == L"mGridTexture" ||
		shaderName2 == L"mScale" ||
		shaderName2 == L"mVertexColors" ||
		shaderName2 == L"mWireframe" ||
		shaderName2 == L"mCurvature") return true;
		else return false;
		}
		else
		{
		if (oShaderParent.GetParameter(Constants::paramTex).GetSources().GetCount() == 1)return true;
		else return false;
		}
		}
		else
		{
		return true;
		}*/
	}

	bool Shader::IsSSSShaderCorrect(CString shaderName)
	{
		if (shaderName == L"mSSSDipole" || shaderName == L"mRoughSSSDipole") return true;

		return false;
	}

	bool Shader::ShaderIsImageType(XSI::Shader& shader)
	{
		//app.LogMessage(L"Shader: " + shader.GetName() + L", className: " + shader.GetClassIDName() + L", class: " + CString(shader.GetClassID()));

		if (shader.GetClassIDName() != L"Texture")
		{
			app.LogMessage(L"ShaderIsImageType: shader class is not Texture! " + shader.GetName(), siWarningMsg);
			return false;
		}

		Parameter paramTex = shader.GetParameter(L"tex");
		if (!paramTex.IsValid() || !paramTex.GetSource().IsValid())
		{
			app.LogMessage(L"ShaderIsImageType: param tex invalid or haven`t source! " + shader.GetName(), siWarningMsg);
			return false;
		}
		
		return true;
	}
}
