-- This is a lua script example for godEngine.

-- EXAMPLE 1
--_____________________________________________________________________________________
-- This is potentially the most used case, where you only want your system/script to

-- only affect entities that you have attached it to.
-- The C_ and S_ in front of component/system names are not mandatory (see EXAMPLE 2).
-- When a component has the same name as a system only differentiated by C_ and S_,
-- attaching the system to an entity will automatically attach the same
-- named component. This is useful when wanting to create scripts that only affect
-- entities that you have attached it to, i.e. explicitly working systems.

--[IsComponent] -> --[IsComponent] Identifier treats the following function as a component
function C_Example1_Paired()
    local var = {
        --[SerializeBool] -> --[SerializeType] Identifier displays the attribute on the editor
        b = true,
        --[SerializeInt]
        i = 1,
        --[SerializeFloat]
        f = 13.4,
        --[SerializeString]
        s = "example"
    }
    return function()
        return var
    end
end

--[IsComponent]
function Example1_RandomComponent()
    local var = {
        df = 0.02
    }
    return function ()
        return var
    end
end

-- The follow system S_Example1_Paired has dependent components:
-- 1. C_Example1_Paired
-- 2. Example1_RandomComponent

--[IsSystem] -> --[IsSystem] Identifier treats the following function as a system
function S_Example1_Paired(e)
    -- Calling the function GetC0mponent(e,...) in a system will automatically make a
    -- system dependent on that component. Attaching a system to an entity automatically
    -- attaches all dependent components to the entity.
    local example_component = GetComponent(e, "C_Example1_Paired")
    example_component.f = example_component.f + GetComponent(e, "Example1_RandomComponent").df
end

-- EXAMPLE 2
--_____________________________________________________________________________________
-- Unlike example 1, without the leading C_ and S_ this use case create "implicit"
-- components/systems. This means that if your system has been defined to be dependent on 
-- Components A and B. Your system will affect all entities with Components A and B and 
-- not just the entities you attached the system/script to. 

--[IsComponent]
function Example2_RandomComponent()
    local var = {
        --[SerializeInt] --> Type identifiers will only affect the first attribute in a row
        a = 2, b = 3
    }
    return function()
        return var
    end
end

-- The follow system S_Example1_Paired has dependent components:
-- 1. C_Example1_Paired

--[IsSystem]
function Example2_RandomSystem(e)
    -- Calling GetC0mponent(e,...) like this makes this system dependent on 
    -- C_Example1_Paired this system will now affect all entities with 
    -- C_Example1_Paired and Example2_RandomComponent whether or not the system 
    -- has been attached to it.
    -- Try attaching system S_Example1_Paired to an entity and see how 
    -- this system also inadvertently affects it. 
    GetComponent(e, "C_Example1_Paired").b = false
end