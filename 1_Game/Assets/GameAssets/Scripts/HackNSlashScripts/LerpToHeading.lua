--[IsComponent]
function C_LerpToHeading()
    local var = {
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_LerpToHeading(e)
    -- print("lerping")
    local player = GetEntity("Player")
    local transform = GetTransform(e)
    if player ~= -1 then
        local parent_physics_controller = GetPhysicsController(player)

        if parent_physics_controller.heading.x * parent_physics_controller.heading.x +
            parent_physics_controller.heading.z * parent_physics_controller.heading.z > 0 then

            local heading_angle = DegreeBetweenVec2(parent_physics_controller.heading.x,
                parent_physics_controller.heading.z
                , 0, 1)

            -- print(parent_physics_controller.heading.x)
            -- print(parent_physics_controller.heading.y)
            -- print(heading_angle)
            local corrected_angle = -heading_angle + 90
            transform.rotation.y = Lerp(transform.rotation.y, corrected_angle, 0.8)
        end
    end
end
