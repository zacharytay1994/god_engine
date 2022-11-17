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



    -- local enemiesList = EntitiesWithScriptComponent("C_EnemyController")
    -- for m = 1, #enemiesList do     
    
    --     local forecastObject = Child(enemiesList[m], 0)
    --     print("Testing forecastObject name:", EntityName(forecastObject))
    
    -- end


end
