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
end
