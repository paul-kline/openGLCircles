#version 410 core
//#extension GL_NV_shader_buffer_load : enable
uniform float radius;
uniform float creationDistance;
uniform int numCircs;
//uniform vec4 circles1_2;uniform vec4 circles3_4;uniform vec4 circles5_6;uniform vec4 circles7_8;uniform vec4 circles9_10;uniform vec4 circles11_12;
// Replace the placeholder implementation here...
in float ldsX, ldsY;
out vec4 fragmentColor;
in vec2 refCoordToFS;

vec4 color1= vec4(1.0,1.0,0.1,1.0);
vec4 brightgreen= vec4( 0.486275, 0.988235, 0,1.0);
vec4 pink= vec4(1, 0.411765, 0.705882,1.0);
vec4 cyan= vec4(0, 1, 1,1.0);
vec4 crimson= vec4(0.862745, 0.0784314, 0.235294,1.0);
vec4 orange= vec4( 1, 0.54902, 0,1.0);
vec4 springgreen= vec4(0, 0.980392, 0.603922,1.0);
vec4 mistryrose= vec4(1, 0.894118, 0.882353,1.0);
vec4 color9= vec4(0.20,0.0,0.0,1.0);
vec4 color10= vec4(0.10,0.0,0.0,1.0);
vec4 color11= vec4(0.0,0.0,0.0,1.0);
vec4 color12= vec4(1.0,0.0,0.0,1.0);
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

vec2 getCircleCoords(int index)
{
//   float** circleCenters2;//[numCircs+1][2];
//   circleCenters2 = (int *)malloc(sizeof(*float)*numCircs);
//   
  float surroundingCircs = numCircs-1;
  if(index==0){return vec2(0.0,0.0);}
  float tau = 2.0 * 3.14159265359;
    float x=creationDistance* sin((tau/surroundingCircs)*index);
    float y=creationDistance* cos((tau/surroundingCircs)*index);
    
    return vec2(x,y);
    //  std::cout << circleCenters[i][0] << ", " << circleCenters[i][1] << "\n";
   }
   



void main()
{
  
  //vec2 * circles[numCircs];
//fragmentColor = vec4(0.0, 0.0, 1.0, 1.0);//1.0, 0.0, 0.0, 1.0);
	int i;
	int counter=0;
	for(i=0; i<numCircs; i++){
	  fragmentColor = mistryrose;//1.0, 0.0, 0.0, 1.0);
	  if(isInCircle(refCoordToFS,getCircleCoords(i))) 
	    {
	      counter++;
	//	fragmentColor = vec4(0.0, 0.0, 1.0, ((float)i/10.0)*1.0);//1.0, 0.0, 0.0, 1.0);
	    }
	  
	}
	if(counter==0){
	    fragmentColor= color1;//vec4(0.0, 0.0, 1.0, 1.0);
	}else if(counter==1){
	    fragmentColor = brightgreen;///(counter+1);//vec4(0.0, 0.0, 0.5, ((float)counter)/numCircs);
	
	}else if(counter==2){
	    fragmentColor = pink;///(counter+1);//vec4(0.0, 0.0, 0.5, ((float)counter)/numCircs);
	
	}else if(counter==3){
	    fragmentColor = cyan;///(counter+1);//vec4(0.0, 0.0, 0.5, ((float)counter)/numCircs);
	
	}
	else if(counter==4){
	    fragmentColor = crimson;///(counter+1);//vec4(0.0, 0.0, 0.5, ((float)counter)/numCircs);
	
	}else if(counter==5){
	    fragmentColor = orange;///(counter+1);//vec4(0.0, 0.0, 0.5, ((float)counter)/numCircs);
	
	}
	else if(counter==6){
	    fragmentColor = springgreen;///(counter+1);//vec4(0.0, 0.0, 0.5, ((float)counter)/numCircs);
	
	}
	else if(counter==7){
	    fragmentColor = mistryrose;///(counter+1);//vec4(0.0, 0.0, 0.5, ((float)counter)/numCircs);
	
	}
	else{
	  float count = counter;
	    fragmentColor =vec4(0.0,0.0,(count)/numCircs,(count)/numCircs);
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
}





