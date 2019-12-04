/*
CSCI 480
Assignment 3 Raytracer

Name: Kevin Zhou
*/

#include <windows.h>
#include <stdlib.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <stdio.h>
#include <string>

#include "opencv2/core/core.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs/imgcodecs.hpp"

#include <iostream>

#define MAX_TRIANGLES 2000
#define MAX_SPHERES 10
#define MAX_LIGHTS 10

char *filename=0;

//different display modes
#define MODE_DISPLAY 1
#define MODE_JPEG 2
int mode=MODE_DISPLAY;

//you may want to make these smaller for debugging purposes
#define WIDTH 640
#define HEIGHT 480
//#define WIDTH 320
//#define HEIGHT 240

//the field of view of the camera
#define fov 60.0

unsigned char buffer[HEIGHT][WIDTH][3];

struct Vertex
{
  double position[3];
  double color_diffuse[3];
  double color_specular[3];
  double normal[3];
  double shininess;
};

typedef struct _Triangle
{
  struct Vertex v[3];
} Triangle;

typedef struct _Sphere
{
  double position[3];
  double color_diffuse[3];
  double color_specular[3];
  double shininess;
  double radius;
} Sphere;

typedef struct _Light
{
  double position[3];
  double color[3];
} Light;

Triangle triangles[MAX_TRIANGLES];
Sphere spheres[MAX_SPHERES];
Light lights[MAX_LIGHTS];
double ambient_light[3];

int num_triangles=0;
int num_spheres=0;
int num_lights=0;

void plot_pixel_display(int x,int y,unsigned char r,unsigned char g,unsigned char b);
void plot_pixel_jpeg(int x,int y,unsigned char r,unsigned char g,unsigned char b);
void plot_pixel(int x,int y,unsigned char r,unsigned char g,unsigned char b);

const double BIAS = 1e-16;

double toRadians(const double degree)
{
	return degree * 3.14159265358979323846 / 180.0;
}

void subtract_vectors(const double first[3], const double second[3], double result[3])
{
	result[0] = first[0] - second[0];
	result[1] = first[1] - second[1];
	result[2] = first[2] - second[2];
}

void add_vectors(const double first[3], const double second[3], double result[3])
{
	result[0] = first[0] + second[0];
	result[1] = first[1] + second[1];
	result[2] = first[2] + second[2];
}

void multiply_by_constant(double vec[3], const double constant)
{
	vec[0] *= constant; vec[1] *= constant; vec[2] *= constant;
}

void multiply_vectors(const double first[3], const double second[3], double result[3])
{
	result[0] = first[0] * second[0];
	result[1] = first[1] * second[1];
	result[2] = first[2] * second[2];
}

double dotProduct(const double first[3], const double second[3])
{
	return ((first[0] * second[0]) + (first[1] * second[1]) + (first[2] * second[2]));
}

void crossProduct(const double first[3], const double second[3], double result[3])
{
	result[0] = first[1] * second[2] - first[2] * second[1];
	result[1] = first[2] * second[0] - first[0] * second[2];
	result[2] = first[0] * second[1] - first[1] * second[0];
}

void normalize(double vec[3])
{
	double xN = vec[0] * vec[0];
	double yN = vec[1] * vec[1];
	double zN = vec[2] * vec[2];
	double add = xN + yN + zN;
	add = sqrt(add);
	vec[0] = vec[0] / add;
	vec[1] = vec[1] / add;
	vec[2] = vec[2] / add;
}

void print_double(const double toPrint)
{
	std::string str = std::to_string(toPrint);
	std::cout << str << std::endl;
}

void printVec(const double vec[3])
{
	std::string x = std::to_string(vec[0]);
	std::string y = std::to_string(vec[1]);
	std::string z = std::to_string(vec[2]);
	std::cout << "<" << x << ", " << y << ", " << z << ">" << std::endl;
}

void findIntersection(const double ray[3], const double hit, const double origin[3], double result[3])
{
	result[0] = (ray[0] * hit) + origin[0];
	result[1] = (ray[1] * hit) + origin[1];
	result[2] = (ray[2] * hit) + origin[2];
}

double magnitude(const double vec[3])
{
	double x = vec[0] * vec[0];
	double y = vec[1] * vec[1];
	double z = vec[2] * vec[2];
	return sqrt(x + y + z);
}

