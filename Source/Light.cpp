#include "Light.h"

using namespace ly;

Light::ShadowProperties::ShadowProperties() :
	mapResolution( 1024 ),
	fov( 30.0f ),
	nearDist( 1.0f ),
	farDist( 100.0f ),
	softening( 0.3f ),
	strength( 1.0f ),
	enabled( false ) {}

ly::Light::Light( Light::Type type ) : mType( type ), intensity( 1.0f )
{
	color = ci::ColorA::white();
	mPrevShadowPropertiesEnabled = false;
}

ly::Light::~Light() {}

void ly::Light::update( const float deltaTime )
{
	Node::update( deltaTime );
	
	/*if ( shadowProperties.enabled ) {
		// Detecting change to do initialization
		if ( !mPrevShadowPropertiesEnabled && !mDepthMap ) {
			mDepthMap = ly::Renderer::get()->createDepthMap( shadowProperties.mapResolution, shadowProperties.mapResolution );
		}
		mShadowCamera.setPerspective( shadowProperties.fov, 1.0f, shadowProperties.nearDist, shadowProperties.farDist );
		mShadowCamera.setEyePoint( globalPosition() );
		mShadowCamera.setWorldUp( Vec3f::yAxis() );
		mShadowCamera.setCenterOfInterestPoint( mTarget );
	}
	else {
		if ( mPrevShadowPropertiesEnabled ) {
			mDepthMap.reset();
		}
	}*/
	
	mPrevShadowPropertiesEnabled = shadowProperties.enabled;
}

void Light::updateScreenEffects( const Camera* camera )
{
	/*if ( mSceneParent == NULL ) return;
	
	// Update lens flares by getting world to screen coordinates and staggering (if enabled)
	if ( !mLensFlareSprites.empty() ) {
		std::vector<Light::LensFlareSprite>::iterator iter;
		float dot = ( globalPosition() - camera->globalPosition() ).normalized().dot( camera->bodyTransform().transformVec( -Vec3f::zAxis() ) );
		float distanceFromCamera = camera->globalPosition().distance( globalPosition() );
		if ( distanceFromCamera < 0.3f || dot <= 0.0f ) {
			for( iter = mLensFlareSprites.begin(); iter != mLensFlareSprites.end(); iter++ ) {
				iter->visible = false;
			}
		}
		else {
			Vec2f lightPos = camera->getCinderCamera().worldToScreen( globalPosition(), app::getWindowWidth(), app::getWindowHeight() );
			for( iter = mLensFlareSprites.begin(); iter != mLensFlareSprites.end(); iter++ ) {
				iter->screenPosition = lightPos;
				Ray ray = camera->rayIntoScene( lightPos );
				iter->visible = mSceneParent->testIntersection( ray, distanceFromCamera ) == NULL;
			}
		}
	}*/
}

void Light::setTarget( ci::Vec3f target )
{
	mTarget = target;
}

ci::Matrix44f Light::getShadowTransformationMatrix( const ci::Camera &camera ) const
{
	ci::Matrix44f shadowTransMatrix = mShadowCamera.getProjectionMatrix();
	shadowTransMatrix *= mShadowCamera.getModelViewMatrix();
	shadowTransMatrix *= camera.getInverseModelViewMatrix();
	return shadowTransMatrix;
}