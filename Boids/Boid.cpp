#include "Boid.h"


#define NEARBY_DISTANCE		100.0f	// how far boids can see

Boid::Boid()
{
	m_position = XMFLOAT3(0, 0, 0);
	m_speed = 30;
	m_personalSpace = 6.0f;
	m_cohesionRadius = 5.0f;
	createRandomDirection();
	//setDirection(nullptr);
}

Boid::~Boid()
{
}

void Boid::createRandomDirection()
{
	float x = (float)(rand() % 10);
	x -= 5;
	float y = (float)(rand() % 10);
	y -= 5;
	float z = 0;
	setDirection(XMFLOAT3(x, y, z));
}

void Boid::setDirection(XMFLOAT3 direction)
{
	XMVECTOR v = XMLoadFloat3(&direction);
	v = XMVector3Normalize(v);
	XMStoreFloat3(&m_direction, v);
}

void Boid::update(float t, vecBoid* boidList)
{
	// create a list of nearby boids
	vecBoid nearBoids = nearbyBoids(boidList);

	// NOTE these functions should always return a normalised vector
	XMFLOAT3  vSeparation = calculateSeparationVector(&nearBoids);
	XMFLOAT3  vAlignment = calculateAlignmentVector(&nearBoids);
	XMFLOAT3  vCohesion = calculateCohesionVector(&nearBoids);

	//weighting
	vSeparation = multiplyFloat3(vSeparation, 0.4);
	vAlignment = multiplyFloat3(vAlignment, 0.3);
	vCohesion = multiplyFloat3(vCohesion, 0.3);

	XMFLOAT3 desiredDirection = subtractFloat3(vAlignment, vSeparation, vCohesion);//vCohesion;
	XMFLOAT3 actualDirection = subtractFloat3(m_direction, desiredDirection);

	setDirection(actualDirection);

	// set me
	XMFLOAT3 normDirection = normaliseFloat3(m_direction);
	XMFLOAT3 travelDistance = multiplyFloat3(normDirection, m_speed * t);
	m_position = addFloat3 (m_position, travelDistance);
	m_position.z = 0;

	DrawableGameObject::update(t);
}

XMFLOAT3 Boid::calculateSeparationVector(vecBoid* boidList)
{
	XMFLOAT3 nearby = XMFLOAT3(0, 0, 0);
	if (boidList == nullptr)
		return nearby;
	
	// work out which is nearest fish, and calculate a vector away from that

	float distance;
	Boid* nearest = nullptr;

	for (Boid* boid : *boidList)
	{	
		if (distanceFloat3(m_position, boid->m_position) > m_personalSpace) {
			continue;
		}
		else if (nearest == nullptr)
		{
			nearest = boid;
		}
		else if (distanceFloat3(m_position, nearest->m_position) > distanceFloat3(m_position, boid->m_position))
		{
			nearest = boid;
		}
	}

	if (nearest == nullptr)
	{
		// if there is not a nearby fish - simply return the current direction. 
		return normaliseFloat3(m_direction);
	}

	XMFLOAT3 directionNearest = nearest->m_direction;

	XMFLOAT3 oppositeDirection = multiplyFloat3(directionNearest, -1);
	//XMFLOAT3 oppositeDirection = createRandomDirection();
	// your code here

	if (nearest != nullptr) {
		return normaliseFloat3(oppositeDirection);
	}

}


XMFLOAT3 Boid::calculateAlignmentVector(vecBoid* boidList)
{
	XMFLOAT3 nearby = XMFLOAT3(0, 0, 0);
	if (boidList == nullptr)
		return nearby;

	// your code here
	// calculate average position of nearby
	XMFLOAT3 sumDirection;
	int numWithinRadius = 0;

	float distFromBoid;
	for (Boid* boid : *boidList)
	{
		distFromBoid = distanceFloat3(m_position, boid->m_position);
		if (distFromBoid <= m_cohesionRadius)
		{
			sumDirection = addFloat3(sumDirection, *boid->getPosition());
			numWithinRadius++;
		}
	}
	if (numWithinRadius > 0)
	{
		nearby = divideFloat3(sumDirection, numWithinRadius);
	}
	return normaliseFloat3(nearby); // return the normalised (average) direction of nearby drawables
}

XMFLOAT3 Boid::calculateCohesionVector(vecBoid* boidList)
{
	XMFLOAT3 nearby = XMFLOAT3(0, 0, 0);

	if (boidList == nullptr)
		return nearby;

	// calculate average position of nearby
	XMFLOAT3 sumPos;
	int numWithinRadius = 0;
	float distFromBoid;
	for (Boid* boid : *boidList) 
	{
		distFromBoid = distanceFloat3(m_position, boid->m_position);
		if (distFromBoid <= m_cohesionRadius)
		{
			sumPos = addFloat3(sumPos, *boid->getPosition());
			numWithinRadius++;
		}
	}
	if (numWithinRadius > 0)
	{
		nearby = divideFloat3(sumPos, numWithinRadius);
	}

	return normaliseFloat3(nearby); // nearby is the direction to where the other drawables are
}



// use but don't alter the methods below

