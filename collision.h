#ifndef COLLISION_H
#define COLLISION_H

AABB
addAABB(v3 min, v3 max)
{
    AABB result = {};
    
    result.min = min;
    result.max = max;
    
    return result;
}


struct Segment_intersect
{
    bool intersect;
    v3 intersectP;
    
    f32 fraction;
};


#endif //COLLISION_H
