//
//  support.cpp
//  proj4
//
//  Created by Karen Fann on 3/14/17.
//  Copyright © 2017 cs32. All rights reserved.
//

#include <stdio.h>
#include "support.h"

bool operator<=(const GeoCoord& g1, const GeoCoord& g2)
{
    if (g1.latitude < g2.latitude)
        return true;
    else if (g1.latitude == g2.latitude)
        return g1.longitude <= g2.longitude;
    else
        return false;
}

bool operator>(const GeoCoord& g1, const GeoCoord& g2)
{
    if (g1.latitude > g2.latitude)
        return true;
    else if (g1.latitude == g2.latitude)
        return g1.longitude > g2.longitude;
    else
        return false;
}

bool operator==(const GeoCoord& g1, const GeoCoord& g2)
{
    return (g1.latitude == g2.latitude) && (g1.longitude == g2.longitude);
}

bool operator==(const StreetSegment &seg1, const StreetSegment &seg2)
{
    return (seg1.segment.start == seg2.segment.start) && (seg1.segment.end == seg2.segment.end);
}
