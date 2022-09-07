--[IsComponent]
function C_MoveRight()
    local var = {
        --[SerializeFloat]
        speed = 0.02,
        --[SerializeString]
        text = "hello"
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_MoveRight(e)
    local transform = GetTransform(e)
    transform.position.x = transform.position.x + GetComponent(e, "C_MoveRight").speed
    transform.position.y = transform.position.y + GetComponent(e, "C_MoveRight").speed
end
