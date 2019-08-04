#include "VDTexture.h"

#include "cinder/gl/Texture.h"
#include "cinder/Xml.h"

using namespace ci;
using namespace ci::app;

namespace videodromm {
	VDTexture::VDTexture(TextureType aType)
		: mPath("")
		, mName("")
		, mFlipV(false)
		, mFlipH(true)
		, mWidth(1280)
		, mHeight(720)
	{
		mBoundsLocked = true;
		mXLeft = 0;
		mYTop = 0;
		mPosition = 1;
		mXRight = mOriginalWidth = mWidth;
		mYBottom = mOriginalHeight = mHeight;
		mAreaWidth = mWidth;
		mAreaHeight = mHeight;
		mFolder = "";
		mSyncToBeat = false;
		mPlaying = true;
		if (mName.length() == 0) {
			mName = mPath;
		}
		// init the texture whatever happens next
		if (mPath.length() > 0) {
			mTexture = ci::gl::Texture::create(ci::loadImage(mPath), ci::gl::Texture::Format().loadTopDown(mFlipV));
			mInputSurface = Surface(loadImage(mPath));
		}
		else {
			mTexture = ci::gl::Texture::create(mWidth, mHeight, ci::gl::Texture::Format().loadTopDown(mFlipV));
			mInputSurface = Surface(mWidth, mHeight, true);
		}
		fboFmt.setColorTextureFormat(fmt);
		mFbo = gl::Fbo::create(mWidth, mHeight, fboFmt);

	}
	VDTexture::~VDTexture(void) {

	}
	VDTextureList VDTexture::readSettings(VDAnimationRef aVDAnimation, const DataSourceRef &source)
	{
		XmlTree			doc;
		VDTextureList	vdtexturelist;
		bool            isValid = true;
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

				//std::string path = detailsXml.getAttributeValue<std::string>("path", "");
				if (texturetype == "image") {
					TextureImageRef t(new TextureImage());
					t->fromXml(detailsXml);
					vdtexturelist.push_back(t);
				}
				else if (texturetype == "imagesequence") {
					TextureImageSequenceRef t(new TextureImageSequence(aVDAnimation));
					t->fromXml(detailsXml);
					vdtexturelist.push_back(t);
				}
				else if (texturetype == "movie") {
#if (defined(  CINDER_MSW) ) || (defined( CINDER_MAC ))
					TextureMovieRef t(new TextureMovie());
					t->fromXml(detailsXml);
					vdtexturelist.push_back(t);
#else
					// movie not supported on this platform
					CI_LOG_V("movie not supported on this platform");
					isValid = false;
#endif
				}
				else if (texturetype == "camera") {
#if (defined(  CINDER_MSW) ) || (defined( CINDER_MAC ))
					TextureCameraRef t(new TextureCamera());
					t->fromXml(detailsXml);
					vdtexturelist.push_back(t);
#else
					// camera not supported on this platform
					CI_LOG_V("camera not supported on this platform");
					isValid = false;
#endif
				}
				else if (texturetype == "shared") {
#if (defined(  CINDER_MSW) ) || (defined( CINDER_MAC ))
					TextureSharedRef t(new TextureShared());
					t->fromXml(detailsXml);
					vdtexturelist.push_back(t);
#else
					// shared textures not supported on this platform
					CI_LOG_V("shared textures not supported on this platform");
					isValid = false;
#endif
				}
				else if (texturetype == "audio") {
					TextureAudioRef t(new TextureAudio(aVDAnimation));
					t->fromXml(detailsXml);
					vdtexturelist.push_back(t);
				}
				else if (texturetype == "stream") {
					TextureStreamRef t(new TextureStream(aVDAnimation));
					t->fromXml(detailsXml);
					vdtexturelist.push_back(t);
				}
				else {
					// unknown texture type
					CI_LOG_V("unknown texture type");
				}
			}
			if (!isValid)
			{
				TextureImageRef t(new TextureImage());
				XmlTree		xml;
				xml.setTag("details");
				xml.setAttribute("path", "0.jpg");
				xml.setAttribute("width", 1280);
				xml.setAttribute("height", 720);
				t->fromXml(xml);
				vdtexturelist.push_back(t);
			}
		}
		else {
			// malformed XML
			CI_LOG_V("malformed XML");
			TextureImageRef t(new TextureImage());
			XmlTree		xml;
			xml.setTag("details");
			xml.setAttribute("path", "0.jpg");
			xml.setAttribute("width", 1280);
			xml.setAttribute("height", 720);
			t->fromXml(xml);
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
			case SEQUENCE: texture.setAttribute("texturetype", "imagesequence"); break;
			case MOVIE: texture.setAttribute("texturetype", "movie"); break;
			case SHARED: texture.setAttribute("texturetype", "shared"); break;
			case CAMERA: texture.setAttribute("texturetype", "camera"); break;
			case AUDIO: texture.setAttribute("texturetype", "audio"); break;
			default: texture.setAttribute("texturetype", "unknown"); break;
			}
			// details specific to texture type
			texture.push_back(vdtexturelist[i]->toXml());

			// add texture doc
			//texture.setAttribute("path", vdtexturelist[i]->mPath);
			doc.push_back(texture);
		}

		// write file
		doc.write(target);
	}
	XmlTree	VDTexture::toXml() const
	{
		XmlTree		xml;
		xml.setTag("details");
		xml.setAttribute("path", mPath);
		xml.setAttribute("width", mWidth);
		xml.setAttribute("height", mHeight);

		return xml;
	}

	bool VDTexture::fromXml(const XmlTree &xml)
	{
		return true;
	}
	void VDTexture::toggleLoadingFromDisk() {

	}
	bool VDTexture::isLoadingFromDisk() {
		return false;
	}
	// play/pause (sequence/movie)
	void VDTexture::togglePlayPause() {

		mPlaying = !mPlaying;
	}

	// sync to beat
	void VDTexture::syncToBeat() {

		mSyncToBeat = !mSyncToBeat;
	}
	void VDTexture::reverse() {
	}
	float VDTexture::getSpeed() {
		return 1;
	}
	void VDTexture::setSpeed(float speed) {
	}

	void VDTexture::setPlayheadPosition(int position) {
	}
	int VDTexture::getMaxFrame() {
		return 1;
	}
	bool VDTexture::loadFromFullPath(string aPath) {
		// initialize texture
		mTexture = ci::gl::Texture::create(mWidth, mHeight, ci::gl::Texture::Format().loadTopDown(mFlipV));
		return true;
	}
	void VDTexture::lockBounds(bool lock, unsigned int aWidth, unsigned int aHeight) {
		mBoundsLocked = lock;
		mAreaWidth = aWidth;
		mAreaHeight = aHeight;
	}
	void VDTexture::setXLeft(int aXleft) {
		mXLeft = aXleft;
		if (mBoundsLocked) {
			mXRight = mXLeft + mAreaWidth;
		}
	};
	void VDTexture::setYTop(int aYTop) {
		mYTop = aYTop;
		if (mBoundsLocked) {
			mYBottom = mYTop + mAreaHeight;
		}
	};
	void VDTexture::setXRight(int aXRight) {
		mXRight = aXRight;
		if (mBoundsLocked) {
			mXLeft = mXRight - mAreaWidth;
		}
	};
	void VDTexture::setYBottom(int aYBottom) {
		mYBottom = aYBottom;
		if (mBoundsLocked) {
			mYTop = mYBottom - mAreaHeight;
		}
	}
	void VDTexture::flipV() {
		mFlipV = !mFlipV;
	}
	void VDTexture::flipH() {
		mFlipH = !mFlipH;
	}
	bool VDTexture::getLockBounds() {
		return mBoundsLocked;
	}
	void VDTexture::toggleLockBounds() {
		mBoundsLocked = !mBoundsLocked;
	};
	unsigned int VDTexture::getTextureWidth() {
		return mWidth;
	};

	unsigned int VDTexture::getTextureHeight() {
		return mHeight;
	};
	unsigned int VDTexture::getOriginalWidth() {
		return mOriginalWidth;
	};
	unsigned int VDTexture::getOriginalHeight() {
		return mOriginalHeight;
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

	std::string VDTexture::getName() {
		return mName;
	}

	ci::gl::TextureRef VDTexture::getTexture() {
		return mTexture;
	}
	/*
	**   Child classes
	*/
	/*
	**   TextureImage for jpg, png
	*/
	TextureImage::TextureImage() {
		mType = IMAGE;
	}
	XmlTree	TextureImage::toXml() const {
		XmlTree xml = VDTexture::toXml();

		// add attributes specific to this type of texture
		xml.setAttribute("path", mPath);
		xml.setAttribute("flipv", mFlipV);
		xml.setAttribute("fliph", mFlipH);
		return xml;
	}

	bool TextureImage::fromXml(const XmlTree &xml)
	{
		VDTexture::fromXml(xml);
		// retrieve attributes specific to this type of texture
		mFlipV = xml.getAttributeValue<bool>("flipv", "true"); // default true
		mFlipH = xml.getAttributeValue<bool>("fliph", "true"); // default true
		mPath = xml.getAttributeValue<string>("path", "");
		mFolder = xml.getAttributeValue<string>("folder", "");
		mName = mPath;
		if (mPath.length() > 0) {
			fs::path fullPath = getAssetPath("") / mFolder / mPath;
			loadFromFullPath(fullPath.string());
		}
		return true;
	}
	bool TextureImage::loadFromFullPath(string aPath) {
		if (fs::exists(aPath)) {
			mTexture = ci::gl::Texture::create(loadImage(aPath));
			mInputSurface = Surface(loadImage(aPath));
		}
		else {
			mTexture = ci::gl::Texture::create(mWidth, mHeight);
			mInputSurface = Surface(mWidth, mHeight, true);
		}
		mXLeft = 0;
		mYTop = 0;
		mXRight = mOriginalWidth = mTexture->getWidth();
		mYBottom = mOriginalHeight = mTexture->getHeight();
		return true;
	}

	ci::gl::Texture2dRef TextureImage::getTexture() {
		Area area(mXLeft, mYTop, mXRight, mYBottom);
		mProcessedSurface = mInputSurface.clone(area);
		mTexture = gl::Texture2d::create(mProcessedSurface, ci::gl::Texture::Format().loadTopDown(mFlipV));
		return mTexture;
	}

	TextureImage::~TextureImage(void) {
	}

	/*
	** TextureImageSequence
	*/
	TextureImageSequence::TextureImageSequence(VDAnimationRef aVDAnimation) {
		// constructor
		mVDAnimation = aVDAnimation;
		mType = SEQUENCE;
		playheadFrameInc = 0.0f;
		mLoadingFilesComplete = true;
		mLoadingPaused = false;
		mFramesLoaded = 0;
		mCurrentLoadedFrame = 0;
		mNextIndexFrameToTry = 0;
		mPlaying = false;
		mSpeed = 0.00023f;
		mExt = "png";
		mPrefix = "none";
		mNextIndexFrameToTry = 0;
		mPosition = 0;
		mNumberOfDigits = 4;
	}
	bool TextureImageSequence::loadFromFullPath(string aPath)
	{
		bool validFile = false; // if no valid files in the folder, we keep existing vector
		string anyImagefileName = "0.jpg";
		string folder = "";
		string fileName;
		fs::path fullPath = aPath;
		if (fs::exists(fullPath)) {
			try {
				bool firstIndexFound = false;
				// loading 2000 files takes a while, I load only the first one
				for (fs::directory_iterator it(fullPath); it != fs::directory_iterator(); ++it)
				{
					if (fs::is_regular_file(*it))
					{
						fileName = it->path().filename().string();
						mName = fileName;
						if (fileName.find_last_of(".") != std::string::npos) {
							int dotIndex = fileName.find_last_of(".");
							mExt = fileName.substr(dotIndex + 1);
							if (mExt == "png" || mExt == "jpg") {
								anyImagefileName = fileName;
							}
							// get the prefix for the image sequence
							// the files are named from p0000.jpg to p2253.jpg for instance
							// sometimes only 3 digits : l000 this time
							// find the first digit
							int firstDigit = fileName.find_first_of("0123456789");
							// if valid image sequence (contains a digit)
							if (firstDigit > -1) {
								mNumberOfDigits = dotIndex - firstDigit;
								int prefixIndex = fileName.find_last_of(".") - mNumberOfDigits;//-4 or -3
								mPrefix = fileName.substr(0, prefixIndex);
								if (!firstIndexFound) {
									firstIndexFound = true;
									mNextIndexFrameToTry = std::stoi(fileName.substr(prefixIndex, dotIndex));
								}
							}
						}
						// only if proper image sequence
						if (firstIndexFound) {
							if (mExt == "png" || mExt == "jpg") {
								if (!validFile) {
									// we found a valid file
									validFile = true;
									mSequenceTextures.clear();
									// TODO only store folder relative to assets, not full path 
									size_t found = fullPath.string().find_last_of("/\\");
									mPath = fullPath.string().substr(found + 1);

									// reset playhead to the start
									//mPlayheadPosition = 0;
									mLoadingFilesComplete = false;
									loadNextImageFromDisk();
									mPlaying = true;
								}
							}
						}

					}
				}
				CI_LOG_V("successfully loaded " + mPath);
			}
			catch (Exception &exc) {
				CI_LOG_EXCEPTION("error loading ", exc);
			}

			// init: if no valid file found we take the default 0.jpg
			if (!validFile) {
				// might want to remove default file as we are now using a boolean to notify the caller
				if (anyImagefileName.length() > 0) {
					mTexture = ci::gl::Texture::create(loadImage(loadAsset(anyImagefileName)), ci::gl::Texture::Format().loadTopDown(mFlipV));
					mSequenceTextures.push_back(ci::gl::Texture::create(loadImage(loadAsset(anyImagefileName)), gl::Texture::Format().loadTopDown(mFlipV)));
					mLoadingFilesComplete = true;
					mFramesLoaded = 1;
				}
			}
		}
		return validFile;
	}
	bool TextureImageSequence::fromXml(const XmlTree &xml)
	{
		bool rtn = false;
		// init		
		mTexture = ci::gl::Texture::create(mWidth, mHeight, ci::gl::Texture::Format().loadTopDown(mFlipV));
		// retrieve attributes specific to this type of texture
		mPath = xml.getAttributeValue<string>("path", "");
		mName = mPath;
		mFlipV = xml.getAttributeValue<bool>("flipv", "false");
		mFlipH = xml.getAttributeValue<bool>("fliph", "false");
		if (mPath.length() > 0) {
			if (fs::exists(mPath)) {
				rtn = loadFromFullPath(mPath);
			}
			else {
				fs::path fullPath = getAssetPath("") / mPath;// TODO / mVDSettings->mAssetsPath
				rtn = loadFromFullPath(fullPath.string());
			}
		}
		return rtn;
	}
	XmlTree	TextureImageSequence::toXml() const {
		XmlTree xml = VDTexture::toXml();

		// add attributes specific to this type of texture
		xml.setAttribute("path", mPath);
		xml.setAttribute("flipv", mFlipV);
		xml.setAttribute("fliph", mFlipH);
		return xml;
	}
	void TextureImageSequence::loadNextImageFromDisk() {
		if (!mLoadingPaused) {

			if (!mLoadingFilesComplete) {
				// thank you Omar!
				char restOfFileName[32];
				if (mNumberOfDigits == 4) {

					sprintf(restOfFileName, "%04d", mNextIndexFrameToTry);
				}
				else {

					sprintf(restOfFileName, "%03d", mNextIndexFrameToTry);
				}
				string fileNameToLoad = mPrefix + restOfFileName + "." + mExt;
				fs::path fileToLoad = getAssetPath("") / mPath / fileNameToLoad;
				if (fs::exists(fileToLoad)) {
					// start profiling
					auto start = Clock::now();

					mSequenceTextures.push_back(ci::gl::Texture::create(loadImage(fileToLoad), gl::Texture::Format().loadTopDown()));
					mCurrentLoadedFrame = mFramesLoaded;
					mFramesLoaded++;
					auto end = Clock::now();
					auto msdur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
					int milli = msdur.count();

					mStatus = fileToLoad.string() + " loaded in ms " + toString(milli);
				}
				else {

					mStatus = fileToLoad.string() + " does not exist";
					if (mFramesLoaded > 0) {
						// if frames have been loaded we hit the last file of the image sequence at this point
						mStatus = "last image loaded";
						mLoadingFilesComplete = true;
					}
				}

				// increment counter for next filename
				mNextIndexFrameToTry++;
				if (mNextIndexFrameToTry > 9999 && mNumberOfDigits == 4) mLoadingFilesComplete = true;
				if (mNextIndexFrameToTry > 999 && mNumberOfDigits == 3) mLoadingFilesComplete = true;
			}
		}
	}
	void TextureImageSequence::updateSequence() {
		int newPosition;

		if (mSequenceTextures.size() > 0) {
			// Call on each frame to update the playhead
			if (mPlaying) {
				playheadFrameInc += mSpeed;
				newPosition = mPosition + (int)playheadFrameInc;
				if (playheadFrameInc > 1.0f) playheadFrameInc = 0.0f;
				if (newPosition < 0) newPosition = mSequenceTextures.size() - 1;
				if (newPosition > mSequenceTextures.size() - 1) newPosition = 0;
			}
			else {
				if (mSyncToBeat) {
					//newPosition = (int)(((int)(mVDAnimation->iBar / mVDAnimation->iPhaseIndex)) % mSequenceTextures.size());
					// TODO newPosition = (int)(((int)(mVDSettings->iPhase / mVDAnimation->iPhaseIndex)) % mSequenceTextures.size());
					//newPosition = (int)(mVDAnimation->getIntUniformValueByName("iPhase") % mSequenceTextures.size());
					// TODO CHECK 20190803 newPosition = (int)(mVDAnimation->getFloatUniformValueByIndex(mVDSettings->IBEAT) % mSequenceTextures.size());
				}
				else {
					newPosition = mPosition;
				}
				
			}
			mPosition = max(0, min(newPosition, (int)mSequenceTextures.size() - 1));
		}
	}

	ci::gl::Texture2dRef TextureImageSequence::getTexture() {

		if (mSequenceTextures.size() > 0) {

			if (mPosition > mFramesLoaded) {
				//error
				mPosition = 0;
			}
			if (!mLoadingFilesComplete) loadNextImageFromDisk();

			if (mPlaying) {
				updateSequence();
			}
			mTexture = mSequenceTextures[mPosition];
		}
		return mTexture;
	}

	// Stops playback and resets the playhead to zero
	void TextureImageSequence::stopSequence() {

		mPlaying = false;
		mPosition = 0;
	}

	int TextureImageSequence::getMaxFrame() {

		return mFramesLoaded;
	}

	// Seek to a new position in the sequence
	void TextureImageSequence::setPlayheadPosition(int position) {

		mPosition = max(0, min(position, (int)mSequenceTextures.size() - 1));
		if (!mLoadingFilesComplete) {
			loadNextImageFromDisk();
		}
	}
	void TextureImageSequence::reverse() {
		mSpeed *= -1.0f;
	}
	float TextureImageSequence::getSpeed() {
		return mSpeed;
	}
	void TextureImageSequence::setSpeed(float speed) {
		mSpeed = speed;
	}
	bool TextureImageSequence::isLoadingFromDisk() {
		return !mLoadingFilesComplete;
	}

	void TextureImageSequence::toggleLoadingFromDisk() {
		mLoadingPaused = !mLoadingPaused;
	}
	TextureImageSequence::~TextureImageSequence(void) {
	}


	/*
	** ---- TextureMovie ------------------------------------------------
	*/
#if (defined(  CINDER_MSW) ) || (defined( CINDER_MAC ))
	TextureMovie::TextureMovie() {
		mType = MOVIE;

	}
	bool TextureMovie::fromXml(const XmlTree &xml)
	{
		bool rtn = false;
		// init		
		mTexture = ci::gl::Texture::create(mWidth, mHeight, ci::gl::Texture::Format().loadTopDown(mFlipV));
		// retrieve attributes specific to this type of texture
		mFlipV = xml.getAttributeValue<bool>("flipv", "false");
		mFlipH = xml.getAttributeValue<bool>("fliph", "false");
		mPath = xml.getAttributeValue<string>("path", "");
		if (mPath.length() > 0) {
			fs::path fullPath = getAssetPath("") / mPath;// TODO / mVDSettings->mAssetsPath
			if (fs::exists(fullPath)) {
				rtn = loadFromFullPath(fullPath.string());
			}
		}
		mName = mPath;
		return rtn;
	}
	XmlTree	TextureMovie::toXml() const {
		XmlTree xml = VDTexture::toXml();

		// add attributes specific to this type of texture
		xml.setAttribute("path", mPath);
		xml.setAttribute("flipv", mFlipV);
		xml.setAttribute("fliph", mFlipH);
		return xml;
	}
#endif
#if (defined(  CINDER_MSW) ) || (defined( CINDER_MAC ))
	bool TextureMovie::loadFromFullPath(string aPath)
	{
		bool rtn = false;
		try {
			mMovie.reset();
			// load up the movie, set it to loop, and begin playing
#if defined( CINDER_MSW )
			mMovie = qtime::MovieGlHap::create(aPath);
#endif
#if defined( CINDER_MAC )
			mMovie = qtime::MovieGl::create(aPath);
#endif
			if (mName.length() > 0) mName = aPath;
			mLoopVideo = true;// Mars aux musees (mMovie->getDuration() < 30.0f);
			mMovie->setLoop(mLoopVideo);
			mMovie->play();
			mWidth = mMovie->getWidth();
			mHeight = mMovie->getHeight();
			mTexture = ci::gl::Texture::create(mWidth, mHeight, ci::gl::Texture::Format().loadTopDown(mFlipV));
			rtn = true;
		}
		catch (ci::Exception &e)
		{
			console() << string(e.what()) << std::endl;
			console() << "Unable to load the movie." << std::endl;
		}
		return true;
	}

	ci::gl::Texture2dRef TextureMovie::getTexture() {
		if (mMovie) {
			// toggle play if necessary
			if (mMovie->isPlaying()) {
				if (!mPlaying) mMovie->stop();
			}
			else {
				if (mPlaying) mMovie->play();
			}
			mTexture = mMovie->getTexture();
			// if codec is not recognized the texture is empty, return an initialized texture
			if (!mTexture) {
				mTexture = ci::gl::Texture::create(mWidth, mHeight, ci::gl::Texture::Format().loadTopDown(mFlipV));
			}
		}
		return mTexture;
	}
	TextureMovie::~TextureMovie(void) {
	}
#endif
	/*
	** ---- TextureCamera ------------------------------------------------
	*/
#if (defined(  CINDER_MSW) ) || (defined( CINDER_MAC ))
	TextureCamera::TextureCamera() {
		mType = CAMERA;
		mFirstCameraDeviceName = "";
		printDevices();

		try {
			mCapture = Capture::create(mWidth, mHeight);
			mCapture->start();
		}
		catch (ci::Exception &exc) {
			CI_LOG_EXCEPTION("Failed to init capture ", exc);
		}
	}
	bool TextureCamera::fromXml(const XmlTree &xml) {
		// init		
		mTexture = ci::gl::Texture::create(mWidth, mHeight, ci::gl::Texture::Format().loadTopDown(mFlipV));
		// retrieve attributes specific to this type of texture
		mPath = xml.getAttributeValue<string>("path", "");
		mName = "camera";
		return true;
	}
	XmlTree	TextureCamera::toXml() const {
		XmlTree xml = VDTexture::toXml();

		// add attributes specific to this type of texture
		xml.setAttribute("camera", mFirstCameraDeviceName);
		xml.setAttribute("flipv", mFlipV);
		xml.setAttribute("fliph", mFlipH);
		xml.setAttribute("path", mPath);
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
	void TextureCamera::printDevices() {
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
#endif

	/*
	** ---- TextureShared ------------------------------------------------
	*/
#if (defined(  CINDER_MSW) ) || (defined( CINDER_MAC ))
	TextureShared::TextureShared() {
		mType = SHARED;
#if defined( CINDER_MAC )
		mClientSyphon.setup();
		mClientSyphon.setServerName("videodromm client");
		mClientSyphon.bind();
#endif
	}
	bool TextureShared::fromXml(const XmlTree &xml)
	{
		// init		
		mTexture = ci::gl::Texture::create(mWidth, mHeight, ci::gl::Texture::Format().loadTopDown(mFlipV));
		// retrieve attributes specific to this type of texture
		mFlipV = xml.getAttributeValue<bool>("flipv", "false");
		mFlipH = xml.getAttributeValue<bool>("fliph", "false");
		mPath = xml.getAttributeValue<string>("path", "");
		mName = "shared";
		return true;
	}
	XmlTree	TextureShared::toXml() const {
		XmlTree xml = VDTexture::toXml();

		// add attributes specific to this type of texture
		xml.setAttribute("path", mPath);
		xml.setAttribute("flipv", mFlipV);
		xml.setAttribute("fliph", mFlipH);
		return xml;
	}

	ci::gl::Texture2dRef TextureShared::getTexture() {
#if defined( CINDER_MSW )

		mTexture = mSpoutIn.receiveTexture();
		// set name for UI
		mName = mSpoutIn.getSenderName();

#endif
#if defined( CINDER_MAC )
		mClientSyphon.draw(vec2(0.f, 0.f));
#endif
		return mTexture;
	}
	TextureShared::~TextureShared(void) {
#if defined( CINDER_MSW )
		mSpoutIn.getSpoutReceiver().ReleaseReceiver();
#endif

	}
#endif
	/*
	** ---- TextureAudio ------------------------------------------------
	*/
	TextureAudio::TextureAudio(VDAnimationRef aVDAnimation) {
		mVDAnimation = aVDAnimation;
		mType = AUDIO;
		mLineInInitialized = false;
		mName = "audio";

		auto fmt = gl::Texture2d::Format().swizzleMask(GL_RED, GL_RED, GL_RED, GL_ONE).internalFormat(GL_RED);
		for (int i = 0; i < 128; ++i) dTexture[i] = (unsigned char)(Rand::randUint() & 0xFF);
		mTexture = gl::Texture::create(dTexture, GL_RED, 64, 2, fmt);
	}
	XmlTree	TextureAudio::toXml() const {
		XmlTree xml = VDTexture::toXml();

		// add attributes specific to this type of texture
		xml.setAttribute("path", mPath);
		xml.setAttribute("flipv", mFlipV);
		xml.setAttribute("fliph", mFlipH);
		xml.setAttribute("uselinein", mVDAnimation->getUseLineIn());

		return xml;
	}

	bool TextureAudio::fromXml(const XmlTree &xml)
	{
		VDTexture::fromXml(xml);
		// retrieve attributes specific to this type of texture
		mFlipV = xml.getAttributeValue<bool>("flipv", "false");
		mFlipH = xml.getAttributeValue<bool>("fliph", "false");
		// prevent linein not present crash mVDAnimation->setUseLineIn(xml.getAttributeValue<bool>("uselinein", "true"));
		mName = (mVDAnimation->getUseLineIn()) ? "line in" : "wave";
		auto fmt = gl::Texture2d::Format().swizzleMask(GL_RED, GL_RED, GL_RED, GL_ONE).internalFormat(GL_RED);
		for (int i = 0; i < 128; ++i) dTexture[i] = (unsigned char)(Rand::randUint() & 0xFF);
		mTexture = gl::Texture::create(dTexture, GL_RED, 64, 2, fmt);
		return true;
	}
	bool TextureAudio::loadFromFullPath(string aPath)
	{
		CI_LOG_V("TextureAudio::loadFromFullPath: " + aPath);
		try {
			if (fs::exists(aPath)) {
				mName = aPath;

				auto ctx = audio::Context::master(); // was audio::master(); !?!
				mSourceFile = audio::load(loadFile(aPath), audio::master()->getSampleRate());
				if (mVDAnimation->isAudioBuffered()) {
					mBufferPlayerNode = ctx->makeNode(new audio::BufferPlayerNode());
					mBufferPlayerNode->loadBuffer(mSourceFile);
					mWaveformPlot.load(mBufferPlayerNode->getBuffer(), mFbo->getBounds());
					mBufferPlayerNode->start();
					mBufferPlayerNode >> mMonitorWaveSpectralNode >> ctx->getOutput();
					ctx->enable();
				}
				else {
					mSamplePlayerNode = ctx->makeNode(new audio::FilePlayerNode(mSourceFile, false));
					mSamplePlayerNode->setLoopEnabled(false);
					mSamplePlayerNode >> mMonitorWaveSpectralNode >> ctx->getOutput();
					mSamplePlayerNode->enable();

					mSamplePlayerNode->seek(0);
					auto filePlayer = dynamic_pointer_cast<audio::FilePlayerNode>(mSamplePlayerNode);
					CI_ASSERT_MSG(filePlayer, "expected sample player to be either BufferPlayerNode or FilePlayerNode");
					// in case another wave is playing

					filePlayer->setSourceFile(mSourceFile);

					mSamplePlayerNode->start();

				}

				mVDAnimation->setUseLineIn(false);
			}
		}
		catch (...) {
			CI_LOG_W("could not open wavefile");
		}
		return true;
	}

	ci::gl::Texture2dRef TextureAudio::getTexture() {

		auto fmt = gl::Texture2d::Format().swizzleMask(GL_RED, GL_RED, GL_RED, GL_ONE).internalFormat(GL_RED);
		if (!mLineInInitialized) {
			CI_LOG_V("TextureAudio::getTexture() init");
			auto ctx = audio::Context::master();
#if (defined( CINDER_MSW ) || defined( CINDER_MAC ))
			if (mVDAnimation->getUseLineIn()) {
				// linein
				mVDAnimation->preventLineInCrash(); // at next launch
				CI_LOG_W("trying to open mic/line in, if no line follows in the log, the app crashed so put UseLineIn to false in the VDSettings.xml file");
				mLineIn = ctx->createInputDeviceNode(); //crashes if linein is present but disabled, doesn't go to catch block
				CI_LOG_V("mic/line in opened");
				mVDAnimation->saveLineIn();
				mName = mLineIn->getDevice()->getName();
				auto scopeLineInFmt = audio::MonitorSpectralNode::Format().fftSize(mVDAnimation->mWindowSize * 2).windowSize(mVDAnimation->mWindowSize);// CHECK is * 2 needed
				mMonitorLineInSpectralNode = ctx->makeNode(new audio::MonitorSpectralNode(scopeLineInFmt));
				mLineIn >> mMonitorLineInSpectralNode;
				mLineIn->enable();
				mLineInInitialized = true;
			}
#endif
			if (mVDAnimation->getUseAudio()) {
				// also initialize wave monitor
				auto scopeWaveFmt = audio::MonitorSpectralNode::Format().fftSize(mVDAnimation->mWindowSize * 2).windowSize(mVDAnimation->mWindowSize);
				mMonitorWaveSpectralNode = ctx->makeNode(new audio::MonitorSpectralNode(scopeWaveFmt));

				ctx->enable();
			}
		}
#if (defined( CINDER_MSW ) || defined( CINDER_MAC ))
		if (mVDAnimation->getUseLineIn()) {
			mMagSpectrum = mMonitorLineInSpectralNode->getMagSpectrum();
		}
		else {
#endif
			if (mVDAnimation->getUseAudio()) {
				if (mVDAnimation->isAudioBuffered()) {
					if (mBufferPlayerNode) {
						mMagSpectrum = mMonitorWaveSpectralNode->getMagSpectrum();
					}
				}
				else {
					if (mSamplePlayerNode) {
						mMagSpectrum = mMonitorWaveSpectralNode->getMagSpectrum();
						mPosition = mSamplePlayerNode->getReadPosition();
					}
				}
			}
#if (defined( CINDER_MSW ) || defined( CINDER_MAC ))
		}
#endif
		if (!mMagSpectrum.empty()) {

			mVDAnimation->maxVolume = 0.0f;//mIntensity
			size_t mDataSize = mMagSpectrum.size();
			if (mDataSize > 0 && mDataSize < mVDAnimation->mWindowSize + 1) {
				float db;
				unsigned char signal[kBands];
				for (size_t i = 0; i < mDataSize; i++) {
					float f = mMagSpectrum[i];
					db = audio::linearToDecibel(f);
					f = db * mVDAnimation->getFloatUniformValueByName("iAudioMult");
					if (f > mVDAnimation->maxVolume)
					{
						mVDAnimation->maxVolume = f;
					}
					mVDAnimation->iFreqs[i] = f;
					// update iFreq uniforms 
					if (i == mVDAnimation->getFreqIndex(0)) mVDAnimation->setFloatUniformValueByName("iFreq0", f); // mVDSettings->IFREQ0
					if (i == mVDAnimation->getFreqIndex(1)) mVDAnimation->setFloatUniformValueByName("iFreq1", f);
					if (i == mVDAnimation->getFreqIndex(2)) mVDAnimation->setFloatUniformValueByName("iFreq2", f);
					if (i == mVDAnimation->getFreqIndex(3)) mVDAnimation->setFloatUniformValueByName("iFreq3", f);

					if (i < 16) {
						int ger = f;
						signal[i] = static_cast<unsigned char>(ger);
					}
				}
				// store it as a 512x2 texture
				mTexture = gl::Texture::create(signal, GL_RED, 64, 2, fmt);
				if (mVDAnimation->isAudioBuffered() && mBufferPlayerNode) {
					gl::ScopedFramebuffer fbScp(mFbo);
					gl::clear(Color::black());
					
					mTexture->bind(0);

					mWaveformPlot.draw();
					// draw the current play position
					mPosition = mBufferPlayerNode->getReadPosition();
					float readPos = (float)mWidth * mPosition / mBufferPlayerNode->getNumFrames();
					gl::color(ColorA(0, 1, 0, 0.7f));
					gl::drawSolidRect(Rectf(readPos - 2, 0, readPos + 2, (float)mHeight));
					mRenderedTexture = mFbo->getColorTexture();
					return mRenderedTexture;					
				}
			}
		}
		else {
			// generate random values
			for (int i = 0; i < 128; ++i) dTexture[i] = (unsigned char)(Rand::randUint() & 0xFF);
			mTexture = gl::Texture::create(dTexture, GL_RED, 64, 2, fmt);
		}

		return mTexture;
	}
	TextureAudio::~TextureAudio(void) {
	}
	/*
	** ---- TextureStream ------------------------------------------------
	*/
	TextureStream::TextureStream(VDAnimationRef aVDAnimation) {
		mVDAnimation = aVDAnimation;
		mType = STREAM;
		mName = "stream";
		mTexture = gl::Texture::create(mWidth, mHeight);
	}
	XmlTree	TextureStream::toXml() const {
		XmlTree xml = VDTexture::toXml();

		// add attributes specific to this type of texture
		xml.setAttribute("path", mPath);
		xml.setAttribute("flipv", mFlipV);
		xml.setAttribute("fliph", mFlipH);

		return xml;
	}

	bool TextureStream::fromXml(const XmlTree &xml)
	{
		VDTexture::fromXml(xml);
		// retrieve attributes specific to this type of texture
		mFlipV = xml.getAttributeValue<bool>("flipv", "false");
		mFlipH = xml.getAttributeValue<bool>("fliph", "false");
		mTexture = gl::Texture::create(mWidth, mHeight);
		return true;
	}
	bool TextureStream::loadFromFullPath(string aStream)
	{
		bool rtn = false;
		size_t len;
		size_t comma = aStream.find(",");
		if (comma == string::npos) {
			CI_LOG_W("comma not found");
		}
		else {
			len = aStream.size() - comma - 1;
			auto buf = make_shared<Buffer>(fromBase64(&aStream[comma + 1], len));
			try {
				shared_ptr<Surface8u> result(new Surface8u(ci::loadImage(DataSourceBuffer::create(buf), ImageSource::Options(), "jpeg")));
				mTexture = gl::Texture2d::create(*result, ci::gl::Texture::Format());
				rtn = true;
			}
			catch (std::exception &exc) {
				CI_LOG_W("failed to parse streamed data image, what: " << exc.what());
			}
		}
		return rtn;
	}

	ci::gl::Texture2dRef TextureStream::getTexture() {
		return mTexture;
	}
	TextureStream::~TextureStream(void) {
	}
} // namespace videodromm
