#include "provided.h"
#include <fstream>
#include <string>
#include <vector>
using namespace std;

class MapLoaderImpl
{
public:
	MapLoaderImpl();
	~MapLoaderImpl();
	bool load(string mapFile);
	size_t getNumSegments() const;
	bool getSegment(size_t segNum, StreetSegment& seg) const;
private:
    vector<StreetSegment> segments;
    int numSeg;
    vector<string> processToStrings(string line);
    GeoSegment processToGeoSegment(string line);
    GeoCoord processToGeoCoord(string line);
    Attraction processToAttraction(string line);
};

MapLoaderImpl::MapLoaderImpl()
{
    numSeg = 0;
}

MapLoaderImpl::~MapLoaderImpl()
{
}

bool MapLoaderImpl::load(string mapFile)
{
    ifstream infile(mapFile);
    if (!infile) // error opening file
        return false;
    string s;
    while (getline(infile, s))
    {
        // get street name
        StreetSegment seg;
        seg.streetName = s;
        
        // get street segment
        getline(infile, s);
        seg.segment = processToGeoSegment(s);
        
        // get attractions if any
        getline(infile, s);
        int numAtt = stoi(s);
        for (int i = 0; i < numAtt; i++)
        {
            getline(infile, s);
            seg.attractions.push_back(processToAttraction(s));
        }
        
        // add segment to vector of street segments
        segments.push_back(seg);
        numSeg++;
    }
    return true;
}

size_t MapLoaderImpl::getNumSegments() const
{
    return numSeg;
}

bool MapLoaderImpl::getSegment(size_t segNum, StreetSegment &seg) const
{
    if (segNum >= getNumSegments())
        return false;
    seg = segments[segNum];
    return true;
}

vector<string> MapLoaderImpl::processToStrings(string line)
{
    int i = 0;
    int c = 0;
    string str = "";
    vector<string> v;
    v.push_back(str);
    while (i < line.size())
    {
        if (line[i] == ',' || line[i] == ' ')
        {
            if ((line[i] == ' ' && line[i-1] != ',') || line[i] == ',')
            {
                c++;
                v.push_back(str);
            }
        }
        else
            v[c] += line[i];
        i++;
    }
    return v;
}

GeoSegment MapLoaderImpl::processToGeoSegment(string line)
{
    vector<string> v = processToStrings(line);
    return GeoSegment(GeoCoord(v[0], v[1]), GeoCoord(v[2], v[3]));
}

GeoCoord MapLoaderImpl::processToGeoCoord(string line)
{
    vector<string> v = processToStrings(line);
    return GeoCoord(v[0], v[1]);
}

Attraction MapLoaderImpl::processToAttraction(string line)
{
    int c = 0;
    Attraction a;
    while (line[c] != '|')
    {
        a.name += line[c];
        c++;
    }
    c++;
    a.geocoordinates = processToGeoCoord(line.substr(c, line.size() - c));
    return a;
}

//******************** MapLoader functions ************************************

// These functions simply delegate to MapLoaderImpl's functions.
// You probably don't want to change any of this code.

MapLoader::MapLoader()
{
	m_impl = new MapLoaderImpl;
}

MapLoader::~MapLoader()
{
	delete m_impl;
}

bool MapLoader::load(string mapFile)
{
	return m_impl->load(mapFile);
}

size_t MapLoader::getNumSegments() const
{
	return m_impl->getNumSegments();
}

bool MapLoader::getSegment(size_t segNum, StreetSegment& seg) const
{
   return m_impl->getSegment(segNum, seg);
}
