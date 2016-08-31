#include "VDUIShaders.h"

using namespace VideoDromm;

VDUIShaders::VDUIShaders(VDSettingsRef aVDSettings, VDMixRef aVDMix, VDRouterRef aVDRouter) {
	mVDSettings = aVDSettings;
	mVDMix = aVDMix;
	mVDRouter = aVDRouter;
	try
	{
		fs::path vertexFile = getAssetPath("") / "passthru.vert";
		if (fs::exists(vertexFile)) {
			mPassthruVextexShaderString = loadString(loadAsset("passthru.vert"));
			CI_LOG_V("passthru.vert loaded");
		}
		else
		{
			CI_LOG_V("passthru.vert does not exist, should quit");
		}
	}
	catch (gl::GlslProgCompileExc &exc)
	{
		mError = string(exc.what());
		CI_LOG_V("unable to load/compile passthru vertex shader:" + string(exc.what()));
	}
	catch (const std::exception &e)
	{
		mError = string(e.what());
		CI_LOG_V("unable to load passthru vertex shader:" + string(e.what()));
	}
}
VDUIShaders::~VDUIShaders() {

}

void VDUIShaders::Run(const char* title) {
	static int shaderToEdit = -1;
	for (int s = 0; s < mVDMix->getShadersCount(); s++) {
		ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargePreviewW, mVDSettings->uiLargePreviewH));
		ui::SetNextWindowPos(ImVec2((s * (mVDSettings->uiLargePreviewW + mVDSettings->uiMargin)) + mVDSettings->uiMargin, mVDSettings->uiYPosRow3));
		int hue = 0;
		sprintf(buf, "%s##s%d", mVDMix->getShaderName(s).c_str(), s);
		ui::Begin(buf, NULL, ImVec2(0, 0), ui::GetStyle().Alpha, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
		{
			ui::PushItemWidth(mVDSettings->mPreviewFboWidth);
			ui::PushID(s);
			ui::Image((void*)mVDMix->getShaderThumb(s)->getId(), ivec2(mVDSettings->mPreviewFboWidth, mVDSettings->mPreviewFboHeight));
			// edit
			if (shaderToEdit == s) {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.8f, 1.0f, 0.5f));
			}
			else {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.0f, 0.1f, 0.1f));
			}
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.8f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.8f, 0.8f, 0.8f));
			sprintf(buf, "E##s%d", s);
			if (ui::Button(buf)){
				//mVDMix->editShader(s);
				shaderToEdit = s;
			}
			ui::PopStyleColor(3);			
			if (ui::IsItemHovered()) ui::SetTooltip("Edit shader");
			ui::SameLine();
			// thumb
			sprintf(buf, "T##s%d", s);
			if (ui::Button(buf)){
				mVDMix->createShaderThumb(s);
			}
			if (ui::IsItemHovered()) ui::SetTooltip("Create thumb");

			// left
			if (mVDMix->getFboFragmentShaderIndex(1) == s) {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.0f, 1.0f, 0.5f));
			}
			else {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.0f, 0.1f, 0.1f));
			}
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.0f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.0f, 0.8f, 0.8f));
			sprintf(buf, "L##s%d", s);
			if (ui::Button(buf)) mVDMix->setFboFragmentShaderIndex(1, s);
			if (ui::IsItemHovered()) ui::SetTooltip("Set shader to left");
			ui::PopStyleColor(3);

			ui::SameLine();
			// right
			if (mVDMix->getFboFragmentShaderIndex(1) == s) {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.3f, 1.0f, 0.5f));
			}
			else {
				ui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.0f, 0.1f, 0.1f));
			}
			ui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.3f, 0.7f, 0.7f));
			ui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.3f, 0.8f, 0.8f));
			sprintf(buf, "R##s%d", s);
			if (ui::Button(buf)) mVDMix->setFboFragmentShaderIndex(2, s);
			if (ui::IsItemHovered()) ui::SetTooltip("Set shader to right");
			ui::PopStyleColor(3);

			ui::PopID();
			ui::PopItemWidth();
		}
		ui::End();
		// editor
#pragma region Editor
		if (shaderToEdit == s) {
			ui::SetNextWindowPos(ImVec2(mVDSettings->uiXPosCol1, mVDSettings->uiYPosRow2), ImGuiSetCond_Once);
			ui::SetNextWindowSize(ImVec2(mVDSettings->uiLargeW * 3, mVDSettings->uiLargeH), ImGuiSetCond_FirstUseEver);
			sprintf(buf, "Editor - %s##s%d", mVDMix->getShaderName(s).c_str(), s);
			ui::Begin(buf);
			{
				size_t const MAX = 32768; // maximum number of chars
				static char mShaderText[MAX] =
					"uniform vec3 iResolution;\n"
					"uniform vec3 iColor;\n"
					"uniform float iGlobalTime;\n"
					"uniform sampler2D iChannel0;\n"
					"uniform sampler2D iChannel1;\n"
					"uniform sampler2D iChannel2;\n"
					"uniform vec3 spectrum;\n"
					"\n"
					"out vec4 oColor;\n"
					"void main(void) {\n"
					"\tvec2 uv = gl_FragCoord.xy / iResolution.xy;\n"
					"\tvec4 t0 = texture(iChannel0, uv);\n"
					"\tvec4 t1 = texture(iChannel1, uv);\n"
					"\tvec4 t2 = texture(iChannel2, uv);\n"
					"\toColor = vec4(t0.x, t1.y, cos(iGlobalTime), 1.0);\n"
					"}\n";
				// check if shader text needs to be loaded in the editor
				if (mVDSettings->shaderEditIndex != shaderToEdit) {
					mFboTextureFragmentShaderString = mVDMix->getFboFragmentShaderText(shaderToEdit);
					mVDSettings->shaderEditIndex = shaderToEdit;
					std::copy(mFboTextureFragmentShaderString.begin(), (mFboTextureFragmentShaderString.size() >= MAX ? mFboTextureFragmentShaderString.begin() + MAX : mFboTextureFragmentShaderString.end()), mShaderText);
				}
				
				ui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
				ui::PopStyleVar();
				ui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "uniform");
				if (ui::InputTextMultiline("##source", mShaderText, IM_ARRAYSIZE(mShaderText), ImVec2(-1.0f, -1.0f), ImGuiInputTextFlags_AllowTabInput)) {
					// text changed // TODO height ? mVDSettings->uiYPosRow2 - 200.0f
					CI_LOG_V("text changed");
					try
					{
						aShader = gl::GlslProg::create(mPassthruVextexShaderString, mShaderText);
						aShader->setLabel("live");
						CI_LOG_V("live.frag loaded and compiled");
						mFboTextureFragmentShaderString = mShaderText;
						stringstream sParams;
						sParams << "/*{ \"title\" : \"" << getElapsedSeconds() << "\" }*/ " << mFboTextureFragmentShaderString;
						mVDRouter->wsWrite(sParams.str());
						//OK mVDRouter->wsWrite("/*{ \"title\" : \"live\" }*/ " + mFboTextureFragmentShaderString);
						mError = "";
					}
					catch (gl::GlslProgCompileExc &exc)
					{
						mError = string(exc.what());
						CI_LOG_V("unable to load/compile live fragment shader:" + string(exc.what()));
					}
					catch (const std::exception &e)
					{
						mError = string(e.what());
						CI_LOG_V("unable to load live fragment shader:" + string(e.what()));
					}
				}
				else {
					// nothing changed 
				}
				ui::TextColored(ImColor(255, 0, 0), mError.c_str());
			}
			ui::End();
		}
#pragma endregion Editor
	}

}