double sphere_intersect(const double origin[3], const double vector[3], const double center[3], const double radius)
{
	double oc[3];
	subtract_vectors(origin, center, oc);
	double a = dotProduct(vector, vector);
	double b = 2.0 * dotProduct(oc, vector);
	double c = dotProduct(oc, oc) - radius * radius;
	double discriminant = b * b - 4 * a * c;
	if (discriminant < 0)
	{
		return -1.0;
	}
	else
	{
		return (-b - sqrt(discriminant)) / (2.0 * a);
	}
}

double triangle_intersect(const double origin[3], const double vector[3], const Triangle triangle)
{
	double A[3] = { triangle.v[0].position[0], triangle.v[0].position[1], triangle.v[0].position[2] };
	double B[3] = { triangle.v[1].position[0], triangle.v[1].position[1], triangle.v[1].position[2] };
	double C[3] = { triangle.v[2].position[0], triangle.v[2].position[1], triangle.v[2].position[2] };

	double AB[3];
	subtract_vectors(B, A, AB);
	double BC[3];
	subtract_vectors(C, B, BC);
	double CA[3];
	subtract_vectors(A, C, CA);
	double AC[3];
	subtract_vectors(C, A, AC);

	double normal[3];
	crossProduct(AB, AC, normal);

	normalize(normal);

	//check if parallel to triangle plane
	double dot = dotProduct(normal, vector);
	if (std::abs(dot) < BIAS) return -1.0;

	//compute d
	double dist[3];
	subtract_vectors(A, origin, dist);
	double d = dotProduct(dist, normal);

	//if t is negative, no intersection found (behind ray)
	double t = d / dot;
	if (t < 0.0) return -1.0;

	double intersect[3];
	findIntersection(vector, t, origin, intersect);
	double Ai[3]; double Bi[3]; double Ci[3];
	subtract_vectors(intersect, A, Ai); subtract_vectors(intersect, B, Bi); subtract_vectors(intersect, C, Ci);

	double cross1[3]; crossProduct(AB, Ai, cross1);
	double cross2[3]; crossProduct(BC, Bi, cross2);
	double cross3[3]; crossProduct(CA, Ci, cross3);
	//if dot prod of normal and vec betwwen two points does not face triangle, do not draw triangle
	if (dotProduct(normal, cross1) < 0.0 || dotProduct(normal, cross2) < 0.0 || dotProduct(normal, cross3) < 0.0) return -1.0;

	return t;
}

void reflect(const double normal[3], const double L[3], double result[3])
{
	double tempNorm[3] = { normal[0], normal[1], normal[2] };
	double tempL[3] = { L[0], L[1], L[2] };

	double twoDot = dotProduct(tempL, tempNorm) * 2;
	//normal[0] = normal[0] * twoDot; normal[1] = normal[1] * twoDot; normal[2] = normal[2] * twoDot;
	multiply_by_constant(tempNorm, twoDot);
	subtract_vectors(tempNorm, tempL, result);
}

double LdotN(const double lightVector[3], const double normal[3])
{
	double ret = dotProduct(lightVector, normal);
	if (ret > 1.0) return 1.0;
	if (ret < 0.0) return 0.0;
	return ret;
}

double RdotV(const double n[3], const double L[3], const double intersect[3], const double origin[3])
{
	double R[3];
	reflect(n, L, R);
	//multiply_by_constant(R, lightMag);
	normalize(R);

	double V[3];
	subtract_vectors(origin, intersect, V);
	normalize(V);

	double rightDot = dotProduct(R, V);
	if (rightDot < 0.0) rightDot = 0.0;
	if (rightDot > 1.0) rightDot = 1.0;
	return rightDot;
}


