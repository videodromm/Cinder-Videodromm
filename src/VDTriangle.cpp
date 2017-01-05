
#include "VDTriangle.h"

namespace VideoDromm {

	VDTriangle::VDTriangle()
	{
		mWindowSize = vec2(float(mWidth), float(mHeight));
		mGlsl = gl::GlslProg::create(loadAsset("passthru.vert"), loadAsset("live.frag"));
		const int r = 400;

		vec2 pt1(0.0, 0.0);
		vec2 pt2(r, 0.0);
		vec2 pt3((cos(M_PI / 3) * r), (sin(M_PI / 3) * r));

		mVertices[0] = pt1;
		mVertices[1] = pt2;
		mVertices[2] = pt3;
	}

	void VDTriangle::draw(const gl::Texture2dRef &texture)
	{
		draw(texture, texture->getBounds(), Rectf(getBounds()));
	}

	void VDTriangle::draw(const gl::Texture2dRef &texture, const Area &srcArea)
	{
		draw(texture, srcArea, Rectf(getBounds()));
	}
	void VDTriangle::draw(const gl::Texture2dRef &texture, const Area &srcArea, const Rectf &destRect)
	{
		/*gl::ScopedTextureBind tex0(texture);

		// clip against bounds
		Area  area = srcArea;
		Rectf rect = destRect;
		clip(area, rect);

		// set texture coordinates
		float w = static_cast<float>(texture->getWidth());
		float h = static_cast<float>(texture->getHeight());

		if (texture->getTarget() == GL_TEXTURE_RECTANGLE_ARB)
			setTexCoords((float)area.x1, (float)area.y1, (float)area.x2, (float)area.y2);
		else
			setTexCoords(area.x1 / w, area.y1 / h, area.x2 / w, area.y2 / h);

		// draw
		draw();*/

	}
	void VDTriangle::draw()
	{
		gl::ScopedGlslProg glslScp(mGlsl);
		//gl::ScopedTextureBind texScp(mTex);
		mGlsl->uniform("iGlobalTime", (float)getElapsedSeconds());
		mGlsl->uniform("iResolution", vec3(640, 480, 1.0));// TODO remove hardcoded

		gl::drawSolidTriangle(mVertices);
	}
	bool VDTriangle::clip(Area &srcArea, Rectf &destRect) const
	{
		bool clipped = false;

		float x1 = destRect.x1 / mWidth;
		float x2 = destRect.x2 / mWidth;
		float y1 = destRect.y1 / mHeight;
		float y2 = destRect.y2 / mHeight;

		if (x1 < 0.0f) {
			destRect.x1 = 0.0f;
			srcArea.x1 -= static_cast<int32_t>(x1 * srcArea.getWidth());
			clipped = true;
		}
		else if (x1 > 1.0f) {
			destRect.x1 = static_cast<float>(mWidth);
			srcArea.x1 -= static_cast<int32_t>((1.0f / x1) * srcArea.getWidth());
			clipped = true;
		}

		if (x2 < 0.0f) {
			destRect.x2 = 0.0f;
			srcArea.x2 -= static_cast<int32_t>(x2 * srcArea.getWidth());
			clipped = true;
		}
		else if (x2 > 1.0f) {
			destRect.x2 = static_cast<float>(mWidth);
			srcArea.x2 -= static_cast<int32_t>((1.0f / x2) * srcArea.getWidth());
			clipped = true;
		}

		if (y1 < 0.0f) {
			destRect.y1 = 0.0f;
			srcArea.y1 -= static_cast<int32_t>(y1 * srcArea.getHeight());
			clipped = true;
		}
		else if (y1 > 1.0f) {
			destRect.y1 = static_cast<float>(mHeight);
			srcArea.y1 -= static_cast<int32_t>((1.0f / y1) * srcArea.getHeight());
			clipped = true;
		}

		if (y2 < 0.0f) {
			destRect.y2 = 0.0f;
			srcArea.y2 -= static_cast<int32_t>(y2 * srcArea.getHeight());
			clipped = true;
		}
		else if (y2 > 1.0f) {
			destRect.y2 = static_cast<float>(mHeight);
			srcArea.y2 -= static_cast<int32_t>((1.0f / y2) * srcArea.getHeight());
			clipped = true;
		}

		return clipped;
	}

