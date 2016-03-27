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
#ifndef __YumeUI_h__
#define __YumeUI_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "Core/YumeEventHub.h"
#include <cef3d/Cef3D.h>
//----------------------------------------------------------------------------
namespace CefUI {
	class Cef3D;
}

namespace YumeEngine
{
	class YumeDebugOverlay;
	class YumeUIElement;

	class YumeAPIExport YumeUI :
		public YumeBase,
		public YumeTimerEventListener,
		public CefOsrDelegate,
		public EngineEventListener,
		public InputEventListener
	{
	public:
		YumeUI();
		virtual ~YumeUI();

		bool Initialize();
		void Shutdown();

		int CreateBrowser(YumeUIElement* element);
		void AddUIElement(YumeUIElement* element);

		virtual void OnRendererContextReady();

		//Time
		virtual void HandleBeginFrame(int frameNumber);

		virtual void HandlePostRenderUpdate(float timeStep);

		void Render();
		void Update();

		//Osr delegate
		virtual void OnPaint(int browserIndex,std::vector<CefUI::CefRect>& rectList,const void* buffer,
			int width,
			int height);
		virtual void OnBrowserReady(unsigned index);

		//Input
		virtual void HandleKeyDown(unsigned key,unsigned mouseButton,int repeat);
		virtual void HandleKeyUp(unsigned key,unsigned mouseButton,int repeat);
		virtual void HandleMouseButtonDown(int modifiers,int button,unsigned buttons);
		virtual void HandleMouseButtonUp(int modifiers,int button,unsigned buttons);
		virtual void HandleMouseMove(int mouseX,int mouseY,int modifiers,unsigned buttons);

		bool GetUIEnabled() const { return renderUI_; }
		void SetUIEnabled(bool enable);

		void SendEvent(const YumeString& name,const YumeString& data);

		const CefUI::Cef3D* GetCefWrapper() const { return cef3d_; }

	private:
		CefUI::Cef3D* cef3d_;
		
		typedef YumeVector<UIEventListener*> UIListeners;
		UIListeners::type listeners_;

		typedef YumeVector<YumeUIElement*> UIElements;
		UIElements::type uiElements_;

		bool renderUI_;

		int mouseX_;
		int mouseY_;

		float lastTimeStep_;
	public:
		void AddListener(UIEventListener* listener);
		void RemoveListener(UIEventListener* listener);
	};

	template <> inline unsigned MakeHash(const Pair<int,bool>& value)
	{
		return (unsigned)value.first;
	}
}


//----------------------------------------------------------------------------
#endif