void sphere_phong(const Sphere sphere, const Light light, const double ray[3], const double hit, const double origin[3], double result[3])
{
	// I = lightColor * (kd * (L dot N) + ks * (R dot V)^a)
	// kd is diffuse coeff for sphere, L is unit vector to light, n is surface normal
	// ks is specular coeff for sphere, R is l reflected about n, V is vector to viewer, a is shininess coeff
	double lightColor[3] = { light.color[0], light.color[1], light.color[2] };
	
	double kd[3] = { sphere.color_diffuse[0], sphere.color_diffuse[1], sphere.color_diffuse[2] };
	
	double intersect[3];
	findIntersection(ray, hit, origin, intersect);
	double temp[3];
	subtract_vectors(light.position, intersect, temp);
	normalize(temp);
	double L[3] = { temp[0], temp[1], temp[2] };
	

	double n[3];
	subtract_vectors(intersect, sphere.position, n);
	normalize(n);

	double ks[3] = { sphere.color_specular[0], sphere.color_specular[1], sphere.color_specular[2] };

	double a = sphere.shininess;
	
	double leftDot = LdotN(L, n);
	multiply_by_constant(kd, leftDot);
	// kd is now kd * (L dot N)


	double rightDot = RdotV(n, L, intersect, origin);
	rightDot = pow(rightDot, a);
	multiply_by_constant(ks, rightDot);
	// ks is now ks * (R dot V)^a

	double p[3];
	add_vectors(kd, ks, p);
	
	multiply_vectors(lightColor, p, result);
}

void triangle_phong(const Triangle triangle, const Light light, const double ray[3], const double hit, const double origin[3], double result[3])
{
	double A[3] = { triangle.v[0].position[0], triangle.v[0].position[1], triangle.v[0].position[2] };
	double B[3] = { triangle.v[1].position[0], triangle.v[1].position[1], triangle.v[1].position[2] };
	double C[3] = { triangle.v[2].position[0], triangle.v[2].position[1], triangle.v[2].position[2] };

	double AB[3];
	subtract_vectors(B, A, AB);
	double AC[3];
	subtract_vectors(C, A, AC);

	double planar[3];
	crossProduct(AB, AC, planar);

	double denom = dotProduct(planar, planar);

	double intersection[3];
	findIntersection(ray, hit, origin, intersection);

	// get u and v
	double BC[3]; subtract_vectors(C, B, BC);
	double distB[3]; subtract_vectors(intersection, B, distB);
	double crossCB[3]; crossProduct(BC, distB, crossCB);

	double CA[3]; subtract_vectors(A, C, CA);
	double distC[3]; subtract_vectors(intersection, C, distC);
	double crossAC[3]; crossProduct(CA, distC, crossAC);

	//barycentric coordinates
	double u = dotProduct(planar, crossCB) / denom; //Alpha
	double v = dotProduct(planar, crossAC) / denom; //Beta
	double w = 1.0 - u - v; //Gamma

	double triangleNormal[3] = { u * triangle.v[0].normal[0] + v * triangle.v[1].normal[0] + w * triangle.v[2].normal[0],
		u * triangle.v[0].normal[1] + v * triangle.v[1].normal[1] + w * triangle.v[2].normal[1],
		u * triangle.v[0].normal[2] + v * triangle.v[1].normal[2] + w * triangle.v[2].normal[2]
	};
	normalize(triangleNormal);

	//normalized light vector
	double lightPos[3] = { light.position[0], light.position[1], light.position[2] };
	double L[3]; subtract_vectors(lightPos, intersection, L);
	normalize(L);

	double kd[3] = { u * triangle.v[0].color_diffuse[0] + v * triangle.v[1].color_diffuse[0] + w * triangle.v[2].color_diffuse[0],
		u * triangle.v[0].color_diffuse[1] + v * triangle.v[1].color_diffuse[1] + w * triangle.v[2].color_diffuse[1],
		u * triangle.v[0].color_diffuse[2] + v * triangle.v[1].color_diffuse[2] + w * triangle.v[2].color_diffuse[2]
	};

	double ks[3] = { u * triangle.v[0].color_specular[0] + v * triangle.v[1].color_specular[0] + w * triangle.v[2].color_specular[0],
		u * triangle.v[0].color_specular[1] + v * triangle.v[1].color_specular[1] + w * triangle.v[2].color_specular[1],
		u * triangle.v[0].color_specular[2] + v * triangle.v[1].color_specular[2] + w * triangle.v[2].color_specular[2]
	};

	if (kd[0] > 1.0) kd[0] = 1.0;
	if (kd[1] > 1.0) kd[1] = 1.0;
	if (kd[2] > 1.0) kd[2] = 1.0;
	if (kd[0] < 0.0) kd[0] = 0.0;
	if (kd[1] < 0.0) kd[1] = 0.0;
	if (kd[2] < 0.0) kd[2] = 0.0;

	if (ks[0] > 1.0) ks[0] = 1.0;
	if (ks[1] > 1.0) ks[1] = 1.0;
	if (ks[2] > 1.0) ks[2] = 1.0;
	if (ks[0] < 0.0) ks[0] = 0.0;
	if (ks[1] < 0.0) ks[1] = 0.0;
	if (ks[2] < 0.0) ks[2] = 0.0;

	double a = u * triangle.v[0].shininess + v * triangle.v[1].shininess + w * triangle.v[2].shininess;

	double leftDot = LdotN(L, triangleNormal);
	multiply_by_constant(kd, leftDot);

	double rightDot = RdotV(triangleNormal, L, intersection, origin);
	rightDot = pow(rightDot, a);
	multiply_by_constant(ks, rightDot);

	double p[3];
	add_vectors(kd, ks, p);

	double lightColor[3] = { light.color[0], light.color[1], light.color[2] };

	multiply_vectors(lightColor, p, result);

}

