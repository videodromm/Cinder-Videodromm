
#include "VDTriangle.h"

namespace VideoDromm {

	VDTriangle::VDTriangle()
	{
		mWindowSize = vec2(float(mWidth), float(mHeight));
		mGlsl = gl::GlslProg::create(loadAsset("passthru.vert"), loadAsset("live.frag"));
		const int r = 400;

		/*vec2 pt1(0.0, 0.0);
		vec2 pt2(r, 0.0);
		vec2 pt3((cos(M_PI / 3) * r), (sin(M_PI / 3) * r));

		mVertices[0] = pt1;
		mVertices[1] = pt2;
		mVertices[2] = pt3;*/
		reset();
		mDeleted = false;
	}
	void VDTriangle::reset()
	{
		mPoints.clear();
		mPoints.push_back(vec2(0.0f, 0.0f));
		mPoints.push_back(vec2(400.0f, 0.0f));
		mPoints.push_back(vec2((cos(M_PI / 3) * 400.0f), (sin(M_PI / 3) * 400.0f)));
	}
	vec2 VDTriangle::getControlPoint(unsigned index) const
	{
		if (index >= mPoints.size()) return vec2(0);
		return mPoints[index];
	}

	void VDTriangle::setControlPoint(unsigned index, const vec2 &pos)
	{
		if (index >= mPoints.size()) return;
		mPoints[index] = pos;
	}

	void VDTriangle::moveControlPoint(unsigned index, const vec2 &shift)
	{
		if (index >= mPoints.size()) return;
		mPoints[index] += shift;
	}

	void VDTriangle::selectControlPoint(unsigned index)
	{
		if (index >= mPoints.size() || index == mSelected) return;

		mSelected = index;
	}

	void VDTriangle::deselectControlPoint()
	{
		mSelected = -1; // since this is an unsigned int, actual value will be 'MAX_INTEGER'
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

		gl::drawSolidTriangle(mPoints[0], mPoints[1], mPoints[2]);
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
		try {
			// try to load the specified json file
			if (json.hasChild("triangles")) {
				JsonTree triangles(json.getChild("triangles"));

				// iterate VDTriangles
				for (size_t i = 0; i < triangles.getNumChildren(); i++) {
					JsonTree child(triangles.getChild(i));

					if (child.hasChild("triangle")) {
						JsonTree triangle(child.getChild("triangle"));
					
						VDTriangle t();
						//t->fromJson(child);
						//VDTriangles.push_back(t);
					}
				}
			}
		}
		catch (const JsonTree::ExcJsonParserError& exc) {
			CI_LOG_W(exc.what());
		}
		catch (...) {
			CI_LOG_W("VDTriangle::load error");
		}

		return VDTriangles;
	}
	//! from json
	void VDTriangle::fromJson(const JsonTree &json)
	{
		if (json.hasChild("triangle")) {
			JsonTree triangle(json.getChild("triangle"));
			mAFboIndex = (triangle.hasChild("afboindex")) ? triangle.getValueForKey<int>("afboindex") : 1;
			mBFboIndex = (triangle.hasChild("bfboindex")) ? triangle.getValueForKey<int>("bfboindex") : 2;
			mAShaderIndex = (triangle.hasChild("ashaderindex")) ? triangle.getValueForKey<int>("ashaderindex") : 1;
			mBShaderIndex = (triangle.hasChild("bshaderindex")) ? triangle.getValueForKey<int>("bshaderindex") : 2;
			mAShaderFilename = (triangle.hasChild("ashaderfilename")) ? triangle.getValueForKey<std::string>("ashaderfilename") : "0.frag";
			mBShaderFilename = (triangle.hasChild("bshaderfilename")) ? triangle.getValueForKey<std::string>("bshaderfilename") : "0.frag";
			mMixFboIndex = (triangle.hasChild("mixfboindex")) ? triangle.getValueForKey<int>("mixfboindex") : 0;
			ABCrossfade = (triangle.hasChild("crossfade")) ? triangle.getValueForKey<float>("crossfade") : 1.0f;
			mActive = (triangle.hasChild("active")) ? triangle.getValueForKey<float>("active") : true;
			mDeleted = false; // if we load it, we don't delete it!
			// load control points
			mPoints.clear();
			JsonTree cps(triangle.getChild("controlpoints"));
			for (size_t i = 0; i < cps.getNumChildren(); i++) {
				JsonTree child = cps.getChild(i);
				float x = (child.hasChild("x")) ? child.getValueForKey<float>("x") : 0.0f;
				float y = (child.hasChild("y")) ? child.getValueForKey<float>("y") : 0.0f;
				CI_LOG_V("controlpoint:" + toString(x) + " " + toString(y));

				mPoints.push_back(vec2(x, y));
			}
		}
	}
	void VDTriangle::save(const VDTriangleList &VDTriangles, const DataTargetRef &target)
	{
		try {
			CI_LOG_V("VDTriangles saving");
			JsonTree		json;
			// create VDTriangles json
			JsonTree trianglesJson = JsonTree::makeArray("triangles");

			for (unsigned i = 0; i < VDTriangles.size(); ++i) {
				if (!VDTriangles[i]->isDeleted()) {
					// create triangle
					JsonTree	triangle(VDTriangles[i]->toJson());
					//triangle.addChild(ci::JsonTree("id", i + 1));

					// create <triangle>
					// TODO fix exception: 
					trianglesJson.pushBack(triangle);
				}
			}
			// write file
			json.pushBack(trianglesJson);
			json.write(target);
			CI_LOG_V("VDTriangles saved");
		}
		catch (const JsonTree::ExcJsonParserError& exc) {
			CI_LOG_W(exc.what());
		}
		catch (...) {
			CI_LOG_W("VDTriangle::save error");
		}

	}
	//! to json
	JsonTree	VDTriangle::toJson() const
	{

		JsonTree		json;
		try {
			JsonTree triangle = JsonTree::makeArray("triangle");

			triangle.addChild(ci::JsonTree("afboindex", mAFboIndex));
			triangle.addChild(ci::JsonTree("bfboindex", mBFboIndex));
			triangle.addChild(ci::JsonTree("ashaderindex", mAShaderIndex));
			triangle.addChild(ci::JsonTree("bshaderindex", mBShaderIndex));
			triangle.addChild(ci::JsonTree("ashaderfilename", mAShaderFilename));
			/*triangle.addChild(ci::JsonTree("bshaderfilename", mBShaderFilename));
			triangle.addChild(ci::JsonTree("mixfboindex", mMixFboIndex));
			triangle.addChild(ci::JsonTree("crossfade", ABCrossfade));
			triangle.addChild(ci::JsonTree("active", mActive));

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
			triangle.pushBack(cps);*/
			//CI_LOG_V(triangle);
			json.pushBack(triangle);
			//CI_LOG_V(json);
		}
		catch (const JsonTree::ExcJsonParserError& exc) {
			CI_LOG_W(exc.what());
		}
		catch (...) {
			CI_LOG_W("VDTriangle::toJson error");
		}
		return json;
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