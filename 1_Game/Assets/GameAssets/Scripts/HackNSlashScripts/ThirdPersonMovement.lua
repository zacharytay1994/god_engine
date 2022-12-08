--[IsComponent]
function C_ThirdPersonMovement()
    local var = {
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_ThirdPersonMovement(e)
    local tp_movement = GetComponent(e, "C_ThirdPersonMovement")
    local tp_camera = GetComponent(e, "C_ThirdPersonCamera")

    local physics_controller = GetPhysicsController(e)

    physics_controller.heading.x = tp_camera.HeadingX
    physics_controller.heading.z = tp_camera.HeadingZ
end
