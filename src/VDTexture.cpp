#include "VDTexture.h"

#include "cinder/gl/Texture.h"
#include "cinder/Xml.h"

using namespace ci;
using namespace ci::app;

namespace VideoDromm {
	VDTexture::VDTexture(TextureType aType)
		: mFilePathOrText("")
		, mName("")
		, mTopDown(false)
		, mFlipV(false)
		, mFlipH(true)
		, mWidth(640)
		, mHeight(480)
	{

		if (mName.length() == 0) {
			mName = mFilePathOrText;
		}
		// init the texture whatever happens next
		if (mFilePathOrText.length() > 0) {
			mTexture = ci::gl::Texture::create(ci::loadImage(mFilePathOrText), ci::gl::Texture::Format().loadTopDown(mTopDown));
		}
		else {
			mTexture = ci::gl::Texture::create(mWidth, mHeight, ci::gl::Texture::Format().loadTopDown(mTopDown));
		}
	}
	VDTexture::~VDTexture(void) {

	}


	VDTextureList VDTexture::readSettings(const DataSourceRef &source)
	{
		XmlTree			doc;
		VDTextureList	vdtexturelist;

		// try to load the specified xml file
		try { doc = XmlTree(source); }
		catch (...) { return vdtexturelist; }

		// check if this is a valid file 
		bool isOK = doc.hasChild("textures");
		//if (!isOK) return vdtexturelist;

		//
		if (isOK) {

			XmlTree texturesXml = doc.getChild("textures");

			// iterate textures
			for (XmlTree::ConstIter child = texturesXml.begin("texture"); child != texturesXml.end(); ++child) {
				// create texture of the correct type
				std::string texturetype = child->getAttributeValue<std::string>("texturetype", "unknown");
				XmlTree detailsXml = child->getChild("details");

				//std::string filepath = detailsXml.getAttributeValue<std::string>("filepath", "");
				if (texturetype == "image") {
					TextureImageRef t(new TextureImage());
					t->fromXml(detailsXml);
					vdtexturelist.push_back(t);
				}
				else if (texturetype == "text") {
					TextureTextRef t(new TextureText());
					t->fromXml(detailsXml);
					vdtexturelist.push_back(t);
				}
				else if (texturetype == "movie") {
					TextureMovieRef t(new TextureMovie());
					t->fromXml(detailsXml);
					vdtexturelist.push_back(t);
				}
				else if (texturetype == "camera") {
#if (defined(  CINDER_MSW) ) || (defined( CINDER_MAC ))
					TextureCameraRef t(new TextureCamera());
					t->fromXml(detailsXml);
					vdtexturelist.push_back(t);
#else
					// unknown texture type
					CI_LOG_V("camera not supported on this platform");
					TextureTextRef t(new TextureText());
					t->setString("camera not supported on this platform");
					vdtexturelist.push_back(t);
#endif
				}
				else {
					// unknown texture type
					CI_LOG_V("unknown texture type");
					TextureTextRef t(new TextureText());
					t->setString("unknown texture type");
					vdtexturelist.push_back(t);
				}
			}
		}
		else {
			// malformed XML
			CI_LOG_V("malformed XML");
			TextureTextRef t(new TextureText());
			t->setString("malformed XML");
			vdtexturelist.push_back(t);
		}
		return vdtexturelist;
	}

	void VDTexture::writeSettings(const VDTextureList &vdtexturelist, const ci::DataTargetRef &target) {

		// create config document and root <textures>
		XmlTree			doc;
		doc.setTag("textures");
		doc.setAttribute("version", "1.0");

		// 
		for (unsigned i = 0; i < vdtexturelist.size(); ++i) {
			// create <texture>
			XmlTree			texture;
			texture.setTag("texture");
			texture.setAttribute("id", i + 1);
			switch (vdtexturelist[i]->mType) {
			case IMAGE: texture.setAttribute("texturetype", "image"); break;
			case TEXT: texture.setAttribute("texturetype", "text"); break;
			case MOVIE: texture.setAttribute("texturetype", "movie"); break;
			case CAMERA: texture.setAttribute("texturetype", "camera"); break;
			default: texture.setAttribute("texturetype", "unknown"); break;
			}
			// details specific to texture type
			texture.push_back(vdtexturelist[i]->toXml());

			// add texture doc
			//texture.setAttribute("filepath", vdtexturelist[i]->mFilePathOrText);
			doc.push_back(texture);
		}

		// write file
		doc.write(target);
	}
	XmlTree	VDTexture::toXml() const
	{
		XmlTree		xml;
		xml.setTag("details");
		xml.setAttribute("filepath", mFilePathOrText);
		xml.setAttribute("width", mWidth);
		xml.setAttribute("height", mHeight);

		return xml;
	}

	void VDTexture::fromXml(const XmlTree &xml)
	{

	}
	void VDTexture::loadImageFromFileFullPath(string aFilePath) {
		mTexture = ci::gl::Texture::create(loadImage(aFilePath), ci::gl::Texture::Format().loadTopDown(mTopDown));

	}
	int VDTexture::getTextureWidth() {
		return mWidth;
	};

