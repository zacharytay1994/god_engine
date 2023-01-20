-- Shows a visual effect when triggered by keypress or event

-- Usage: When you want a VFX to show up (e.g. when Triton attacks and you want to see white slashes), 
-- call SpawnVFX() and pass in the VFX prefab name, duration before fading / disappearing, and X/Y/Z positions.
-- The removal of the VFX will be controlled by this script (S_VFXShow). 

--[IsComponent]
function C_VFXShow()
    local var = {
        -- Time before VFX starts to fade away / disappear
        --[SerializeFloat]
        fadeDelay = 1.5,

        -- How quickly the VFX fades away
        fadeRate = 1,

        timer = 0.0
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_VFXShow(e)

    local VFXFadeComponent = GetComponent(e, "C_VFXShow")

    if (VFXFadeComponent.timer < VFXFadeComponent.fadeDelay) then
        VFXFadeComponent.timer = VFXFadeComponent.timer + GetDeltaTime()
    
    else 

        -- gradually increase transparency
        -- local renderComponent = GetRenderable3D(e)
        -- if (renderComponent == nil) then
        --     print("renderComponent is nil")
        -- else
        --     print(renderComponent.tint.w)
        --     renderComponent.tint.w = renderComponent.tint.w - 0.05
        -- end

        -- then remove instance
        RemoveInstance(e)

    end
end

function SpawnVFX(vfxName, duration, posX, posY, posZ)

    print("SPAWNVFX()")
    print("vfxName:", vfxName)
    print("duration:", duration)
    print("position:", posX, posY, posZ)

    local spawnedVFX = InstancePrefabNow(vfxName, posX, posY, posZ)

    local VFXFadeComponent = GetComponent(spawnedVFX, "C_VFXShow")
    VFXFadeComponent.fadeDelay = duration
end