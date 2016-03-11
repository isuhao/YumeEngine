//----------------------------------------------------------------------------
//Yume Engine
//Copyright (C) 2015  arkenthera
//This program is free software; you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation; either version 2 of the License, or
//(at your option) any later version.
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//You should have received a copy of the GNU General Public License along
//with this program; if not, write to the Free Software Foundation, Inc.,
//51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.*/
//----------------------------------------------------------------------------
//
// File : <Filename>
// Date : <Date>
// Comments :
//
//----------------------------------------------------------------------------
#include "YumeHeaders.h"
#include "YumeMaterial.h"

#include "YumeTexture.h"
#include "YumeTexture2D.h"
#include "Core/YumeIO.h"
#include "Core/YumeFile.h"
#include "Renderer/YumeResourceManager.h"
#include "Scene/YumeScene.h"
#include "Core/YumeComponentAnimation.h"
#include "Renderer/YumeRenderPass.h"

#include <boost/algorithm/string.hpp>
#include "Core/YumeDefaults.h"
#include "Core/YumeThread.h"
#include "Core/YumeVectorBuffer.h"
#include "Engine/YumeEngine.h"

#include "Logging/logging.h"


namespace YumeEngine
{

	extern const char* wrapModeNames[];

	static const char* textureUnitNames[] =
	{
		"diffuse",
		"normal",
		"specular",
		"emissive",
		"environment",
		"volume",
		"custom1",
		"custom2",
		"lightramp",
		"lightshape",
		"shadowmap",
		"faceselect",
		"indirection",
		"depth",
		"light",
		"zone",
		0
	};

	static const char* cullModeNames[] =
	{
		"none",
		"ccw",
		"cw",
		0
	};

	static const char* fillModeNames[] =
	{
		"solid",
		"wireframe",
		"point",
		0
	};

	TextureUnit ParseTextureUnitName(YumeString name)
	{
		boost::to_lower(name);
		boost::trim(name);

		TextureUnit unit = (TextureUnit)GetStringListIndex(name.c_str(),textureUnitNames,MAX_TEXTURE_UNITS);
		if(unit == MAX_TEXTURE_UNITS)
		{
			// Check also for shorthand names
			if(name == "diff")
				unit = TU_DIFFUSE;
			else if(name == "albedo")
				unit = TU_DIFFUSE;
			else if(name == "norm")
				unit = TU_NORMAL;
			else if(name == "spec")
				unit = TU_SPECULAR;
			else if(name == "env")
				unit = TU_ENVIRONMENT;
			// Finally check for specifying the texture unit directly as a number
			else if(name.length() < 3)
				unit = (TextureUnit)Clamp(ToInt(name),0,MAX_TEXTURE_UNITS - 1);
		}

		if(unit == MAX_TEXTURE_UNITS)
			YUMELOG_ERROR("Unknown texture unit name " + name);

		return unit;
	}

	static TechniqueEntry noEntry;

	bool CompareTechniqueEntries(const TechniqueEntry& lhs,const TechniqueEntry& rhs)
	{
		if(lhs.lodDistance_ != rhs.lodDistance_)
			return lhs.lodDistance_ > rhs.lodDistance_;
		else
			return lhs.qualityLevel_ > rhs.qualityLevel_;
	}

	TechniqueEntry::TechniqueEntry():
		qualityLevel_(0),
		lodDistance_(0.0f)
	{
	}

	TechniqueEntry::TechniqueEntry(YumeRenderTechnique* tech,unsigned qualityLevel,float lodDistance):
		technique_(tech),
		qualityLevel_(qualityLevel),
		lodDistance_(lodDistance)
	{
	}

	TechniqueEntry::~TechniqueEntry()
	{
	}

	ShaderParameterAnimationInfo::ShaderParameterAnimationInfo(YumeMaterial* target,const YumeString& name,ValueAnimation* attributeAnimation,
		WrapMode wrapMode,float speed):
		ValueAnimationInfo(target,attributeAnimation,wrapMode,speed),
		name_(name)
	{
	}

	ShaderParameterAnimationInfo::ShaderParameterAnimationInfo(const ShaderParameterAnimationInfo& other):
		ValueAnimationInfo(other),
		name_(other.name_)
	{
	}

	ShaderParameterAnimationInfo::~ShaderParameterAnimationInfo()
	{
	}

	void ShaderParameterAnimationInfo::ApplyValue(const Variant& newValue)
	{
		static_cast<YumeMaterial*>(target_.get())->SetShaderParameter(name_,newValue);
	}