	int VDTexture::getTextureHeight() {
		return mHeight;
	};

	ci::ivec2 VDTexture::getSize() {
		return mTexture->getSize();
	}

	ci::Area VDTexture::getBounds() {
		return mTexture->getBounds();
	}

	GLuint VDTexture::getId() {
		return mTexture->getId();
	}

	std::string VDTexture::getName(){
		return mName;
	}

	ci::gl::TextureRef VDTexture::getTexture() {
		return mTexture;
	}
	/*
	*   child classes
	*/
	TextureImage::TextureImage() {
	}
	XmlTree	TextureImage::toXml() const {
		XmlTree xml = VDTexture::toXml();

		// add attributes specific to this type of texture
		xml.setAttribute("path", getAssetPath("").string());
		xml.setAttribute("topdown", mTopDown);
		return xml;
	}

	void TextureImage::fromXml(const XmlTree &xml)
	{
		VDTexture::fromXml(xml);
		mType = IMAGE;
		// retrieve attributes specific to this type of texture
		mTopDown = xml.getAttributeValue<bool>("topdown", "false");
		mFilePathOrText = xml.getAttributeValue<string>("filepath", "");
		if (mFilePathOrText.length() > 0) {
			fs::path fullPath = getAssetPath("") / mFilePathOrText;// TODO / mVDSettings->mAssetsPath
			if (fs::exists(fullPath)) {
				// TODO mTopDown has no effect!?!
				mTexture = ci::gl::Texture::create(loadImage(loadAsset(mFilePathOrText)), ci::gl::Texture::Format().loadTopDown(mTopDown));
			}
			else {
				mTexture = ci::gl::Texture::create(mWidth, mHeight, ci::gl::Texture::Format().loadTopDown(mTopDown));
			}
		}
	}


	ci::gl::Texture2dRef TextureImage::getTexture() {
		return mTexture;
	}
	TextureImage::~TextureImage(void) {
	}
	// TextureText
	TextureText::TextureText() {
		// font
#if defined( CINDER_MSW )
		Font customFont(Font(loadAsset("Signika-Regular.ttf"), 100));
		gl::TextureFont::Format f;
		f.enableMipmapping(true);
		mTextureFont = gl::TextureFont::create(customFont, f);

		// camera
		mCamDist = 600.0f;
		mCam.setPerspective(75.0f, getWindowAspectRatio(), 0.1f, 15000.0f);
		mCam.lookAt(vec3(0.0f, 0.0f, mCamDist), vec3(0), vec3(0, 1, 0));
		mCam.setLensShiftHorizontal(-0.6);
		// scene
		mSceneMatrix = mSceneDestMatrix = mat4(1.0f); // identity

		// init text
		addChar('V'); addChar('I'); addChar('D'); addChar('E'); addChar('O'); 
		addChar('D'); addChar('R'); addChar('O'); addChar('M'); addChar('M');

		stringIndex = 0;

		currentFrame = -1;
		startFrame = 0;
		// fbo
		gl::Fbo::Format format;
		//format.setSamples( 4 ); // uncomment this to enable 4x antialiasing
		mFbo = gl::Fbo::create(mWidth, mHeight, format.depthTexture());
#endif	
	}
	void TextureText::setString(string aString) {
		mStrings.push_back(aString);
	}
	void TextureText::fromXml(const XmlTree &xml)
	{
		mType = TEXT;
		// retrieve attributes specific to this type of texture
		mFilePathOrText = xml.getAttributeValue<string>("filepath", "");
		if (mFilePathOrText.length() > 0) {
			fs::path fullPath = getAssetPath("") / mFilePathOrText;// TODO / mVDSettings->mAssetsPath
			if (fs::exists(fullPath)) {

				try {
					mText = JsonTree(app::loadAsset(mFilePathOrText));// TODO / mVDSettings->mAssetsPath
					mStrings.clear();

					for (const auto &line : mText["lines"]) {
						mStrings.push_back(line.getValue());
					}

					CI_LOG_V("successfully loaded " + mFilePathOrText);
				}
				catch (Exception &exc) {
					CI_LOG_EXCEPTION("error loading ", exc);
				}
			}
		}
	}
	XmlTree	TextureText::toXml() const {
		XmlTree xml = VDTexture::toXml();

		// add attributes specific to this type of texture
		xml.setAttribute("filepath", mFilePathOrText);
		return xml;
	}
	ci::gl::Texture2dRef TextureText::getTexture() {
#if defined( CINDER_MSW )
		// should be in update?
		//addChar('O');

		if (frame < mStrings[stringIndex].size() - 1) {
			char c[2];
			printf_s(c, "%s", mStrings[stringIndex].substr(frame, 1).c_str());
			addChar(*c);
		}
		gl::ScopedFramebuffer fbScp(mFbo);
		gl::clear(Color::black());
		// setup the viewport to match the dimensions of the FBO
		gl::ScopedViewport scpVp(ivec2(0), mFbo->getSize());
		for (vector<Character>::iterator it = mCharacters.begin(); it != mCharacters.end(); ++it)
			it->draw();

		for (list<Character>::iterator it = mDyingCharacters.begin(); it != mDyingCharacters.end(); ++it)
			it->draw();

		if ((!mDyingCharacters.empty()) && mDyingCharacters.front().isDead())
			mDyingCharacters.pop_front();
		return mFbo->getColorTexture();
#else
		return mTexture;
#endif		
	}
#if defined( CINDER_MSW )
	void TextureText::addChar(char c)
	{
		if (c == 32) {
			c = 97;
		}
		else {
			c = toupper(c);
		}
		int count = mCharacters.size();

		if (count > 0)
			mSceneDestMatrix = glm::translate(mSceneDestMatrix, vec3(mCharacters.back().getKernBounds().getWidth() / 2.0f, 0.0f, 0.0f));

		glm::mat4 randStartMatrix = mSceneDestMatrix;
		randStartMatrix = glm::translate(randStartMatrix, vec3(300.0f));

		mCharacters.push_back(Character(mTextureFont, string(&c, 1), randStartMatrix));

		//mSceneDestMatrix.translate(vec3(mCharacters.back().getKernBounds().getWidth() / 2.0f, 0.0f, 0.0f));
		mSceneDestMatrix = glm::translate(mSceneDestMatrix, vec3(mCharacters.back().getKernBounds().getWidth() / 2.0f, 0.0f, 0.0f));

		float t = (count + 281) / 50.0f;
		//mSceneDestMatrix.rotate(Vec3f(sin(t)*0.1f, cos(t)*0.2f, cos(t)*0.05f)); // makes the scene meander

		mCharacters.back().animIn(timeline(), mSceneDestMatrix);

		timeline().apply(&mSceneMatrix, mSceneDestMatrix, 1.0f, EaseOutAtan(10));

	}

