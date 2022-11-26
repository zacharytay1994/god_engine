-- EnemyController will be attached to each enemy. 
-- What it does is it makes the enemy move and attack, and then ends its turn.

-- For EnemyController to work, every enemy must have the following components attached:
-- 1) C_EnemyController
-- 2) C_Character
-- 3) an enemy movement script (e.g. MoveDummee)
-- 4) an enemy attack script (e.g. EnemyDummee)

--[IsComponent]
function C_EnemyController()
    local var = {

        -- will be set to true by the movement script attached to this enemy
        hasMoved = false,
        
        -- will be set to true by the attack script attached to this enemy
        hasAttacked = false,

        doingForecast = false,

        movementForecast = false,

        attackForecast = false,

        initializedForecast = false,

        indicatorEntity = nil,

        -- -- can be added in the future, if needed
        -- hasPerformedSpecialBehaviour = false

        -- name of the enemy's movement script (e.g. MoveDummee)
        -- movementScriptName will be initialized by using the inspector
        --[SerializeString]
        movementScriptName = "",

        -- name of the enemy's movement script (e.g. EnemyDummee)
        -- attackScriptName will be initialized by using the inspector
        --[SerializeString]
        attackScriptName = "",

        -- custom attack script purely for forecasting attack
        -- each enemy type will have a different one
        --[SerializeString]
        forecastAttackScriptName = ""

    }
    return function()
        return var
    end
end

--[IsSystem]
function S_EnemyController(e)

    local characterComponent = GetComponent(e, "C_Character")
    local enemyEntity = e

    local enemyController = GetComponent(e, "C_EnemyController")

    if (enemyController.doingForecast == true and enemyController.initializedForecast == false) then

        print("[EnemyController.lua] Checkpoint.")
        local movementScript = GetComponent(enemyEntity, enemyController.movementScriptName)
        enemyController.movementForecast = true
        movementScript.executeMove = true   

        local enemyGrid = GetGridCell(enemyEntity)
        enemyController.indicatorEntity = InstancePrefabParentedOnGridNow(GetEntity("Floor"), "ForecastIndicator", enemyGrid.x, enemyGrid.y, enemyGrid.z)
        print("[EnemyController.lua] ForecastIndicator instanced!")

        -- add to indicatorsList
        local enemyForecastEntity = GetEntity("EnemyForecast")
        local enemyForecastComponent = nil
        if (enemyForecastEntity ~= -1) then
            enemyForecastComponent = GetComponent(enemyForecastEntity, "C_EnemyForecast")
        end
        enemyForecastComponent.indicatorsList[#enemyForecastComponent.indicatorsList + 1] = enemyController.indicatorEntity

        enemyController.initializedForecast = true
        print("[EnemyController.lua] Executing forecast!")       
    end

    -- forecast attack based on ForecastIndicator's location
    if (enemyController.attackForecast == true) then
    
        local attackForecastScript = GetComponent(enemyEntity, enemyController.forecastAttackScriptName)
        attackForecastScript.performAttackForecast = true
    end

    if (enemyController.movementForecast == false and enemyController.attackForecast == false) then
        -- print("[EnemyController.lua] Done with both movement and attack forecast!")
        enemyController.doingForecast = false
        enemyController.initializedForecast = false
    end
    
    -- don't run the script if the enemy is not currently active
    if (characterComponent.isActive == false) then
        enemyController.hasMoved = false
        enemyController.hasAttacked = false
        return
    end

    -- -- enemy shouldn't do anything if afflicted with Frozen or Immobilised status
    -- if (characterComponent.statusAilment == "Frozen") then
    --     print("[EnemyController.lua]", EntityName(e), GetEntityData(e).id, "is Frozen. Ending turn.")
    --     characterComponent.endTurn = true
    --     return
    -- elseif (characterComponent.statusAilment == "Immobilised") then
    --     print("[EnemyController.lua]", EntityName(e), GetEntityData(e).id, "is Immobilised. Ending turn.")
    --     characterComponent.endTurn = true
    --     return
    -- end

    -- inform movementScript to start moving the enemy.
    -- once done, movementScript will set hasMoved to true.
    if (enemyController.hasMoved == false) then 
        local movementScript = GetComponent(enemyEntity, enemyController.movementScriptName)
        movementScript.executeMove = true        

    -- after enemy has moved, inform attackScript to try attacking the player.
    -- once done, attackScript will set hasAttacked to true.    
    elseif (enemyController.hasAttacked == false) then
        local attackScript = GetComponent(enemyEntity, enemyController.attackScriptName)
        attackScript.executeAttack = true

    -- after enemy has moved and attacked, reset variables and end turn
    else
        print("[EnemyController.lua] Enemy", EntityName(e), "has moved and attacked, ending turn.")
        enemyController.hasMoved = false
        enemyController.hasAttacked = false
        characterComponent.endTurn = true
    end
end