--[IsComponent]
function C_test1()
    local var = {
        --[SerializeInt]
        x = 3,
        --[SerializeFloat]
        y = 1,
        --[SerializeBool]
        b = true,
        --[SerializeString]
        s = "something"
    };
    return function()
        return var
    end
end

--[IsComponent]
function Transform()
    local var = {
        --[SerializeInt]
        x = 5,
        y = 0,
        z = 0
    };
    return function()
        return var
    end
end

--b[IsComponent]
-- function Component2()
--     local var = {
--         x = 4
--     };
--     return function()
--         return var
--     end
-- end

--[IsSystem]
function S_test1(e)
    -- if (false) then
    --     print("hello");
    -- end
    c1 = GetComponent(e, "C_test1")
    if (e == 0) then
        c1.x = c1.x + 1
    end
    -- print("Hi")
    -- local transform = GetTransform(e)
    -- transform.x = transform.x + 1
    -- GetComponent(e, "Transform")
    local p = GetTransform(e)
    p.position.x = 2
    p.position.y = 3
    p.scale.y = 5.6

    local eg = GetExampleComponent(e)
    print(eg.s)
end