XMFLOAT3 Boid::addFloat3(XMFLOAT3& f1, XMFLOAT3& f2)
{
	XMFLOAT3 out;
	out.x = f1.x + f2.x;
	out.y = f1.y + f2.y;
	out.z = f1.z + f2.z;

	return out;
}

XMFLOAT3 Boid::addFloat3(XMFLOAT3& f1, XMFLOAT3& f2, XMFLOAT3& f3)
{
	XMFLOAT3 out;
	out.x = f1.x + f2.x + f3.x;
	out.y = f1.y + f2.y + f3.y;
	out.z = f1.z + f2.z + f3.z;

	return out;
}

XMFLOAT3 Boid::subtractFloat3(XMFLOAT3& f1, XMFLOAT3& f2)
{
	XMFLOAT3 out;
	out.x = f1.x - f2.x;
	out.y = f1.y - f2.y;
	out.z = f1.z - f2.z;

	return out;
}
XMFLOAT3 Boid::subtractFloat3(XMFLOAT3& f1, XMFLOAT3& f2, XMFLOAT3& f3)
{
	XMFLOAT3 out;
	out.x = f1.x - f2.x - f3.x;
	out.y = f1.y - f2.y - f3.y;
	out.z = f1.z - f2.z - f3.z;

	return out;
}

XMFLOAT3 Boid::multiplyFloat3(XMFLOAT3& f1, const float scalar)
{
	XMFLOAT3 out;
	out.x = f1.x * scalar;
	out.y = f1.y * scalar;
	out.z = f1.z * scalar;

	return out;
}

XMFLOAT3 Boid::divideFloat3(XMFLOAT3& f1, const float scalar)
{
	XMFLOAT3 out;
	out.x = f1.x / scalar;
	out.y = f1.y / scalar;
	out.z = f1.z / scalar;

	return out;
}

float Boid::distanceFloat3(XMFLOAT3& f1, XMFLOAT3& f2)
{
	float aSquared = pow(f1.x - f2.x, 2);
	float bSquared = pow(f1.y - f2.y, 2);
	float cSquared = 0;
	
	float distance = abs(sqrt(aSquared + bSquared + cSquared));

	return distance;
}

float Boid::magnitudeFloat3(XMFLOAT3& f1)
{
	return sqrt((f1.x * f1.x) + (f1.y * f1.y) + (f1.z * f1.z));
}

XMFLOAT3 Boid::normaliseFloat3(XMFLOAT3& f1)
{
	if (f1.x == 0 && f1.y == 0 && f1.z == 0)
	{
		//CATCH ERROR
		//if vector has no value then division can't be done so just return a vector of no value
		return XMFLOAT3(0, 0, 0);
	}
	float length = sqrt((f1.x * f1.x) + (f1.y * f1.y) + (f1.z * f1.z));


	f1.x /= length;
	f1.y /= length;
	f1.z /= length;

	return f1;
}

vecBoid Boid::nearbyBoids(vecBoid* boidList)
{
	vecBoid nearBoids;
	if (boidList->size() == 0)
		return nearBoids;

	for (Boid* boid : *boidList) {
		// ignore self
		if (boid == this)
			continue;

		// get the distance between the two
		XMFLOAT3 vB = *(boid->getPosition());
		XMFLOAT3 vDiff = subtractFloat3(m_position, vB);
		float l = magnitudeFloat3(vDiff);
		if (l < NEARBY_DISTANCE) {
			nearBoids.push_back(boid);
		}
	}

	return nearBoids;
}


void Boid::checkIsOnScreenAndFix(const XMMATRIX&  view, const XMMATRIX&  proj)
{
	XMFLOAT4 v4;
	v4.x = m_position.x;
	v4.y = m_position.y;
	v4.z = m_position.z;
	v4.w = 1.0f;

	XMVECTOR vScreenSpace = XMLoadFloat4(&v4);
	XMVECTOR vScreenSpace2 = XMVector4Transform(vScreenSpace, view);
	XMVECTOR vScreenSpace3 = XMVector4Transform(vScreenSpace2, proj);

	XMFLOAT4 v;
	XMStoreFloat4(&v, vScreenSpace3);
	v.x /= v.w;
	v.y /= v.w;
	v.z /= v.w;
	v.w /= v.w;

	float fOffset = 10; // a suitable distance to rectify position within clip space
	if (v.x < -1 || v.x > 1 || v.y < -1 || v.y > 1)
	{
		if (v.x < -1 || v.x > 1) {
			v4.x = -v4.x + (fOffset * v.x);
		}
		else if (v.y < -1 || v.y > 1) {
			v4.y = -v4.y + (fOffset * v.y);
		}

		// throw a bit of randomness into the mix
		//createRandomDirection();
	}

	// method 1 - appear on the other side
	//m_position.x = v4.x;
	//m_position.y = v4.y;
	//m_position.z = v4.z;


	// method2 - bounce off sides and head to centre
	if (v.x < -1 || v.x > 1 || v.y < -1 || v.y > 1)
	{
		m_direction = multiplyFloat3(m_direction, -1);;
		m_direction = normaliseFloat3(m_direction);
	}

	return;
}