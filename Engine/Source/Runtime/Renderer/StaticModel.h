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
#ifndef __StaticModel_h__
#define __StaticModel_h__
//----------------------------------------------------------------------------
#include "YumeRequired.h"
#include "SceneNode.h"
#include "AssimpMesh.h"
//----------------------------------------------------------------------------
namespace YumeEngine
{
	class YumeAPIExport StaticModel : public SceneNode
	{
	public:
		StaticModel(const YumeString& resName);
		virtual ~StaticModel();

		void Initialize();

	private:
		YumeMesh internalMesh_;
		YumeString modelName_;
	};
}


//----------------------------------------------------------------------------
#endif
