#include <GL/freeglut.h>
#include <GL/glu.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <glm/glm.hpp>

using namespace std;
using glm::vec3;
using glm::vec4;
using glm::mat4;
int currentSegment = 0;
float currentT = 0.0;

struct Vertex {
	float x, y, z;
};

struct Poly {
	vector<int> indices;
};

vector<Vertex> vertices;
vector<Poly> polygons;
vector<Vertex> points;
vec3 minCoords = vec3(FLT_MAX);
vec3 maxCoords = vec3(-FLT_MAX);
mat4 M = (1.0f / 6.0f) * mat4(
	-1, 3, -3, 1,
	3, -6, 0, 4,
	-3, 3, 3, 1,
	1, 0, 0, 0
);

void readAndloadObj(const string& filename) {

	ifstream file(filename);
	string line;

	while (getline(file, line)) {

		if (line[0] == 'v' && line[1] == ' ') {
			Vertex vertex;
			sscanf(line.c_str(), "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);

			minCoords.x = min(minCoords.x, vertex.x);
			minCoords.y = min(minCoords.y, vertex.y);
			minCoords.z = min(minCoords.z, vertex.z);

			maxCoords.x = max(maxCoords.x, vertex.x);
			maxCoords.y = max(maxCoords.y, vertex.y);
			maxCoords.z = max(maxCoords.z, vertex.z);

			//printf("Vertex: (%f, %f, %f)\n", vertex.x, vertex.y, vertex.z);

			vertices.push_back(vertex);

		} else if(line[0] == 'f'){
			Poly polygon;
			int index;
			printf("%c", line[1]);
			char* value = strtok(&line[1], " ");

			while (value != nullptr) {
				sscanf(value, "%d", &index);
				polygon.indices.push_back(index - 1);
				value = strtok(nullptr, " ");
			}

			polygons.push_back(polygon);
		}
	}

}

void loadBSpline(const string& filename) {

	points.clear();

	ifstream file(filename);
	Vertex point;

	while (file >> point.x >> point.y >> point.z) {
		point.x = point.x;
		point.y = point.y;
		point.z = point.z;
		points.push_back(point);
		//printf("Vertex: (%f, %f, %f)\n", point.x, point.y, point.z);
	}

}

vec3 computeTangent(float t, const vector<Vertex>& controlPoints) {
	vec4 T(3*t * t, 2 * t, 1, 0);

	vec4 Rx(controlPoints[0].x, controlPoints[1].x, controlPoints[2].x, controlPoints[3].x);
	vec4 Ry(controlPoints[0].y, controlPoints[1].y, controlPoints[2].y, controlPoints[3].y);
	vec4 Rz(controlPoints[0].z, controlPoints[1].z, controlPoints[2].z, controlPoints[3].z);

	vec3 tangent;
	tangent.x = dot(T, M * Rx);
	tangent.y = dot(T, M * Ry);
	tangent.z = dot(T, M * Rz);

	return normalize(tangent);
}

void computeRotationAxisAndAngle(const vec3& startOrientation, const vec3& endOrientation, vec3& axis, float& angle) {
	axis = cross(startOrientation, endOrientation);
	float cosTheta = dot(startOrientation, endOrientation) / length(startOrientation) * length(endOrientation);
	angle = acos(cosTheta) * (180.0f / 3.14159265f);
}

/*mat4 computeDCM(const vec3& tangent) {
	vec3 up(0, 1, 0);  
	vec3 right = cross(up, tangent);  
	right = normalize(right); 
	vec3 correctedUp = cross(tangent, right);  

	mat4 DCM(
		right.x, right.y, right.z, 0,
		correctedUp.x, correctedUp.y, correctedUp.z, 0,
		tangent.x, tangent.y, tangent.z, 0,
		0, 0, 0, 1
	);

	return DCM;
}*/

void update(int value) {
	currentT += 0.01;
	if (currentT > 1.0) {
		currentT = 0.0;
		currentSegment++;
		if (currentSegment >= points.size() - 3) {
			currentSegment = 0; 
		}
	}
	glutPostRedisplay();
	glutTimerFunc(15, update, 0); 
}

void display() {
	
	//clear window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//set projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(45, (float)800 / 600, 0.1, 30);

	//set model-view matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < points.size() - 3; i++) {
		
		vector<Vertex> segmentPoints = { points[i], points[i + 1], points[i + 2], points[i + 3] };
		mat4 R = mat4(
			segmentPoints[0].x, segmentPoints[1].x, segmentPoints[2].x, segmentPoints[3].x,
			segmentPoints[0].y, segmentPoints[1].y, segmentPoints[2].y, segmentPoints[3].y,
			segmentPoints[0].z, segmentPoints[1].z, segmentPoints[2].z, segmentPoints[3].z,
			1.0f, 1.0f, 1.0f, 1.0f
		);

		for (float t = 0; t <= 1.0; t += 0.01) {

			vec4 T(t * t * t, t * t, t, 1);

			vec4 result = T * M * R;
			Vertex position = { result.x, result.y, result.z };
			glVertex3f(position.x/10, position.y/10, position.z/10);

		}

	}
	glEnd();

	vector<Vertex> segmentPoints = { points[currentSegment], points[currentSegment + 1], points[currentSegment + 2], points[currentSegment + 3] };
	vec4 T(currentT * currentT * currentT * currentT, currentT * currentT, currentT, 1);
	mat4 R = mat4(
		segmentPoints[0].x, segmentPoints[1].x, segmentPoints[2].x, segmentPoints[3].x,
		segmentPoints[0].y, segmentPoints[1].y, segmentPoints[2].y, segmentPoints[3].y,
		segmentPoints[0].z, segmentPoints[1].z, segmentPoints[2].z, segmentPoints[3].z,
		1.0f, 1.0f, 1.0f, 1.0f
	);

	vec3 result = T * M * R;
	Vertex position = { result.x, result.y, result.z };
	vec3 tangent = computeTangent(currentT, segmentPoints);
	vec3 axis;
	float angle;
	computeRotationAxisAndAngle(vec3(0, 0, 1), tangent, axis, angle);
	//mat4 orientationDCM = computeDCM(tangent);

	glPushMatrix();
	glTranslatef(position.x, position.y, position.z);
	glRotatef(angle, axis.x, axis.y, axis.z);
	//glMultMatrixf(&orientationDCM[0][0]);

	glBegin(GL_TRIANGLES);
	for (const auto& polygon : polygons) {
		for (const auto& index : polygon.indices) {
			const auto& vertex = vertices[index];
			glColor3f(1.0, 0.0, 0.0);
			glVertex3f(vertex.x/10, vertex.y/10, vertex.z/10);
		}
	}
	glEnd();
	glPopMatrix();

	glutSwapBuffers();
}


int main(int argc, char** argv) {

	readAndloadObj("C:\\Users\\doric\\Desktop\\RACAN\\RA-labosi\\RA-labosi\\Primjeri\\teddy.obj.txt");
	loadBSpline("C:\\Users\\doric\\Desktop\\RACAN\\RA-labosi\\RA-labosi\\Primjeri\\bspline_points.txt");

	vec3 center = (minCoords + maxCoords) * 0.5f;
	float maxExtent = max(maxCoords.x - minCoords.x, max(maxCoords.y - minCoords.y, maxCoords.z - minCoords.z));
	float scale = 2.0f / maxExtent;

	for (auto& vertex : vertices) {
		vertex.x = (vertex.x - center.x) * scale;
		vertex.y = (vertex.y - center.y) * scale;
		vertex.z = (vertex.z - center.z) * scale;
		printf("Vertex: (%f, %f, %f)\n", vertex.x, vertex.y, vertex.z);
	}

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Window");
	glutDisplayFunc(display);
	glutTimerFunc(0, update, 0);
	glEnable(GL_DEPTH_TEST);

	glutMainLoop();
	return 0;

}