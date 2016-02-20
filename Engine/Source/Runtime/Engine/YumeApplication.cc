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
// File : YumeGraphics.h
// Date : 2.19.2016
// Comments :
//
//----------------------------------------------------------------------------
#include "YumeHeaders.h"
#include "Engine/YumeApplication.h"
#include "Engine/YumeEngine.h"

#include <boost/make_shared.hpp>

namespace YumeEngine
{
	YumeApplication::YumeApplication()
		: exitCode_(0)
	{
		engine_ = boost::shared_ptr<YumeEngine3D>(new YumeEngine3D);
	}

	YumeApplication::~YumeApplication()
	{

	}

	int YumeApplication::Run()
	{
		Setup();

		if(!engine_->Initialize())
		{
			exitCode_ = -1;
			return exitCode_;
		}


		Start();
		if(exitCode_ == 1)
			return exitCode_;

		while(!engine_->IsExiting())
			engine_->Run();

		engine_->Exit();

		return exitCode_;
	}
}