	//! to json
	JsonTree	VDTriangle::toJson() const
	{
		JsonTree		json;
		JsonTree VDTriangle = JsonTree::makeArray("VDTriangle");

		VDTriangle.addChild(ci::JsonTree("afboindex", mAFboIndex));
		VDTriangle.addChild(ci::JsonTree("bfboindex", mBFboIndex));
		VDTriangle.addChild(ci::JsonTree("ashaderindex", mAShaderIndex));
		VDTriangle.addChild(ci::JsonTree("bshaderindex", mBShaderIndex));
		VDTriangle.addChild(ci::JsonTree("ashaderfilename", mAShaderFilename));
		VDTriangle.addChild(ci::JsonTree("bshaderfilename", mBShaderFilename));
		VDTriangle.addChild(ci::JsonTree("mixfboindex", mMixFboIndex));
		VDTriangle.addChild(ci::JsonTree("crossfade", ABCrossfade));
		VDTriangle.addChild(ci::JsonTree("active", mActive));

		// add <controlpoint> tags (column-major)
		JsonTree	cps = JsonTree::makeArray("controlpoints");
		std::vector<vec2>::const_iterator itr;
		unsigned int i = 0;
		for (itr = mPoints.begin(); itr != mPoints.end(); ++itr) {
			JsonTree	cp;
			cp.addChild(ci::JsonTree("controlpoint", i));
			cp.addChild(ci::JsonTree("x", (*itr).x));
			cp.addChild(ci::JsonTree("y", (*itr).y));
			cps.pushBack(cp);
			i++;
		}
		VDTriangle.pushBack(cps);
		json.pushBack(VDTriangle);
		return json;
	}
	//! from json
	void VDTriangle::fromJson(const JsonTree &json)
	{
		if (json.hasChild("VDTriangle")) {
			JsonTree VDTriangle(json.getChild("VDTriangle"));
			mAFboIndex = (VDTriangle.hasChild("afboindex")) ? VDTriangle.getValueForKey<int>("afboindex") : 1;
			mBFboIndex = (VDTriangle.hasChild("bfboindex")) ? VDTriangle.getValueForKey<int>("bfboindex") : 2;
			mAShaderIndex = (VDTriangle.hasChild("ashaderindex")) ? VDTriangle.getValueForKey<int>("ashaderindex") : 1;
			mBShaderIndex = (VDTriangle.hasChild("bshaderindex")) ? VDTriangle.getValueForKey<int>("bshaderindex") : 2;
			mAShaderFilename = (VDTriangle.hasChild("ashaderfilename")) ? VDTriangle.getValueForKey<std::string>("ashaderfilename") : "0.frag";
			mBShaderFilename = (VDTriangle.hasChild("bshaderfilename")) ? VDTriangle.getValueForKey<std::string>("bshaderfilename") : "0.frag";
			mMixFboIndex = (VDTriangle.hasChild("mixfboindex")) ? VDTriangle.getValueForKey<int>("mixfboindex") : 0;
			ABCrossfade = (VDTriangle.hasChild("crossfade")) ? VDTriangle.getValueForKey<float>("crossfade") : 1.0f;
			mActive = (VDTriangle.hasChild("active")) ? VDTriangle.getValueForKey<float>("active") : true;
			mDeleted = false; // if we load it, we don't delete it!
			// load control points
			mPoints.clear();
			JsonTree cps(VDTriangle.getChild("controlpoints"));
			for (size_t i = 0; i < cps.getNumChildren(); i++) {
				JsonTree child = cps.getChild(i);
				float x = (child.hasChild("x")) ? child.getValueForKey<float>("x") : 0.0f;
				float y = (child.hasChild("y")) ? child.getValueForKey<float>("y") : 0.0f;
				CI_LOG_V("controlpoint:" + toString(x) + " " + toString(y));

				mPoints.push_back(vec2(x, y));
			}
		}
	}
	void VDTriangle::setSize(int w, int h)
	{
		mWidth = w;
		mHeight = h;
	}

	void VDTriangle::setSize(const VDTriangleList &VDTriangles, int w, int h)
	{
		for (VDTriangleConstIter itr = VDTriangles.begin(); itr != VDTriangles.end(); ++itr)
			(*itr)->setSize(w, h);
	}

	VDTriangleList VDTriangle::load(const DataSourceRef &source)
	{
		VDTriangleList	VDTriangles;
		JsonTree json(source);

		// try to load the specified json file
		if (json.hasChild("triangles")) {
			JsonTree ws(json.getChild("triangles"));

			// iterate VDTriangles
			for (size_t i = 0; i < ws.getNumChildren(); i++) {
				JsonTree child(ws.getChild(i));

				if (child.hasChild("triangle")) {
					JsonTree w(child.getChild("triangle"));
					// create VDTriangle of the correct type
					if (child.hasChild("afboindex")) {
						// TODO
					}
					VDTriangle t();
					//t->fromJson(child);
					//VDTriangles.push_back(t);
				}


			}

		}
		return VDTriangles;
	}

	void VDTriangle::save(const VDTriangleList &VDTriangles, const DataTargetRef &target)
	{
		JsonTree		json;
		// create VDTriangles json
		JsonTree VDTrianglesJson = JsonTree::makeArray("triangles");
		//VDTrianglesJson.addChild(ci::JsonTree("VDTriangles", "unknown"));
		// 
		for (unsigned i = 0; i < VDTriangles.size(); ++i) {
			if (!VDTriangles[i]->isDeleted()) {
				// create VDTriangle
				JsonTree	VDTriangle(VDTriangles[i]->toJson());
				VDTriangle.addChild(ci::JsonTree("id", i + 1));

				// create <VDTriangle>
				VDTrianglesJson.pushBack(VDTriangle);
			}
		}
		// write file
		json.pushBack(VDTrianglesJson);
		json.write(target);
	}

	bool VDTriangle::handleResize(VDTriangleList &VDTriangles)
	{
		for (VDTriangleIter itr = VDTriangles.begin(); itr != VDTriangles.end(); ++itr)
			(*itr)->resize();

		return false;
	}

	bool VDTriangle::handleResize(VDTriangleList &VDTriangles, const ivec2 &size)
	{
		for (VDTriangleIter itr = VDTriangles.begin(); itr != VDTriangles.end(); ++itr)
			(*itr)->resize(size);

		return false;
	}

	void VDTriangle::resize()
	{
		resize(getWindowSize());
	}

	void VDTriangle::resize(const ivec2 &size)
	{
		mWindowSize = vec2(size);
	}

} // namespace VideoDromm