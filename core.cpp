#include "libs.h"
#include "math.h"
#include "core.h"

//Camera turn speed
const float turnSpeed = 0.05;
const float moveSpeed = 4.0;
const int dim = 512; // heightmap is dim x dim pixels

Core::Core(InputState & inputstate) : inputstate(inputstate), positions(dim * dim * 6), normals(dim * dim * 6), enabled(dim * dim * 6)
{
}

bool Core::initialize()
{
	//attempt to load our terrain data
	if (!loadTerrain(Vector3 (0.0f, 0.0f, 0.0f)))
		return false;

	//attempt to load our teapot data
	//if (!loadTeapot(3.0f, 200.0f, 300.0f, 0.0f))
	//	return false;

	//if (!teapotshader.loadShaderProgram("teapot"))
	//{
	//	std::cout << teapotshader.getErrorMessage() << std::endl;
	//	return false;
	//}

	if (!terrainshader.loadShaderProgram("terrain"))
	{
		std::cout << terrainshader.getErrorMessage() << std::endl;
		return false;

	}

	//enable depth testing and set to "Less or Equal" mode
	//only pixels that are closer or equally close are shown
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);

	playerPos = Vector3(0.0f, -600.0f, 150.0f);
	localx = Vector3(0.0f, 1.0f, 0.0f); 
	localy = Vector3(1.0f, 0.0f, 0.0f);
	localz = Vector3(0.0f, 0.0f, 1.0f);
	
	viewpoint = playerPos;
	viewtarget = viewpoint + localx * 100.0f;

	light_position = Vector3(500.0f, 0.0f, 100.0f);
	light_intensity = Vector3(0.6f, 0.6f, 0.6f);

	time = 0.0;

	return true;
}

void Core::update(int width, int height, double deltatime)
{
	//turn camera left by pressing left arrow
	if (inputstate.keysdown[SDLK_LEFT]) {
		localx = localx.rotateZ(turnSpeed);
		localy = localy.rotateZ(turnSpeed);
		localz = localz.rotateZ(turnSpeed);
		//localx = localx * cos(-turnSpeed) + (localx ^ localz) * sin(-turnSpeed) + localz * (localx * localz) * (1 - cos(-turnSpeed));
		//localy = localy * cos(-turnSpeed) + (localy ^ localz) * sin(-turnSpeed) + localz * (localy * localz) * (1 - cos(-turnSpeed));
	}
	//turn camera right by pressing right arrow
	if (inputstate.keysdown[SDLK_RIGHT]) {
		localx = localx.rotateZ(-turnSpeed);
		localy = localy.rotateZ(-turnSpeed);
		localz = localz.rotateZ(-turnSpeed);
		//localx = localx * cos(turnSpeed) + (localx ^ localz) * sin(turnSpeed) + localz * (localx * localz) * (1 - cos(turnSpeed));
		//localy = localy * cos(turnSpeed) + (localy ^ localz) * sin(turnSpeed) + localz * (localy * localz) * (1 - cos(turnSpeed));
	}
	//turn camera up by pressing up arrow
	if (inputstate.keysdown[SDLK_UP]) {
		localx = localx * cos(-turnSpeed) + (localx ^ localy) * sin(-turnSpeed) + localy * (localx * localy) * (1 - cos(-turnSpeed));
		localz = localz * cos(-turnSpeed) + (localz ^ localy) * sin(-turnSpeed) + localy * (localz * localy) * (1 - cos(-turnSpeed));
	}
	//turn camera down by pressing down arrrow
	if (inputstate.keysdown[SDLK_DOWN]) {
		localx = localx * cos(turnSpeed) + (localx ^ localy) * sin(turnSpeed) + localy * (localx * localy) * (1 - cos(turnSpeed));
		localz = localz * cos(turnSpeed) + (localz ^ localy) * sin(turnSpeed) + localy * (localz * localy) * (1 - cos(turnSpeed));
	}
	
	//move camera forwards by pressing "w"
	if (inputstate.keysdown[SDLK_w]) {
		playerPos += localx * moveSpeed;
	}
	//move camera backwards by pressing "s"
	if (inputstate.keysdown[SDLK_s]) {
		playerPos -= localx * moveSpeed;
	}
	//move camera left by pressing "a"
	if (inputstate.keysdown[SDLK_a]) {
		playerPos -= localy * moveSpeed;
	}
	//move camera right by pressing "d"
	if (inputstate.keysdown[SDLK_d]) {
		playerPos += localy * moveSpeed;
	}
	//move camera up by pressing "SPACE"
	if (inputstate.keysdown[SDLK_SPACE]) {
		//playerPos += localz * moveSpeed;
		playerPos += Vector3(0.0f, 0.0f, moveSpeed);
	}
	//move camera down by pressing "LSHIFT"
	if (inputstate.keysdown[SDLK_LSHIFT]) {
		//playerPos -= localz * moveSpeed;
		playerPos -= Vector3(0.0f, 0.0f, moveSpeed);
	}

	viewpoint = playerPos;
	viewtarget = viewpoint + localx * 100.0f;

	this->time += deltatime;
	this->width = width;
	this->height = height;
}

