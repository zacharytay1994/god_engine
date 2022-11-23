-- This script will show where an enemy is going to move and where it is going to attack.
-- If enemy is not going to move, the forecastIndicator will be in the same location as the enemy.
-- If enemy is not going to attack, no attack will be forecasted.

-- When forecasting, each enemy will instance a ForecastIndicator prefab. 
-- C_Pathfind will be added to ForecastIndicator.
-- At the start of each round or after player moves, forecast will be done.

-- Movement will be forecasted by using the movement script, but instead of moving
-- the enemy, it will move ForecastIndicator.

-- Attack will be forecasted using a "forecast version" of the attack script.
-- Cannot use the original attack script because it's too complicated to insert 
-- a forecast function.

--[IsComponent]
function C_EnemyForecast()
    local var = {

        --[SerializeBool]
        seeForecast = true,
        
        -- will be set to true by TurnOrderManager, at the start of each round.
        -- reset once to false once done
        performForecast = false,

        -- a list of all movement indicator entities
        indicatorsList = { },

        -- pauses drawLine for the attack forecast, because indicatorsList will be cleared
        -- and DrawLine() won't be able to get a position for the indicator.
        pauseDrawLine = false

    }
    return function()
        return var
    end
end

--[IsSystem]
function S_EnemyForecast(e)

    -- getting C_EnemyForecast
    local forecastComponent = GetComponent(e, "C_EnemyForecast")

    -- if seeForecast is unchecked in the inspector, then skip this whole script.
    -- movement and attack forecasts will not be shown.
    if (forecastComponent.seeForecast == false) then
        return
    end
    
    -- if TurnOrderManager tells EnemyForecast to perform forecast, then EnemyForecast will tell all enemies to do a forecast
    if (forecastComponent.performForecast) then
        
        -- getting each enemy's C_EnemyController and setting doingForecast to true
        local enemiesList = EntitiesWithScriptComponent("C_EnemyController")
        for i = 1, #enemiesList do
            GetComponent(enemiesList[i], "C_EnemyController").doingForecast = true
            print("[EnemyForecast.lua]", EntityName(enemiesList[i]), "doingForecast set to",  GetComponent(enemiesList[i], "C_EnemyController").doingForecast)
        end

        -- unpause the drawing of attack indicators 
        forecastComponent.pauseDrawLine = false

        -- reset variable
        forecastComponent.performForecast = false
    else
        
        -- getting player's C_Character
        local playerEntity = GetEntity("Player")
        local playerCharacterComponent = nil
        if (playerEntity ~= -1) then
            playerCharacterComponent = GetComponent(playerEntity, "C_Character")
        end
        
        -- if it's no longer the player's turn, then stop showing the indicators.
        if (playerCharacterComponent.isActive == false) then        
            ClearIndicatorsList()
        end
    end    
end

-- helper function for clearing indicatorsList.
-- pauses the drawing of attack indicators as well, to prevent crashing when
-- the attack forecast script tries to draw a line from a non-existent entity.
function ClearIndicatorsList()
    
    print("[EnemyForecast.lua] Clearing indicatorsList!")

    -- getting C_EnemyForecast
    local enemyForecastEntity = GetEntity("EnemyForecast")
    if (enemyForecastEntity == -1 ) then
        print("[EnemyForecast.lua] ERROR: EnemyForecast entity cannot be found! Returning.")
        return
    end
    local enemyForecastComponent = GetComponent(enemyForecastEntity, "C_EnemyForecast")
    
    -- pause drawing of attack indicators
    enemyForecastComponent.pauseDrawLine = true

    -- remove all indicators in the list and set them to nil
    if (#enemyForecastComponent.indicatorsList > 0) then
        print("[EnemyForecast.lua] Clearing all forecast indicators!")
        for i = 1, #enemyForecastComponent.indicatorsList do
            RemoveInstance(enemyForecastComponent.indicatorsList[i])
            enemyForecastComponent.indicatorsList[i] = nil
        end
        print("[EnemyForecast.lua] #indicatorsList:", #enemyForecastComponent.indicatorsList)
    end

    print("[EnemyForecast.lua] Done clearing indicatorsList!")

end


-- clears indicatorsList
-- sets initializedForecast for each enemyController to false
-- then sets performForecast to true
function RefreshEnemyForecast()

    print("[EnemyForecast.lua] Refreshing enemy forecasts!")
    
    -- getting C_EnemyForecast
    local enemyForecastEntity = GetEntity("EnemyForecast")
    if (enemyForecastEntity == -1 ) then
        print("[EnemyForecast.lua] ERROR: EnemyForecast entity cannot be found! Returning.")
        return
    end
    local enemyForecastComponent = GetComponent(enemyForecastEntity, "C_EnemyForecast")

    -- clear indicatorsList
    ClearIndicatorsList()

    -- make all enemies initialize forecast again
    local enemiesList = EntitiesWithScriptComponent("C_EnemyController")
    for j = 1, #enemiesList do
        GetComponent(enemiesList[j], "C_EnemyController").initializedForecast = false
    end

    -- make EnemyForecast tell all enemies to do a forecast again
    enemyForecastComponent.performForecast = true

    print("[EnemyForecast.lua] Finished refreshing enemy forecasts!")
end