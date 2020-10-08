#pragma once
class Point
{
public:
    int m_x = 0;
    int m_y = 0;

public:
    Point() :m_x(0), m_y(0)
    {}
    Point(int x, int y) :m_x(x), m_y(y) {}
    Point(const Point& other) :m_x(other.m_x), m_y(other.m_y) {}

    ~Point() {}


};