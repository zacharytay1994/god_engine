-- This script animates an explosion effect.
  -- Min Ye

--[IsComponent]
function C_VFXExplosion()
    local var = {
        
        -- flag, for spawning the red sphere
        explosionInitialized = false,

        -- a list storing the scale of each child
        childrenScales = { },

        -- timer for the animation
        timer = 0.0,

        -- duration of each expand / contract animation
        expandContractTime = 0.2,

        shockwaveEntity = nil,

        -- current state of the red sphere
        expandState = true,
    
        -- number of particles that shoots out from the explosion (idk what it's called)
        strayParticles = 8
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_VFXExplosion(e)

    local vfxExplosionComponent = GetComponent(e, "C_VFXExplosion")
    local vfxShowComponent = GetComponent(e, "C_VFXShow")

    -- play explosion animation (expand and contract, then maybe add particles)
    
    -- start of animation: store the ending size of each explosion child, then reduce their sizes 
    if (vfxExplosionComponent.explosionInitialized == false) then
        
        local childCount = ChildCount(e)

        for i = 0, childCount - 1 do
            local childScale = GetTransform(Child(e, i)).scale
            vfxExplosionComponent.childrenScales[i] = childScale.x -- no need Y or Z, because they are all equal
            
            childScale.x = childScale.x / 5
            childScale.y = childScale.y / 5
            childScale.z = childScale.z / 5
        end

        local parentPosition = GetTransform(e).position
        vfxExplosionComponent.shockwaveEntity = InstancePrefabParentedNow(e, "Shockwave", parentPosition.x, parentPosition.y - 50, parentPosition.z - 125)
        local shockwaveScale = GetTransform(vfxExplosionComponent.shockwaveEntity).scale
        shockwaveScale.x = 0.1
        shockwaveScale.y = 0.1
        shockwaveScale.z = 0.1

        vfxExplosionComponent.explosionInitialized = true
        print("VFXExplosion initialized!")
    end

    -- -- for checking if scales were stored correctly
    -- for i = 1, #vfxExplosionComponent.childrenScales do
    --     print(vfxExplosionComponent.childrenScales[i])
    -- end

    if (vfxExplosionComponent.timer < vfxExplosionComponent.expandContractTime) then
        vfxExplosionComponent.timer = vfxExplosionComponent.timer + GetDeltaTime()
    else
        vfxExplosionComponent.expandState = false
        --vfxExplosionComponent.timer = 0.0
    end

    

    if (vfxExplosionComponent.expandState) then

        local childCount = ChildCount(e)

        -- expand each child
        for i = 0, childCount - 1 do

            local maxScale = vfxExplosionComponent.childrenScales[i]            
            local childScale = GetTransform(Child(e, i)).scale
            
            -- childScale.x = childScale.x + ((GetDeltaTime() / vfxExplosionComponent.expandContractTime) * maxScale * 2)
            -- childScale.y = childScale.y + ((GetDeltaTime() / vfxExplosionComponent.expandContractTime) * maxScale * 2)
            -- childScale.z = childScale.z + ((GetDeltaTime() / vfxExplosionComponent.expandContractTime) * maxScale * 2)

            childScale.x = childScale.x + maxScale * 0.2
            childScale.y = childScale.y + maxScale * 0.2
            childScale.z = childScale.z + maxScale * 0.2
        end
    else
        
        local shockwaveScale = GetTransform(vfxExplosionComponent.shockwaveEntity).scale
        shockwaveScale.x = shockwaveScale.x + 1
        shockwaveScale.z = shockwaveScale.z + 1

        -- contract each child
        local childCount = ChildCount(e)

        for i = 0, childCount - 1 do

            local maxScale = vfxExplosionComponent.childrenScales[i]            
            local childScale = GetTransform(Child(e, i)).scale
            
            if (childScale.x > 0) then
                childScale.x = childScale.x - maxScale * 0.1
                childScale.y = childScale.y - maxScale * 0.1
                childScale.z = childScale.z - maxScale * 0.1
            end

            local pointLightColour = GetPointLight(Child(e, i)).colour
            if (pointLightColour.x > 2) then
                pointLightColour.x = pointLightColour.x - 2
            else
                pointLightColour.x = 1
            end

            if (childScale.x <= 0) then
                RemoveInstance(e)
            end
            
        end    
        
        
    end
end