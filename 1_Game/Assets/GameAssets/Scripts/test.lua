--[IsComponent]
function C_test1()
    local var = {
        x = 3,
        y = 1
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
    print(c1.x)
    print(c1.y)
    -- test();
    -- if (e == 0) then
    --     c1.x = c1.x + 1;
    -- end
    -- print(c1.x)

    -- print("hello")
    -- print("1:")
    -- print(e)
    -- if (e == 0) then
    --     c1.x = c1.x + 1
    -- end
    -- print("2:")
    -- print(c1.x)
    -- print("3:")
    -- print(c2.x)
    -- print(c1.x)
    -- print("2:")
    -- print(c2.x)
    -- print(c1.x)
    -- c1.x = c2.x + 1
    -- print(component.x)
end
