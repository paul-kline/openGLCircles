#version 410 core
uniform float radius;
// Replace the placeholder implementation here...
in float ldsX, ldsY;
out vec4 fragmentColor;
in vec2 refCoordToFS;

//uniform float* circleCenters;
bool isInCircle(vec2 point, vec2 center)
{

  float d = sqrt(pow(abs(point[0]-center[0]),2) + pow(abs(point[1]-center[1]),2));
  if(d<=radius){
    return true;
  }else{
      return false;
  }
}




void main()
{

	if(isInCircle(refCoordToFS,vec2(0.0,0.0)))
	{
	  fragmentColor = vec4(0.0, 0.0, 1.0, 1.0);//1.0, 0.0, 0.0, 1.0);
	}else{
	  fragmentColor = vec4(1.0, 0.0, 0.0, 1.0);//1.0, 0.0, 0.0, 1.0);
	
	}
	
}





