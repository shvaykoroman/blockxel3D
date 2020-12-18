bool
clipLine(u32 dimension, AABB aabb, v3 startP, v3 endP,f32 *fractionLowPtr,f32 *fractionHighPtr)
{
    f32 fractionHigh = *fractionHighPtr;
    f32 fractionLow  = *fractionLowPtr; 
    
    bool result = true;
    
    f32 entireVector =  (endP.e[dimension] - startP.e[dimension]);
    // NOTE(shvayko):Compute intersection t value of ray with near and far plane of slab
    f32 tMin = (aabb.min.e[dimension] - startP.e[dimension]);
    f32 tMax = (aabb.max.e[dimension] - startP.e[dimension]);
    f32 dimensionLow  = (tMin / entireVector);
    f32 dimensionHigh = (tMax / entireVector);
    
    // NOTE(shvayko):Make tMin be intersection with near plane, tMax with far plane
    if(dimensionLow > dimensionHigh)
    {
        f32 temp = dimensionLow;
        dimensionLow = dimensionHigh;
        dimensionHigh = temp;
    }
    
    // NOTE(shvayko):Compute the intersection of slab intersection intervals
    if(dimensionHigh < fractionLow)
    {
        result = false;
    }
    if(dimensionLow > fractionHigh)
    {
        result = false;
    }
    
    // NOTE(shvayko): AS I UNDERSTAND HERE WE FINDING NEAR AND FAR VALUES!
    *fractionLowPtr = MAX(fractionLow, dimensionLow);
    *fractionHighPtr = MIN(fractionHigh,dimensionHigh);
    
    if(fractionLow > fractionHigh)
    {
        result = false;
    }
    
    return result;
}

bool
testLineSegmentCollision(AABB aabb, v3 startP, v3 endP)
{
    // NOTE(shvayko): if collision, return true
    bool result = false;
    bool isIntersect = true;
    f32 lowestFraction = 1.0f;
    f32 testFraction = 0;
    
    v3 testIntersectP;
    v3 intersectP;
    
    f32 fractionLow = 0.0f;     // NOTE(shvayko):Closest to startP
    f32 fractionHigh = 1.0f;
    
    f32 resultFraction = 0;
    
    // NOTE(shvayko):For all 3 slabs
    for(s32 dimensionIndex = 0; dimensionIndex < 3; dimensionIndex++)
    {
        if(!clipLine(dimensionIndex,aabb,startP,endP,&fractionLow,&fractionHigh))
        {
            isIntersect = false;
        }
        else
        {
            resultFraction = fractionLow;
        }
    }
    
    v3 entireVector = endP - startP;
    testIntersectP = startP + entireVector * fractionLow;
    
    if(isIntersect)
    {
        intersectP = testIntersectP;
        lowestFraction = resultFraction;
    }
    
    if(lowestFraction < 1)
    {
        result = true;
    }
    
    return result;
}