void Core::render()
{
	//set the size of the rendering area
	glViewport(0, 0, width, height);
	
	
	//modelview transform matrix
	Matrix4 modelviewmatrix = Matrix4::lookAtMatrix(viewpoint,
	                                                viewtarget,
	                                                Vector3(0.0f, 0.0f, 1.0f));
	//projection transform matrix
	Matrix4 projectionmatrix = Matrix4::perspectiveMatrix((float)M_PI_4, (float)width / height, 1.0f, 10000.0f);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

/*    ////////////////////////////////////////////////////////////
    //DRAW TEAPOT SECTION
    ////////////////////////////////////////////////////////////
	//use the teapot shader
	glUseProgram(teapotshader.getProgram());
	//set our matrices as "uniform" variables
	glUniformMatrix4fv(glGetUniformLocation(teapotshader.getProgram(), "matmodelview"), 1, GL_TRUE, modelviewmatrix.elements());
	glUniformMatrix4fv(glGetUniformLocation(teapotshader.getProgram(), "matprojection"), 1, GL_TRUE, projectionmatrix.elements());


	glUniform3f(glGetUniformLocation(teapotshader.getProgram(), "camera_position"), viewpoint.x(), viewpoint.y(), viewpoint.z());
	glUniform3f(glGetUniformLocation(teapotshader.getProgram(), "light_intensity"), light_intensity.x(), light_intensity.y(), light_intensity.z());
	glUniform3f(glGetUniformLocation(teapotshader.getProgram(), "light_position"), light_position.x(), light_position.y(), light_position.z());
	
	//set teapot materials (gold)
	glUniform3f(glGetUniformLocation(teapotshader.getProgram(), "material.ambient"), 0.24725f, 0.1995f, 0.0745f);
	glUniform3f(glGetUniformLocation(teapotshader.getProgram(), "material.diffuse"), 0.75164f, 0.60648f, 0.22648f);
	glUniform3f(glGetUniformLocation(teapotshader.getProgram(), "material.specular"), 0.628281f, 0.555802f, 0.366065f);
	glUniform1f(glGetUniformLocation(teapotshader.getProgram(), "material.shininess"), 0.4f * 128.0f);

	//set vertex position data
	glBindBuffer(GL_ARRAY_BUFFER, vertexbufferobject_teapot_position);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	//set vertex normal data
	glBindBuffer(GL_ARRAY_BUFFER, vertexbufferobject_teapot_normal);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//Draw teapot
	glDrawArrays(GL_QUADS, 0, teapot_count/2);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
*/
	
    ////////////////////////////////////////////////////////////
    //DRAW TERRAIN SECTION
    ////////////////////////////////////////////////////////////
	//switch to the terrain shader
	glUseProgram(terrainshader.getProgram());
	//set our matrices as "uniform" variables again
	glUniformMatrix4fv(glGetUniformLocation(terrainshader.getProgram(), "matmodelview"), 1, GL_TRUE, modelviewmatrix.elements());
	glUniformMatrix4fv(glGetUniformLocation(terrainshader.getProgram(), "matprojection"), 1, GL_TRUE, projectionmatrix.elements());


	glUniform3f(glGetUniformLocation(terrainshader.getProgram(), "camera_position"), viewpoint.x(), viewpoint.y(), viewpoint.z());
	glUniform3f(glGetUniformLocation(terrainshader.getProgram(), "light_intensity"), light_intensity.x(), light_intensity.y(), light_intensity.z());
	glUniform3f(glGetUniformLocation(terrainshader.getProgram(), "light_position"), light_position.x(), light_position.y(), light_position.z());
	glUniform1f(glGetUniformLocation(terrainshader.getProgram(), "time"), time);
	
	//set mountain materials (jade)
	glUniform3f(glGetUniformLocation(terrainshader.getProgram(), "mountain_mat.ambient"), 0.135f, 0.2225f, 0.1575f);
	glUniform3f(glGetUniformLocation(terrainshader.getProgram(), "mountain_mat.diffuse"), 0.54f, 0.89f, 0.63f);
	glUniform3f(glGetUniformLocation(terrainshader.getProgram(), "mountain_mat.specular"), 0.316228f, 0.316228f, 0.316228f);
	glUniform1f(glGetUniformLocation(terrainshader.getProgram(), "mountain_mat.shininess"), 0.1f * 128.0f);
	
	//set water materials (turquoise)
	glUniform3f(glGetUniformLocation(terrainshader.getProgram(), "water_mat.ambient"), 0.1f, 0.18725f, 0.1745);
	glUniform3f(glGetUniformLocation(terrainshader.getProgram(), "water_mat.diffuse"), 0.396f, 0.54151f, 0.89102f);
	glUniform3f(glGetUniformLocation(terrainshader.getProgram(), "water_mat.specular"), 0.297254f, 0.30829f,	0.306678f);
	glUniform1f(glGetUniformLocation(terrainshader.getProgram(), "water_mat.shininess"), 0.3f * 128.0f);
	
	//set vertex position data
	glBindBuffer(GL_ARRAY_BUFFER, vertexbufferobject_terrain_position);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//set vertex normal data
	glBindBuffer(GL_ARRAY_BUFFER, vertexbufferobject_terrain_normal);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//Draw terrain
	glDrawArrays(GL_TRIANGLES, 0, (dim+1) * (dim+1) * 6);
	
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}


