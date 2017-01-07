#include "VDUITriangles.h"

using namespace VideoDromm;

VDUITriangles::VDUITriangles(VDSettingsRef aVDSettings, VDSessionRef aVDSession) {
	mVDSettings = aVDSettings;
	mVDSession = aVDSession;
}
VDUITriangles::~VDUITriangles() {

}

void VDUITriangles::Run(const char* title) {
	static int currentNode = 0;

	for (int w = 0; w < mVDSession->getTriangleCount(); w++) {
		ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiLargePreviewH), ImGuiSetCond_Once);
		ui::SetNextWindowPos(ImVec2((w * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin, mVDSettings->uiYPosRow3), ImGuiSetCond_Once);
		//int hue = 0;
		sprintf(buf, "%s##trin%d", mVDSession->getTriangleName(w).c_str(), w);
		ui::Begin(buf, NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoSavedSettings);
		{
			ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
			ui::PushID(w);
			ui::Image((void*)mVDSession->getTriangleTexture(w)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
			if (ui::IsItemHovered()) ui::SetTooltip(mVDSession->getTriangleName(w).c_str());
			// loop on the fbos
			for (unsigned int a = 0; a < mVDSession->getFboListSize(); a++) {
				if (a > 0 && (a%6 != 0)) ui::SameLine();
				if (mVDSession->getTriangleAFboIndex(w) == a) {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(a / 7.0f, 1.0f, 1.0f));
				}
				else {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(a / 7.0f, 0.1f, 0.1f));
				}
				ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(a / 7.0f, 0.7f, 0.7f));
				ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(a / 7.0f, 0.8f, 0.8f));

				sprintf(buf, "%d##tria%d%d", a, w, a);
				if (ui::Button(buf)) mVDSession->setTriangleAFboIndex(w, a);
				sprintf(buf, "Set input fbo A to %s", mVDSession->getShaderName(a).c_str());
				if (ui::IsItemHovered()) ui::SetTooltip(buf);
				ui::PopStyleColor(3);
			}
			// loop on the fbos
			for (unsigned int b = 0; b < mVDSession->getFboListSize(); b++) {
				if (b > 0 && (b % 6 != 0)) ui::SameLine();
				if (mVDSession->getTriangleBFboIndex(w) == b) {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(b / 7.0f, 1.0f, 1.0f));
				}
				else {
					ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(b / 7.0f, 0.1f, 0.1f));
				}
				ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(b / 7.0f, 0.7f, 0.7f));
				ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(b / 7.0f, 0.8f, 0.8f));

				sprintf(buf, "%d##trib%d%d", b, w, b);
				if (ui::Button(buf)) mVDSession->setTriangleBFboIndex(w, b);
				sprintf(buf, "Set input fbo B to %s", mVDSession->getShaderName(b).c_str());
				if (ui::IsItemHovered()) ui::SetTooltip(buf);
				ui::PopStyleColor(3);
			}

			// crossfade
			float xFade = mVDSession->getTriangleCrossfade(w);
			sprintf(buf, "xfade##trixf%d", w);
			if (ui::SliderFloat(buf, &xFade, 0.0f, 1.0f))
			{
				mVDSession->setTriangleCrossfade(w, xFade);
			}
			// nodes
			if (currentNode == w) {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.8f, 1.0f, 0.5f));
			}
			else {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.0f, 0.1f, 0.1f));
			}
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.8f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.8f, 0.8f, 0.8f));
			sprintf(buf, "N##trinod%d", w);
			if (ui::Button(buf)){
				if (w == currentNode) {
					// if the same button pressed we hide the nodes
					currentNode = -1;
				}
				else{
					currentNode = w;
				}
			}
			ui::PopStyleColor(3);
			if (ui::IsItemHovered()) ui::SetTooltip("Show nodes");
			ui::SameLine();
			// active
			if (mVDSession->isTriangleActive(w)) {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.9f, 1.0f, 0.5f));
				sprintf(buf, "AC##tra%d", w);
			}
			else {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.0f, 0.1f, 0.1f));
				sprintf(buf, "A##tra%d", w);
			}
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.9f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.9f, 0.8f, 0.8f));
			if (ui::Button(buf)) {
				mVDSession->toggleTriangleActive(w);
			}
			ui::PopStyleColor(3);
			if (ui::IsItemHovered()) ui::SetTooltip("Toggle Triangle Active");
			ui::SameLine();
			// solo
			if (mVDSession->isTriangleSolo(w)) {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.9f, 1.0f, 0.5f));
				sprintf(buf, "SOLO##trs%d", w);
			}
			else {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.0f, 0.1f, 0.1f));
				sprintf(buf, "S##trs%d", w);
			}
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.9f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.9f, 0.8f, 0.8f));
			if (ui::Button(buf)) {
				mVDSession->toggleTriangleSolo(w);
			}
			ui::PopStyleColor(3);
			if (ui::IsItemHovered()) ui::SetTooltip("Toggle Triangle Solo");
			ui::SameLine();
			// delete
			if (mVDSession->isTriangleDeleted(w)) {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.9f, 1.0f, 0.5f));
			}
			else {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.0f, 0.1f, 0.1f));
			}
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.9f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.9f, 0.8f, 0.8f));
			sprintf(buf, "X##trx%d", w);
			if (ui::Button(buf)) {
				mVDSession->toggleDeleteTriangle(w);
			}
			ui::PopStyleColor(3);
			if (ui::IsItemHovered()) ui::SetTooltip("Toggle Delete Triangle");

			ui::PopID();
			ui::PopItemWidth();
		}
		// current triangle editing
		if (currentNode == w) {
			ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiLargePreviewH), ImGuiSetCond_Once);
			ui::SetNextWindowPos(ImVec2( (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin) + mVDSettings->uiMargin + mVDSettings->uiXPosCol1, mVDSettings->uiYPosRow2), ImGuiSetCond_Once);
			sprintf(buf, "%s##tricnt%d", mVDSession->getTriangleName(w).c_str(), w);
			ui::Begin(buf, NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoSavedSettings);
			{
				ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
				ui::Image((void*)mVDSession->getTriangleTexture(w)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
				ui::PopItemWidth();
			}
			if (ui::IsItemHovered()) ui::SetTooltip(buf);
			// points
			static float p0x = mVDSession->getTrianglePointX(w, 0);
			sprintf(buf, "p0x##p0x%d", w);
			if (ui::SliderFloat(buf, &p0x, 0.0f, mVDSettings->mRenderWidth))
			{
				mVDSession->setTrianglePointX(w, 0, p0x);
			}
			static float p0y = mVDSession->getTrianglePointY(w, 0);
			sprintf(buf, "p0y##p0y%d", w);
			if (ui::SliderFloat(buf, &p0y, 0.0f, mVDSettings->mRenderHeight))
			{
				mVDSession->setTrianglePointY(w, 0, p0y);
			}

			static float p1x = mVDSession->getTrianglePointX(w, 1);
			sprintf(buf, "p1x##p1x%d", w);
			if (ui::SliderFloat(buf, &p1x, 0.0f, mVDSettings->mRenderWidth))
			{
				mVDSession->setTrianglePointX(w, 1, p1x);
			}
			static float p1y = mVDSession->getTrianglePointY(w, 1);
			sprintf(buf, "p1y##p1y%d", w);
			if (ui::SliderFloat(buf, &p1y, 0.0f, mVDSettings->mRenderHeight))
			{
				mVDSession->setTrianglePointY(w, 1, p1y);
			}

			static float p2x = mVDSession->getTrianglePointX(w, 2);
			sprintf(buf, "p2x##p2x%d", w);
			if (ui::SliderFloat(buf, &p2x, 0.0f, mVDSettings->mRenderWidth))
			{
				mVDSession->setTrianglePointX(w, 2, p2x);
			}
			static float p2y = mVDSession->getTrianglePointY(w, 2);
			sprintf(buf, "p2y##p2y%d", w);
			if (ui::SliderFloat(buf, &p2y, 0.0f, mVDSettings->mRenderHeight))
			{
				mVDSession->setTrianglePointY(w, 2, p2y);
			}
			ui::End();
		}
		ui::End();
	}
}
