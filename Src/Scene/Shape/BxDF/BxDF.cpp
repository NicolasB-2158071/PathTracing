#include "BxDF.h"
#include "LambertianDiffuseBRDF.h"
#include "BlinnPhongBRDF.h"
#include "PhongBRDF.h"
#include "ConductorBxDF.h"
#include "DielectricBxDF.h"

std::unique_ptr<BxDF> BxDF::CreateFromMaterial(const Json::Value& mat) {
	if (mat["BxDF"].asString() == "LambertianDiffuseBRDF") {
		return std::make_unique<LambertianDiffuseBRDF>(mat);
	}
	if (mat["BxDF"].asString() == "BlinnPhongBRDF") {
		return std::make_unique<BlinnPhongBRDF>(mat);
	}
	if (mat["BxDF"].asString() == "PhongBRDF") {
		return std::make_unique<PhongBRDF>(mat);
	}
	if (mat["BxDF"].asString() == "ConductorBxDF") {
		return std::make_unique<ConductorBxDF>(mat);
	}
	if (mat["BxDF"].asString() == "DielectricBxDF") {
		return std::make_unique<DielectricBxDF>(mat);
	}
}