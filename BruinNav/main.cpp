//
//  main.cpp
//  proj4
//
//  Created by Karen Fann on 3/10/17.
//  Copyright © 2017 cs32. All rights reserved.
//

#include "provided.h"
#include <iostream>
using namespace std;

int main() {
    string mapFile = "/Users/Karen/Documents/cs32/proj4/proj4/mapdata.txt";
    MapLoader map;
    if (map.load(mapFile))
    {
        cout << "loaded" << endl;
        cout << map.getNumSegments() << endl;
    }
    AttractionMapper amap;
    amap.init(map);
    GeoCoord geo;
    if (amap.getGeoCoord("Ami Sushi", geo))
    {
        cout << "found location" << endl;
        cout << geo.latitude << " " << geo.longitude << endl;
    }
    SegmentMapper smap;
    smap.init(map);
    vector<StreetSegment> v = smap.getSegments(geo);
    if (!v.empty())
    {
        cout << "found segments" << endl;
        for (int i = 0; i < v.size(); i++)
        {
            cout << v[i].streetName << endl;
            cout << v[i].segment.start.latitude << " " << v[i].segment.start.longitude << " " << v[i].segment.end.latitude << " " << v[i].segment.start.longitude << endl;
        }
    }
    Navigator nav;
    nav.loadMapData(mapFile);
    vector<NavSegment> directions;
    if (nav.navigate("Ami Sushi", "ami sushi", directions) == NAV_SUCCESS)
    {
        cout << "success" << endl;
        for (auto i: directions)
            cout << i.m_geoSegment.start.latitude << " " << i.m_geoSegment.start.longitude << " " << i.m_geoSegment.end.latitude << " " << i.m_geoSegment.end.longitude << " " << i.m_direction << " " << i.m_streetName << endl;
    }
return 0;
}
