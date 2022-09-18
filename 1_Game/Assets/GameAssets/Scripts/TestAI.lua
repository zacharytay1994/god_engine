--[IsComponent]
function C_TestAI()
    local var = {
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_TestAI(e)
    -- local transform = GetTransform(GetEntity("Entity2"));
    -- transform.position.y = transform.position.y + 1;
    -- local transform2 = GetTransform(GetEntity("Entity3"));
    -- transform2.position.x = transform2.position.x + 1;
    -- print("hi")
    local entity3 = GetEntity("Entity3")
    local example = GetComponent(entity3, "C_System2")
    -- example.o = example.o + 100;
    -- example.s = "changed";
    -- print(entity3);
    example.o = example.o + 1
    print(example.o)
    -- print(example.s);
    -- print("hello")
    -- local gridcell = GetGridCell(GetEntity("Entity3"));
    -- gridcell.z = gridcell.z + 1;
end
