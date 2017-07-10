#include "provided.h"
#include "MyMap.h"
#include <cctype>
#include <string>
using namespace std;

class AttractionMapperImpl
{
public:
    AttractionMapperImpl();
    ~AttractionMapperImpl();
    void init(const MapLoader& ml);
    bool getGeoCoord(string attraction, GeoCoord& gc) const;
private:
    MyMap<string, GeoCoord> attractionMap;
};

AttractionMapperImpl::AttractionMapperImpl()
{
}

AttractionMapperImpl::~AttractionMapperImpl()
{
}

void AttractionMapperImpl::init(const MapLoader& ml)
{
    StreetSegment seg;
    for (int i = 0; i < ml.getNumSegments(); i++) // get every segment
    {
        ml.getSegment(i, seg);
        for (auto i: seg.attractions) // for every attraction on this segment, map to this segment
        {
            for (int c = 0; c < i.name.length(); c++)
            {
                i.name[c] = tolower(i.name[c]);
            }
            attractionMap.associate(i.name, i.geocoordinates);
        }
    }
}

bool AttractionMapperImpl::getGeoCoord(string attraction, GeoCoord& gc) const
{
    for (int c = 0; c < attraction.length(); c++)
        attraction[c] = tolower(attraction[c]); // change attraction name to lower case for case-insensitivity
    const GeoCoord* at = attractionMap.find(attraction);
    
    if (at != nullptr) // attraction exists
    {
        gc = *at;
        return true;
    }
    return false;
}

//******************** AttractionMapper functions *****************************

// These functions simply delegate to AttractionMapperImpl's functions.
// You probably don't want to change any of this code.

AttractionMapper::AttractionMapper()
{
    m_impl = new AttractionMapperImpl;
}

AttractionMapper::~AttractionMapper()
{
    delete m_impl;
}

void AttractionMapper::init(const MapLoader& ml)
{
    m_impl->init(ml);
}

bool AttractionMapper::getGeoCoord(string attraction, GeoCoord& gc) const
{
    return m_impl->getGeoCoord(attraction, gc);
}
