#pragma once
#include "Resource.h"
#include "Texture.h"
#include "ShaderProgram.h"

#include <glm.hpp>
#include <array>
#include <assimp/scene.h>


// this is temporary -> tengo plans

class Material final
{
public:
    enum class Map { 
        /** The texture is combined with the result of the diffuse
        *  lighting equation.
        */
        DIFFUSE = 1,

        /** The texture is combined with the result of the specular
        *  lighting equation.
        */
        SPECULAR = 2,

        /** The texture is combined with the result of the ambient
        *  lighting equation.
        */
        AMBIENT = 3,

        /** The texture is added to the result of the lighting
        *  calculation. It isn't influenced by incoming light.
        */
        EMISSIVE = 4,

        /** By convention, higher gray-scale values stand for
        *  higher elevations from the base height.
        */
        HEIGHT = 5,

        /** Again, there are several conventions for tangent-space
        *  normal maps. Assimp does (intentionally) not
        *  distinguish here.
        */
        NORMALS = 6,

        /** The glossiness is in fact the exponent of the specular
        *  (phong) lighting equation. Usually there is a conversion
        *  function defined to map the linear color values in the
        *  texture to a suitable exponent. Have fun.
        */
        SHININESS = 7,

        /** Usually 'white' means opaque and 'black' means
        *  'transparency'. Or quite the opposite. Have fun.
        */
        OPACITY = 8,

        /* The exact purpose and format is application-dependent.
        * Higher color values stand for higher vertex displacements.
        */
        DISPLACEMENT = 9,

        /** Both 'Lightmaps' and dedicated 'ambient occlusion maps' are
        *  covered by this material property. The texture contains a
        *  scaling value for the final color value of a pixel. Its
        *  intensity is not affected by incoming light.
        */
        LIGHTMAP = 10,

        /** Contains the color of a perfect mirror reflection.
        * Rarely used, almost never for real-time applications.
        */
        REFLECTION = 11,

        /** PBR Model */
        BASE_COLOR = 12,
        /** PBR Model */
        NORMAL_CAMERA = 13,
        /** PBR Model */
        EMISSION_COLOR = 14,
        /** PBR Model */
        METALNESS = 15,
        /** PBR Model */
        DIFFUSE_ROUGHNESS = 16,
        /** PBR Model */
        AMBIENT_OCCLUSION = 17,
    };


public:
    std::shared_ptr<Texture> m_diff_map;
    std::shared_ptr<Texture> m_norm_map;
    std::shared_ptr<Texture> m_spec_map;
    std::shared_ptr<Texture> m_shin_map;

public:
    Material() { }
    void bind() const;
};