	YumeMaterial::YumeMaterial():
		auxViewFrameNumber_(0),
		shaderParameterHash_(0),
		occlusion_(true),
		specular_(false),
		subscribed_(false),
		batchedParameterUpdate_(false)
	{
		ResetToDefaults();
	}

	YumeMaterial::~YumeMaterial()
	{
	}

	bool YumeMaterial::BeginLoad(YumeFile& source)
	{
		
		//Graphics* graphics = GetSubsystem<Graphics>();
		//if(!graphics)
		//	return true;

		//String extension = GetExtension(source.GetName());

		//bool success = false;
		//if(extension == ".xml")
		//{
		//	success = BeginLoadXML(source);
		//	if(!success)
		//		success = BeginLoadJSON(source);

		//	if(success)
		//		return true;
		//}
		//else // Load JSON file
		//{
		//	success = BeginLoadJSON(source);
		//	if(!success)
		//		success = BeginLoadXML(source);

		//	if(success)
		//		return true;
		//}

		
		//ResetToDefaults();
		//loadJSONFile_.Reset();
		return false;
	}

	bool YumeMaterial::EndLoad()
	{
		
		//Graphics* graphics = GetSubsystem<Graphics>();
		//if(!graphics)
		//	return true;

		//bool success = false;
		//if(loadXMLFile_)
		//{
		//	// If async loading, get the techniques / textures which should be ready now
		//	XMLElement rootElem = loadXMLFile_->GetRoot();
		//	success = Load(rootElem);
		//}

		//if(loadJSONFile_)
		//{
		//	JSONValue rootVal = loadJSONFile_->GetRoot();
		//	success = Load(rootVal);
		//}

		//loadXMLFile_.Reset();
		//loadJSONFile_.Reset();
		return false;
	}

	bool YumeMaterial::BeginLoadXML(YumeFile& source)
	{
		//		ResetToDefaults();
		//		loadXMLFile_ = new XMLFile(context_);
		//		if(loadXMLFile_->Load(source))
		//		{
		//			// If async loading, scan the XML content beforehand for technique & texture resources
		//			// and request them to also be loaded. Can not do anything else at this point
		//			if(GetAsyncLoadState() == ASYNC_LOADING)
		//			{
		//				ResourceCache* cache = GetSubsystem<ResourceCache>();
		//				XMLElement rootElem = loadXMLFile_->GetRoot();
		//				XMLElement techniqueElem = rootElem.GetChild("technique");
		//				while(techniqueElem)
		//				{
		//					cache->BackgroundLoadResource<Technique>(techniqueElem.GetAttribute("name"),true,this);
		//					techniqueElem = techniqueElem.GetNext("technique");
		//				}
		//
		//				XMLElement textureElem = rootElem.GetChild("texture");
		//				while(textureElem)
		//				{
		//					String name = textureElem.GetAttribute("name");
		//					// Detect cube maps by file extension: they are defined by an XML file
		//					
		//					if(GetExtension(name) == ".xml")
		//					{
		//#ifdef DESKTOP_GRAPHICS
		//						TextureUnit unit = TU_DIFFUSE;
		//						if(textureElem.HasAttribute("unit"))
		//							unit = ParseTextureUnitName(textureElem.GetAttribute("unit"));
		//						if(unit == TU_VOLUMEMAP)
		//							cache->BackgroundLoadResource<Texture3D>(name,true,this);
		//						else
		//#endif
		//							cache->BackgroundLoadResource<TextureCube>(name,true,this);
		//				}
		//					else
		//						cache->BackgroundLoadResource<Texture2D>(name,true,this);
		//					textureElem = textureElem.GetNext("texture");
		//			}
		//		}
		//
		//			return true;
		//	}
		//
		return false;
	}


	bool YumeMaterial::Save(YumeFile& dest) const
	{
		/*SharedPtr<XMLFile> xml(new XMLFile(context_));
		XMLElement materialElem = xml->CreateRoot("material");

		Save(materialElem);
		return xml->Save(dest);*/
		return false;
	}

	bool YumeMaterial::Load(const XmlNode& source)
	{
		
		return true;
	}

	bool YumeMaterial::Save(XmlNode& dest) const
	{
		return true;
	}

	void YumeMaterial::SetNumTechniques(unsigned num)
	{
		if(!num)
			return;

		techniques_.resize(num);
		RefreshMemoryUse();
	}

	void YumeMaterial::SetTechnique(unsigned index,YumeRenderTechnique* tech,unsigned qualityLevel,float lodDistance)
	{
		if(index >= techniques_.size())
			return;

		techniques_[index] = TechniqueEntry(tech,qualityLevel,lodDistance);
		CheckOcclusion();
	}

