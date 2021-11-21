	#version 420
layout( std140, binding = 0 ) uniform g_constantBuffer_frame
{
	mat4 g_transform_worldToCamera;
	mat4 g_transform_cameraToProjected;
	float g_elapsedSecondCount_systemTime;
	float g_elapsedSecondCount_simulationTime;
	vec2 g_padding;
};
layout( std140, binding = 2 ) uniform g_constantBuffer_perDrawCall
{
    mat4 g_transform_localToWorld;
};
out vec4 o_color;
void main()
{
	o_color = vec4(
		1.0, 1.0, 1.0,
		1.0);
	float n1 = cos(g_elapsedSecondCount_simulationTime);
	if (n1 < 0) {
		n1 = n1 * -1;
	}
	float n2 = sin(g_elapsedSecondCount_simulationTime);
	if (n2 < 0) {
		n2 = n2 * -1;
	}
	o_color.g = o_color.g * n1;
	o_color.r = o_color.r * n2;
}
