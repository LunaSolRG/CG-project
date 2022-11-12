#include "InputState.h"
#include "ShaderProgram.h"
#include "geometry.h"


class Core
{
public:
	Core(InputState & inputstate);
	//setup data
	bool initialize();
	//called before each render, deltatime is in seconds
	void update(int width, int height, double deltatime);
	//render the scene
	void render();

private:
	InputState & inputstate;

	int width, height;
	float time;

	Vector3 playerPos;
	Vector3 localx, localy, localz; //player's coordinate frame

	Vector3 viewpoint, viewtarget;
	
	std::vector<Vector3> positions;
        std::vector<Vector3> normals;
        std::vector<bool> enabled;

	//four buffers to store our vertex data
	//GLuint vertexbufferobject_teapot_position;
	//GLuint vertexbufferobject_teapot_normal;

	GLuint vertexbufferobject_terrain_position;
	GLuint vertexbufferobject_terrain_normal;

	ShaderProgram terrainshader;//, teapotshader;

	Vector3 light_position;
	Vector3 light_intensity;

	//bool loadTeapot(float scale, float x, float y, float z);
	bool loadTerrain(Vector3 offset);
	Vector3 getTerrainVertex(std::vector<unsigned char> & heightmap, size_t x, size_t y);
	Vector3 getTerrainNormal(std::vector<unsigned char> & heightmap, size_t x, size_t y);
	void updateVertices();
};
