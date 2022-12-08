--[IsComponent]
function C_ThirdPersonCamera()
    local var = {
        FollowDistance = 5.0,
        HAngle = 0.0,
        CameraHeightOffset = 0.0,
        HSensitivity = 0.5,
        VSensitivity = 0.1,
        MaxHeight = 2.0,
        MinHeight = -2.0,
        HeadingX = 0.0,
        HeadingZ = 0.0
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_ThirdPersonCamera(e)
    local transform = GetTransform(e)
    local third_person = GetComponent(e, "C_ThirdPersonCamera")

    -- look at target
    SetCameraLookAt(transform.position.x, transform.position.y, transform.position.z)

    -- position follow distance from target
    third_person.HAngle = third_person.HAngle + CheckMouseOffsetX() * third_person.HSensitivity

    -- increment and clamp camera height
    third_person.CameraHeightOffset = third_person.CameraHeightOffset + CheckMouseOffsetY() * third_person.VSensitivity
    third_person.CameraHeightOffset = math.min(math.max(third_person.CameraHeightOffset, third_person.MinHeight),
        third_person.MaxHeight)


    local heading = RotateAngle(third_person.FollowDistance, 0, third_person.HAngle)
    SetCameraPosition(transform.position.x + heading.x, transform.position.y + third_person.CameraHeightOffset,
        transform.position.z + heading.y)

    third_person.HeadingX = -heading.x
    third_person.HeadingZ = -heading.y
end
