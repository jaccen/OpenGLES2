#pragma once

#include "Node.h"
#include "Camera.h"

#include <list>

#include "cinder/Camera.h"
#include "cinder/Color.h"

namespace ly {
	
class Scene;
	
class Light : public Node {
public:
	typedef enum {
		Direct, Omni, Spot, Ambient
	} Type;
	
	struct ShadowProperties {
		ShadowProperties();
		int		mapResolution;
		float	fov;
		float	nearDist;
		float	farDist;
		float	softening;
		bool	enabled;
		float	strength;
	};
	
	class LensFlareSprite {
	public:
		Texture* texture;
		ci::Vec2f screenPosition;
		bool visible;
	};
	
	Light( Type type );
	virtual ~Light();
	
	ci::ColorA				color;
	float					intensity;
	ShadowProperties		shadowProperties;
	
	const ci::ColorA		getColor() const { return color * intensity; }
	const Type				getType() const { return mType; }
	const ci::CameraPersp&	getShadowCamera() const { return mShadowCamera; }
	
	/** Ripped from Cinder source for now, maybe re-integrate with ci::Light */
	ci::Matrix44f			getShadowTransformationMatrix( const ci::Camera &camera ) const;
	
	std::vector<LensFlareSprite> mLensFlareSprites;
	void					updateScreenEffects( const Camera* camera );
	
	void					setTarget( ci::Vec3f target );
	virtual void			update( const float deltaTime );
	//ci::gl::Fbo&			getDepthMap() { return mDepthMap; }
	
private:
	ci::Vec3f				mTarget;
	bool					mPrevShadowPropertiesEnabled;
	//ci::gl::Fbo				mDepthMap;
	
	Type					mType;
	Scene*					mSceneRef;
	ci::CameraPersp			mShadowCamera;
};

}