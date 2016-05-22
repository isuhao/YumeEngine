//--------------------------------------------------------------------------------
//This is a file from Arkengine
//
//
//Copyright (c) arkenthera.All rights reserved.
//
//BasicRenderWindow.cpp
//--------------------------------------------------------------------------------


#include "Core/YumeHeaders.h"
#include "FrustumCulling.h"
#include "Logging/logging.h"
#include "Core/YumeMain.h"
#include "Renderer/YumeLPVCamera.h"
#include <boost/shared_ptr.hpp>

#include "Input/YumeInput.h"

#include "Renderer/YumeTexture2D.h"

#include "Renderer/YumeResourceManager.h"


#include "Engine/YumeEngine.h"

#include "UI/YumeDebugOverlay.h"

#include "Renderer/Light.h"
#include "Renderer/StaticModel.h"
#include "Renderer/Scene.h"

#include "UI/YumeOptionsMenu.h"
#include "Renderer/RenderPass.h"

YUME_DEFINE_ENTRY_POINT(YumeEngine::GodRays);

#define TURNOFF 1
//#define NO_MODEL
//#define OBJECTS_CAST_SHADOW
#define NO_SKYBOX
#define NO_PLANE

namespace YumeEngine
{



	GodRays::GodRays()
		: angle1_(0),
		updown1_(0),
		leftRight1_(0)
	{
		REGISTER_ENGINE_LISTENER;
	}

	GodRays::~GodRays()
	{

	}

	void GodRays::Start()
	{
		YumeResourceManager* rm_ = gYume->pResourceManager;
		YumeMiscRenderer* renderer = gYume->pRenderer;
		Scene* scene = renderer->GetScene();
		YumeCamera* camera = renderer->GetCamera();

		gYume->pInput->AddListener(this);

#ifndef DISABLE_CEF

		optionsMenu_ = new YumeOptionsMenu;
		gYume->pUI->AddUIElement(optionsMenu_);
		optionsMenu_->SetVisible(true);

		overlay_ = new YumeDebugOverlay;
		overlay_->GetBinding("SampleName")->SetValue("Frustum Culling");
		gYume->pUI->AddUIElement(overlay_);
		overlay_->SetVisible(true);


		SharedPtr<RenderPass> newPass = SharedPtr<RenderPass>(new RenderPass);
		newPass->Load("RenderCalls/DeferredNoSkybox.xml");
		newPass->Load("RenderCalls/Bloom.xml",true);
		newPass->Load("RenderCalls/FXAA.xml",true);
		newPass->Load("RenderCalls/LensDistortion.xml",true);
		newPass->Load("RenderCalls/ShowGBuffer.xml",true);
		newPass->DisableRenderCalls("ShowGBuffer");
		newPass->DisableRenderCalls("Bloom");
		renderer->SetDefaultPass(newPass);
#endif

		MaterialPtr emissiveBlue = YumeAPINew Material;
		emissiveBlue->SetShaderParameter("DiffuseColor",DirectX::XMFLOAT4(1,1,1,1));
		emissiveBlue->SetShaderParameter("SpecularColor",DirectX::XMFLOAT4(1,1,1,1));
		emissiveBlue->SetShaderParameter("Roughness",1);
		emissiveBlue->SetShaderParameter("ShadingMode",0);
		emissiveBlue->SetShaderParameter("has_diffuse_tex",false);
		emissiveBlue->SetShaderParameter("has_alpha_tex",false);
		emissiveBlue->SetShaderParameter("has_specular_tex",false);
		emissiveBlue->SetShaderParameter("has_normal_tex",false);
		emissiveBlue->SetShaderParameter("has_roughness_tex",false);

		renderer->SetFrustumCulling(true);

		float boxScale = 0.15f;

		boxBlue = CreateModel("Models/dragon/dragon.yume",DirectX::XMFLOAT3(0,0,0),DirectX::XMFLOAT4(0,0,0,0),DirectX::XMFLOAT3(15,15,15));
		boxBlue->SetMaterial(emissiveBlue);

		for(int i=0; i < 3; ++i)
		{
			for(int j=0; j < 3; ++j)
			{
				StaticModel* d1 = CreateModel("Models/dragon/dragon.yume",DirectX::XMFLOAT3(i * 25,j * 25,0),DirectX::XMFLOAT4(0,0,0,0),DirectX::XMFLOAT3(15,15,15));
				d1->SetMaterial(emissiveBlue);
			}

		}


		Light* dirLight = new Light;
		dirLight->SetName("DirLight");
		dirLight->SetType(LT_DIRECTIONAL);
		dirLight->SetPosition(DirectX::XMVectorSet(0,20,0,0));
		dirLight->SetDirection(DirectX::XMVectorSet(0,-1,0,0));
		dirLight->SetRotation(DirectX::XMVectorSet(-1,0,0,0));
		dirLight->SetColor(YumeColor(1,1,1,0));

		scene->AddNode(dirLight);
	}


	void GodRays::MoveCamera(float timeStep)
	{

	}


	void GodRays::HandleUpdate(float timeStep)
	{
		const float YOrbitRadius = 5.f;
		const float ZOrbitRadius = 5.f;
		const float XOrbitRadius = 8;
		angle1_ += M_PI * 0.1f * timeStep;
		updown1_ += M_PI * 0.8f * timeStep;
		leftRight1_ += M_PI * 0.34f * timeStep;

		if(updown1_ > M_PI * 2)
			updown1_ = 0.0f;

		DirectX::XMVECTOR blueRot = DirectX::XMVectorSet(0,angle1_,0,0);

		SceneNodes::type& renderables = gYume->pRenderer->GetScene()->GetRenderables();

		for(int i=0; i < renderables.size(); ++i)
		{
			SceneNode* node = renderables[i];

			node->SetRotation(blueRot);
		}

		
	}
	void GodRays::HandleRenderUpdate(float timeStep)
	{

	}

	void GodRays::Setup()
	{
		engineVariants_["GI"] = NoGI;
		engineVariants_["WindowWidth"] = 1024;
		engineVariants_["WindowHeight"] = 768;

		BaseApplication::Setup();
	}
}
