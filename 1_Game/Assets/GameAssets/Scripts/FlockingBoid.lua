--[IsComponent]
function C_FlockingBoid()
    local var = {
        --[SerializeString]
        Target = "None"
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_FlockingBoid(e)
    -- for now just move towards target
    -- in future here is where to put the flocking code
    local boid = GetComponent(e, "C_FlockingBoid")
    if (boid.Target ~= "None") then
        local target = GetEntity(boid.Target)
        if (target ~= -1) then
            local target_transform = GetTransform(target)
            local boid_transform = GetTransform(e)
            local physics_controller = GetPhysicsController(e)
            physics_controller.heading = Heading(target_transform.position, boid_transform.position)
        end
    end

    if CheckKeyPress(70) then
        AddForce(e, 50, 0, 0)
        local vfx = InstancePrefabParentedNow(e, "VFX_OnHit1", 0, 0, 0)
        local main1 = Child(vfx, 0)
        local main2 = Child(vfx, 1)
        -- ChangeTexture(main1, "VFX_Splash" .. GenerateRandomNumberInRange(1, 4))
        -- ChangeTexture(main2, "VFX_Splash" .. GenerateRandomNumberInRange(1, 4))
        ChangeTexture(main1, "VFX_Splash1")
        ChangeTexture(main2, "VFX_Splash2")
    end
end