//MODIFY THIS FUNCTION
void draw_scene()
{
  unsigned int x,y;
  //simple output
  //for(x=0; x<WIDTH; x++)
  //{
  //  glPointSize(2.0);  
  //  glBegin(GL_POINTS);
  //  for(y=0;y < HEIGHT;y++)
  //  {
  //    plot_pixel(x,y,x%256,y%256,(x+y)%256);
  //  }
  //  glEnd();
  //  glFlush();
  //}
  
  
  //stuff for the ray generation
  double a = (double)WIDTH / (double)HEIGHT;
  double xVal = a * tan(toRadians(fov) / 2);
  double xVal2 = xVal * -1;
  double yVal = tan(toRadians(fov) / 2);
  double yVal2 = yVal * -1;
  double zVal = -1.0;

  double xDist = xVal - xVal2;
  double yDist = yVal - yVal2;

  print_double(a);
  print_double(xVal2);
  print_double(xVal);
  print_double(yVal2);
  print_double(yVal);
  print_double(xDist);
  print_double(yDist);


  //test
  int total = 0;


  for (y = 0; y < HEIGHT; y++)
  {
	  glPointSize(2.0);
	  glBegin(GL_POINTS);
	  for (x = 0; x < WIDTH; x++)
	  {
		  double rayOrigin[3] = { 0.0, 0.0, 0.0 };
		  double rayVector[3] = { 0.0, 0.0, 0.0 };

		  double rayX = xDist * (x / (double)WIDTH);
		  double rayY = yDist * (y / (double)HEIGHT);
		  rayX += xVal2;
		  rayY += yVal2;

		  rayVector[0] = rayX;
		  rayVector[1] = rayY;
		  rayVector[2] = zVal;

		  normalize(rayVector);

		  double total_color[3] = { 0.0, 0.0, 0.0 };

		  double closest = -1.0;
		  int ind = 0;
		  bool isSphere = false;
		  //check for sphere intersect
		  for (size_t i = 0; i < num_spheres; i++)
		  {
			  double result = sphere_intersect(rayOrigin, rayVector, spheres[i].position, spheres[i].radius);

			  if (result < 0) continue;

			  if (closest == -1.0)
			  {
				  closest = result;
				  ind = i;
				  isSphere = true;
			  }
			  else if (result < closest)
			  {
				  closest = result;
				  ind = i;
				  isSphere = true;
			  }
			  
		  }
		  //check for triangle intersect
		  for(size_t t = 0; t < num_triangles; t++)
		  {
			  double result = triangle_intersect(rayOrigin, rayVector, triangles[t]);

			  if (result < 0) continue;
			  if (closest == -1.0)
			  {
				  closest = result;
				  ind = t;
				  isSphere = false;
			  }
			  else if (result < closest)
			  {
				  closest = result;
				  ind = t;
				  isSphere = false;
			  }
		  }
		  if (closest > 0)
		  {
			  //intersection found, now check for shadow stuff
			  for (size_t l = 0; l < num_lights; l++)
			  {
				  bool inShadow = false;
				  Light theLight = lights[l];
				  double intersect[3];
				  findIntersection(rayVector, closest, rayOrigin, intersect);
				  
				  double toLight[3];
				  subtract_vectors(theLight.position, intersect, toLight);
				  normalize(toLight);

				  for (size_t i = 0; i < num_spheres; i++)
				  {
					  if (i == ind && isSphere) continue;
					  double sh = sphere_intersect(intersect, toLight, spheres[i].position, spheres[i].radius);
					  if (sh > 0)
					  {
						  double new_intersect[3]; findIntersection(toLight, sh, intersect, new_intersect);
						  double ex[3]; subtract_vectors(new_intersect, intersect, ex);
						  double why[3]; subtract_vectors(theLight.position, intersect, why);
						  if (magnitude(ex) < magnitude(why))
						  {//make sure intersection point not behind the light
							  inShadow = true;
							  break;
						  }
					  }
				  }
				  for (size_t t = 0; t < num_triangles; t++)
				  {
					  if (t == ind && !isSphere) continue;
					  double tr = triangle_intersect(intersect, toLight, triangles[t]);
					  if (tr > 0)
					  {
						  double new_intersect[3]; findIntersection(toLight, tr, intersect, new_intersect);
						  double ex[3]; subtract_vectors(new_intersect, intersect, ex);
						  double why[3]; subtract_vectors(theLight.position, intersect, why);
						  if (magnitude(ex) < magnitude(why))
						  {//make sure intersection point not behind the light
							  inShadow = true;
							  break;
						  }
					  }
				  }

				  if (inShadow) continue;

				  if (isSphere)
				  {
					  double sphere_color[3];
					  sphere_phong(spheres[ind], lights[l], rayVector, closest, rayOrigin, sphere_color);
					  add_vectors(total_color, sphere_color, total_color);
				  }
				  else
				  {
					  double triangle_color[3];
					  triangle_phong(triangles[ind], lights[l], rayVector, closest, rayOrigin, triangle_color);
					  add_vectors(total_color, triangle_color, total_color);
				  }
				   
			  }

			  double final_color[3];

			  add_vectors(ambient_light, total_color, final_color);

			  if (final_color[0] > 1.0) final_color[0] = 1.0;
			  if (final_color[1] > 1.0) final_color[1] = 1.0;
			  if (final_color[2] > 1.0) final_color[2] = 1.0;

			  plot_pixel(x, y, final_color[0] * 255.0, final_color[1] * 255.0, final_color[2] * 255.0);
		  }
		  else
		  {
			  plot_pixel(x, y, 255.0, 255.0, 255.0);
		  }
	  }
	  glEnd();
	  glFlush();

  }
  printf("Done!\n"); fflush(stdout);
}