Vector3 Core::getTerrainVertex(std::vector<unsigned char> & heightmap, size_t x, size_t y)
{
        //return the position of a point on the heightmap
        //The y axis is reversed here
        return Vector3(x - ((float)dim/2), ((float)dim/2) -y, (float)heightmap[y * (dim+1) + x]);
}


Vector3 Core::getTerrainNormal(std::vector<unsigned char> & heightmap, size_t x, size_t y)
{
        //get the height difference along the X and Y directions
        float dx, dy;
        if (x == 0)
                dx = (float)heightmap[y * (dim+1) + (x + 1)] * 0.5f - (float)heightmap[y * (dim+1) + x] * 0.5f;
        else if (x == dim)
                dx = (float)heightmap[y * (dim+1) + x] * 0.5f - (float)heightmap[y * (dim+1) + (x - 1)] * 0.5f;
        else
                dx = (float)heightmap[y * (dim+1) + (x + 1)] * 0.5f - (float)heightmap[y * (dim+1) + (x - 1)] * 0.5f;
        if (y == 0)
                dy = (float)heightmap[(y + 1) * (dim+1) + x] * 0.5f - (float)heightmap[y * (dim+1) + x] * 0.5f;
        else if (y == dim)
                dy = (float)heightmap[y * (dim+1) + x] * 0.5f - (float)heightmap[(y - 1) * (dim+1) + x] * 0.5f;
        else
                dy = (float)heightmap[(y + 1) * (dim+1) + x] * 0.5f - (float)heightmap[(y - 1) * (dim+1) + x] * 0.5f;
        
        //use this to compute a normal vector:
        //u = (1, 0, dx); v = (0, 1, dy)
        //n = || (u x v) ||  ->  normalize(u cross v)
        //becomes: n = || (-dx, -dy, 1) ||
        float sizefactor = 1.0f / sqrt(dx * dx + dy * dy + 1.0f);
        return Vector3(-dx * sizefactor, -dy * sizefactor, 1.0f * sizefactor);
}


