#define __NR_LIGHT_CPP__

/*
 * Light rendering helpers
 *
 * Author:
 *   Jean-Rene Reinhard <jr@komite.net>
 *
 * Copyright (C) 2006 Jean-Rene Reinhard
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

#include <cmath>

#include "libnr/nr-pixops.h"
#include "display/nr-light.h"
#include "display/nr-3dutils.h"
#include "sp-fedistantlight.h"
#include "sp-fepointlight.h"
#include "sp-fespotlight.h"

namespace NR {
   
DistantLight::DistantLight(SPFeDistantLight *light, guint32 lighting_color) {
    color = lighting_color;
    azimuth = M_PI / 180 * light->azimuth;
    elevation = M_PI / 180 * light->elevation;
}

DistantLight::~DistantLight() {}

void DistantLight::light_vector(Fvector &v) {
    v[X_3D] = std::cos(azimuth)*std::cos(elevation);
    v[Y_3D] = std::sin(azimuth)*std::cos(elevation);
    v[Z_3D] = std::sin(elevation);
} 

void DistantLight::light_components(Fvector &lc) {
    lc[LIGHT_RED] = NR_RGBA32_R(color);
    lc[LIGHT_GREEN] = NR_RGBA32_G(color);
    lc[LIGHT_BLUE] = NR_RGBA32_B(color);
}

PointLight::PointLight(SPFePointLight *light, guint32 lighting_color, const Matrix &trans) {
    color = lighting_color;
    l_x = light->x;
    l_y = light->y;
    l_z = light->z;
    convert_coord(l_x, l_y, l_z, trans);
}

PointLight::~PointLight() {}

void PointLight::light_vector(Fvector &v, gdouble x, gdouble y, gdouble z) {
    v[X_3D] = l_x - x;
    v[Y_3D] = l_y - y;
    v[Z_3D] = l_z - z;
    normalize_vector(v);
} 

void PointLight::light_components(Fvector &lc) {
    lc[LIGHT_RED] = NR_RGBA32_R(color);
    lc[LIGHT_GREEN] = NR_RGBA32_G(color);
    lc[LIGHT_BLUE] = NR_RGBA32_B(color);
}

SpotLight::SpotLight(SPFeSpotLight *light, guint32 lighting_color, const Matrix &trans) {
    gdouble p_x, p_y, p_z;
    color = lighting_color;
    l_x = light->x;
    l_y = light->y;
    l_z = light->z;
    p_x = light->pointsAtX;
    p_y = light->pointsAtY;
    p_z = light->pointsAtZ;
    cos_lca = std::cos(M_PI / 180 * light->limitingConeAngle);
    speExp = light->specularExponent;
    convert_coord(l_x, l_y, l_z, trans);
    convert_coord(p_x, p_y, p_z, trans);
    S[X_3D] = p_x - l_x;
    S[Y_3D] = p_y - l_y;
    S[Z_3D] = p_z - l_z;
    normalize_vector(S);
    
}

SpotLight::~SpotLight() {}

void SpotLight::light_vector(Fvector &v, gdouble x, gdouble y, gdouble z) {
    v[X_3D] = l_x - x;
    v[Y_3D] = l_y - y;
    v[Z_3D] = l_z - z;
    normalize_vector(v);
} 

void SpotLight::light_components(Fvector &lc, const Fvector &L) {
    gdouble spmod = (-1) * scalar_product(L, S);
    if (spmod <= cos_lca)
        spmod = 0;
    else
        spmod = std::pow(spmod, speExp);
    lc[LIGHT_RED] = spmod * NR_RGBA32_R(color);
    lc[LIGHT_GREEN] = spmod * NR_RGBA32_G(color);
    lc[LIGHT_BLUE] = spmod * NR_RGBA32_B(color);
}

} /* namespace NR */

/*
  Local Variables:
  mode:c++
  c-file-style:"stroustrup"
  c-file-offsets:((innamespace . 0)(inline-open . 0)(case-label . +))
  indent-tabs-mode:nil
  fill-column:99
  End:
*/
// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=8:softtabstop=4:encoding=utf-8:textwidth=99 :
