#version 130
struct Material{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

uniform Material mountain_mat;
uniform Material water_mat;
 
// Output color
out vec4 fragcolor;

uniform vec3 light_position;
uniform vec3 light_intensity;
uniform vec3 camera_position;

// Variables from vertex to fragment shader
in vec3 _normal; //normal vector
in vec4 _position; //position vector


void main()
{
    //material selection
    Material mat;
    mat = mountain_mat;
    
    //ambient
    vec3 ambient = clamp(mat.ambient * light_intensity, 0.0, 1.0);

    //diffuse
    vec3 norm = normalize(_normal);
    vec3 light_dir = normalize(light_position - _position.xyz);
    float NdotL = dot(norm, light_dir);
    vec3 diffuse = clamp(mat.diffuse * NdotL * light_intensity, 0.0, 1.0);

    //specular
    vec3 specular = vec3(0.0, 0.0, 0.0);

    vec3 light = ambient + diffuse + specular;
    fragcolor = vec4(light, 1.0);	
}
