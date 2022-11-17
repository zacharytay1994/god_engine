-- This script will show where an enemy is going to move and where it is going to attack.
-- If enemy is not going to move, no path will be forecasted.
-- If enemy is not going to attack, no attack will be forecasted.

-- The enemy prefab will have a child object called ForecastAction.
-- ForecastAction will be attached with EnemyForecast script, Pathfind Script, as well as
-- the corresponding enemy movement script and a custom attack script.
-- At the start player's turn, every enemy character will do a forecast.

-- Movement will be forecasted by using the movement script, but instead of moving
-- the enemy, it will move a red ball. After the red ball is at the destination, 
-- a line will be traced back to the enemy using smaller red balls.

-- Attack will be forecasted using a "forecast version" of the attack script.
-- Cannot use the original attack script because it's too complicated to insert 
-- a forecast function.

-- TDLR; to make enemy forecast it's action, do the following:
-- 1) add C_Pathfind to Forecast
-- 2) add movement script to 
-- 3)  

--[IsComponent]
function C_EnemyForecast()
    local var = {

        -- will be set to true by TurnOrderManager, at the start of each round.
        -- reset once to false once done
        performForecast = false

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
        
            ForecastAction(enemiesList[i])
        
        end

        forecastComponent.performForecast = false
    
    end
end

function ForecastAction(enemyEntity)

    -- FORECAST MOVEMENT -------------------------------------------------------------
    -- instantiate a ball that with floor as the parent
    
    -- add movement script to the ball based on enemy type
    
    -- add c_pathfind to the ball
    
    -- set the ball.moveScript.executeMove = true
    -- END OF FORECAST MOVEMENT ------------------------------------------------------

    -- FORECAST ATTACK ---------------------------------------------------------------

    -- add custom attack script to the ball based on enemy type

    -- END OF FORECAST ATTACK --------------------------------------------------------

end