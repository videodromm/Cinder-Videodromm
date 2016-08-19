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
	typedef std::shared_ptr<class VDFbo> 	VDFboRef;
	typedef std::vector<VDFboRef>			VDFboList;

	class VDFbo : public VDTexture{
	public:
		VDFbo(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDTextureList aTextureList);
		~VDFbo(void);
		static VDFboRef create(VDSettingsRef aVDSettings, VDAnimationRef aVDAnimation, VDTextureList aTextureList) {
			return std::make_shared<VDFbo>(aVDSettings, aVDAnimation, aTextureList);
		}
		//! returns a shared pointer to this fbo
		VDFboRef						getPtr() { return std::static_pointer_cast<VDFbo>(shared_from_this()); }
		ci::ivec2						getSize();
		ci::Area						getBounds();
		GLuint							getId();
		//! returns the type
		TextureType						getType() { return mType; };
		std::string						getName();
		std::string						getLabel();
		void							setLabel(string aLabel) { mFboTextureShader->setLabel(aLabel); };
		//bool							isFlipH() { return mFlipH; };
		//bool							isFlipV() { return mFlipV; };
		//!
		void							fromXml(const ci::XmlTree &xml);
		//!
		XmlTree							toXml() const;
		// move, rotate, zoom methods
		void							setPosition(int x, int y);
		void							setZoom(float aZoom);
		// shader
		void							setShaderIndex(unsigned int aShaderIndex);
		unsigned int					getShaderIndex() { return mShaderIndex; };
		void							setFragmentShader(string aFragmentShaderString, string aName);
		// textures
		void							setInputTexture(unsigned int aTextureIndex);
		unsigned int					getInputTextureIndex() { return inputTextureIndex; };
		ci::gl::Texture2dRef			getTexture();
		gl::GlslProgRef					getShader() { return mFboTextureShader; };
	protected:
		std::string						mFboName;
		//bool							mFlipV;
		//bool							mFlipH;
		TextureType						mType;
		std::string						mFilePathOrText;
		//bool							mTopDown;
		float							mPosX;
		float							mPosY;
		float							mZoom;
		//! default vertex shader
		std::string						mPassthruVextexShaderString;
		//! default fragment shader
		std::string						mFboTextureFragmentShaderString;
		//! passthru shader
		gl::GlslProgRef					mFboTextureShader;

		string							mError;
		// uniforms
		vec3							iChannelResolution0;
	private:
		// Animation
		VDAnimationRef					mVDAnimation;
		// Settings
		VDSettingsRef					mVDSettings;

		//! Fbo
		gl::FboRef						mFbo;
		//! Textures
		VDTextureList					mTextureList;
		unsigned int					inputTextureIndex;
		//! Shaders
		string							mShaderName;
		unsigned int					mShaderIndex;
		string							mId;
	};
}
