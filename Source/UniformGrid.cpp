#include "UniformGrid.h"
#include "GameConstants.h"

using namespace ci;

UniformGrid::Cell::Cell()
{
	mDebugColor = ColorA( randFloat(), randFloat(), randFloat(), 1.0f );
}

void UniformGrid::Cell::setArea(const ci::Area &area)
{
	mArea = area;
	mArea3x3.x1 = mArea.x1 - mArea.getWidth();
	mArea3x3.y1 = mArea.y1 - mArea.getHeight();
	mArea3x3.x2 = mArea.x2 + mArea.getWidth();
	mArea3x3.y2 = mArea.y2 + mArea.getHeight();
}

void UniformGrid::Cell::add( Unit* unit )
{
	mChildren.push_back( unit );
}

void UniformGrid::Cell::clear()
{
	mChildren.clear();
}

void UniformGrid::Cell::findAll( const Unit::Predicate& predicate, std::list<Unit*>& output ) const
{
	for( auto child : mChildren ) {
		if ( predicate( child ) ) {
			output.push_back( child );
		}
	}
}

UniformGrid::UniformGrid() {}

UniformGrid::~UniformGrid()
{
	clear();
}

void UniformGrid::clear()
{
	for( auto c : mCells ) {
		delete c;
	}
}

void UniformGrid::build( const ci::Rectf& size, const ci::Vec2i& resolution )
{
	clear();
	
	mSize = size;
	mResolution = resolution;
	
	Vec2f cellSize = Vec2f( (float)mSize.getWidth() / (float)mResolution.x, (float)mSize.getHeight() / (float)mResolution.y );
	for( int y = 0; y < mResolution.y; y++ ) {
		for( int x = 0; x < mResolution.x; x++ ) {
			Cell* cell = new Cell();
			float cx = mSize.x1 + x * cellSize.x;
			float cy = mSize.y1 + y * cellSize.y;
			cell->setArea( ci::Area( cx, cy, cx + cellSize.x, cy + cellSize.y ) );
			mCells.push_back( cell );
		}
	}
}

void UniformGrid::sort( const std::vector<Unit*>& units )
{
	for( auto cell : mCells ) {
		cell->clear();
		for( auto unit : units ) {
			if ( cell->test( unit->getNode()->position.xz() ) ) {
				cell->add( unit );
			}
		}
	}
}

Unit* UniformGrid::findNearest( const Unit::Predicate& predicate ) const
{
	std::list<Unit*> results;
	for( auto cell : mCells ) {
		if ( cell->test3x3( predicate.getUnit()->getNode()->position.xz() ) ) {
			cell->findAll( predicate, results );
		}
	}
	
	Unit* nearestUnit = NULL;
	float nearestDistance = MAXFLOAT;
	for( auto unit : results ) {
		const float distance = unit->getNode()->position.xz().distance( predicate.getUnit()->getNode()->position.xz() );
		if ( distance < nearestDistance ) {
			nearestDistance = distance;
			nearestUnit = unit;
		}
	}
	return nearestUnit;
}

void UniformGrid::findAll( const Unit::Predicate& predicate, std::list<Unit*>& output ) const
{
	for( auto cell : mCells ) {
		if ( cell->test3x3( predicate.getUnit()->getNode()->position.xz() ) ) {
			cell->findAll( predicate, output );
		}
	}
}