bool Core::loadTerrain(Vector3 offset)
{
	//this array contains the heights at the terrain points
        std::vector<unsigned char> heightmap((dim+1) * (dim+1));
        
        //read heightmap image file into array "heightmap"
        std::ifstream terrainfile("heightmap.raw", std::ios::binary);
        terrainfile.read(reinterpret_cast<char *>(&heightmap[0]), (dim+1) * (dim+1));
        terrainfile.close();
        
        //large arrays of position and normal vectors
        //containing dim x dim tiles
        //each tile consists of 2 triangles (3 * 2 = 6 vectors)
        //std::vector<Vector3> positions(dim * dim * 6);
        //std::vector<Vector3> normals(dim * dim * 6);
        
        size_t x;   
        size_t y;
        for (y = 0; y < dim; y++) {
                for (x = 0; x < dim; x++) {
                        positions[(y * dim + x) * 6 + 0] = getTerrainVertex(heightmap, x, y) + offset;
                        positions[(y * dim + x) * 6 + 1] = getTerrainVertex(heightmap, x + 1, y) + offset;
                        positions[(y * dim + x) * 6 + 2] = getTerrainVertex(heightmap, x + 1, y + 1) + offset;
                        positions[(y * dim + x) * 6 + 3] = getTerrainVertex(heightmap, x + 1, y + 1) + offset;
                        positions[(y * dim + x) * 6 + 4] = getTerrainVertex(heightmap, x, y + 1) + offset;
                        positions[(y * dim + x) * 6 + 5] = getTerrainVertex(heightmap, x, y) + offset;
                
                        normals[(y * dim + x) * 6 + 0] = getTerrainNormal(heightmap, x, y);
                        normals[(y * dim + x) * 6 + 1] = getTerrainNormal(heightmap, x + 1, y);
                        normals[(y * dim + x) * 6 + 2] = getTerrainNormal(heightmap, x + 1, y + 1);
                        normals[(y * dim + x) * 6 + 3] = getTerrainNormal(heightmap, x + 1, y + 1);
                        normals[(y * dim + x) * 6 + 4] = getTerrainNormal(heightmap, x, y + 1);
                        normals[(y * dim + x) * 6 + 5] = getTerrainNormal(heightmap, x, y);
                        
                        enabled[(y * dim + x) * 6 + 0] = true;
                        enabled[(y * dim + x) * 6 + 1] = true;
                        enabled[(y * dim + x) * 6 + 2] = true;
                        enabled[(y * dim + x) * 6 + 3] = true;
                        enabled[(y * dim + x) * 6 + 4] = true;
                        enabled[(y * dim + x) * 6 + 5] = true;
                }
        }
                
                
        //Use the arrays "positions" and "normals" to fill two vertex buffer objects
        //These are essentially arrays of data to be stored in the GPU memory
        //First create a new vertex buffer object
        glGenBuffers(1, &vertexbufferobject_terrain_position);
        //then bind it, so that the next operation is performed on this object
        glBindBuffer(GL_ARRAY_BUFFER, vertexbufferobject_terrain_position);
        //send the data to the GPU 
        //arguments after GL_ARRAY_BUFFER: size of data in bytes, pointer to start of data, expected method of updating the data
        glBufferData(GL_ARRAY_BUFFER, dim * dim * 6 * 3 * sizeof(float), &positions[0], GL_STATIC_DRAW);
        
        //repeat of the above, now for the normal array
        glGenBuffers(1, &vertexbufferobject_terrain_normal);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbufferobject_terrain_normal);
        glBufferData(GL_ARRAY_BUFFER, dim * dim * 6 * 3 * sizeof(float), &normals[0], GL_STATIC_DRAW);
                
        return true;

}

