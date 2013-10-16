#include "LensFlare.h"
#include "Game.h"
#include "Camera.h"

using namespace ci;

const static int kNumSprites = 9;

LensFlare::LensFlare()
{
	mLightPosition = Vec3f::zero();
	RenderingEngine::get()->addNode( &mRoot );
}

LensFlare::~LensFlare()
{
	while( !mSprites.empty() ) {
		delete *mSprites.begin();
		mSprites.erase( mSprites.begin() );
	}
}

void LensFlare::addSprite( Texture* texture, const ci::ColorA color, const ci::Vec2i size )
{
	Node2d* sprite = new Node2d();
	sprite->getNode()->mLayer = Node::LayerLighting;
	sprite->setTexture( texture );
	sprite->size = size / 2.0f;
	sprite->setColor( color );
	sprite->anchor = Vec2f( 0.5f, 0.5f );
	mRoot.addChild( sprite );
	mSprites.push_back( sprite );
}

void LensFlare::debugDraw()
{
}

Node* LensFlare::rayCast( const ci::Ray& ray )
{
	for( auto iter = RenderingEngine::get()->getObjectNodes().begin(); iter != RenderingEngine::get()->getObjectNodes().end(); iter++ ) {
		Node* node = *iter;
		if ( node->mLayer == Node::LayerLighting || node->mLayer == Node::LayerNone ) continue;
		if ( node->getMesh() == NULL ) continue;
		
		if( node->getBoundingBox().intersects( ray ) ) {
			float distance = 0.0f;
			int len = node->getMesh()->triMesh.getNumTriangles();
			for( size_t i = 0; i < len; i++ ) {
				Vec3f v0, v1, v2;
				node->getMesh()->triMesh.getTriangleVertices(i, &v0, &v1, &v2);
				v0 = node->getTransform().transformPointAffine(v0);
				v1 = node->getTransform().transformPointAffine(v1);
				v2 = node->getTransform().transformPointAffine(v2);
				if( ray.calcTriangleIntersection(v0, v1, v2, &distance) ) {
					return node;
				}
			}
		}
	}
	return NULL;
}

float angle = 0.0f;

void LensFlare::update( const float deltaTime )
{
	Camera* camera = Camera::get();
	
	const Vec2i start = camera->getWorldToScreen( mLightPosition );
	
	if ( !camera->getScreenRect().contains( start ) ) {
		mRoot.hide();
	}
	else {
		const Vec2i center = camera->getScreenCenter();
		const Vec2i dir = start - center;
		const Vec2i end = center - dir;
		angle += 0.02f;
		Vec3f destination = camera->getGlobalPosition();
		Vec3f origin = mLightPosition;
		mRay = Ray( origin, destination - origin );
		bool rayHit = rayCast( mRay );
		if ( !rayHit ) {
			int i = -1;
			int total = mSprites.size();
			for( auto iter = mSprites.begin(); iter != mSprites.end(); iter++ ) {
				float r = (float) i++ / (float) total;
				(*iter)->position = start + r * ( end - start );
				(*iter)->update();
			}
			mRoot.show();
		}
		else {
			mRoot.hide();
		}
	}
}