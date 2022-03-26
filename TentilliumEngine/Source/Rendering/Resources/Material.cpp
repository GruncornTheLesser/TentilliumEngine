#include "Material.h"
#include <glew.h>
#include <glfw3.h>

void Material::bind() const
{
	if (m_diff_map) m_diff_map->bind(MATERIAL_MAP_DIFF_SLOT);
	if (m_norm_map) m_norm_map->bind(MATERIAL_MAP_NORM_SLOT);
	if (m_shin_map) m_shin_map->bind(MATERIAL_MAP_SHIN_SLOT);
	if (m_spec_map) m_spec_map->bind(MATERIAL_MAP_SPEC_SLOT);
}