void Core::updateVertices() 
{
	for (y = 1; y < dim-1; y++) {
                for (x = 1; x < dim-1; x++) {
                        enabled[(y * dim + x) * 6 + 0] = ((positions[(y * dim + x) * 6 + 0] - playerPos) * (positions[(y * dim + x) * 6 + 0] - playerPos) < 100);
                        enabled[(y * dim + x) * 6 + 1] = ((positions[(y * dim + x) * 6 + 0] - playerPos) * (positions[(y * dim + x) * 6 + 0] - playerPos) < 100);
                        enabled[(y * dim + x) * 6 + 2] = ((positions[(y * dim + x) * 6 + 0] - playerPos) * (positions[(y * dim + x) * 6 + 0] - playerPos) < 100);
                        enabled[(y * dim + x) * 6 + 3] = ((positions[(y * dim + x) * 6 + 0] - playerPos) * (positions[(y * dim + x) * 6 + 0] - playerPos) < 100);
                        enabled[(y * dim + x) * 6 + 4] = ((positions[(y * dim + x) * 6 + 0] - playerPos) * (positions[(y * dim + x) * 6 + 0] - playerPos) < 100);
                        enabled[(y * dim + x) * 6 + 5] = ((positions[(y * dim + x) * 6 + 0] - playerPos) * (positions[(y * dim + x) * 6 + 0] - playerPos) < 100);
                        
                        //TODO: does this ignore borders? 
                }
        }
}


/*
bool Core::loadTeapot(float scale, float x, float y, float z)
{
	//large arrays of position and normal vectors
	//each tile consists of 2 triangles (3 * 2 = 6 vectors)
	std::vector<Vector3> positions(teapot_count/6);
	std::vector<Vector3> normals(teapot_count/6);

	size_t i;
	for (i = 0; i < positions.size(); i++) {
		positions[i] = Vector3(teapot[i*6+0], teapot[i*6+1], teapot[i*6+2]);
		positions[i] *= scale;
		positions[i] += Vector3(x, y, z);
	}

	for (i = 0; i < normals.size(); i++){
		normals[i] = Vector3(teapot[i*6+3], teapot[i*6+4], -teapot[i*6+5]);
		normals[i] = normals[i].normalize();
	}
	
	//Use the arrays "positions" and "normals" to fill two vertex buffer objects
	//These are essentially arrays of data to be stored in the GPU memory
	//First create a new vertex buffer object
	glGenBuffers(1, &vertexbufferobject_teapot_position);
	//then bind it, so that the next operation is performed on this object
	glBindBuffer(GL_ARRAY_BUFFER, vertexbufferobject_teapot_position);
	//send the data to the GPU
	//arguments after GL_ARRAY_BUFFER: size of data in bytes, pointer to start of data, expected method of updating the data
	glBufferData(GL_ARRAY_BUFFER, 0.5 * teapot_count * sizeof(float), &positions[0], GL_STATIC_DRAW);
	
	//repeat of the above, now for the normal array
	glGenBuffers(1, &vertexbufferobject_teapot_normal);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbufferobject_teapot_normal);
	glBufferData(GL_ARRAY_BUFFER, 0.5 * teapot_count * sizeof(float), &normals[0], GL_STATIC_DRAW);

	return true;
}
*/
