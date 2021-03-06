#include <cmath>
#include <algorithm>

#include "BVH.h"
#include "Ray.h"
#include "Camera.h"
#include "Triangle.h"
#include "utils.h"
#include "globals.h"

bool traverseFlatArray(float* flat_array, int idx, Ray* ray, Vec3f* color, int traceDepthToGo, int* node_counter);


Vec3f* Illumination(Ray* reflected, int traceDepthToGo, int* node_counter)
{
    float nearest_t = std::numeric_limits<float>::infinity();
    Vec3f* color = new Vec3f();
    
    // See if any objects obscure the light
    traverseFlatArray(flat_array, 0, reflected, color, traceDepthToGo-1, node_counter);

    return color;
    
}

Ray* findReflection(Ray* normal, Ray* viewDir)
{
    Vec3f v = Vec3f( normal->source[0]-viewDir->source[0], normal->source[1]-viewDir->source[1], normal->source[2]-viewDir->source[2] );
    
    double VdotN = dotProduct(v, normal->unitDir);
    Vec3f reflDir = Vec3f( v[0]-2*VdotN*normal->unitDir[0], v[1]-2*VdotN*normal->unitDir[1], v[2]-2*VdotN*normal->unitDir[2] );
    
    return new Ray( normal->source, reflDir );
}


/*
 * Returns the Phong shaing value
 */
float CalculateShading(LightingParameters* lp, Vec3f viewDirection, Ray* normalRay)
{
    Vec3f normal = normalRay->unitDir;
    Vec3f point = normalRay->source;
    
    // Calc dot product of Incoming light and normal
    double LdotN = dotProduct(lp->lightDir, normal);
    
    // Calc the diffuse lighting contribution: diffuse = abs(LightDirec dot sufaceNormal)
    double diffuse = abs( LdotN );
    
    // Calc the strength of the directly refelcted light
    Vec3f refl = Vec3f( 2 * LdotN * normal[0]  - lp->lightDir[0], 
                        2 * LdotN * normal[1]  - lp->lightDir[1],
                        2 * LdotN * normal[2]  - lp->lightDir[2]);
    
    // Calc the specular lighting contribution
    float spec;
    if (dotProduct(refl, viewDirection) < 0.0)
        spec = 0.0;
    else
        spec = pow(dotProduct(refl, viewDirection), lp->gamma);
    
    //cerr << "normal: " << "(" << normal[0] << ", " << normal[1] << ", " << normal[2] << "),  and Reflection: " << refl[0] << ", " << refl[1] << ", " << refl[2] << ") " << "with Ambient: " << lp->Ka << " diffuse " << lp->Kd*diffuse << " spec: " << lp->Ks*spec << " Total Shading: " << lp->Ka + lp->Kd*diffuse + lp->Ks*spec << endl;
    
    //  ----------- Find shadows --------------
    // Direction to light source
    Vec3f reflec = Vec3f(   -(lp->lightDir[0]*1500-point[0]),
                            -(lp->lightDir[1]*1500-point[1]),
                            -(lp->lightDir[2]*1500-point[2]));
    // Reflection ray
    Ray* reflection = new Ray(point, reflec);
    reflection->normalize();
    
    return lp->Ka + lp->Kd*diffuse + lp->Ks*spec;
}




/* triangleData = [Tri ID, data] */
void
getColor(Vec3f* color, float* triangleData, Ray* viewDir, int traceDepthToGo, int* node_counter)
{
    /* COLOR BY DISTANCE FROM VIEW POINT */
    int nearPlane = 16;
    int farPlane = 50;

    double dist_ratio = (triangleData[1]-nearPlane)/(double)(farPlane-nearPlane);
    int color_ratio = dist_ratio > 1 ? 255 : dist_ratio*255;

    color->x = 255;
    color->y = color_ratio;
    color->z = color_ratio;

    /* Get Triangle with the appropriate id */
    Triangle curTri;
    GetTriangleFromID(tris, numTriangles, triangleData[0], &curTri);
                
    // --- GET NORMAL @ INTERSECT POINT
    Vec3f pointOfIntersection = viewDir->unitDir*triangleData[1];
    Ray* normAtIntersect = new Ray(pointOfIntersection, curTri.normal->unitDir);

    // --- FIND SHADING -----------
    double PhShade = CalculateShading(lp, viewDir->unitDir, curTri.normal);
    // cerr << "PhShade is " << PhShade << endl;

    // --- FIND REFLECTIONS -------
    Ray* reflected = findReflection(normAtIntersect, viewDir);
    Vec3f* ill = Illumination(reflected, traceDepthToGo, node_counter);
    // cerr << "ill is " << *ill << endl;

    // --- FIND COLORS SEEN VIA OPACITY -------
    Ray* rayThruIntersection = new Ray(pointOfIntersection, viewDir->unitDir);
    Vec3f* opColor = new Vec3f();
    if(opacity>0){
        traverseFlatArray(flat_array, 0, rayThruIntersection, opColor, traceDepthToGo, node_counter);
    }

    // --- APPLY SHADING ----------
    color->x = (unsigned char) ceil441( std::min(255.0, color->x*PhShade));
    color->y = (unsigned char) ceil441( std::min(255.0, color->y*PhShade));
    color->z = (unsigned char) ceil441( std::min(255.0, color->z*PhShade));
                
    // --- APPLY OPACITY ----------
    if(opacity>0){
        color->x = (unsigned char) ceil441( std::min(255.0, color->x*(1-opacity) + opColor->x*opacity));
        color->y = (unsigned char) ceil441( std::min(255.0, color->y*(1-opacity) + opColor->y*opacity));
        color->z = (unsigned char) ceil441( std::min(255.0, color->z*(1-opacity) + opColor->z*opacity));
    }

    // --- APPLY REFLECTIONS ------
    color->x = (unsigned char) ceil441( std::min(255.0, color->x + GLOBAL_REFLECT_COEF * ill->x));
    color->y = (unsigned char) ceil441( std::min(255.0, color->y + GLOBAL_REFLECT_COEF * ill->y));
    color->z = (unsigned char) ceil441( std::min(255.0, color->z + GLOBAL_REFLECT_COEF * ill->z));

    delete normAtIntersect;
    delete ill;
}