	void TextureText::removeChar()
	{
		if (!mCharacters.empty()) {
			mDyingCharacters.push_back(mCharacters.back());
			mCharacters.pop_back();

			if (!mCharacters.empty())
				mSceneDestMatrix = mCharacters.back().getDestMatrix();
			else
				mSceneDestMatrix = mat4(1.0f);

			mat4 randStartMatrix = glm::translate( mSceneDestMatrix, vec3(300.0f));
			//randStartMatrix.translate(300.0f);

			//randStartMatrix.rotate(getRandomVec3f(2.0f, 6.0f));

			mDyingCharacters.back().animOut(timeline(), randStartMatrix);

			timeline().apply(&mSceneMatrix, mSceneDestMatrix, 1.0f, EaseOutAtan(10));
		}
	}
#else

#endif
	TextureText::~TextureText(void) {

	}

	// TextureMovie
	TextureMovie::TextureMovie() {

	}
	void TextureMovie::fromXml(const XmlTree &xml)
	{
		// retrieve attributes specific to this type of texture
		mFilePathOrText = xml.getAttributeValue<string>("filepath", "");
		mType = MOVIE;

	}
	XmlTree	TextureMovie::toXml() const {
		XmlTree xml = VDTexture::toXml();

		// add attributes specific to this type of texture
		xml.setAttribute("filepath", mFilePathOrText);
		return xml;
	}
	ci::gl::Texture2dRef TextureMovie::getTexture() {
		return mTexture;
	}
	TextureMovie::~TextureMovie(void) {

	}

	// TextureCamera
#if (defined(  CINDER_MSW) ) || (defined( CINDER_MAC ))
	TextureCamera::TextureCamera() {
		mFirstCameraDeviceName = "";
		printDevices();

		try {
			mCapture = Capture::create(640, 480);
			mCapture->start();
		}
		catch (ci::Exception &exc) {
			CI_LOG_EXCEPTION("Failed to init capture ", exc);
		}
	}
	void TextureCamera::fromXml(const XmlTree &xml)
	{
		// retrieve attributes specific to this type of texture
		mFilePathOrText = xml.getAttributeValue<string>("filepath", "");
		mType = CAMERA;

	}
	XmlTree	TextureCamera::toXml() const {
		XmlTree xml = VDTexture::toXml();

		// add attributes specific to this type of texture
		xml.setAttribute("camera", mFirstCameraDeviceName);
		return xml;
	}
	ci::gl::Texture2dRef TextureCamera::getTexture() {
		if (mCapture && mCapture->checkNewFrame()) {
			if (!mTexture) {
				// Capture images come back as top-down, and it's more efficient to keep them that way
				mTexture = gl::Texture::create(*mCapture->getSurface(), gl::Texture::Format().loadTopDown());
			}
			else {
				mTexture->update(*mCapture->getSurface());
			}
		}
		return mTexture;
	}
	void TextureCamera::printDevices()
	{
		for (const auto &device : Capture::getDevices()) {
			console() << "Device: " << device->getName() << " "
#if defined( CINDER_COCOA_TOUCH )
				<< (device->isFrontFacing() ? "Front" : "Rear") << "-facing"
#endif
				<< endl;
			mFirstCameraDeviceName = device->getName();
		}
	}
	TextureCamera::~TextureCamera(void) {

	}
#else

#endif
} // namespace VideoDromm