	void YumeMaterial::SetShaderParameter(const YumeString& name,const Variant& value)
	{
		MaterialShaderParameter newParam;
		newParam.name_ = name;
		newParam.value_ = value;
		YumeHash nameHash(name);
		shaderParameters_[nameHash] = newParam;

		if(nameHash == PSP_MATSPECCOLOR)
		{
			VariantType type = value.GetType();
			if(type == VAR_VECTOR3)
			{
				const Vector3& vec = value.GetVector3();
				specular_ = vec.x_ > 0.0f || vec.y_ > 0.0f || vec.z_ > 0.0f;
			}
			else if(type == VAR_VECTOR4)
			{
				const Vector4& vec = value.GetVector4();
				specular_ = vec.x_ > 0.0f || vec.y_ > 0.0f || vec.z_ > 0.0f;
			}
		}

		if(!batchedParameterUpdate_)
		{
			RefreshShaderParameterHash();
			RefreshMemoryUse();
		}
	}

	void YumeMaterial::SetShaderParameterAnimation(const YumeString& name,ValueAnimation* animation,WrapMode wrapMode,float speed)
	{
		ShaderParameterAnimationInfo* info = GetShaderParameterAnimationInfo(name);

		if(animation)
		{
			if(info && info->GetAnimation() == animation)
			{
				info->SetWrapMode(wrapMode);
				info->SetSpeed(speed);
				return;
			}

			if(shaderParameters_.find(name) == shaderParameters_.end())
			{
				YUMELOG_ERROR(GetName() + " has no shader parameter: " + name);
				return;
			}

			YumeHash nameHash(name);
			shaderParameterAnimationInfos_[nameHash] = SharedPtr<ShaderParameterAnimationInfo>(new ShaderParameterAnimationInfo(this,name,animation,wrapMode,speed));
			UpdateEventSubscription();
		}
		else
		{
			if(info)
			{
				YumeHash nameHash(name);
				shaderParameterAnimationInfos_.erase(nameHash);
				UpdateEventSubscription();
			}
		}
	}

	void YumeMaterial::SetShaderParameterAnimationWrapMode(const YumeString& name,WrapMode wrapMode)
	{
		ShaderParameterAnimationInfo* info = GetShaderParameterAnimationInfo(name);
		if(info)
			info->SetWrapMode(wrapMode);
	}

	void YumeMaterial::SetShaderParameterAnimationSpeed(const YumeString& name,float speed)
	{
		ShaderParameterAnimationInfo* info = GetShaderParameterAnimationInfo(name);
		if(info)
			info->SetSpeed(speed);
	}

	void YumeMaterial::SetTexture(TextureUnit unit,YumeTexture* texture)
	{
		if(unit < MAX_TEXTURE_UNITS)
		{
			if(texture)
				textures_[unit] = SharedPtr<YumeTexture>(texture);
			else
				textures_.erase(unit);
		}
	}

	void YumeMaterial::SetUVTransform(const Vector2& offset,float rotation,const Vector2& repeat)
	{
		Matrix3x4 transform(Matrix3x4::IDENTITY);
		transform.m00_ = repeat.x_;
		transform.m11_ = repeat.y_;
		transform.m03_ = -0.5f * transform.m00_ + 0.5f;
		transform.m13_ = -0.5f * transform.m11_ + 0.5f;

		Matrix3x4 rotationMatrix(Matrix3x4::IDENTITY);
		rotationMatrix.m00_ = Cos(rotation);
		rotationMatrix.m01_ = Sin(rotation);
		rotationMatrix.m10_ = -rotationMatrix.m01_;
		rotationMatrix.m11_ = rotationMatrix.m00_;
		rotationMatrix.m03_ = 0.5f - 0.5f * (rotationMatrix.m00_ + rotationMatrix.m01_);
		rotationMatrix.m13_ = 0.5f - 0.5f * (rotationMatrix.m10_ + rotationMatrix.m11_);

		transform = rotationMatrix * transform;

		Matrix3x4 offsetMatrix = Matrix3x4::IDENTITY;
		offsetMatrix.m03_ = offset.x_;
		offsetMatrix.m13_ = offset.y_;

		transform = offsetMatrix * transform;

		SetShaderParameter("UOffset",Vector4(transform.m00_,transform.m01_,transform.m02_,transform.m03_));
		SetShaderParameter("VOffset",Vector4(transform.m10_,transform.m11_,transform.m12_,transform.m13_));
	}

