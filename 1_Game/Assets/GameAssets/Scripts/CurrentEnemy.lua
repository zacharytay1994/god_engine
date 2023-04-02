-- checks which enemy triton is currently fighting

--[IsComponent]
function C_CurrentEnemy()
    local var = {     
        currentEnemyType = "",
    };
    return function()
        return var
    end
end

--[IsSystem]
function S_CurrentEnemy(e)
    
    local currentEnemyComponent = GetComponent(e, "C_CurrentEnemy")

    local animatedEntities = EntitiesWithEngineComponent("SkeleAnim3D")

    for i = 1, #animatedEntities do

        --print(i, ":", EntityName(animatedEntities[i]))
    
        if (EntityName(animatedEntities[i]) == "350DummishModel" and CurrentAnimationFrame(animatedEntities[i]) > 51.0) then
            currentEnemyComponent.currentEnemyType = "fish"     
            -- print("current enemy:", currentEnemyComponent.currentEnemyType)
            return
        elseif (EntityName(animatedEntities[i]) == "350SotongModel" and CurrentAnimationFrame(animatedEntities[i]) > 141.0) then
            currentEnemyComponent.currentEnemyType = "squid"
            -- print("current enemy:", currentEnemyComponent.currentEnemyType)
            return
        else
            currentEnemyComponent.currentEnemyType = "no enemy"
        end

        
        
    end
end