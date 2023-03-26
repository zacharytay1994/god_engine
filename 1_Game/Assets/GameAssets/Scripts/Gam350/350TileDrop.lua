--[IsComponent]
function C_350TileDrop()
    local var = {
        --[SerializeFloat]
        DropSpeed = 5.0,
        EmissionRate = 0.2
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_350TileDrop(e)
    -- move transform back to 0
    local C_350TileDrop = GetComponent(e, "C_350TileDrop")
    local transform = GetTransform(e)
    local dt = GetDeltaTime()
    transform.position = LerpVec3(transform.position.x, transform.position.y, transform.position.z, 0, 0, 0, dt*C_350TileDrop.DropSpeed)
    if (transform.position.y > 0.1) then
        if (C_350TileDrop.EmissionRate > 0) then
            C_350TileDrop.EmissionRate = C_350TileDrop.EmissionRate - dt;
        else
            C_350TileDrop.EmissionRate = 0.2
            local bubble = InstancePrefabParentedNow(e, "BubbleParticle3D", 0, 0, 0)
            local bubble_particle = GetComponent(bubble, 'C_BubbleParticle')
            local bubble_transform = GetTransform(bubble)
            local rand_size = GenerateRandomNumberInRange(1,30)/100.0
            local rand_x = GenerateRandomNumberInRange(1,10)/10.0-0.5
            local rand_z = GenerateRandomNumberInRange(1,10)/10.0-0.5
            bubble_particle.initialHeight = transform.position.y
            bubble_transform.scale.x = rand_size
            bubble_transform.scale.y = rand_size
            bubble_transform.scale.z = rand_size
            bubble_transform.position.x = rand_x
            bubble_transform.position.z = rand_z
            bubble_particle.Speed = rand_size * 30.0
            bubble_particle.Lifetime = 6.0
        end
    end
end