	void YumeMaterial::SetUVTransform(const Vector2& offset,float rotation,float repeat)
	{
		SetUVTransform(offset,rotation,Vector2(repeat,repeat));
	}

	void YumeMaterial::SetCullMode(CullMode mode)
	{
		cullMode_ = mode;
	}

	void YumeMaterial::SetShadowCullMode(CullMode mode)
	{
		shadowCullMode_ = mode;
	}

	void YumeMaterial::SetFillMode(FillMode mode)
	{
		fillMode_ = mode;
	}

	void YumeMaterial::SetDepthBias(const BiasParameters& parameters)
	{
		depthBias_ = parameters;
		depthBias_.Validate();
	}

	void YumeMaterial::SetRenderOrder(unsigned char order)
	{
		renderOrder_ = order;
	}

	void YumeMaterial::SetScene(YumeScene* scene)
	{
		subscribed_ = false;
		scene_ = SharedPtr<YumeScene>(scene);
		UpdateEventSubscription();
	}

	void YumeMaterial::RemoveShaderParameter(const YumeString& name)
	{
		YumeHash nameHash(name);
		shaderParameters_.erase(nameHash);

		if(nameHash == PSP_MATSPECCOLOR)
			specular_ = false;

		RefreshShaderParameterHash();
		RefreshMemoryUse();
	}

	void YumeMaterial::ReleaseShaders()
	{
		for(unsigned i = 0; i < techniques_.size(); ++i)
		{
			YumeRenderTechnique* tech = techniques_[i].technique_.get();
			if(tech)
				tech->ReleaseShaders();
		}
	}

	SharedPtr<YumeMaterial> YumeMaterial::Clone(const YumeString& cloneName) const
	{
		SharedPtr<YumeMaterial> ret = boost::shared_ptr<YumeMaterial>(new YumeMaterial);
		
		ret->SetName(cloneName);
		ret->techniques_ = techniques_;
		ret->shaderParameters_ = shaderParameters_;
		ret->shaderParameterHash_ = shaderParameterHash_;
		ret->textures_ = textures_;
		ret->occlusion_ = occlusion_;
		ret->specular_ = specular_;
		ret->cullMode_ = cullMode_;
		ret->shadowCullMode_ = shadowCullMode_;
		ret->fillMode_ = fillMode_;
		ret->renderOrder_ = renderOrder_;
		ret->RefreshMemoryUse();

		return ret;
	}

	void YumeMaterial::SortTechniques()
	{
		std::sort(techniques_.begin(),techniques_.end(),CompareTechniqueEntries);
	}

	void YumeMaterial::MarkForAuxView(unsigned frameNumber)
	{
		auxViewFrameNumber_ = frameNumber;
	}

	const TechniqueEntry& YumeMaterial::GetTechniqueEntry(unsigned index) const
	{
		return index < techniques_.size() ? techniques_[index] : noEntry;
	}

	YumeRenderTechnique* YumeMaterial::GetTechnique(unsigned index) const
	{
		return index < techniques_.size() ? techniques_[index].technique_.get() : (YumeRenderTechnique*)0;
	}

	YumeRenderPass* YumeMaterial::GetPass(unsigned index,const YumeString& passName) const
	{
		YumeRenderTechnique* tech = index < techniques_.size() ? techniques_[index].technique_.get() : (YumeRenderTechnique*)0;
		return tech ? tech->GetPass(passName) : 0;
	}

	YumeTexture* YumeMaterial::GetTexture(TextureUnit unit) const
	{
		YumeMap<TextureUnit,SharedPtr<YumeTexture> >::const_iterator i = textures_.find(unit);
		return i != textures_.end() ? i->second.get() : (YumeTexture*)0;
	}

	const Variant& YumeMaterial::GetShaderParameter(const YumeString& name) const
	{
		YumeMap<YumeHash,MaterialShaderParameter>::const_iterator i = shaderParameters_.find(name);
		return i != shaderParameters_.end() ? i->second.value_ : Variant::EMPTY;
	}

	ValueAnimation* YumeMaterial::GetShaderParameterAnimation(const YumeString& name) const
	{
		ShaderParameterAnimationInfo* info = GetShaderParameterAnimationInfo(name);
		return info == 0 ? 0 : info->GetAnimation();
	}

	WrapMode YumeMaterial::GetShaderParameterAnimationWrapMode(const YumeString& name) const
	{
		ShaderParameterAnimationInfo* info = GetShaderParameterAnimationInfo(name);
		return info == 0 ? WM_LOOP : info->GetWrapMode();
	}

