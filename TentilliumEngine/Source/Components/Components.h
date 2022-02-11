#pragma once

#include "Transform.h"
#include "Hierarchy.h"


struct Render {
private:
	unsigned int shader;

};


struct LightEmitter { };
struct ShadowCaster { };
struct LightOccluder { };
struct CustomMaterial { };

/*
https://www.3dgep.com/forward-plus/#Forward
https://ubm-twvideo01.s3.amazonaws.com/o1/vault/gdc2015/presentations/Thomas_Gareth_Advancements_in_Tile-Based.pdf

tiled deferred rendering pipeline:
construct screenspace grid/tiles with fixed pixel size

1. deferred pre pass - render normal, specular, albedo, roughness into GBuffers
2. Find min and max depth of each tile
3. create per tile frustum between min and max depth
4. Cull lights outside of tile frustum
5. calculate all light that affect this tile


*/

// geometry pre pass
/*
render into geometry buffers
*/

// calculate the min and max depth for each tile
/**/

// calculate the per tile frustums created by the corner of each tile
/*
float maxDepthZ = float(float(maxDepth) / float(0xffffffffu));
float minDepthZ = float(minDepth / float(0xffffffffu));

//At this point, switch over to process the lights and create the frustum tiles
uint minX = MAX_WORK_GROUP_SIZE * gl_WorkGroupID.x;
uint minY = MAX_WORK_GROUP_SIZE * gl_WorkGroupID.y;
uint maxX = MAX_WORK_GROUP_SIZE * (gl_WorkGroupID.x + 1);
uint maxY = MAX_WORK_GROUP_SIZE * (gl_WorkGroupID.y + 1);

//Convert these corners into NDC and then convert them to view space
vec4 tileCorners[4];
tileCorners[0] = unProject(vec4( (float(minX)/SCREEN_WIDTH) * 2.0f - 1.0f, (float(minY)/SCREEN_HEIGHT) * 2.0f - 1.0f, 1.0f, 1.0f));
tileCorners[1] = unProject(vec4( (float(maxX)/SCREEN_WIDTH) * 2.0f - 1.0f, (float(minY)/SCREEN_HEIGHT) * 2.0f - 1.0f, 1.0f, 1.0f));
tileCorners[2] = unProject(vec4( (float(maxX)/SCREEN_WIDTH) * 2.0f - 1.0f, (float(maxY)/SCREEN_HEIGHT) * 2.0f - 1.0f, 1.0f, 1.0f));
tileCorners[3] = unProject(vec4( (float(minX)/SCREEN_WIDTH) * 2.0f - 1.0f, (float(maxY)/SCREEN_HEIGHT) * 2.0f - 1.0f, 1.0f, 1.0f));

//Create the frustum planes by using the cross product between these points
vec4 frustum[4];
for(int i = 0; i &amp;lt; 4; i++)
frustum[i] = CreatePlane(tileCorners[i],tileCorners[(i+1) &amp;amp; 3]);

barrier();
*/

// cull light per tile
/*
//Now check the lights against the frustum and append them to the list to see what works or doesnt.
int threadsPerTile = MAX_WORK_GROUP_SIZE * MAX_WORK_GROUP_SIZE;
for (uint i = 0; i &amp; lt; NUM_OF_LIGHTS; i+= threadsPerTile)
{
	uint il = gl_LocalInvocationIndex + i;
	if (il &amp;lt; NUM_OF_LIGHTS)
	{
		PointLight p = pointLights[il];

		vec4 viewPos = viewMatrix * p.position;
		float r = p.radius;

		if( ( GetSignedDistanceFromPlane( viewPos, frustum[0] ) &amp;lt; r ) &amp;amp;&amp;amp;
			( GetSignedDistanceFromPlane( viewPos, frustum[1] ) &amp;lt; r ) &amp;amp;&amp;amp;
			( GetSignedDistanceFromPlane( viewPos, frustum[2] ) &amp;lt; r ) &amp;amp;&amp;amp;
			( GetSignedDistanceFromPlane( viewPos, frustum[3] ) &amp;lt; r) )

		{
			uint id = atomicAdd(pointLightCount, 1);
			pointLightIndex[id] = il;
		}
	}
}

*/

// process the light for each tile
/*
//Switch to processing lights

vec3 normal = texture(normalTexture, vec2(texCoord)).xyz;
vec3 albedo = texture(albedoTexture, vec2(texCoord)).xyz;
vec3 storedViewPosition = texture(positionTexture, vec2(texCoord)).xyz;

vec4 color = vec4(albedo,1.0f);

for(uint i = 0; i &amp;lt; pointLightCount; ++i)
{
	uint lightIndex = pointLightIndex[i];
	PointLight light = pointLights[lightIndex];
	vec4 lightInViewPos = viewMatrix * vec4(light.position.xyz, 1.0f);
	//Avoid coloring the background
	if(depthFloat != 1.0f)
		color += CalcPointLight(storedViewPosition, normal, lightInViewPos.xyz, light.radius, 0.5f, vec3(light.color.xyz));
}

barrier();
imageStore(finalImage, pixelPosition, color);

*/