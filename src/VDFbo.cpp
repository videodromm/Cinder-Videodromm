#include "VDFbo.h"

#include "cinder/gl/Texture.h"
#include "cinder/Xml.h"

using namespace ci;
using namespace ci::app;

namespace VideoDromm {
	VDFbo::VDFbo(FboType aType)
		: mFilePathOrText("")
		, mFboName("fbo")
		/*, mTopDown(true)
		, mFlipV(false)
		, mFlipH(true)*/
		, mWidth(640)
		, mHeight(480)
	{
		mType = TEXTURE;
		mPosX = mPosY = 0.0f;
		mZoom = 1.0f;
		// init the fbo whatever happens next
		gl::Fbo::Format format;
		//format.setSamples( 4 ); // uncomment this to enable 4x antialiasing
		mFbo = gl::Fbo::create(mWidth, mHeight, format.depthTexture());
		// init with passthru shader
		mShaderName = "fbotexture";
		// load shadertoy uniform variables declarations
		shaderInclude = loadString(loadAsset("shadertoy.inc"));
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
		// load passthru fragment shader
		try
		{
			fs::path fragFile = getAssetPath("") / "fbotexture.frag";
			if (fs::exists(fragFile)) {
				mFboTextureFragmentShaderString = loadString(loadAsset("fbotexture.frag"));
			}
			else
			{
				mError = "fbotexture.frag does not exist, should quit";
				CI_LOG_V(mError);
			}
			mFboTextureShader = gl::GlslProg::create(mPassthruVextexShaderString, mFboTextureFragmentShaderString);
			mFboTextureShader->setLabel(mShaderName);
			CI_LOG_V("fbotexture.frag loaded and compiled");
		}
		catch (gl::GlslProgCompileExc &exc)
		{
			mError = string(exc.what());
			CI_LOG_V("unable to load/compile fbotexture fragment shader:" + string(exc.what()));
		}
		catch (const std::exception &e)
		{
			mError = string(e.what());
			CI_LOG_V("unable to load fbotexture fragment shader:" + string(e.what()));
		}
	}
	VDFbo::~VDFbo(void) {

	}
	int VDFbo::loadPixelFragmentShader(string aFilePath)
	{
		int rtn = -1;
		// reset 
		//mVDSettings->iFade = false;
		//mVDSettings->controlValues[22] = 1.0f;
		try
		{
			fs::path fr = aFilePath;
			string name = "unknownshader";
			string mFile = fr.string();
			if (mFile.find_last_of("\\") != std::string::npos) name = mFile.substr(mFile.find_last_of("\\") + 1);
			//mFragFileName = name;
			if (fs::exists(fr))
			{
				//validFrag = false;
				std::string fs = shaderInclude + loadString(loadFile(aFilePath));
				rtn = setGLSLString(fs, name);
				if (rtn > -1)
				{
					//CI_LOG_V(mFragFile + " loaded and compiled");
					//mVDSettings->mMsg = name + " loadPixelFragmentShader success";
					//mVDSettings->newMsg = true;
					//mFragmentShadersNames[rtn] = name;
					mFboTextureShader->setLabel(name);
				}
			}
			else
			{
				//CI_LOG_V(mFragFile + " does not exist");
			}
		}
		catch (gl::GlslProgCompileExc &exc)
		{
			mError = string(exc.what());
			CI_LOG_V(aFilePath + " unable to load/compile shader err:" + mError);
			//mVDSettings->mMsg = mError;
			//mVDSettings->newMsg = true;
		}
		catch (const std::exception &e)
		{
			mError = string(e.what());
			CI_LOG_V(aFilePath + " unable to load shader err:" + mError);
			//mVDSettings->mMsg = mError;
			//mVDSettings->newMsg = true;
		}

		return rtn;
	}

	int VDFbo::setGLSLString(string pixelFrag, string name)
	{
		int foundIndex = 0;

		try
		{
			mFboTextureShader = gl::GlslProg::create(mPassthruVextexShaderString, pixelFrag);
			mShaderName = name;

			//preview the new loaded shader
			//mVDSettings->mPreviewFragIndex = foundIndex;
			CI_LOG_V("setGLSLString success");
			//mVDSettings->mMsg = name + " setGLSLString success";
			//mVDSettings->newMsg = true;
			mError = "";
			//validFrag = true;
		}
		catch (gl::GlslProgCompileExc exc)
		{
			//validFrag = false;
			// TODO CI_LOG_E("Problem Compiling ImGui::Renderer shader " << exc.what());
			foundIndex = -1;
			mError = string(exc.what());
			//mVDSettings->mMsg = "setGLSLString file: " + name + " error:" + mError;
			//mVDSettings->newMsg = true;
			//CI_LOG_V(mVDSettings->mMsg);
		}
		return foundIndex;
	}
	VDFboList VDFbo::readSettings(const DataSourceRef &source)
	{
		XmlTree			doc;
		VDFboList	VDFbolist;

		// try to load the specified xml file
		try { doc = XmlTree(source); }
		catch (...) { return VDFbolist; }

		// check if this is a valid file 
		bool isOK = doc.hasChild("fbos");
		//
		if (isOK) {

			XmlTree fboXml = doc.getChild("fbos");

			// iterate textures
			for (XmlTree::ConstIter child = fboXml.begin("fbo"); child != fboXml.end(); ++child) {
				// create fbo of the correct type
				std::string texturetype = child->getAttributeValue<std::string>("fbotype", "unknown");
				XmlTree detailsXml = child->getChild("details");

				if (texturetype == "texture") {
					VDFboRef t(new VDFbo());
					t->fromXml(detailsXml);
					VDFbolist.push_back(t);
				}
			}
		}
		else {
			// malformed XML
			CI_LOG_V("malformed XML");
			VDFboRef t(new VDFbo());
			XmlTree		initXml;
			initXml.setTag("details");
			initXml.setAttribute("filepath", "0.jpg");
			initXml.setAttribute("width", 640);
			initXml.setAttribute("height", 480);
			initXml.setAttribute("shadername", "0.glsl");
			t->fromXml(initXml);
			VDFbolist.push_back(t);
		}
		return VDFbolist;
	}