bool
traverseFlatArray(float* flat_array, int idx, Ray* ray, Vec3f* color, int traceDepthToGo, int* node_counter)
{
    if (traceDepthToGo <= 0){ return false; } /* ALL DONE */

    *node_counter += 1; /* USED TO COUNT NUMBER OF NODE VISITS */

    /* ---- LEAF NODE ---- */
    if (flat_array[idx] == LEAF_FLAG){
        idx++; //increment here, after 'if' statement comparison in case of a FALSE

        float closest[2] = {-1,std::numeric_limits<float>::infinity()};//[Tri ID, data]
        int triangle_count = flat_array[idx++];

        // Intersect Triangles
        for(int i=0; i<triangle_count; i++){
            float data = std::numeric_limits<float>::infinity();
            int cur_idx = idx+i;

            /* Get Triangle with the appropriate id */
            Triangle curTri;
            GetTriangleFromID(tris, numTriangles, flat_array[cur_idx], &curTri);

            bool hit = curTri.intersect(*ray, &data);
            if(hit){
                if(data < closest[1]){
                    /* NEW INTERSECTION IS CLOSER */
                    closest[0] = flat_array[cur_idx];
                    closest[1] = data;
                }
            }
        }
        // cerr << "closest triangle: id " << closest[0] << endl;

        // Calc color specs for closest intersected Triangle
        if(closest[0] >= 0){
            getColor(color, closest, ray, traceDepthToGo, node_counter);
            return true;

        }else{ return false; }
    }

    /* ---- INNER NODE ---- */
    else{
        int num_hits = 0;

        std::vector<float> mins (branching_factor, 0);// idx == which BBox, val == tnear of BBox (if intersect else 0)
        std::vector<float> maxs (branching_factor, 0);// idx == which BBox, val == tfar of BBox (if intersect else 0)

        /* FIND Bbox INTERSECTIONS */
        for(int i=0; i<branching_factor; i++){
            float min_x = flat_array[idx++];
            float min_y = flat_array[idx++];
            float min_z = flat_array[idx++];
            float max_x = flat_array[idx++];
            float max_y = flat_array[idx++];
            float max_z = flat_array[idx++];

            // Construct Bounding Box
            Vec3f min = Vec3f(min_x, min_y, min_z);
            Vec3f max = Vec3f(max_x, max_y, max_z);
            BBox* box = new BBox(min, max);

            // Intersect Bounding Box
            float tnear;
            float tfar;
            
            bool bb = box->intersect(ray->unitDir, ray->source, ray->invDir, ray->sign, &tnear, &tfar);
            if ( bb ){
                // cerr << "intersection! " << tnear << endl;
                num_hits++;
                mins[i] = tnear;
                maxs[i] = tfar;
            }
            delete box;
        }
        
        /* IF NO BBox INTERSECTION, THEN DONE. */
        if(num_hits == 0){ return false; }

        /* TRAVERSE CLOSEST INTERSECTED BBox */
        while(1){
            int least_box = -1;

            for(int i=0; i<branching_factor; i++){
                if( mins[i] != 0){
                    if(least_box == -1 || mins[i] < mins[least_box] ){
                        least_box=i;
                    }
                }
            }

            if( least_box == -1){ return false; } /* No BBox intersection, I think this is redundant */

            /* IF MULTIPLE HITS ... */
            if(num_hits > 1){
                int dups[MAX_BRANCHING_FACTOR] = {-1};
                int guilty_index = -1;

                if( IsDuplicated(mins, branching_factor, mins[least_box], dups)==true){
                    for(int d=0; d<branching_factor; d++){
                        if(dups[d] < 0){ continue; }
                        if(dups[d] != least_box){
                            if (maxs[dups[d]] > 0   &&   maxs[dups[d]] < maxs[least_box] ){
                                least_box=dups[d];
                            }
                        }
                    }
                }
            }

            int idx_next_box = idx+least_box;

            // cerr << "idx indicating next_box = " << idx_next_box << endl;
            // cerr << "will go to box next: " << flat_array[idx_next_box] << endl;

            /* IF THERE IS AN INTERSECTION IN BVH... */
            if (traverseFlatArray(flat_array, flat_array[idx_next_box], ray, color, traceDepthToGo, node_counter)){ 
                return true;
            }
            /* ELSE, TRY THE NEXT POSSIBLITY (KEEP LOOPING TIL WE GET THE INTERSECTION) */
            else{
                mins[least_box] = 0;
                num_hits--;
            }
        }
        cerr << "shouldn't have gotten here...." << endl;
    }
}


