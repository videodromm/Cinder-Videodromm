#include "VDUIRender.h"

using namespace VideoDromm;

VDUIRender::VDUIRender(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
	
	// contour
	minContour = getMinUniformValueByIndex(26);
	maxContour = getMaxUniformValueByIndex(26);
	iResolutionX = (int)getValueByName("iResolutionX");
	iResolutionY = (int)getValueByName("iResolutionY");

}
VDUIRender::~VDUIRender() {

}
float VDUIRender::getValueByName(string aCtrlName) {
	return mVDSession->getFloatUniformValueByName(aCtrlName);
}
float VDUIRender::getValue(unsigned int aCtrl) {
	return mVDSession->getFloatUniformValueByIndex(aCtrl);
}
void VDUIRender::setValue(unsigned int aCtrl, float aValue) {
	mVDSession->setFloatUniformValueByIndex(aCtrl, aValue);
}
void VDUIRender::toggleAuto(unsigned int aCtrl) {
	mVDSession->toggleAuto(aCtrl);
}
void VDUIRender::toggleTempo(unsigned int aCtrl) {
	mVDSession->toggleTempo(aCtrl);
}
void VDUIRender::resetAutoAnimation(unsigned int aCtrl) {
	mVDSession->resetAutoAnimation(aCtrl);
}
float VDUIRender::getMinUniformValueByIndex(unsigned int aIndex) {
	return mVDSession->getMinUniformValueByIndex(aIndex);
}
float VDUIRender::getMaxUniformValueByIndex(unsigned int aIndex) {
	return mVDSession->getMaxUniformValueByIndex(aIndex);
}
void VDUIRender::Run(const char* title) {
	ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargeW, mVDSettings->uiLargeH), ImGuiSetCond_Once);
	ui::SetNextWindowPos(ImVec2(mVDSettings->uiMargin, mVDSettings->uiYPosRow2), ImGuiSetCond_Once);

	ui::Begin("Render");
	{
		ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
		// iResolution
		ctrl = 29;
		//iResolutionX = getValueByName("iResolutionX");
		if (ui::DragInt("iResolutionX", &iResolutionX, 1.0f, (int)getMinUniformValueByIndex(ctrl), (int)getMaxUniformValueByIndex(ctrl)))
		{
			setValue(ctrl, (float)iResolutionX);
		}
		ctrl = 30;
		//iResolutionY = getValueByName("iResolutionY");
		if (ui::DragInt("iResolutionY", &iResolutionY, 1.0f, (int)getMinUniformValueByIndex(ctrl), (int)getMaxUniformValueByIndex(ctrl)))
		{
			setValue(ctrl, (float)iResolutionY);
		}
		// use triangles
		if (mVDSession->isWarpTriangle()) {
			ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.9f, 1.0f, 0.5f));
		}
		else {
			ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.0f, 0.1f, 0.1f));
		}
		ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.9f, 0.7f, 0.7f));
		ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.9f, 0.8f, 0.8f));

		if (ui::Button("Triangles")) {
			mVDSession->toggleWarpTriangle();
		}
		ui::PopStyleColor(3);
		if (ui::IsItemHovered()) ui::SetTooltip("Use Triangle");

		// iContour
		ctrl = 26;
		if (ui::Button("a##contour")) { toggleAuto(ctrl); }
		ui::SameLine();
		if (ui::Button("t##contour")) { toggleTempo(ctrl); }
		ui::SameLine();
		if (ui::Button("x##contour")) { resetAutoAnimation(ctrl); }
		ui::SameLine();
		contour = mVDSession->getFloatUniformValueByIndex(ctrl);
		if (ui::DragFloat("contour", &contour, 0.001f, minContour, maxContour))
		{
			setValue(ctrl, contour);
		}
		ui::DragFloat("mincr", &minContour, 0.001f, getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl));
		ui::SameLine();
		ui::DragFloat("maxcr", &maxContour, 0.001f, getMinUniformValueByIndex(ctrl), getMaxUniformValueByIndex(ctrl));
        // windows
		if (ui::Button("Create Window")) {
			mVDSession->createWindow();
		}
		ui::SameLine();
		if (ui::Button("Delete Window")) {
			mVDSession->deleteWindow();
		}
		if (ui::Button("Create Warp")) {
			mVDSession->createWarpMix();
		}
		ui::SameLine();
		if (ui::Button("Create Triangle")) {
			mVDSession->createTriangleMix();
		}

		// alpha blending
		/*if (mVDSession->isEnabledAlphaBlending()) {
			ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.9f, 1.0f, 0.5f));
		}
		else {
			ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.0f, 0.1f, 0.1f));
		}
		ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.9f, 0.7f, 0.7f));
		ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.9f, 0.8f, 0.8f));

		if (ui::Button("Alpha Blending")) {
			mVDSession->toggleEnabledAlphaBlending();
		}
		ui::PopStyleColor(3);
		if (ui::IsItemHovered()) ui::SetTooltip("Enabled Alpha Blending");
		ui::SameLine();
		// Render Texture
		if (mVDSession->isRenderTexture()) {
			ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.9f, 1.0f, 0.5f));
		}
		else {
			ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.0f, 0.1f, 0.1f));
		}
		ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.9f, 0.7f, 0.7f));
		ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.9f, 0.8f, 0.8f));

		if (ui::Button("Render Texture")) {
			mVDSession->toggleRenderTexture();
		}
		ui::PopStyleColor(3);
		if (ui::IsItemHovered()) ui::SetTooltip("Render Texture or Mix");*/
		// Activate animation
		if (mVDSession->isWarpAnimationActive()) {
			ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.9f, 1.0f, 0.5f));
		}
		else {
			ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.0f, 0.1f, 0.1f));
		}
		ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.9f, 0.7f, 0.7f));
		ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.9f, 0.8f, 0.8f));

		if (ui::Button("Warp Animation")) {
			mVDSession->toggleWarpAnimationActive();
		}
		ui::PopStyleColor(3);
		if (ui::IsItemHovered()) ui::SetTooltip("Activate Warp Animation");

		ui::Text("fp %dx%d f %dx%d r %dx%d", mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight, mVDSettings->mFboWidth, mVDSettings->mFboHeight, mVDSettings->mRenderWidth, mVDSettings->mRenderHeight);
		ui::Text("solo %d", mVDSession->getSolo());

		ui::PopItemWidth();
	}
	ui::End();
}