	float YumeMaterial::GetShaderParameterAnimationSpeed(const YumeString& name) const
	{
		ShaderParameterAnimationInfo* info = GetShaderParameterAnimationInfo(name);
		return info == 0 ? 0 : info->GetSpeed();
	}

	YumeScene* YumeMaterial::GetScene() const
	{
		return scene_.get();
	}

	YumeString YumeMaterial::GetTextureUnitName(TextureUnit unit)
	{
		return textureUnitNames[unit];
	}

	Variant YumeMaterial::ParseShaderParameterValue(const YumeString& value)
	{
		YumeString valueTrimmed = value;
		boost::trim(valueTrimmed);
		if(valueTrimmed.length() && IsAlpha((unsigned)valueTrimmed[0]))
			return Variant(ToBool(valueTrimmed));
		else
			return ToVectorVariant(valueTrimmed);
	}

	void YumeMaterial::CheckOcclusion()
	{
		// Determine occlusion by checking the base pass of each technique
		occlusion_ = false;
		for(unsigned i = 0; i < techniques_.size(); ++i)
		{
			YumeRenderTechnique* tech = techniques_[i].technique_.get();
			if(tech)
			{
				YumeRenderPass* pass = tech->GetPass("base");
				if(pass && pass->GetDepthWrite() && !pass->GetAlphaMask())
					occlusion_ = true;
			}
		}
	}

	void YumeMaterial::ResetToDefaults()
	{
		// Needs to be a no-op when async loading, as this does a GetResource() which is not allowed from worker threads
		if(!YumeThreadWrapper::IsMainThread())
			return;

		SetNumTechniques(1);
		SetTechnique(0,YumeEngine3D::Get()->GetResourceManager()->PrepareResource<YumeRenderTechnique>("Techniques/NoTexture.xml").get());

		textures_.clear();

		batchedParameterUpdate_ = true;
		shaderParameters_.clear();
		SetShaderParameter("UOffset",Vector4(1.0f,0.0f,0.0f,0.0f));
		SetShaderParameter("VOffset",Vector4(0.0f,1.0f,0.0f,0.0f));
		SetShaderParameter("MatDiffColor",Vector4::ONE);
		SetShaderParameter("MatEmissiveColor",Vector3::ZERO);
		SetShaderParameter("MatEnvMapColor",Vector3::ONE);
		SetShaderParameter("MatSpecColor",Vector4(0.0f,0.0f,0.0f,1.0f));
		batchedParameterUpdate_ = false;

		cullMode_ = CULL_CCW;
		shadowCullMode_ = CULL_CCW;
		fillMode_ = FILL_SOLID;
		depthBias_ = BiasParameters(0.0f,0.0f);
		renderOrder_ = DEFAULT_RENDER_ORDER;

		RefreshShaderParameterHash();
		RefreshMemoryUse();
	}

	void YumeMaterial::RefreshShaderParameterHash()
	{
		/*VectorBuffer temp;
		for(YumeMap<YumeHash,MaterialShaderParameter>::const_iterator i = shaderParameters_.begin();
			i != shaderParameters_.end(); ++i)
		{
			temp.WriteYumeHash(i->first);
			temp.WriteVariant(i->second.value_);
		}

		shaderParameterHash_ = 0;
		const unsigned char* data = temp.GetData();
		unsigned dataSize = temp.GetSize();
		for(unsigned i = 0; i < dataSize; ++i)
			shaderParameterHash_ = SDBMHash(shaderParameterHash_,data[i]);*/
	}

	void YumeMaterial::RefreshMemoryUse()
	{
		unsigned memoryUse = sizeof(YumeMaterial);

		memoryUse += techniques_.size() * sizeof(TechniqueEntry);
		memoryUse += MAX_TEXTURE_UNITS * sizeof(SharedPtr<YumeTexture>);
		memoryUse += shaderParameters_.size() * sizeof(MaterialShaderParameter);

		SetMemoryUsage(memoryUse);
	}

	ShaderParameterAnimationInfo* YumeMaterial::GetShaderParameterAnimationInfo(const YumeString& name) const
	{
		YumeHash nameHash(name);
		YumeMap<YumeHash,SharedPtr<ShaderParameterAnimationInfo> >::const_iterator i = shaderParameterAnimationInfos_.find(nameHash);
		if(i == shaderParameterAnimationInfos_.end())
			return 0;
		return i->second.get();
	}

	void YumeMaterial::UpdateEventSubscription()
	{
		if(shaderParameterAnimationInfos_.size() && !subscribed_)
		{
			subscribed_ = true;
		}
		else if(subscribed_)
		{
			subscribed_ = false;
		}
	}
}