void plot_pixel_display(int x,int y,unsigned char r,unsigned char g,unsigned char b)
{
  glColor3f(((double)r)/256.f,((double)g)/256.f,((double)b)/256.f);
  glVertex2i(x,y);
}

void plot_pixel_jpeg(int x,int y,unsigned char r,unsigned char g,unsigned char b)
{
  buffer[HEIGHT-y-1][x][0]=r;
  buffer[HEIGHT-y-1][x][1]=g;
  buffer[HEIGHT-y-1][x][2]=b;
}

void plot_pixel(int x,int y,unsigned char r,unsigned char g, unsigned char b)
{
  plot_pixel_display(x,y,r,g,b);
  if(mode == MODE_JPEG)
      plot_pixel_jpeg(x,y,r,g,b);
}

/* Write a jpg image from buffer*/
void save_jpg()
{
	if (filename == NULL)
		return;

	// Allocate a picture buffer // 
	cv::Mat3b bufferBGR = cv::Mat::zeros(HEIGHT, WIDTH, CV_8UC3); //rows, cols, 3-channel 8-bit.
	printf("File to save to: %s\n", filename);

	// unsigned char buffer[HEIGHT][WIDTH][3];
	for (int r = 0; r < HEIGHT; r++) {
		for (int c = 0; c < WIDTH; c++) {
			for (int chan = 0; chan < 3; chan++) {
				unsigned char red = buffer[r][c][0];
				unsigned char green = buffer[r][c][1];
				unsigned char blue = buffer[r][c][2];
				bufferBGR.at<cv::Vec3b>(r,c) = cv::Vec3b(blue, green, red);
			}
		}
	}
	if (cv::imwrite(filename, bufferBGR)) {
		printf("File saved Successfully\n");
	}
	else {
		printf("Error in Saving\n");
	}
}

void parse_check(char *expected,char *found)
{
  if(stricmp(expected,found))
    {
      char error[100];
      printf("Expected '%s ' found '%s '\n",expected,found);
      printf("Parse error, abnormal abortion\n");
      exit(0);
    }

}

