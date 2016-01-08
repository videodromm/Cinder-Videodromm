#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/App.h"
#include "cinder/DataSource.h"
#include "cinder/Utilities.h"
// log
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace VideoDromm
{
	// stores the pointer to the VDLog instance
	typedef std::shared_ptr<class VDLog> VDLogRef;

	class VDLog {
	public:		
		VDLog();

		static VDLogRef	create()
		{
			return shared_ptr<VDLog>(new VDLog());
		}

	private:

	};


}