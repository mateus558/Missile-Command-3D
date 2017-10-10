#include "utils.h"

using namespace std;

namespace Collision {
	bool SphereCollision(Sphere a, Sphere b){
		Point posa = a.getPosition();
		Point posb = b.getPosition();
		float dist = posa.distance(posb);
		
		if(dist <= (a.getRadius() + b.getRadius())){
			return true;
		}	
		
		return false;
	}
	
	bool SpherePlaneCollision(Sphere s, float a, float b, float c, float d){
		Point pos = s.getPosition();
		float dist = (a*pos.x + b*pos.y + c*pos.z)/sqrt(a*a + b*b + c*c);
		
		if(dist <= s.getRadius()){
			return true;
		}
		
		return false;
	}
	
	bool PointLineCollision(float coordinates[2][2], float x, float y){

		float fx = ((float)((coordinates[0][1]-coordinates[1][1])*x + (coordinates[0][0]*coordinates[1][1] - coordinates[1][0]*coordinates[0][1]))/(-1*(coordinates[1][0]-coordinates[0][0])));
		
	   	if(y >= fx)
			return true;
		else
			return false;
	}
}
