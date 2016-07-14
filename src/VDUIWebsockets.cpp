#include "VDUIWebsockets.h"

using namespace VideoDromm;

VDUIWebsockets::VDUIWebsockets(VDSettingsRef aVDSettings, VDMixRef aVDMix, VDRouterRef aVDRouter) {
	mVDSettings = aVDSettings;
	mVDMix = aVDMix;
	mVDRouter = aVDRouter;
}
VDUIWebsockets::~VDUIWebsockets() {
	
}

void VDUIWebsockets::Run(const char* title) {
	ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargeW, mVDSettings->uiLargeH));
	ui::SetNextWindowPos(ImVec2(mVDSettings->uiMargin, mVDSettings->uiYPosRow2));

	ui::Begin("Websockets");
	{
		// websockets
		if (mVDSettings->mIsWebSocketsServer)
		{
			ui::Text("WS Server %d", mVDSettings->mWebSocketsPort);
			ui::Text("IP %s", mVDSettings->mWebSocketsHost.c_str());
		}
		else
		{
			ui::Text("WS Client %d", mVDSettings->mWebSocketsPort);
			ui::Text("IP %s", mVDSettings->mWebSocketsHost.c_str());
		}
		if (ui::Button("Connect")) { mVDRouter->wsConnect(); }
		ui::SameLine();
		if (ui::Button("Ping")) { mVDRouter->wsPing(); }
		ui::Text(">%s", mVDSettings->mWebSocketsMsg.c_str());

		//static char str0[128] = mParameterBag->mWebSocketsHost.c_str();
		//static int i0 = mVDSettings->mWebSocketsPort;
		//ui::InputText("address", str0, IM_ARRAYSIZE(str0));
		//if (ui::InputInt("port", &i0)) mVDSettings->mWebSocketsPort = i0;

	}
	ui::End();
}
