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
	#if (defined(  CINDER_MSW) ) || (defined( CINDER_MAC ))
	ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargeW, mVDSettings->uiLargeH), ImGuiSetCond_Once);
	ui::SetNextWindowPos(ImVec2(mVDSettings->uiMargin, mVDSettings->uiYPosRow2), ImGuiSetCond_Once);
	ui::Begin("Websockets");
	{
		// websockets
		if (mVDSettings->mIsWebSocketsServer)
		{
			ui::Text("WS Server %s%s:%d", mVDSettings->mWebSocketsProtocol.c_str(), mVDSettings->mWebSocketsHost.c_str(), mVDSettings->mWebSocketsPort);
			if (ui::Button("srv->clt"))
			{
				mVDSettings->mIsWebSocketsServer = false;
				mVDRouter->wsConnect();
			}
			if (ui::IsItemHovered()) ui::SetTooltip("Change to a WS client");
		}
		else
		{
			ui::Text("WS Client %s%s:%d", mVDSettings->mWebSocketsProtocol.c_str(), mVDSettings->mWebSocketsHost.c_str(), mVDSettings->mWebSocketsPort);
			if (ui::Button("clt->srv"))
			{
				mVDSettings->mIsWebSocketsServer = true;
				mVDRouter->wsConnect();
			}
			if (ui::IsItemHovered()) ui::SetTooltip("Change to a WS server");
		}
		ui::SameLine();
		// toggle secure protocol
		sprintf(buf, "%s", mVDSettings->mWebSocketsProtocol.c_str());
		if (ui::Button(buf))
		{
			if (mVDSettings->mWebSocketsProtocol == "ws://") {
				mVDSettings->mWebSocketsProtocol = "wss://";
			}
			else {
				mVDSettings->mWebSocketsProtocol = "ws://";
			}
			mVDRouter->wsConnect();
		}
		if (ui::IsItemHovered()) ui::SetTooltip("Change WS protocol");
		ui::SameLine();
		if (ui::Button("Connect")) { mVDRouter->wsConnect(); }
		ui::SameLine();
		if (ui::Button("Ping")) { mVDRouter->wsPing(); }
		static char host[128] = "127.0.0.1";
		std::copy(mVDSettings->mWebSocketsHost.begin(), (mVDSettings->mWebSocketsHost.size() >= 128 ? mVDSettings->mWebSocketsHost.begin() + 128 : mVDSettings->mWebSocketsHost.end()), host);

		static int port = mVDSettings->mWebSocketsPort;
		ui::InputText("address", host, IM_ARRAYSIZE(host));
		if (ui::InputInt("port", &port)) mVDSettings->mWebSocketsPort = port;
		//ui::PushItemWidth(mVDSettings->uiLargeW/3); // useless?
		ui::TextWrapped(">%s", mVDSettings->mWebSocketsMsg.c_str());
		//ui::PopItemWidth();

	}
	ui::End();
	#endif
}
