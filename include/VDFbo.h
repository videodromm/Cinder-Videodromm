#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Xml.h"
#include "cinder/Json.h"
#include "cinder/Capture.h"
#include "cinder/Log.h"
#include "cinder/Timeline.h"

// Settings
#include "VDSettings.h"
// Animation
#include "VDAnimation.h"
// textures
#include "VDTexture.h"
// shaders
#include "VDShader.h"

#include <atomic>
#include <vector>

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace VideoDromm;

namespace VideoDromm
{
	// stores the pointer to the VDFbo instance
	typedef std::shared_ptr<class VDFbo> 			VDFboRef;
	typedef std::vector<VDFboRef>					VDFboList;

	class VDFbo : public VDTexture{
	public:
		VDFbo(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation);
		~VDFbo(void);
		static VDFboRef create(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation) {
			return std::make_shared<VDFbo>(aVDSettings, aVDAnimation);
		}
		//! returns a shared pointer to this fbo
		VDFboRef						getPtr() { return std::static_pointer_cast<VDFbo>(shared_from_this()); }
		ci::ivec2						getSize();
		ci::Area						getBounds();
		GLuint							getId();
		//! returns the type
		TextureType						getType() { return mType; };
		std::string						getName();
		std::string						getShaderName();
		void							setLabel(string aLabel) { mFboTextureShader->setLabel(aLabel); };
		bool							isFlipH() { return mFlipH; };
		bool							isFlipV() { return mFlipV; };
		void							flipV() { mFlipV = !mFlipV; };
		//!
		bool							fromXml(const ci::XmlTree &xml);
		//!
		XmlTree							toXml() const;
		// move, rotate, zoom methods
		void							setPosition(int x, int y);
		void							setZoom(float aZoom);
		// shader
		void							setShaderIndex(unsigned int aShaderIndex);
		unsigned int					getShaderIndex() { return mShaderIndex; };
		void							setFragmentShader(unsigned int aShaderIndex, string aFragmentShaderString, string aName);
		// textures
		void							setInputTexture(VDTextureList aTextureList, unsigned int aTextureIndex = 0);
		unsigned int					getInputTextureIndex() { return mInputTextureIndex; };
		ci::gl::Texture2dRef			getFboTexture();
		void							updateThumbFile();
		gl::GlslProgRef					getShader();
		ci::gl::Texture2dRef			getRenderedTexture();

	protected:
		std::string						mFboName;
		bool							mFlipV;
		bool							mFlipH;
		TextureType						mType;
		std::string						mFilePathOrText;
		//bool							mTopDown;
		float							mPosX;
		float							mPosY;
		float							mZoom;
		//! default fragment shader
		std::string						mFboTextureFragmentShaderString;
		//! shader
		gl::GlslProgRef					mFboTextureShader;

		string							mError;
	private:
		// Settings
		VDSettingsRef					mVDSettings;
		// Animation
		VDAnimationRef					mVDAnimation;

		//! Fbo
		gl::FboRef						mFbo;
		gl::Texture::Format				fmt;
		gl::Fbo::Format					fboFmt;
		//! Input textures
		//map<int, ci::gl::Texture2dRef>	mInputTextures;
		VDTextureList					mTextureList;
		unsigned int					mInputTextureIndex;
		//! Shaders
		string							mShaderName;
		unsigned int					mShaderIndex;
		string							mId;
		// Output texture
		ci::gl::Texture2dRef			mRenderedTexture;
		bool							isReady;
	};
}
