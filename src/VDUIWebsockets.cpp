#include "VDUIWebsockets.h"

using namespace VideoDromm;

VDUIWebsockets::VDUIWebsockets(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
}
VDUIWebsockets::~VDUIWebsockets() {

}

void VDUIWebsockets::Run(const char* title) {
	#if (defined(  CINDER_MSW) ) || (defined( CINDER_MAC ))
	ImGui::SetNextWindowSize(ImVec2(mVDSettings->uiLargeW, mVDSettings->uiLargeH), ImGuiSetCond_Once);
	ImGui::SetNextWindowPos(ImVec2(mVDSettings->uiMargin, mVDSettings->uiYPosRow2), ImGuiSetCond_Once);
	ImGui::Begin("Websockets");
	{
		// websockets
		if (mVDSettings->mIsWebSocketsServer)
		{
			ImGui::Text("WS Server %s%s:%d", mVDSettings->mWebSocketsProtocol.c_str(), mVDSettings->mWebSocketsHost.c_str(), mVDSettings->mWebSocketsPort);
			if (ImGui::Button("srv->clt"))
			{
				mVDSettings->mIsWebSocketsServer = false;
				mVDSession->wsConnect();
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change to a WS client");
		}
		else
		{
			ImGui::Text("WS Client %s%s:%d", mVDSettings->mWebSocketsProtocol.c_str(), mVDSettings->mWebSocketsHost.c_str(), mVDSettings->mWebSocketsPort);
			if (ImGui::Button("clt->srv"))
			{
				mVDSettings->mIsWebSocketsServer = true;
				mVDSession->wsConnect();
			}
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change to a WS server");
		}
		ImGui::SameLine();
		// toggle secure protocol
		sprintf(buf, "%s", mVDSettings->mWebSocketsProtocol.c_str());
		if (ImGui::Button(buf))
		{
			if (mVDSettings->mWebSocketsProtocol == "ws://") {
				mVDSettings->mWebSocketsProtocol = "wss://";
			}
			else {
				mVDSettings->mWebSocketsProtocol = "ws://";
			}
			mVDSession->wsConnect();
		}
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Change WS protocol");
		ImGui::SameLine();
		if (ImGui::Button("Connect")) { mVDSession->wsConnect(); }
		ImGui::SameLine();
		if (ImGui::Button("Ping")) { mVDSession->wsPing(); }
		static char host[128] = "127.0.0.1";
		std::copy(mVDSettings->mWebSocketsHost.begin(), (mVDSettings->mWebSocketsHost.size() >= 128 ? mVDSettings->mWebSocketsHost.begin() + 128 : mVDSettings->mWebSocketsHost.end()), host);

		static int port = mVDSettings->mWebSocketsPort;
		if (ImGui::InputText("address", host, IM_ARRAYSIZE(host)))
		{
			mVDSettings->mWebSocketsHost = host; // CHECK if ok
		}
		if (ImGui::InputInt("port", &port)) mVDSettings->mWebSocketsPort = port;
		//ImGui::PushItemWidth(mVDSettings->uiLargeW/3); // useless?
		ImGui::TextWrapped(">%s", mVDSettings->mWebSocketsMsg.c_str());
		//ImGui::PopItemWidth();

	}
	ImGui::End();
	#endif
}
