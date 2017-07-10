#include "provided.h"
#include "MyMap.h"
#include <utility>
#include <vector>
using namespace std;

class SegmentMapperImpl
{
public:
	SegmentMapperImpl();
	~SegmentMapperImpl();
	void init(const MapLoader& ml);
	vector<StreetSegment> getSegments(const GeoCoord& gc) const;
private:
    MyMap<GeoCoord, vector<StreetSegment>> segmentMap;
};

SegmentMapperImpl::SegmentMapperImpl()
{
}

SegmentMapperImpl::~SegmentMapperImpl()
{
}

void SegmentMapperImpl::init(const MapLoader& ml)
{
    StreetSegment seg;
    for (int i = 0; i < ml.getNumSegments(); i++)
    {
        ml.getSegment(i, seg);
        
        GeoCoord coord = seg.segment.start;
        
        // find all segments related to start of current segment
        vector<StreetSegment>* v = segmentMap.find(coord);
        if (v == nullptr)
        {
            vector<StreetSegment> sn;
            sn.push_back(seg);
            segmentMap.associate(coord, sn);
        }
        
        else
            v->push_back(seg);
        
        // find all segments related to end of current segment
        coord = seg.segment.end;
        v = segmentMap.find(coord);
        if (v == nullptr)
        {
            vector<StreetSegment> sn;
            sn.push_back(seg);
            segmentMap.associate(coord, sn);
        }
        else
            v->push_back(seg);
        
        // find all segments related to each attraction
        for (auto i: seg.attractions)
        {
            coord = i.geocoordinates;
            v = segmentMap.find(coord);
            if (v == nullptr)
            {
                vector<StreetSegment> sn;
                sn.push_back(seg);
                segmentMap.associate(coord, sn);
            }
            else
                v->push_back(seg);
        }
    }
}

vector<StreetSegment> SegmentMapperImpl::getSegments(const GeoCoord& gc) const
{
	const vector<StreetSegment>* v;
    v = segmentMap.find(gc);
    if (v != nullptr)
        return *v;
    else // no segments found
    {
        vector<StreetSegment> segments;
        return segments;
    }
}

//******************** SegmentMapper functions ********************************

// These functions simply delegate to SegmentMapperImpl's functions.
// You probably don't want to change any of this code.

SegmentMapper::SegmentMapper()
{
	m_impl = new SegmentMapperImpl;
}

SegmentMapper::~SegmentMapper()
{
	delete m_impl;
}

void SegmentMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

vector<StreetSegment> SegmentMapper::getSegments(const GeoCoord& gc) const
{
	return m_impl->getSegments(gc);
}
