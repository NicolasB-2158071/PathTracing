#pragma once

#include <glm/glm.hpp>

class GGX {
public:
	GGX(float alphaX, float alphaY);

	float D(const glm::vec3& wm) const;
	float D(const glm::vec3& wo, const glm::vec3& wm) const;
	float G(const glm::vec3& wo, const glm::vec3& wi) const;
	float G1(const glm::vec3& wo) const;
	float Lambda(const glm::vec3& w) const;
	bool IsSmooth() const;
	
	// https://www.jcgt.org/published/0007/04/01/paper.pdf
	glm::vec3 SampleEllipsoid(const glm::vec3& wo, const glm::vec2& uv) const; 

private:
	float m_alphaX;
	float m_alphaY;
};

// Taken from:
// https://pbr-book.org/4ed/Reflection_Models/Roughness_Using_Microfacet_Theory
// Everything done in local coordinate system

// Interesting:
// https://cdn2.unrealengine.com/Resources/files/2013SiggraphPresentationsNotes-26915738.pdf
// https://media.disneyanimation.com/uploads/production/publication_asset/48/asset/s2012_pbs_disney_brdf_notes_v3.pdf