void parse_doubles(FILE*file, char *check, double p[3])
{
  char str[100];
  fscanf(file,"%s",str);
  parse_check(check,str);
  fscanf(file,"%lf %lf %lf",&p[0],&p[1],&p[2]);
  printf("%s %lf %lf %lf\n",check,p[0],p[1],p[2]);
}

void parse_rad(FILE*file,double *r)
{
  char str[100];
  fscanf(file,"%s",str);
  parse_check("rad:",str);
  fscanf(file,"%lf",r);
  printf("rad: %f\n",*r);
}

void parse_shi(FILE*file,double *shi)
{
  char s[100];
  fscanf(file,"%s",s);
  parse_check("shi:",s);
  fscanf(file,"%lf",shi);
  printf("shi: %f\n",*shi);
}

int loadScene(char *argv)
{
  FILE *file = fopen(argv,"r");
  int number_of_objects;
  char type[50];
  int i;
  Triangle t;
  Sphere s;
  Light l;
  fscanf(file,"%i",&number_of_objects);

  printf("number of objects: %i\n",number_of_objects);
  char str[200];

  parse_doubles(file,"amb:",ambient_light);

  for(i=0;i < number_of_objects;i++)
    {
      fscanf(file,"%s\n",type);
      printf("%s\n",type);
      if(stricmp(type,"triangle")==0)
	{

	  printf("found triangle\n");
	  int j;

	  for(j=0;j < 3;j++)
	    {
	      parse_doubles(file,"pos:",t.v[j].position);
	      parse_doubles(file,"nor:",t.v[j].normal);
	      parse_doubles(file,"dif:",t.v[j].color_diffuse);
	      parse_doubles(file,"spe:",t.v[j].color_specular);
	      parse_shi(file,&t.v[j].shininess);
	    }

	  if(num_triangles == MAX_TRIANGLES)
	    {
	      printf("too many triangles, you should increase MAX_TRIANGLES!\n");
	      exit(0);
	    }
	  triangles[num_triangles++] = t;
	}
      else if(stricmp(type,"sphere")==0)
	{
	  printf("found sphere\n");

	  parse_doubles(file,"pos:",s.position);
	  parse_rad(file,&s.radius);
	  parse_doubles(file,"dif:",s.color_diffuse);
	  parse_doubles(file,"spe:",s.color_specular);
	  parse_shi(file,&s.shininess);

	  if(num_spheres == MAX_SPHERES)
	    {
	      printf("too many spheres, you should increase MAX_SPHERES!\n");
	      exit(0);
	    }
	  spheres[num_spheres++] = s;
	}
      else if(stricmp(type,"light")==0)
	{
	  printf("found light\n");
	  parse_doubles(file,"pos:",l.position);
	  parse_doubles(file,"col:",l.color);

	  if(num_lights == MAX_LIGHTS)
	    {
	      printf("too many lights, you should increase MAX_LIGHTS!\n");
	      exit(0);
	    }
	  lights[num_lights++] = l;
	}
      else
	{
	  printf("unknown type in scene description:\n%s\n",type);
	  exit(0);
	}
    }
  return 0;
}

void display()
{

}

void init()
{
  glMatrixMode(GL_PROJECTION);
  glOrtho(0,WIDTH,0,HEIGHT,1,-1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glClearColor(0,0,0,0);
  glClear(GL_COLOR_BUFFER_BIT);
}

void idle()
{
  //hack to make it only draw once
  static int once=0;
  if(!once)
  {
      draw_scene();
      if(mode == MODE_JPEG)
	save_jpg();
    }
  once=1;
}

int main (int argc, char ** argv)
{
  if (argc<2 || argc > 3)
  {  
    printf ("usage: %s <scenefile> [jpegname]\n", argv[0]);
    exit(0);
  }
  if(argc == 3)
    {
      mode = MODE_JPEG;
      filename = argv[2];
    }
  else if(argc == 2)
    mode = MODE_DISPLAY;
  
  glutInit(&argc,argv);
  loadScene(argv[1]);

  glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);
  glutInitWindowPosition(0,0);
  glutInitWindowSize(WIDTH,HEIGHT);
  int window = glutCreateWindow("Ray Tracer");
  glutDisplayFunc(display);
  glutIdleFunc(idle);
  init();
  glutMainLoop();
}
