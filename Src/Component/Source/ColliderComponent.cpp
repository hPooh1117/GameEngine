#include "ColliderComponent.h"

#include <algorithm>

#include "./Engine/Actor.h"

//----------------------------------------------------------------------------------------------------------------------------

DirectX::XMMATRIX ColliderComponent::GetWorldMatrix()
{
	DirectX::XMMATRIX world;
	world = DirectX::XMMatrixScaling(mScale.x, mScale.y, mScale.z);
	//world *= XMMatrixRotationQuaternion(m_rotation);
	world *= DirectX::XMLoadFloat4x4(&Matrix::CreateFromQuaternion(m_pOwner.lock()->GetQuaternion()));
	world *= DirectX::XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);
	return world;
}

//----------------------------------------------------------------------------------------------------------------------------

void ColliderComponent::SetCenterPosition(Vector3& pos)
{
	mPosition = pos; m_pOwner.lock()->SetPosition(pos);
}

//----------------------------------------------------------------------------------------------------------------------------

float ColliderComponent::CalculateBetweenPointAndLine(const Vector3& p, const Line& l, Vector3& h, float& t)
{
	float lengthSq = l.d.dot(l.d);
	t = 0.0f;
	if (lengthSq > 0.0f)
	{
		t = l.d.dot(p - l.p) / lengthSq;
	}

	h = l.p + t * l.d;
	return (h - p).getLength();
}

//----------------------------------------------------------------------------------------------------------------------------

float ColliderComponent::CalculateBetweenLines(const Line& l11, const Line& l22, Vector3& h1, Vector3& h2, float& t1, float& t2)
{
    Line l1 = l11;
    Line l2 = l22;
    l1.d.normalize();
    l2.d.normalize();

    // two lines are parallel
    if (l1.d.IsParallel(l2.d))
    {
        float length = CalculateBetweenPointAndLine(l1.p, l2, h2, t2);
        h1 = l1.p;
        t1 = 0.0f;
        return length;
    }

    // two lines are skew
    float Dd1_d2 = l1.d.dot(l2.d);
    float Dd1_d1 = l1.d.dot(l1.d);
    float Dd2_d2 = l2.d.dot(l2.d);
    Vector3 p2_p1 = l1.p - l2.p;

    t1 = (Dd1_d2 * l2.d.dot(p2_p1) - Dd2_d2 * l1.d.dot(p2_p1)) /
        (Dd1_d1 * Dd2_d2 - Dd1_d2 * Dd1_d2);
    h1 = l1.p + t1 * l1.d;
    t2 = l2.d.dot(h1 - l2.p) / Dd2_d2;
    h2 = l2.p + t2 * l2.d;

    return (h2 - h1).getLength();
}

//----------------------------------------------------------------------------------------------------------------------------

float ColliderComponent::CalculateBetweenPointAndSegment(const Vector3& p, const Segment& s, Vector3& h, float& t)
{
    const Vector3 e = s.end;

    Line line;
    line.p = s.start;
    line.d = e - s.start;
    //line.d.normalize();

    // length of perpendicular
    float length = CalculateBetweenPointAndLine(p, line, h, t);

    if (IsAcute(p, s.start, e) == false)
    {
        //outside of segment's start point
        h = s.start;
        return (s.start - p).getLength();
    }
    else if (IsAcute(p, e, s.start) == false)
    {
        // outside of segment's end point
        h = e;
        return (e - p).getLength();
    }

    return length;
}

//----------------------------------------------------------------------------------------------------------------------------

float ColliderComponent::CalculateBetweenSegments(const Segment& s1, const Segment& s2, Vector3& h1, Vector3& h2, float t1, float t2)
{
    Vector3 d1 = s1.end - s1.start;
    //d1.normalize();

    Vector3 d2 = s2.end - s2.start;
    //d2.normalize();

    if (d1.dot(d1) < 0.00001f)
    {
        if (d2.dot(d2) < 0.00001f)
        {
            float length = (s2.start - s1.start).getLength();
            h1 = s1.start;
            h2 = s2.start;
            t1 = t2 = 0.0f;
            return length;
        }
        else
        {
            float length = CalculateBetweenPointAndSegment(s1.start, s2, h2, t2);
            h1 = s1.start;
            t1 = 0.0f;
            t2 = std::clamp<float>(t2, 0.0f, 1.0f);
            return length;
        }
    }
    else if ((d2).dot(d2) < 0.00001f)
    {
        float length = CalculateBetweenPointAndSegment(s2.start, s1, h1, t1);
        h2 = s2.start;
        t1 = std::clamp<float>(t1, 0.0f, 1.0f);
        t2 = 0.0f;
        return length;
    }

    if ((d1).IsParallel(d2))
    {
        t1 = 0.0f;
        h1 = s1.start;
        float length = CalculateBetweenPointAndSegment(s1.start, s2, h2, t2);
        if (t2 >= 0.0f && t2 <= 1.0f) return length;
    }
    else
    {
        Line l1, l2;
        l1.p = s1.start;
        l1.d = s1.end - s1.start;
        //l1.d.normalize();

        l2.p = s2.start;
        l2.d = s2.end - s2.start;
        //l2.d.normalize();

        float length = CalculateBetweenLines(l1, l2, h1, h2, t1, t2);
        if (t1 >= 0.0f && t1 <= 1.0f &&
            t2 >= 0.0f && t2 <= 1.0f)
        {
            return length;
        }
    }

    t1 = std::clamp<float>(t1, 0.0f, 1.0f);
    h1 = s1.start + t1 * d1;
    float length = CalculateBetweenPointAndSegment(h1, s2, h2, t2);
    if (t2 >= 0.0f && t2 <= 1.0f) return length;

    t2 = std::clamp<float>(t2, 0.0f, 1.0f);
    h2 = s2.start + t2 * d2;
    length = CalculateBetweenPointAndSegment(h2, s1, h1, t1);
    if (t1 >= 0.0f && t1 <= 1.0f) return length;

    t1 = std::clamp<float>(t1, 0.0f, 1.0f);
    h1 = s1.start + t1 * d1;
    return (h2 - h1).getLength();
}

//----------------------------------------------------------------------------------------------------------------------------

bool ColliderComponent::IsAcute(const Vector3& p1, const Vector3& p2, const Vector3& p3)
{
    return (p1 - p2).IsAcute(p3 - p2);
}

//----------------------------------------------------------------------------------------------------------------------------
