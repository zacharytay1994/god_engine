-- This script will show where an enemy is going to move and where it is going to attack.
-- If enemy is not going to move, no path will be forecasted.
-- If enemy is not going to attack, no attack will be forecasted.

-- When forecasting, each enemy will instance a ForecastIndicator prefab. 
-- The enemy's movement script, C_Pathfind, and (a modified) attack script will be added to ForecastIndicator.
-- At the start of each round, forecast will be done.

-- Movement will be forecasted by using the movement script, but instead of moving
-- the enemy, it will move ForecastIndicator. After ForecastIndicator is at the destination, 
-- a line will be traced back to the enemy using smaller red balls.

-- Attack will be forecasted using a "forecast version" of the attack script.
-- Cannot use the original attack script because it's too complicated to insert 
-- a forecast function.

-- TDLR; to make enemy forecast it's action, do the following:
-- 1) Instance a ForecastIndicator prefab
-- 2) Add movement script and attack script, based on enemy type, through script (below)
-- 3)

--[IsComponent]
function C_EnemyForecast()
    local var = {

        -- will be set to true by TurnOrderManager, at the start of each round.
        -- reset once to false once done
        performForecast = false,

        indicatorsList = { }

    }
    return function()
        return var
    end
end

--[IsSystem]
function S_EnemyForecast(e)

    local forecastComponent = GetComponent(e, "C_EnemyForecast")

    if (forecastComponent.performForecast) then
        local enemiesList = EntitiesWithScriptComponent("C_EnemyController")
        for i = 1, #enemiesList do
            GetComponent(enemiesList[i], "C_EnemyController").doingForecast = true
        end

        forecastComponent.performForecast = false
    else
        
        local playerEntity = GetEntity("Player")
        local playerCharacterComponent = nil
        if (playerEntity ~= -1) then
            playerCharacterComponent = GetComponent(playerEntity, "C_Character")
        end
        
        if (playerCharacterComponent.isActive == false) then        
            if (#forecastComponent.indicatorsList > 0) then
                print("[EnemyForecast.lua] Clearing all forecast indicators!")
                for i = 1, #forecastComponent.indicatorsList do
                    RemoveInstance(forecastComponent.indicatorsList[i])
                    forecastComponent.indicatorsList[i] = nil
                end
                print("[EnemyForecast.lua] #indicatorsList:", #forecastComponent.indicatorsList)
            end
        end
    end
end

function ForecastAction(enemyEntity, e)

    
    -- FORECAST MOVEMENT -------------------------------------------------------------
    local enemyGrid = GetGridCell(enemyEntity)
    local characterComponent = GetComponent(enemyEntity, "C_Character")
    local forecastComponent = GetComponent(e, "C_EnemyForecast")
    
    -- instantiate a ForecastIndicator with the floor as its parent
    local indicatorInstance = InstancePrefabParentedOnGridNow(GetEntity("Floor"), "ForecastIndicator", enemyGrid.x, enemyGrid.y, enemyGrid.z)
    forecastComponent.indicatorsList[#forecastComponent.indicatorsList + 1] = indicatorInstance
    
    -- add movement script to the ball based on enemy type and set the moveScript.executeMove = true
    if (characterComponent.characterType == "Dummee") then
        AttachScriptComponent(indicatorInstance, "C_EnemyMoveDummee")
        GetComponent(indicatorInstance, "C_EnemyMoveDummee").executeMove = true
        print("indicatorInstance's executeMove =", GetComponent(indicatorInstance, "C_EnemyMoveDummee").executeMove)
    elseif (characterComponent.characterType == "Squinky") then
        AttachScriptComponent(indicatorInstance, "C_EnemyMoveSqinky")
        GetComponent(indicatorInstance, "C_EnemyMoveSqinky").executeMove = true
    end
    -- END OF FORECAST MOVEMENT ------------------------------------------------------


    -- FORECAST ATTACK ---------------------------------------------------------------

    -- add custom attack script to the ball based on enemy type

    -- END OF FORECAST ATTACK --------------------------------------------------------

end