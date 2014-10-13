#include <cmath>
#include <algorithm>

#include "BVH_kevin.h"
#include "Ray.h"
#include "Camera.h"
#include "Triangle.h"
#include "utils.h"
#include "globals.h"

bool traverseFlatArray(float* flat_array, int idx, Ray* ray, Vec3f* color, int traceDepthToGo, int* node_counter);


Vec3f* Illumination(Ray* reflected, int traceDepthToGo)
{
    float nearest_t = std::numeric_limits<float>::infinity();
    Vec3f* color = new Vec3f();
    
    // See if any objects obscure the light
    int unused;
    
    traverseFlatArray(flat_array, 0, reflected, color, traceDepthToGo-1, &unused);

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
    // double* refl = new double[3];
    // refl[0] = 2 * LdotN * normal[0]  - lp->lightDir[0];
    // refl[1] = 2 * LdotN * normal[1]  - lp->lightDir[1];
    // refl[2] = 2 * LdotN * normal[2]  - lp->lightDir[2];
    
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
getColor(Vec3f* color, float* triangleData, Ray* viewDir, int traceDepthToGo)
{
    /* COLOR CUBE */
        // if((int)triangleData[0]%12 == 0){ color->x = 255; color->y = 000; color->z = 000; } // RED
        // if((int)triangleData[0]%12 == 1){ color->x = 255; color->y = 128; color->z = 000; } // ORANGE
        // if((int)triangleData[0]%12 == 2){ color->x = 255; color->y = 255; color->z = 000; } // YELLOW
        // if((int)triangleData[0]%12 == 3){ color->x = 000; color->y = 128; color->z = 255; } // SKY BLUE
        // if((int)triangleData[0]%12 == 4){ color->x = 000; color->y = 255; color->z = 000; } // GREEN
        // if((int)triangleData[0]%12 == 5){ color->x = 000; color->y = 153; color->z = 076; } // DEEP SEA FOAM
        // if((int)triangleData[0]%12 == 6){ color->x = 000; color->y = 255; color->z = 255; } // TEAL
        // if((int)triangleData[0]%12 == 7){ color->x = 204; color->y = 255; color->z = 153; } // FADED LIME GREEN
        // if((int)triangleData[0]%12 == 8){ color->x = 000; color->y = 000; color->z = 255; } // BLUE
        // if((int)triangleData[0]%12 == 9){ color->x = 127; color->y = 000; color->z = 255; } // PURPLE
        // if((int)triangleData[0]%12 ==10){ color->x = 255; color->y = 153; color->z = 255; } // BRIGHT MAGENTA
        // if((int)triangleData[0]%12 ==11){ color->x = 255; color->y = 000; color->z = 127; } // PINK

    /* COLOR BY DISTANCE */
        int nearPlane = 0;
        int farPlane = 2;

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
    Vec3f* ill = Illumination(reflected, traceDepthToGo);
    // cerr << "ill is " << *ill << endl;

    // --- FIND COLORS SEEN VIA OPACITY -------
    Ray* rayThruIntersection = new Ray(pointOfIntersection, viewDir->unitDir);
    Vec3f* opColor = new Vec3f();
    int unused;
    traverseFlatArray(flat_array, 0, rayThruIntersection, opColor, traceDepthToGo, &unused);
    // cerr << "opColor " << *opColor << endl;


    // --- APPLY SHADING ----------
    color->x = (unsigned char) ceil441( std::min(255.0, color->x*PhShade));
    color->y = (unsigned char) ceil441( std::min(255.0, color->y*PhShade));
    color->z = (unsigned char) ceil441( std::min(255.0, color->z*PhShade));
                
    // --- APPLY OPACITY ----------
    color->x = (unsigned char) ceil441( std::min(255.0, color->x*(1-opacity)+opColor->x*opacity));
    color->y = (unsigned char) ceil441( std::min(255.0, color->y*(1-opacity)+opColor->y*opacity));
    color->z = (unsigned char) ceil441( std::min(255.0, color->z*(1-opacity)+opColor->z*opacity));

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

    // cerr << "\nBox at " << idx << endl;
    // cerr << "flat_array[idx] = " << flat_array[idx] << endl;

    *node_counter += 1; /* USED TO COUNT NUMBER OF NODE VISITS */

    // LEAF NODE
    if (flat_array[idx] == LEAF_FLAG){
        // cerr << "LEAF NODE\n" << endl;
        idx++; //increment here, after 'if' statement comparison in case of a FALSE
        float closest[2] = {-1,std::numeric_limits<float>::infinity()};//[Tri ID, data]
        int triangle_count = flat_array[idx++];
        // Intersect Triangles
        for(int i=0; i<triangle_count; i++){
            int cur_idx = idx+i;
            // cerr << "looking at triangle, id = " << flat_array[cur_idx] << endl;
            float data = std::numeric_limits<float>::infinity();

            /* Get Triangle with the appropriate id */
            Triangle curTri;
            GetTriangleFromID(tris, numTriangles, flat_array[cur_idx], &curTri);

            bool hit = curTri.intersect(*ray, &data);
            // cerr << "data is " << data << endl;
            if(hit){
                if(data < closest[1]){
                    // cerr << "it's less, replace closest" << endl;
                    closest[0] = flat_array[cur_idx];
                    closest[1] = data;
                }
            }
        }
        // cerr << "closest triangle: id " << closest[0] << endl;
        // Calc color specs for closest intersected Triangle

        if(closest[0] >= 0){
            // Ray* viewDir = RayFromPoints(*c->position, *c->focus);
            // viewDir->normalize();
            getColor(color, closest, ray, traceDepthToGo);
            return true;

        }else{ return false; }
    }
    // INNER NODE
    else{
        // cerr << "INNER NODE" << endl;
        int num_hits = 0;

        std::vector<float> mins (branching_factor, 0);// idx == which BBox, val == tnear of BBox (if intersect else 0)
        std::vector<float> maxs (branching_factor, 0);// idx == which BBox, val == tfar of BBox (if intersect else 0)

        // Intersect bounding boxes
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

            // cerr << "bbox " << i << endl;
            // cerr << "MIN: " << min << endl;
            // cerr << "MAX: " << max << endl;

            // Intersect Bounding Box
            float tnear;
            float tfar;
            // bool bb = box->intersect(*ray, &tnear, &tfar);
            bool bb = box->intersect(ray->unitDir, ray->source, ray->invDir, ray->sign, &tnear, &tfar);
            if ( bb ){
                // cerr << "intersection! " << tnear << endl;
                num_hits++;

                // Get th
                mins[i] = tnear;
                maxs[i] = tfar;
                // cerr << "~~intsection, mins["<<i<<"] got " << mins[i] << ", maxs["<<i<<"] got " << maxs[i] << endl;
            }
            delete box;
        }
        // cerr << "idx is: " << idx << endl;

        if(num_hits == 0){ return false; } /* No BBox intersection */

        // Traverse closest intersected Bounding Box
        // bool still_searching = true;
        while(1){
            // cerr << "another iteration of while" << endl;
            int least_box = -1;

            for(int i=0; i<branching_factor; i++){
                if( mins[i] != 0){
                    if(least_box == -1 || mins[i] < mins[least_box] ){
                        least_box=i;
                        // cerr << "New least box " << least_box << endl;
                    }
                }
            }

            if( least_box == -1){ return false; } /* No BBox intersection, I think this is redundant */

            if(num_hits > 1){
                // cerr << "mult hits" << endl;
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
                // if(HasDuplicates(mins, branching_factor, &guilty_index)==true){
                //     // cerr << "HasDuplicates, guilty_index = " << guilty_index << endl;
                
                //     if ( least_box==guilty_index ){ /* since new least box requires strictly '<' and guilty_index is the first of the guilt indices, this works */
                //         /* ie if there's a tie for closest box (by front face) */
                //         // cerr << "dup for closest box. " << endl;
                //         /* Go thru maxs to find which has closer back */
                //         for(int i=0; i<branching_factor; i++){
                //             if( maxs[i] != 0 && maxs[i] < maxs[least_box] ){
                //                 least_box=i;
                //                 // cerr << "New least box " << least_box << endl;
                //             }
                //         }
                //     }
                // }
            }

            // cerr << "least_box = " << least_box << endl;

            int idx_next_box = idx+least_box;

            // cerr << "idx indicating next_box = " << idx_next_box << endl;
            // cerr << "will go to box next: " << flat_array[idx_next_box] << endl;

            if (traverseFlatArray(flat_array, flat_array[idx_next_box], ray, color, traceDepthToGo, node_counter)){ 
                // cerr << "returning true"<< endl;
                return true;
            }
            else{ 
                // cerr << "in the else..."<<endl;
                mins[least_box] = 0;
                num_hits--;
                // cerr << "num_hits is now " << num_hits << endl;
            }
        }
        cerr << "shouldn't have gotten here...." << endl;
    }
}