	void VDFbo::writeSettings(const VDFboList &VDFbolist, const ci::DataTargetRef &target) {

		// create config document and root <textures>
		XmlTree			doc;
		doc.setTag("fbos");
		doc.setAttribute("version", "1.0");

		// 
		for (unsigned int i = 0; i < VDFbolist.size(); ++i) {
			// create <texture>
			XmlTree			fbo;
			fbo.setTag("fbo");
			fbo.setAttribute("id", i + 1);
			switch (VDFbolist[i]->mType) {
			case TEXTURE: fbo.setAttribute("fbotype", "texture"); break;
			default: fbo.setAttribute("fbotype", "unknown"); break;
			}
			// details specific to texture type
			fbo.push_back(VDFbolist[i]->toXml());

			// add fbo to doc
			doc.push_back(fbo);
		}

		// write file
		doc.write(target);
	}
	XmlTree	VDFbo::toXml() const
	{
		XmlTree		xml;
		xml.setTag("details");
		xml.setAttribute("filepath", mFilePathOrText);
		xml.setAttribute("width", mWidth);
		xml.setAttribute("height", mHeight);
		xml.setAttribute("shadername", mShaderName);

		return xml;
	}

	void VDFbo::fromXml(const XmlTree &xml)
	{
		mType = TEXTURE;
		// retrieve texture specific to this fbo
		mFilePathOrText = xml.getAttributeValue<string>("filepath", "0.jpg");

		fs::path fullPath = getAssetPath("") / mFilePathOrText;// TODO / mVDSettings->mAssetsPath
		try {
			mTexs.push_back(TextureImage::create());
			mTexs[0]->loadImageFromFileFullPath(fullPath.string());
			CI_LOG_V("successfully loaded " + mFilePathOrText);
		}
		catch (Exception &exc) {
			CI_LOG_EXCEPTION("error loading ", exc);
		}

		// retrieve shader specific to this fbo
		string mGlslPath = xml.getAttributeValue<string>("shadername", "0.glsl");
		if (mGlslPath.length() > 0) {
			fs::path fr = getAssetPath("") / mGlslPath;// TODO / mVDSettings->mAssetsPath
			if (fs::exists(fr)) {
				try {
					loadPixelFragmentShader(fr.string());
					CI_LOG_V("successfully loaded " + mGlslPath);
				}
				catch (Exception &exc) {
					CI_LOG_EXCEPTION("error loading ", exc);
				}
			}
		}
	}
	void VDFbo::setPosition(int x, int y) {
		mPosX = ((float)x / (float)mWidth) - 0.5;
		mPosY = ((float)y / (float)mHeight) - 0.5;
	}
	void VDFbo::setZoom(float aZoom) {
		mZoom = aZoom;
	}
	int VDFbo::getTextureWidth() {
		return mWidth;
	};

	int VDFbo::getTextureHeight() {
		return mHeight;
	};

	ci::ivec2 VDFbo::getSize() {
		return mFbo->getSize();
	}

	ci::Area VDFbo::getBounds() {
		return mFbo->getBounds();
	}

	GLuint VDFbo::getId() {
		return mFbo->getId();
	}

	std::string VDFbo::getName(){
		return mFboName;
	}

	ci::gl::Texture2dRef VDFbo::getInputTexture(unsigned int aIndex) {
		if (aIndex > mTexs.size() - 1) aIndex = mTexs.size() - 1;
		return mTexs[aIndex]->getTexture();
	}
	void VDFbo::loadImageFile(string aFile, unsigned int aTextureIndex) {
		if (aTextureIndex > mTexs.size() - 1) aTextureIndex = mTexs.size() - 1;
		mTexs[aTextureIndex]->loadImageFromFileFullPath(aFile);
	}
	ci::gl::Texture2dRef VDFbo::getTexture() {
		iChannelResolution0 = vec3(mPosX, mPosY, 0.5);
		gl::ScopedFramebuffer fbScp(mFbo);
		gl::clear(Color::black());
		// setup the viewport to match the dimensions of the FBO
		gl::ScopedViewport scpVp(ivec2(0), mFbo->getSize());
		gl::ScopedGlslProg shaderScp(mFboTextureShader);
		//mShader->bind();
		mFboTextureShader->uniform("iGlobalTime", (float)getElapsedSeconds()); //TODO
		mFboTextureShader->uniform("iResolution", vec3(mWidth, mHeight, 1.0));
		mFboTextureShader->uniform("iChannelResolution[0]", iChannelResolution0);
		mFboTextureShader->uniform("iChannel0", 0);
		mFboTextureShader->uniform("iZoom", mZoom);
		gl::ScopedTextureBind tex(mTexs[0]->getTexture());
		gl::drawSolidRect(Rectf(0, 0, mWidth, mHeight));
		return mFbo->getColorTexture();
	}
} // namespace VideoDromm

