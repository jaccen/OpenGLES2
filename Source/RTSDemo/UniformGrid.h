#pragma once

#include <list>

#include "cinder/Vector.h"

#include "Unit.h"

class UniformGrid {
public:
	class Cell {
	public:
		Cell();
		void setArea( const ci::Area& area );
		void add( Unit* unit );
		void clear();
		bool test( const ci::Vec2i& pos ) const { return mArea.contains( pos ); }
		bool test3x3( const ci::Vec2i& pos ) const { return mArea3x3.contains( pos ); }
		void findAll( const Unit::Predicate& predicate, std::list<Unit*>& output ) const;
		
		const std::list<Unit*> getChildren() const { return mChildren; }
		const ci::Area& getArea() const { return mArea; }
		const ci::Area& getArea3x3() const { return mArea3x3; }
		
		ci::ColorA mDebugColor;
		
	private:
		std::list<Unit*> mChildren;
		ci::Area mArea;
		ci::Area mArea3x3;
	};
	
	UniformGrid();
	virtual ~UniformGrid();
	
	const ci::Vec2i& getResolution() const { return mResolution; }
	const ci::Rectf& getSize() const { return mSize; }
	
	void build( const ci::Rectf& size, const ci::Vec2i& resolution );
	void clear();
	void sort( const std::vector<Unit*>& units );
	
	void findAll( const Unit::Predicate& predicate, std::list<Unit*>& output ) const;
	Unit* findNearest( const Unit::Predicate& predicate ) const;
	
	const std::list<Cell*> getCells() const { return mCells; }
	
private:
	ci::Rectf mSize;
	ci::Vec2i mResolution;
	
	std::list<Cell*> mCells;
};