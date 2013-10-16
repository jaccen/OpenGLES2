#include "Projectile.h"
#include "RenderingEngine.h"
#include "ResourceManager.h"
#include "GameConstants.h"
#include "Unit.h"

#include "cinder/Quaternion.h"

using namespace ci;

Projectile::Projectile( Unit* owner ) : mSpeed( 1.0f ), mLifetime( 4.0f ), mElapsedTime(0.0f ), mOwner( owner )
{
	//mContainer = new Node();
	//mContainer->mLayer = Node::LayerLighting;
	mBody = new Node();
	//mBody->setParent( mContainer );
	
	//RenderingEngine::get()->addSpriteNode( mContainer );
	RenderingEngine::get()->addSpriteNode( mBody );
	
	mBody->mLayer = Node::LayerLighting;
	mBody->mFaceCamera = true;
	mBody->rotation.x = 90.0f;
	mBody->rotation.y = 90.0f;
	mBody->setMesh( ResourceManager::get()->getMesh( "models/quad_plane.obj" ) );
	mBody->scale = Vec3f::one() * 15.0f;
	mBody->mMaterial.mShader = ResourceManager::get()->getShader( "unlit" );
	mBody->mMaterial.setProperty( "Scale", Vec2f::one() );
	mBody->mMaterial.setTexture( "DiffuseTexture", ResourceManager::get()->getTexture( "textures/projectile.png" ) );
	mBody->mMaterial.setColor( "DiffuseMaterial", ColorA::white() );
		
	mForward = Vec3f::zAxis();
	
	//mContainer->update();
}

Projectile::~Projectile()
{
	//RenderingEngine::get()->removeNode( mContainer );
	RenderingEngine::get()->removeNode( mBody );
}

void Projectile::setDirection( const ci::Vec3f direction )
{
	mForward = direction;
	/*Quatf q( Matrix44f::alignZAxisWithTarget( mForward, Vec3f::yAxis() ) );
	Vec3f axis;
	float angle;
	q.getAxisAngle( &axis, &angle );
	getNode()->rotation = axis * angle * kToDeg;
	getNode()->update();*/
}

void Projectile::update( const float deltaTime )
{
	mElapsedTime += deltaTime;
	getNode()->position += mForward * mSpeed * deltaTime;
}