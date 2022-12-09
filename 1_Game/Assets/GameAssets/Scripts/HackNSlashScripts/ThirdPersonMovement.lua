--[IsComponent]
function C_ThirdPersonMovement()
    local var = {
        --[SerializeFloat]
        Speed = 0.03
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

    -- sideway vector
    local left_right_vector = RotateVec2(tp_camera.HeadingX, tp_camera.HeadingZ, 90)
    
    physics_controller.speed = 0.0
    
    local front_back = 0
    local left_right = 0
    if CheckKeyDown(87) then
        front_back = front_back + 1
    end
    if CheckKeyDown(83) then
        front_back = front_back - 1
    end
    if CheckKeyDown(65) then
        left_right = left_right - 1
    end
    if CheckKeyDown(68) then
        left_right = left_right + 1
    end

    if CheckKeyPress(32) then
        physics_controller.jump = 0.35
    end
    
    local frontX = tp_camera.HeadingX * front_back
    local frontZ = tp_camera.HeadingZ * front_back
    
    local sideX = left_right_vector.x * left_right
    local sideZ = left_right_vector.y * left_right
    
    physics_controller.heading.x = frontX + sideX
    physics_controller.heading.z = frontZ + sideZ
    
    if front_back ~= 0 or left_right ~= 0 then
        physics_controller.speed = tp_movement.Speed
    end
end
