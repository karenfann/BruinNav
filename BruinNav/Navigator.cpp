#include "provided.h"
#include "MyMap.h"
#include "support.h"
#include <algorithm>
#include <string>
#include <vector>
#include <queue>
#include <iostream>
using namespace std;

class NavigatorImpl
{
public:
    NavigatorImpl();
    ~NavigatorImpl();
    bool loadMapData(string mapFile);
    NavResult navigate(string start, string end, vector<NavSegment>& directions) const;
private:
    MapLoader map;
    SegmentMapper smap;
    AttractionMapper amap;
    struct Node {
        GeoCoord coord;
        StreetSegment seg;
        bool operator>(const Node& other) const;
        bool operator<=(const Node& other) const;
        bool operator==(const Node& other) const;
        double fScore;
    };
    void reconstructPath(MyMap<Node, Node>& cameFrom, Node current, vector<NavSegment>& directions) const;
    string getProceedDirection(double angle) const;
    string getTurnDirection(double angle) const;
    double getActualAngle(const Node& current, const Node& from) const;
};

NavigatorImpl::NavigatorImpl()
{
}

NavigatorImpl::~NavigatorImpl()
{
}

bool NavigatorImpl::loadMapData(string mapFile)
{
    if(!map.load(mapFile))
        return false;
    smap.init(map);
    amap.init(map);
    return true;
}

NavResult NavigatorImpl::navigate(string start, string end, vector<NavSegment> &directions) const
{
    // check if start location is valid
    Node source;
    if (!amap.getGeoCoord(start, source.coord))
        return NAV_BAD_SOURCE;
    
    // check if end location is valid
    Node goal;
    if (!amap.getGeoCoord(end, goal.coord))
        return NAV_BAD_DESTINATION;
    
    // get start's street segment
    source.seg = smap.getSegments(source.coord)[0];
    
    // get end's street segment
    goal.seg = smap.getSegments(goal.coord)[0];
    
    // f score = g score + heuristic score
    // start's fscore is heuristic score
    source.fScore = distanceEarthMiles(source.coord, goal.coord);
    
    // g score is cost to get to that node
    MyMap<Node, double> gScore;
    gScore.associate(source, 0);
    
    priority_queue<Node, vector<Node>, greater<Node>> open;
    vector<Node> openV; // to traverse through open
    open.push(source);
    openV.push_back(source);
    
    vector<Node> closed;
    MyMap<Node, Node> cameFrom; // to reconstruct path
    
    while (!open.empty())
    {
        Node current = open.top();
        
        // reach destination
        if (current.coord == goal.coord)
        {
            reconstructPath(cameFrom, current, directions);
            return NAV_SUCCESS;
        }
        
        // move Node from open to closed
        open.pop();
        vector<Node>::iterator it = openV.begin();
        while (it != openV.end())
        {
            if (it->coord == current.coord)
                it = openV.erase(it);
            else
                it++;
        }
        closed.push_back(current);
        
        // get potential neighbors (start coordinate, end coordinate and coordinates of attractions)
        vector<StreetSegment> segments = smap.getSegments(current.coord);
        vector<Node> neighbors;
        for (auto s: segments)
        {
            Node n;
            n.coord = s.segment.start;
            n.seg = s;
            neighbors.push_back(n);
            n.coord = s.segment.end;
            n.seg = s;
            neighbors.push_back(n);
            for (auto i: s.attractions)
            {
                n.coord = i.geocoordinates;
                n.seg = s;
                neighbors.push_back(n);
            }
            
            // add potential neighbors to open if not already closed
            for (auto c: neighbors)
            {
                // check if has already been visited
                if (find(closed.begin(), closed.end(), c) != closed.end())
                    continue;
                
                double tentativeG = *(gScore.find(current)) + distanceEarthMiles(current.coord, c.coord); // g score = g score of current node + length between current node and neighbor
                c.fScore = tentativeG + distanceEarthMiles(c.coord, goal.coord);
                if (find(openV.begin(), openV.end(), c) == openV.end())
                {
                    open.push(c);
                    openV.push_back(c);
                }
                
                // check if better path
                else if (tentativeG >= *(gScore.find(*find(openV.begin(), openV.end(), c))))
                    continue;
                gScore.associate(c, tentativeG);
                cameFrom.associate(c, current);
            }
        }
    }
    
    return NAV_NO_ROUTE;
}

bool NavigatorImpl::Node::operator>(const NavigatorImpl::Node &other) const
{
    return fScore > other.fScore;
}

bool NavigatorImpl::Node::operator<=(const NavigatorImpl::Node &other) const
{
    return fScore <= other.fScore;
}

bool NavigatorImpl::Node::operator==(const NavigatorImpl::Node& other) const
{
    return coord == other.coord;
}

void NavigatorImpl::reconstructPath(MyMap<Node, Node>& cameFrom, Node current, vector<NavSegment>& directions) const
{
    Node from;
    Node after; // for turn nav segments
    while (cameFrom.find(current) != nullptr)
    {
        from = *cameFrom.find(current);
        GeoSegment gs = GeoSegment(from.coord, current.coord);
        if (from.seg.streetName == current.seg.streetName) // if nodes are on same street, then proceed
        {
            NavSegment n = NavSegment(getProceedDirection(angleOfLine(gs)), current.seg.streetName, distanceEarthMiles(from.coord, current.coord), GeoSegment(from.coord, current.coord));
            directions.insert(directions.begin(), n);
        }
        else // if nodes are on different streets, then there is a turn nav segment
        {
            double angle = angleOfLine(gs);
            
            // add proceed nav segment leading up to turn
            NavSegment p = NavSegment(getProceedDirection(angle), current.seg.streetName, distanceEarthMiles(from.coord, current.coord), GeoSegment(from.coord, current.coord));
            directions.insert(directions.begin(), p);
            Node beforeFrom = *cameFrom.find(from);
            GeoSegment before = GeoSegment(beforeFrom.coord, from.coord);
            
            // find the angle between the two streets
            double fromAngle = angleOfLine(before);
            angle -= fromAngle;
            if (angle < 0)
                angle += 360;
            
            // add turn segment
            p = NavSegment(getTurnDirection(angle), current.seg.streetName);
            directions.insert(directions.begin(), p);
        }
        after = current;
        current = from;
    }
}

string NavigatorImpl::getProceedDirection(double angle) const
{
    if (angle >= 0 && angle <= 22.5)
        return "east";
    else if (angle > 22.5 && angle <= 67.5)
        return "northeast";
    else if (angle > 67.5 && angle <= 112.5)
        return "north";
    else if (angle > 112.5 && angle <= 157.5)
        return "northwest";
    else if (angle > 157.5 && angle <= 202.5)
        return "west";
    else if (angle > 202.5 && angle <= 247.5)
        return "southwest";
    else if (angle > 247.5 && angle <= 292.5)
        return "south";
    else if (angle > 292.5 && angle <= 337.5)
        return "southeast";
    else
        return "east";
}

string NavigatorImpl::getTurnDirection(double angle) const
{
    if (angle < 180)
        return "left";
    return "right";
}
//******************** Navigator functions ************************************

// These functions simply delegate to NavigatorImpl's functions.
// You probably don't want to change any of this code.

Navigator::Navigator()
{
    m_impl = new NavigatorImpl;
}

Navigator::~Navigator()
{
    delete m_impl;
}

bool Navigator::loadMapData(string mapFile)
{
    return m_impl->loadMapData(mapFile);
}

NavResult Navigator::navigate(string start, string end, vector<NavSegment>& directions) const
{
    return m_impl->navigate(start, end, directions);
}
